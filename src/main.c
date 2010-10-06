/* gah - an annotated hex viewer.

   Copyright (C) 2006 Dan Hipschman.

   Written by Dan Hipschman, starting 2006-07-28.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
   Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#define _GNU_SOURCE
#include <ctype.h>
#include <getopt.h>
#include <limits.h>
#include <ncurses.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include "annot.h"
#include "util.h"

#define HIGHLIGHT	1

size_t offset;			/* window offset */
size_t cursor;			/* cursor offset relative to window */
size_t absoff;			/* absolute offset of cursan */
struct annot *top;		/* top level annotation (whole file) */
struct annot *cursan;		/* annotation for cursor */

static WINDOW *whex;		/* hex window */
static WINDOW *wmsg;		/* message window */
char *msg_text;			/* show in msg window */
static unsigned char *data;	/* hex data */
static size_t data_size;	/* hex data length */
static size_t columns;		/* num hex columns per row in window */
static size_t rows;		/* num rows in hex window */
static size_t msghi = 2;	/* height of message window */
static char unprintable = '.';	/* ascii display for unprintable chars */
static char nodata = ' ';	/* ascii display for no more data */

static FILE *dbgfp;		/* debug file pointer */

enum
  {
    OPTHELP = 256,
    OPTDEBUGFILE,
    OPTVERSION,
  };

struct option longopts[] =
  {
    {"help", no_argument, NULL, OPTHELP},
    {"debug-file", required_argument, NULL, OPTDEBUGFILE},
    {"version", no_argument, NULL, OPTVERSION},
    {0, 0, 0, 0},
  };

static void
help(FILE *f)
{
  fprintf(f, "Usage: %s hexfile gahfile\n", PACKAGE);
}

void
free_annot(struct annot *an)
{
  if (an)
    {
      FREE(an->name);
      free_annot(an->child);
      free_annot(an->next);
      FREE(an);
    }
}

void *
xmalloc(size_t n)
{
  void *p = malloc(n);
  if (! p)
    abort();
  return p;
}

char *
xstrdup(const char *s)
{
  char *d = xmalloc(strlen(s) + 1);
  strcpy(d, s);
  return d;
}

int
streq(const char *s1, const char *s2)
{
  if (s1 == s2)
    return TRUE;
  else if (! s1 || ! s2)
    return FALSE;
  else
    return strcmp(s1, s2) == 0;
}

void
vset_msg(const char *m, va_list ap)
{
  FREE(msg_text);
  if (m)
    {
      vasprintf(&msg_text, m, ap);
      dbg("%s", msg_text);
    }
}

void
set_msg(const char *m, ...)
{
  va_list ap;
  va_start(ap, m);
  vset_msg(m, ap);
  va_end(ap);
}

void
bug(const char *m, ...)
{
  char *msg;
  va_list ap;
  va_start(ap, m);
  vasprintf(&msg, m, ap);
  va_end(ap);

  set_msg("%s; please send a bug report", msg);
  FREE(msg);
}

void
opendbg(const char *filename)
{
  if (dbgfp)
    fclose(dbgfp);

  dbgfp = fopen(filename, "w");
}

void
vdbg(const char *fmt, va_list ap)
{
  if (dbgfp)
    {
      vfprintf(dbgfp, fmt, ap);
      fprintf(dbgfp, "\n");
    }
}

void
dbg(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vdbg(fmt, ap);
  va_end(ap);
}

int
is_neg(struct val *v)
{
  valbits_t signbit = 1 << (CHAR_BIT * sizeof(valbits_t) - 1);
  return v->type == V_SGN && (v->bits & signbit);
}

#define SLEN(t) (CHAR_BIT * sizeof(t))

const char *
nstr(size_t n)
{
  static char buf[SLEN(n) + 2];
  char *p = buf + sizeof buf - 1;

  *p = '\0';
  do
    *--p = '0' + n % 10;
  while (n /= 10);

  return p;
}

