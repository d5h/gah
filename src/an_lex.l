%{
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

#include <ctype.h>
#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "annot.h"
#include "an_parse.h"
#include "util.h"

size_t line;
int stop_parser;

static int parse_id(const char *);
static int parse_int(const char *);

int yyparse(void);
int yywrap(void)
{
  return 1;
}

%}

%%

"s"			{ return K_S; }
"if"			{ return K_IF; }
"else"			{ return K_ELSE; }
"while"			{ return K_WHILE; }
"=="			{ return K_EQ; }
"!="			{ return K_NE; }
"<="			{ return K_LE; }
">="			{ return K_GE; }
"&&"			{ return K_AND; }
"||"			{ return K_OR; }
"<<"			{ return K_SL; }
">>"			{ return K_SR; }
"set"			{ return K_SET; }
"lsbfirst"		{ return K_LSBFIRST; }
"msbfirst"		{ return K_MSBFIRST; }
[a-z_A-Z][a-z_A-Z0-9]*	{ return parse_id(yytext); }
[0-9]+			{ return parse_int(yytext); }
0[xX][0-9a-fA-F]+	{ return parse_int(yytext); }
"//".*			/* end of line comments */
[ \t]+			/* eat whitespace */
\n			{ ++line; }
.			{ return yytext[0]; }

%%

#define NTYPES	(sizeof types / sizeof types[0])

static struct
{
  const char *name;
  unsigned type;
} types[] = {
  { "u", 0 },
  { "so", T_ONESCOMP },
  { "st", T_TWOSCOMP },
  { "sm", T_SIGNMAG },
  { "b", T_BITFIELD },
  { "s", T_GROUP },
};

static int
make_id(const char *id)
{
  yylval.id = xstrdup(id);
  return K_ID;
}

static int
parse_id(const char *id)
{
  size_t dig = strcspn(id, "0123456789");

  if (! isdigit(id[dig]))
    {
      return make_id(id);
    }
  else
    {
      size_t i;

      for (i = 0; i < NTYPES; ++i)
	{
	  const char *n = types[i].name;
	  if (dig == strlen(n) && strncmp(id, n, dig) == 0)
	    break;
	}

      if (i < NTYPES)
	{
	  char *sex;
	  unsigned long len = strtoul(id + dig, &sex, 10);
	  unsigned type = types[i].type;

	  yylval.type.defsex = TRUE;
	  if (strcmp(sex, "l") == 0)
	    {
	      yylval.type.defsex = FALSE;
	      type |= T_LSBFIRST;
	    }
	  else if (strcmp(sex, "m") == 0)
	    yylval.type.defsex = FALSE;
	  else if (sex[0])
	    return make_id(id);

	  yylval.type.len = len;
	  yylval.type.type = type;
	  return K_TYPE;
	}
      else
	{
	  return make_id(id);
	}
    }
}

static int
parse_int(const char *digits)
{
  yylval.val = strtol(digits, NULL, 0);
  return K_INT;
}

static void
cbreak_handler(int n)
{
  (void) n;
  stop_parser = TRUE;
}

static void
start_parse(void)
{
  line = 1;
  stop_parser = FALSE;
  signal(SIGINT, cbreak_handler);
  cbreak();
}

static void
done_parse(void)
{
  raw();
}

void
parse_string(const char *s)
{
  start_parse();
  YY_BUFFER_STATE b = yy_scan_string(s);
  yyparse();
  yy_delete_buffer(b);
  done_parse();
}

void
parse_file(const char *fname)
{
  start_parse();
  yyin = fopen(fname, "r");
  if (yyin)
    {
      yyparse();
      fclose(yyin);
    }
  yyin = NULL;
  done_parse();
}

void
yyerror(const char *s)
{
  set_msg("%zi: %s on '%s'", line, s, yytext);
}
