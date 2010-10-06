# Configure template for GNU Annotated Hex.	-*- Autoconf -*-

# Copyright (C) 2006 Dan Hipschman.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.

AC_PREREQ(2.59)
AC_INIT(gah, 0.5b, dsh@linux.ucla.edu)
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LEX			# Should check for flex.
AC_PROG_YACC			# Only tested with bison.

# Checks for libraries.
AC_CHECK_LIB([ncurses], [initscr],,
	     AC_MSG_ERROR([Missing ncurses library required for build]))

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stddef.h stdlib.h string.h unistd.h],,
		 AC_MSG_ERROR([Missing a header file required for build]))

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([atexit memset strcspn strtol strtoul],,
	       AC_MSG_ERROR([Missing function required for build]))

AC_CONFIG_FILES([Makefile
                 data/Makefile
                 doc/Makefile
                 src/Makefile])
AC_OUTPUT
