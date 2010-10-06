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

#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED 1

#include "annot.h"

#define NBUCK 47

struct entry
{
  char *id;
  struct val val;
  struct entry *next;
};

struct table
{
  struct entry *buck[NBUCK];
};

struct table *make_table(void);
void free_table(struct table *);
int insert(struct table *, const char *, struct val *);
int lookup(struct table *, const char *, struct val *);

#endif /* TABLE_H_INCLUDED */
