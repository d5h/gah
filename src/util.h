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

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED 1

#include <stdarg.h>
#include <stdlib.h>

#if ! defined(TRUE) && ! defined(FALSE)
#define TRUE	1
#define FALSE	0
#endif

#define FREE(p)	(free((p)), (p) = NULL)

void *xmalloc(size_t n);
char *xstrdup(const char *s);

int streq(const char *, const char *);

void vset_msg(const char *, va_list);
void set_msg(const char *, ...);
void bug(const char *, ...);

void opendbg(const char *);
void vdbg(const char *, va_list);
void dbg(const char *, ...);

#endif /* UTIL_H_INCLUDED */
