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

#ifndef ANNOT_H_INCLUDED
#define ANNOT_H_INCLUDED 1

#include <stddef.h>

/* type flags */
#define T_GROUP		1
#define T_BITFIELD	2
#define T_LSBFIRST	4
#define T_ONESCOMP	8
#define T_TWOSCOMP	16
#define T_SIGNMAG	32

#define ANNOT_TERMINAL(a)	(! ((a)->type & (T_GROUP | T_BITFIELD)))

#define NAMEMAX		63

enum val_type
  {
    V_USN,
    V_SGN,
  };

/* an unsigned type at least four bytes wide */
typedef unsigned long valbits_t;
struct val
{
  enum val_type type;
  valbits_t bits;
};

struct annot
{
  char *name;
  size_t nth;
  struct val param;
  size_t reloff;
  size_t len;
  valbits_t repeat;
  unsigned type;
  struct annot *parent;
  struct annot *child;
  struct annot *next;
  unsigned repeats : 1;
  unsigned allow_repeat : 1;
  unsigned has_param : 1;
};

struct an_type
{
  unsigned type;
  size_t len;
  int defsex;
};

extern size_t line;		/* input line number for error messages */
extern int stop_parser;		/* interrupt flag */

extern size_t offset;		/* window offset */
extern size_t cursor;		/* cursor offset relative to window */
extern size_t absoff;		/* absolute offset of cursan */
extern struct annot *top;	/* top level annotation (whole file) */
extern struct annot *cursan;	/* annotation for cursor */

int is_neg(struct val *);
const char *nstr(size_t); /* returns static storage */
const char *valstr(struct val *); /* returns static storage */
const char *typestr(unsigned, size_t); /* returns static storage */

struct annot *make_annot(void);
void parse_string(const char *);
void parse_file(const char *);
int valbits_at(unsigned, size_t, size_t, valbits_t *, int *);
void free_annot(struct annot *);
struct annot *find_annot(struct annot *, size_t, size_t *);

#endif /* ANNOT_H_INCLUDED */