const char *
valstr(struct val *v)
{
  static char buf[SLEN(v->bits) + 2];
  char *p = buf + sizeof buf - 1;
  valbits_t bits = v->bits;
  int neg = is_neg(v);

  if (neg)
    bits = ~bits + 1;

  *p = '\0';
  do
    *--p = '0' + bits % 10;
  while (bits /= 10);

  if (v->bits && neg)
    *--p = '-';

  return p;
}

const char *
typestr(unsigned t, size_t n)
{
  static char buf[SLEN(n) + 4];
  char *p = buf + sizeof buf - 1;

  *p = '\0';
  if (! (t & (T_GROUP | T_BITFIELD)) && 1 < n)
    *--p = t & T_LSBFIRST ? 'l' : 'm';

  do
    *--p = '0' + n % 10;
  while (n /= 10);

  if (t & T_GROUP)
    *--p = 's';
  else if (t & T_BITFIELD)
    *--p = 'b';
  else if (t & T_TWOSCOMP)
    *--p = 't', *--p = 's';
  else if (t & T_ONESCOMP)
    *--p = 'o', *--p = 's';
  else if (t & T_SIGNMAG)
    *--p = 'm', *--p = 's';
  else
    *--p = 'u';

  return p;
}

static void
cleanup(void)
{
  free_annot(top);
  FREE(msg_text);
  FREE(data);
  delwin(wmsg);
  delwin(whex);
  endwin();
}

static void
initialize(void)
{
  initscr();
  start_color();
  init_pair(HIGHLIGHT, COLOR_GREEN, COLOR_YELLOW);
  raw();
  noecho();

  rows = LINES - (msghi + 2) - 2;
  columns = 4;
  while (12 + 13 * (columns / 2) + 2 * columns <= COLS)
    columns *= 2;
  whex = newwin(rows + 2, COLS, 0, 0);
  box(whex, 0, 0);
  keypad(whex, TRUE);

  wmsg = newwin(msghi + 2, COLS, rows + 2, 0);
  box(wmsg, 0, 0);
  keypad(wmsg, TRUE);

  atexit(cleanup);
}

struct annot *
make_annot(void)
{
  struct annot *an = xmalloc(sizeof *an);
  an->name = NULL;
  an->nth = 1;
  an->param.type = V_SGN;
  an->param.bits = 0;
  an->reloff = 0;
  an->len = 1;
  an->repeat = 1;
  an->type = T_GROUP;
  an->parent = NULL;
  an->child = NULL;
  an->next = NULL;
  an->repeats = FALSE;
  an->allow_repeat = TRUE;
  an->has_param = FALSE;
  return an;
}

static void
readfp(FILE *fp)
{
  fseek(fp, 0, SEEK_END);
  data_size = ftell(fp);
  /* can't handle empty files, so this is a temporary (crappy) solution */
  if (data_size == 0)
    exit(EXIT_SUCCESS);
  data = xmalloc(data_size);
  rewind(fp);
  fread(data, 1, data_size, fp);

  top = cursan = make_annot();
  top->len = data_size;
  top->type = T_GROUP;
}

static void
readin(const char *filename)
{
  FILE *fp;

  fp = fopen(filename, "rb");
  if (! fp)
    exit(1);

  readfp(fp);
  fclose(fp);
}

