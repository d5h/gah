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

#include <string.h>
#include "table.h"
#include "util.h"

struct table *
make_table(void)
{
  struct table *t = xmalloc(sizeof *t);
  size_t i;

  for (i = 0; i < NBUCK; ++i)
    t->buck[i] = NULL;

  return t;
}

static void
free_entry(struct entry *e)
{
  if (e)
    {
      FREE(e->id);
      free_entry(e->next);
      FREE(e);
    }
}

void
free_table(struct table *t)
{
  if (t)
    {
      size_t i;

      for (i = 0; i < NBUCK; ++i)
	free_entry(t->buck[i]);

      FREE(t);
    }
}

static size_t
hash(const char *id)
{
  size_t h = 0;

  for ( ; *id; ++id)
    h = 2113 * h + (unsigned char) *id;

  return h;
}

int
insert(struct table *t, const char *id, struct val *v)
{
  size_t h = hash(id) % NBUCK;
  struct entry *e = t->buck[h];

  for ( ; e; e = e->next)
    {
      if (strcmp(id, e->id) == 0)
	{
	  e->val = *v;
	  return 1;
	}
    }

  e = xmalloc(sizeof *e);
  e->id = xstrdup(id);
  e->val = *v;
  e->next = t->buck[h];
  t->buck[h] = e;
  return 0;
}

int
lookup(struct table *t, const char *id, struct val *v)
{
  if (id)
    {
      size_t h = hash(id) % NBUCK;
      struct entry *e = t->buck[h];

      for ( ; e; e = e->next)
	{
	  if (strcmp(id, e->id) == 0)
	    {
	      *v = e->val;
	      return 0;
	    }
	}
    }

  return -1;
}
