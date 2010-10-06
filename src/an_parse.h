/* A Bison parser, made by GNU Bison 2.0.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     K_INT = 258,
     K_ID = 259,
     K_TYPE = 260,
     K_S = 261,
     K_IF = 262,
     K_ELSE = 263,
     K_WHILE = 264,
     K_SET = 265,
     K_LSBFIRST = 266,
     K_MSBFIRST = 267,
     K_OR = 268,
     K_AND = 269,
     K_NE = 270,
     K_EQ = 271,
     K_GE = 272,
     K_LE = 273,
     K_SR = 274,
     K_SL = 275,
     K_NEG = 276
   };
#endif
#define K_INT 258
#define K_ID 259
#define K_TYPE 260
#define K_S 261
#define K_IF 262
#define K_ELSE 263
#define K_WHILE 264
#define K_SET 265
#define K_LSBFIRST 266
#define K_MSBFIRST 267
#define K_OR 268
#define K_AND 269
#define K_NE 270
#define K_EQ 271
#define K_GE 272
#define K_LE 273
#define K_SR 274
#define K_SL 275
#define K_NEG 276




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 166 "an_parse.y"
typedef union YYSTYPE {
  struct an_type type;
  struct expr *expr;
  struct ast *ast;
  size_t n;
  int val;
  char *id;
} YYSTYPE;
/* Line 1318 of yacc.c.  */
#line 88 "an_parse.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