static void
show_hex(void)
{
  size_t histart = absoff, histop = histart + cursan->len;
  size_t a = offset;
  size_t r;
  int hi = 0;

  if (cursan == top)
    histart = histop = 0;

  wmove(whex, 1, 1);
  for (r = 0; r < rows; ++r)
    {
      size_t start = a;
      size_t c;

      if (start < data_size)
	wprintw(whex, "%08X", start);
      else
	wprintw(whex, "        ");

      for (c = 0; c < columns; ++a, ++c)
	{
	  if (hi && histop <= a)
	    {
	      wattroff(whex, COLOR_PAIR(HIGHLIGHT));
	      hi = 0;
	    }

	  if (c % 4 == 0)
	    wprintw(whex, " ");

	  wprintw(whex, " ");
	  if (a == offset + cursor)
	    wattron(whex, A_REVERSE);
	  if (! hi && histart <= a && a < histop)
	    {
	      wattron(whex, COLOR_PAIR(HIGHLIGHT));
	      hi = 1;
	    }

	  if (a < data_size)
	    wprintw(whex, "%02X", data[a]);
	  else
	    wprintw(whex, "  ");

	  if (a == offset + cursor)
	    wattroff(whex, A_REVERSE);
	}

      if (hi)
	{
	  wattroff(whex, COLOR_PAIR(HIGHLIGHT));
	  hi = 0;
	}
      wprintw(whex, "  ");
      a = start;
      for (c = 0; c < columns; ++a, ++c)
	{
	  if (hi && histop <= a)
	    {
	      wattroff(whex, COLOR_PAIR(HIGHLIGHT));
	      hi = 0;
	    }

	  if (a == offset + cursor)
	    wattron(whex, A_REVERSE);
	  if (! hi && histart <= a && a < histop)
	    {
	      wattron(whex, COLOR_PAIR(HIGHLIGHT));
	      hi = 1;
	    }

	  if (data_size < start)
	    wprintw(whex, " ");
	  else if (a < data_size)
	    wprintw(whex, "%c", isprint(data[a]) ? data[a] : unprintable);
	  else
	    wprintw(whex, "%c", nodata);

	  if (a == offset + cursor)
	    wattroff(whex, A_REVERSE);
	}

      if (hi)
	{
	  wattroff(whex, COLOR_PAIR(HIGHLIGHT));
	  hi = 0;
	}

      wmove(whex, r + 2, 1);
    }

  curs_set(0);
  wrefresh(whex);
}

static size_t
print_names(WINDOW *w, struct annot *an)
{
  if (an == top)
    return 0;
  else
    {
      size_t cursoff = offset + cursor;
      size_t paroff = print_names(w, an->parent);
      size_t off = paroff + an->reloff;
      size_t index = (cursoff - off) / an->len;
      if (an->parent != top)
	wprintw(w, ".");
      wprintw(w, "%s", an->name);
      if (an->repeats)
	wprintw(w, "[%zi]", index);
      if (an->has_param)
	wprintw(w, "(%s)", valstr(&an->param));
      return off + index * an->len;
    }
}

static void
clear_msg(void)
{
  size_t i;
  for (i = 0; i < msghi; ++i)
    {
      wmove(wmsg, i + 1, 1);
      wprintw(wmsg, "%*s", COLS - 2, "");
    }
  wmove(wmsg, 1, 1);
}

int
valbits_at(unsigned t, size_t len, size_t off, valbits_t *res, int *s)
{
  size_t m = ! (t & T_LSBFIRST);
  valbits_t u = 0;
  unsigned long sbit = 1 << (CHAR_BIT * len - 1);
  valbits_t mask = sbit - 1;
  int neg = FALSE;

  if (data_size < off + len)
    {
      set_msg("value outside file");
      return -1;
    }

  if (! m)
    off += len - 1;

  while (0 < len--)
    {
      u = (u << CHAR_BIT) + data[off];
      off = m ? off + 1 : off - 1;
    }

  if (u & sbit)
    {
      neg = TRUE;
      if (t & T_ONESCOMP)
	u = ~u & mask;
      else if (t & T_TWOSCOMP)
	u = (~u + 1) & mask;
      else if (t & T_SIGNMAG)
	u = u & mask;
      else
	neg = FALSE;
    }

  *res = u;
  if (s)
    *s = neg;

  return 0;
}

static void
print_value(WINDOW *w, struct annot *an)
{
  int n;
  valbits_t u;

  if (valbits_at(an->type, an->len, absoff, &u, &n))
    {
      wprintw(w, "value outside file");
      FREE(msg_text);
    }
  else
    {
      const char *s = n ? "-" : "";
      wprintw(w, "%s%lu", s, u);
    }
}

static void
show_msg(void)
{
  clear_msg();
  if (msg_text)
    {
      wprintw(wmsg, "%s", msg_text);
      FREE(msg_text);
    }
  else
    {
      print_names(wmsg, cursan);
      wmove(wmsg, 2, 1);
      if (~cursan->type & T_GROUP)
	print_value(wmsg, cursan);
    }
  wrefresh(wmsg);
}

static void
readstr(WINDOW *w, char *buf, size_t buflen)
{
  echo();
  curs_set(1);
  wgetnstr(w, buf, buflen);
  curs_set(0);
  noecho();
}

static void
annotate_cursan(void)
{
  char buf[256];

  clear_msg();
  wprintw(wmsg, "Annotate: ");
  wrefresh(wmsg);
  readstr(wmsg, buf, sizeof buf - 1);
  parse_string(buf);
}

struct annot *
find_annot(struct annot *an, size_t pt, size_t *po)
{
  size_t index, base;
  size_t off = *po;

  if (pt < off || off + an->len * an->repeat <= pt)
    return NULL;

  index = (pt - off) / an->len;
  base = off + index * an->len;
  if (an->type & T_GROUP)
    {
      struct annot *ch;
      for (ch = an->child; ch; ch = ch->next)
	{
	  size_t choff = base + ch->reloff;
	  struct annot *chan = find_annot(ch, pt, &choff);
	  if (chan)
	    {
	      *po = choff;
	      return chan;
	    }
	}
    }

  *po = base;
  return an;
}

static void
set_cursan(void)
{
  if (data_size <= offset + cursor)
    cursor = data_size - offset - 1;

  absoff = 0;
  cursan = find_annot(top, offset + cursor, &absoff);
}

static void
main_loop(void)
{
  int ch = 0;
  int quit = 0;
  int rehex = 1;
  int remsg = 1;

  while (! quit)
    {
      set_cursan();

      if (rehex)
	show_hex();
      if (remsg)
	show_msg();

      rehex = remsg = 0;
      ch = wgetch(whex);
      switch (ch)
	{
	case 'a':
	  annotate_cursan();
	  rehex = remsg = 1;
	  break;
	case 'q':
	  quit = 1;
	  break;
	case KEY_DOWN:
	  if (cursor + columns < columns * rows)
	    cursor += columns;
	  else if (offset + columns < data_size)
	    offset += columns;
	  rehex = remsg = 1;
	  break;
	case KEY_UP:
	  if (columns <= cursor)
	    cursor -= columns;
	  else if (columns <= offset)
	    offset -= columns;
	  rehex = remsg = 1;
	  break;
	case KEY_NPAGE:
	  if (offset + columns * rows < data_size)
	    offset += columns * rows;
	  rehex = remsg = 1;
	  break;
	case KEY_PPAGE:
	  if (columns * rows <= offset)
	    offset -= columns * rows;
	  else
	    offset = 0;
	  rehex = remsg = 1;
	  break; 
	case KEY_RIGHT:
	  if (cursor + 1 < columns * rows)
	    ++cursor;
	  rehex = remsg = 1;
	  break;
	case KEY_LEFT:
	  if (1 <= cursor)
	    --cursor;
	  rehex = remsg = 1;
	  break;
	}
    }
}

int
main(int argc, char *argv[])
{
  int c;

  while ((c = getopt_long(argc, argv, "", longopts, NULL)) != -1)
    switch (c)
      {
      case OPTHELP:
	help(stdout);
	exit(EXIT_SUCCESS);
	break;

      case OPTDEBUGFILE:
	opendbg(optarg);
	break;

      case OPTVERSION:
	printf("%s\n", VERSION);
	exit(EXIT_SUCCESS);
	break;
      }

  if (argc - optind == 0 || 2 < argc - optind)
    {
      help(stderr);
      exit(EXIT_FAILURE);
    }

  initialize();
  readin(argv[optind]);

  if (argc - optind == 2)
    parse_file(argv[optind + 1]);

  main_loop();

  return 0;
}
