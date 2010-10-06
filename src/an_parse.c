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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 1 "an_parse.y"

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

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "annot.h"
#include "an_parse.h"
#include "table.h"
#include "util.h"

static unsigned defsex;

enum expr_op
  {
    E_INT,
    E_SYM,
    E_EQ,
    E_NE,
    E_LT,
    E_LE,
    E_GT,
    E_GE,
    E_ADD,
    E_SUB,
    E_MUL,
    E_DIV,
    E_MOD,
    E_NEG,
    E_SL,
    E_SR,
    E_BITAND,
    E_BITOR,
    E_BITXOR,
    E_BITNOT,
    E_AND,
    E_OR,
    E_NOT,
    E_COND,
    E_AT,
  };

enum ast_op
  {
    A_TERMINAL,
    A_STRUCT,
    A_IF,
    A_WHILE,
    A_SET,
    A_SETSEX,
  };

struct expr
{
  size_t line;
  enum expr_op op;
  union
  {
    struct
    {
      struct expr *p1, *p2, *p3;
    } f;
    struct
    {	
      char *id;
      size_t self;
      size_t nth;
      struct expr *sub;
      struct expr *memb;
    } sym;
    struct
    {
      unsigned type;
      size_t len;
      struct expr *off;
    } at;
    int val;
  } u;
};

struct ast
{
  size_t line;
  enum ast_op op;
  union
  {
    struct
    {
      char *id;
      size_t nth;
      struct expr *param;
      unsigned type;
      size_t len;
      int defsex;
      struct ast *membs;
      struct expr *rep;
    } annot;
    struct
    {
      struct expr *cond;
      struct ast *s1, *s2;
    } control;
    struct
    {
      char *id;
      struct expr *val;
    } set;
    unsigned setsex;
  } u;
  struct ast *next;
};

struct env
{
  struct table *tab;
  struct annot *self;
  size_t selfoff;
  size_t curoff;
};

static void add_desc(struct ast *);
static struct ast *make_terminal(char *, struct an_type *, struct expr *,
				 struct expr *);
static struct ast *make_struct(char *, struct ast *, struct expr *,
			       struct expr *);
static struct ast *make_if(struct expr *, struct ast *, struct ast *);
static struct ast *make_while(struct expr *, struct ast *);
static struct ast *make_set(char *, struct expr *);
static struct ast *make_setsex(unsigned);
static struct expr *make_val(int);
static struct expr *make_sym(char *, struct expr *);
static struct expr *make_self(size_t self, struct expr *);
static struct expr *make_nth(struct expr *);
static struct expr *make_f1(enum expr_op, struct expr *);
static struct expr *make_f2(enum expr_op, struct expr *, struct expr *);
static struct expr *make_f3(enum expr_op, struct expr *, struct expr *,
			    struct expr *);
static struct expr *make_at(struct an_type *, struct expr *);

int yylex(void);
void yyerror(const char *);



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

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
/* Line 190 of yacc.c.  */
#line 291 "an_parse.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 213 of yacc.c.  */
#line 303 "an_parse.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  21
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   436

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  48
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  16
/* YYNRULES -- Number of rules. */
#define YYNRULES  57
/* YYNRULES -- Number of states. */
#define YYNSTATES  122

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   276

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    34,     2,    46,    47,    32,    19,     2,
      41,    42,    30,    28,     2,    29,    45,    31,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    14,    37,
      22,    40,    23,    13,    33,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    43,     2,    44,    18,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    38,    17,    39,    35,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    15,    16,
      20,    21,    24,    25,    26,    27,    36
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     5,    11,    20,    22,    24,    30,    33,
      36,    37,    41,    47,    55,    61,    65,    67,    70,    72,
      74,    76,    77,    81,    82,    86,    88,    90,    94,    97,
     100,   103,   107,   111,   115,   119,   123,   127,   131,   135,
     139,   143,   147,   151,   155,   159,   163,   167,   171,   175,
     179,   185,   189,   191,   194,   197,   200,   203
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      49,     0,    -1,    55,    -1,    56,    57,    59,    51,    37,
      -1,     6,    38,    55,    39,    57,    59,    51,    37,    -1,
      52,    -1,    53,    -1,    10,    57,    40,    60,    37,    -1,
      11,    37,    -1,    12,    37,    -1,    -1,    41,    60,    42,
      -1,     7,    41,    60,    42,    54,    -1,     7,    41,    60,
      42,    54,     8,    54,    -1,     9,    41,    60,    42,    54,
      -1,    38,    55,    39,    -1,    50,    -1,    50,    55,    -1,
      50,    -1,     5,    -1,     4,    -1,    -1,    43,    60,    44,
      -1,    -1,    43,    60,    44,    -1,     3,    -1,    61,    -1,
      41,    60,    42,    -1,    29,    60,    -1,    34,    60,    -1,
      35,    60,    -1,    56,    33,    60,    -1,    60,    21,    60,
      -1,    60,    20,    60,    -1,    60,    22,    60,    -1,    60,
      25,    60,    -1,    60,    23,    60,    -1,    60,    24,    60,
      -1,    60,    28,    60,    -1,    60,    29,    60,    -1,    60,
      30,    60,    -1,    60,    31,    60,    -1,    60,    32,    60,
      -1,    60,    27,    60,    -1,    60,    26,    60,    -1,    60,
      19,    60,    -1,    60,    17,    60,    -1,    60,    18,    60,
      -1,    60,    16,    60,    -1,    60,    15,    60,    -1,    60,
      13,    60,    14,    60,    -1,    62,    45,    61,    -1,    62,
      -1,    57,    58,    -1,    63,    58,    -1,    46,    58,    -1,
      63,    47,    -1,    47,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   203,   203,   207,   209,   211,   212,   213,   214,   215,
     219,   220,   224,   225,   230,   234,   235,   239,   240,   244,
     248,   252,   253,   257,   258,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
     287,   291,   292,   296,   297,   298,   302,   303
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "K_INT", "K_ID", "K_TYPE", "K_S", "K_IF",
  "K_ELSE", "K_WHILE", "K_SET", "K_LSBFIRST", "K_MSBFIRST", "'?'", "':'",
  "K_OR", "K_AND", "'|'", "'^'", "'&'", "K_NE", "K_EQ", "'<'", "'>'",
  "K_GE", "K_LE", "K_SR", "K_SL", "'+'", "'-'", "'*'", "'/'", "'%'", "'@'",
  "'!'", "'~'", "K_NEG", "';'", "'{'", "'}'", "'='", "'('", "')'", "'['",
  "']'", "'.'", "'#'", "'$'", "$accept", "start", "desc", "opt_param",
  "ifdesc", "whiledesc", "thendesc", "desc_list", "type", "id", "opt_sub",
  "opt_rep", "expr", "symex", "sym", "selfs", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,    63,    58,   268,   269,   124,    94,    38,
     270,   271,    60,    62,   272,   273,   274,   275,    43,    45,
      42,    47,    37,    64,    33,   126,   276,    59,   123,   125,
      61,    40,    41,    91,    93,    46,    35,    36
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    48,    49,    50,    50,    50,    50,    50,    50,    50,
      51,    51,    52,    52,    53,    54,    54,    55,    55,    56,
      57,    58,    58,    59,    59,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    61,    61,    62,    62,    62,    63,    63
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     5,     8,     1,     1,     5,     2,     2,
       0,     3,     5,     7,     5,     3,     1,     2,     1,     1,
       1,     0,     3,     0,     3,     1,     1,     3,     2,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       5,     3,     1,     2,     2,     2,     2,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,    19,     0,     0,     0,     0,     0,     0,     0,    18,
       5,     6,     2,     0,     0,     0,     0,    20,     0,     8,
       9,     1,    17,    23,     0,    25,     0,     0,     0,     0,
      21,    57,     0,    21,     0,    26,    52,    21,     0,     0,
       0,    10,     0,    28,    29,    30,     0,     0,    55,     0,
      53,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    56,    54,     0,     0,     0,     0,     0,    23,
      27,     0,    31,     0,    49,    48,    46,    47,    45,    33,
      32,    34,    36,    37,    35,    44,    43,    38,    39,    40,
      41,    42,     0,    16,    12,    51,    14,     7,    24,     0,
       3,    10,    22,     0,     0,     0,    11,     0,    50,    15,
      13,     4
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     8,     9,    78,    10,    11,   104,    12,    32,    33,
      48,    41,    34,    35,    36,    37
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -52
static const short int yypact[] =
{
      79,   -52,   -28,   -26,   -20,     8,   -24,   -15,    25,    79,
     -52,   -52,   -52,     8,    79,    58,    58,   -52,   -13,   -52,
     -52,   -52,   -52,    -5,   -11,   -52,    58,    58,    58,    58,
      17,   -52,    33,    17,   161,   -52,    -9,   -42,   189,    58,
      58,    37,     8,   -52,   -52,   -52,   217,    58,   -52,    58,
     -52,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      70,    22,   -52,   -52,    70,   273,   101,    58,    46,    -5,
     -52,   131,   -52,   298,   335,   351,   366,   380,   393,   404,
     404,   108,   108,   108,   108,     3,     3,    41,    41,   -52,
     -52,   -52,    79,   -52,    86,   -52,   -52,   -52,   -52,   245,
     -52,    37,   -52,    58,    57,    70,   -52,    60,   318,   -52,
     -52,   -52
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -52,   -52,   -50,    -4,   -52,   -52,   -51,    -7,     0,    -2,
     -29,    19,   -10,    29,   -52,   -52
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      13,    47,    22,    18,    50,    72,    38,    24,    73,    13,
      14,    23,    17,    19,    13,    15,    43,    44,    45,    46,
     103,    16,    20,   106,   103,    21,    17,    39,    42,    75,
      76,    65,    66,    67,    68,    69,    71,    81,    40,    82,
      79,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
      47,    25,    17,     1,   120,   103,    49,   109,    30,    31,
      13,    67,    68,    69,    13,     1,     2,     3,    77,     4,
       5,     6,     7,   110,     1,     2,     3,    26,     4,     5,
       6,     7,    27,    28,   115,   114,   119,   121,   111,    29,
     105,     0,    13,   118,    30,    31,     0,   117,   102,     0,
       0,     0,     0,     0,    51,    13,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    63,    64,    65,    66,    67,    68,
      69,     0,     0,     0,    51,   108,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    51,   112,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,     0,     0,     0,     0,     0,     0,
       0,     0,    51,    70,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,     0,     0,     0,     0,     0,     0,     0,     0,
      51,    74,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
       0,     0,     0,     0,     0,     0,     0,     0,    51,    80,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,     0,     0,
       0,     0,     0,     0,     0,     0,    51,   116,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,     0,     0,     0,     0,
     107,    51,   113,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    51,     0,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69
};

static const yysigned_char yycheck[] =
{
       0,    43,     9,     5,    33,    47,    16,    14,    37,     9,
      38,    13,     4,    37,    14,    41,    26,    27,    28,    29,
      70,    41,    37,    74,    74,     0,     4,    40,    39,    39,
      40,    28,    29,    30,    31,    32,    45,    47,    43,    49,
      42,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      43,     3,     4,     5,   115,   115,    33,    77,    46,    47,
      70,    30,    31,    32,    74,     5,     6,     7,    41,     9,
      10,    11,    12,    37,     5,     6,     7,    29,     9,    10,
      11,    12,    34,    35,     8,   102,    39,    37,    79,    41,
      71,    -1,   102,   113,    46,    47,    -1,   111,    38,    -1,
      -1,    -1,    -1,    -1,    13,   115,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    26,    27,    28,    29,    30,    31,
      32,    -1,    -1,    -1,    13,    44,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    13,    44,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    13,    42,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      13,    42,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    13,    42,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    13,    42,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    -1,    -1,    -1,    -1,
      37,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    13,    -1,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     5,     6,     7,     9,    10,    11,    12,    49,    50,
      52,    53,    55,    56,    38,    41,    41,     4,    57,    37,
      37,     0,    55,    57,    55,     3,    29,    34,    35,    41,
      46,    47,    56,    57,    60,    61,    62,    63,    60,    40,
      43,    59,    39,    60,    60,    60,    60,    43,    58,    33,
      58,    13,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      42,    45,    47,    58,    42,    60,    60,    41,    51,    57,
      42,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    38,    50,    54,    61,    54,    37,    44,    60,
      37,    59,    44,    14,    55,     8,    42,    51,    60,    39,
      54,    37
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  register short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;


  yyvsp[0] = yylval;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 203 "an_parse.y"
    { add_desc((yyvsp[0].ast)); }
    break;

  case 3:
#line 208 "an_parse.y"
    { (yyval.ast) = make_terminal((yyvsp[-3].id), &(yyvsp[-4].type), (yyvsp[-2].expr), (yyvsp[-1].expr)); }
    break;

  case 4:
#line 210 "an_parse.y"
    { (yyval.ast) = make_struct((yyvsp[-3].id), (yyvsp[-5].ast), (yyvsp[-2].expr), (yyvsp[-1].expr)); }
    break;

  case 7:
#line 213 "an_parse.y"
    { (yyval.ast) = make_set((yyvsp[-3].id), (yyvsp[-1].expr)); }
    break;

  case 8:
#line 214 "an_parse.y"
    { (yyval.ast) = make_setsex(T_LSBFIRST); }
    break;

  case 9:
#line 215 "an_parse.y"
    { (yyval.ast) = make_setsex(0); }
    break;

  case 10:
#line 219 "an_parse.y"
    { (yyval.expr) = NULL; }
    break;

  case 11:
#line 220 "an_parse.y"
    { (yyval.expr) = (yyvsp[-1].expr); }
    break;

  case 12:
#line 224 "an_parse.y"
    { (yyval.ast) = make_if((yyvsp[-2].expr), (yyvsp[0].ast), NULL); }
    break;

  case 13:
#line 226 "an_parse.y"
    { (yyval.ast) = make_if((yyvsp[-4].expr), (yyvsp[-2].ast), (yyvsp[0].ast)); }
    break;

  case 14:
#line 230 "an_parse.y"
    { (yyval.ast) = make_while((yyvsp[-2].expr), (yyvsp[0].ast)); }
    break;

  case 15:
#line 234 "an_parse.y"
    { (yyval.ast) = (yyvsp[-1].ast); }
    break;

  case 17:
#line 239 "an_parse.y"
    { (yyval.ast) = (yyvsp[-1].ast); (yyval.ast)->next = (yyvsp[0].ast); }
    break;

  case 21:
#line 252 "an_parse.y"
    { (yyval.expr) = NULL; }
    break;

  case 22:
#line 253 "an_parse.y"
    { (yyval.expr) = (yyvsp[-1].expr); }
    break;

  case 23:
#line 257 "an_parse.y"
    { (yyval.expr) = NULL; }
    break;

  case 24:
#line 258 "an_parse.y"
    { (yyval.expr) = (yyvsp[-1].expr); }
    break;

  case 25:
#line 262 "an_parse.y"
    { (yyval.expr) = make_val((yyvsp[0].val)); }
    break;

  case 27:
#line 264 "an_parse.y"
    { (yyval.expr) = (yyvsp[-1].expr); }
    break;

  case 28:
#line 265 "an_parse.y"
    { (yyval.expr) = make_f1(E_NEG, (yyvsp[0].expr)); }
    break;

  case 29:
#line 266 "an_parse.y"
    { (yyval.expr) = make_f1(E_NOT, (yyvsp[0].expr)); }
    break;

  case 30:
#line 267 "an_parse.y"
    { (yyval.expr) = make_f1(E_BITNOT, (yyvsp[0].expr)); }
    break;

  case 31:
#line 268 "an_parse.y"
    { (yyval.expr) = make_at(&(yyvsp[-2].type), (yyvsp[0].expr)); }
    break;

  case 32:
#line 269 "an_parse.y"
    { (yyval.expr) = make_f2(E_EQ, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 33:
#line 270 "an_parse.y"
    { (yyval.expr) = make_f2(E_NE, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 34:
#line 271 "an_parse.y"
    { (yyval.expr) = make_f2(E_LT, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 35:
#line 272 "an_parse.y"
    { (yyval.expr) = make_f2(E_LE, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 36:
#line 273 "an_parse.y"
    { (yyval.expr) = make_f2(E_GT, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 37:
#line 274 "an_parse.y"
    { (yyval.expr) = make_f2(E_GE, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 38:
#line 275 "an_parse.y"
    { (yyval.expr) = make_f2(E_ADD, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 39:
#line 276 "an_parse.y"
    { (yyval.expr) = make_f2(E_SUB, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 40:
#line 277 "an_parse.y"
    { (yyval.expr) = make_f2(E_MUL, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 41:
#line 278 "an_parse.y"
    { (yyval.expr) = make_f2(E_DIV, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 42:
#line 279 "an_parse.y"
    { (yyval.expr) = make_f2(E_MOD, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 43:
#line 280 "an_parse.y"
    { (yyval.expr) = make_f2(E_SL, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 44:
#line 281 "an_parse.y"
    { (yyval.expr) = make_f2(E_SR, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 45:
#line 282 "an_parse.y"
    { (yyval.expr) = make_f2(E_BITAND, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 46:
#line 283 "an_parse.y"
    { (yyval.expr) = make_f2(E_BITOR, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 47:
#line 284 "an_parse.y"
    { (yyval.expr) = make_f2(E_BITXOR, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 48:
#line 285 "an_parse.y"
    { (yyval.expr) = make_f2(E_AND, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 49:
#line 286 "an_parse.y"
    { (yyval.expr) = make_f2(E_OR, (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 50:
#line 287 "an_parse.y"
    { (yyval.expr) = make_f3(E_COND, (yyvsp[-4].expr), (yyvsp[-2].expr), (yyvsp[0].expr)); }
    break;

  case 51:
#line 291 "an_parse.y"
    { (yyval.expr) = (yyvsp[-2].expr); (yyval.expr)->u.sym.memb = (yyvsp[0].expr); }
    break;

  case 53:
#line 296 "an_parse.y"
    { (yyval.expr) = make_sym((yyvsp[-1].id), (yyvsp[0].expr)); }
    break;

  case 54:
#line 297 "an_parse.y"
    { (yyval.expr) = make_self((yyvsp[-1].n), (yyvsp[0].expr)); }
    break;

  case 55:
#line 298 "an_parse.y"
    { (yyval.expr) = make_nth((yyvsp[0].expr)); }
    break;

  case 56:
#line 302 "an_parse.y"
    { (yyval.n) = (yyvsp[-1].n) + 1; }
    break;

  case 57:
#line 303 "an_parse.y"
    { (yyval.n) = 1; }
    break;


    }

/* Line 1037 of yacc.c.  */
#line 1613 "an_parse.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {

		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 yydestruct ("Error: popping",
                             yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yydestruct ("Error: discarding lookahead",
              yytoken, &yylval);
  yychar = YYEMPTY;
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 306 "an_parse.y"


static int evaluate(struct expr *, struct env *, struct val *);
static int add_ast(struct ast *, struct annot *, struct env *, size_t,
		   size_t *);

static int
val_at(unsigned t, size_t len, size_t off, struct val *v)
{
  int s;

  if (valbits_at(t, len, off, &v->bits, &s))
    return -1;

  v->type = t & (T_TWOSCOMP | T_ONESCOMP | T_SIGNMAG) ? V_SGN : V_USN;

  if (s)
    v->bits = ~v->bits + 1;

  return 0;
}

static int
evalsym(struct expr *e, struct annot *an, struct env *env, size_t off,
	struct val *res)
{
  const char *id = e->u.sym.id;
  struct annot *ch = an->child;
  size_t choff;

  if (e->u.sym.self)
    {
      set_msg("%zi: cannot use $ as a member derived from $", e->line);
      return -1;
    }

  if (e->u.sym.nth)
    {
      if (e->u.sym.sub)
	{
	  set_msg("%zi: # can't have a subscript", e->line);
	  return -1;
	}
      else if (e->u.sym.memb)
	{
	  set_msg("%zi: # can't have members", e->line);
	  return -1;
	}
      else
	{
	  res->type = V_USN;
	  res->bits = an->nth;
	  return 0;
	}
    }

  while (ch)
    {
      if (strcmp(id, ch->name) == 0)
        break;
      ch = ch->next;
    }

  if (! ch)
    {
      set_msg("%zi: '%s' not found", e->line, id);
      return -1;
    }
  else
    {
      struct val sub;
      if (e->u.sym.sub)
	{
	  if (evaluate(e->u.sym.sub, env, &sub))
	    return -1;
	}
      else
	{
	  sub.type = V_USN;
	  sub.bits = 0;
	}

      if (is_neg(&sub) || ch->repeat <= sub.bits)
        {
          set_msg("%zi: '%s' doesn't have index %s", e->line, id,
		  valstr(&sub));
          return -1;
        }

      choff = off + ch->reloff + sub.bits * ch->len;
    }

  if (ANNOT_TERMINAL(ch) && ! e->u.sym.memb)
    {
      if (val_at(ch->type, ch->len, choff, res))
	return -1;

      return 0;
    }
  else if (! ANNOT_TERMINAL(ch) && e->u.sym.memb)
    {
      return evalsym(e->u.sym.memb, ch, env, choff, res);
    }
  else
    {
      set_msg("%zi: wrong type for '%s'", e->line, id);
      return -1;
    }
}

static int
check_self(struct annot *self, struct expr *e)
{
  for ( ; self && self->allow_repeat; self = self->parent)
    {
      if (self->repeats)
	{
	  set_msg("%zi: cannot reference an object that repeats with $",
		  e->line);
	  return -1;
	}
      self->allow_repeat = FALSE;
    }

  return 0;
}

static valbits_t
val_lt(struct val *v1, struct val *v2)
{
  int v1_neg = is_neg(v1);
  int v2_neg = is_neg(v2);

  if (v1_neg && v2_neg)
    return (~v2->bits + 1) < (~v1->bits + 1);
  else if (v1_neg)
    return 1;
  else if (v2_neg)
    return 0;
  else
    return v1->bits < v2->bits;
}

static int
evaluate(struct expr *e, struct env *env, struct val *res)
{
  struct val val;

  if (e->op == E_INT)
    {
      val.type = V_SGN;
      val.bits = e->u.val;
    }
  else if (e->op == E_SYM)
    {
      if (e->u.sym.self)
	{
	  struct annot *self = env->self;
	  size_t off = env->selfoff;
	  size_t n = e->u.sym.self;

	  if (e->u.sym.sub)
	    {
	      set_msg("%zi: subscript is illegal on $", e->line);
	      return -1;
	    }
	  else if (! e->u.sym.memb)
	    {
	      set_msg("%zi: expected member access for $", e->line);
	      return -1;
	    }

	  while (--n && self)
	    {
	      off -= self->reloff;
	      self = self->parent;
	    }

	  if (! self)
	    {
	      set_msg("%zi: $'s nested too deep", e->line);
	      return -1;
	    }
	  else if (check_self(self, e))
	    return -1;

	  if (evalsym(e->u.sym.memb, self, env, off, &val))
	    return -1;
	}
      else if (! e->u.sym.sub && ! e->u.sym.memb
	       && streq(e->u.sym.id, "_"))
	{
	  val.type = V_USN;
	  val.bits = env->curoff;
	}
      else if (e->u.sym.sub || e->u.sym.memb
	       || lookup(env->tab, e->u.sym.id, &val))
	if (evalsym(e, top, env, 0, &val))
	  return -1;
    }
  else if (e->op == E_AT)
    {
      struct val off;
      if (evaluate(e->u.at.off, env, &off))
	return -1;

      if (is_neg(&off))
	{
	  set_msg("%zi: offset cannot be negative", e->line);
	  return -1;
	}

      if (val_at(e->u.at.type, e->u.at.len, off.bits, &val))
	return -1;
    }
  else if (e->op == E_AND || e->op == E_OR)
    {
      if (evaluate(e->u.f.p1, env, &val))
	return -1;

      if ((e->op == E_AND && val.bits) || (e->op == E_OR && ! val.bits))
	if (evaluate(e->u.f.p2, env, &val))
	  return -1;

      val.type = V_SGN;
      val.bits = val.bits ? 1 : 0;
    }
  else if (e->op == E_COND)
    {
      struct val cond;

      /* we don't check that p2 and p3 have the same type,
	 I don't know if we should yet  */
      if (evaluate(e->u.f.p1, env, &cond)
	  || evaluate(cond.bits ? e->u.f.p2 : e->u.f.p3, env, &val))
	return -1;
    }
  else
    {
      struct val p1, p2;

      if (evaluate(e->u.f.p1, env, &p1)
	  || (e->u.f.p2 && evaluate(e->u.f.p2, env, &p2)))
	return -1;

      val.type =
	e->u.f.p2
	? (p1.type == V_USN || p2.type == V_USN ? V_USN : V_SGN)
	: p1.type;

      switch (e->op)
	{
	case E_EQ:
	  val.type = V_SGN;
	  val.bits = p1.bits == p2.bits;
	  break;

	case E_NE:
	  val.type = V_SGN;
	  val.bits = p1.bits != p2.bits;
	  break;

	case E_LT:
	  val.type = V_SGN;
	  val.bits = val_lt(&p1, &p2);
	  break;

	case E_LE:
	  val.type = V_SGN;
	  val.bits = ! val_lt(&p2, &p1);
	  break;

	case E_GT:
	  val.type = V_SGN;
	  val.bits = val_lt(&p2, &p1);
	  break;

	case E_GE:
	  val.type = V_SGN;
	  val.bits = ! val_lt(&p1, &p2);
	  break;

	case E_ADD:
	  val.bits = p1.bits + p2.bits;
	  break;

	case E_SUB:
	  val.bits = p1.bits - p2.bits;
	  break;

	case E_MUL:
	  val.bits = p1.bits * p2.bits;
	  break;

	case E_DIV:
	  if (p2.bits == 0)
	    {
	      set_msg("%zi: division by zero", e->line);
	      return -1;
	    }
	  else
	    val.bits = p1.bits / p2.bits;
	  break;

	case E_MOD:
	  if (p2.bits == 0)
	    {
	      set_msg("%zi: division by zero", e->line);
	      return -1;
	    }
	  else
	    val.bits = p1.bits % p2.bits;
	  break;

	case E_NEG:
	  val.bits = ~p1.bits + 1;
	  break;

	case E_NOT:
	  val.type = V_SGN;
	  val.bits = ! p1.bits;
	  break;

	case E_BITNOT:
	  val.bits = ~p1.bits;
	  break;

	case E_SL:
	  val.bits = p1.bits << p2.bits;
	  break;

	case E_SR:
	  val.bits = p1.bits >> p2.bits;
	  break;

	case E_BITAND:
	  val.bits = p1.bits & p2.bits;
	  break;

	case E_BITOR:
	  val.bits = p1.bits | p2.bits;
	  break;

	case E_BITXOR:
	  val.bits = p1.bits ^ p2.bits;
	  break;

	default:
	  bug("%zi: couldn't evaluate expression", e->line);
	  return -1;
	  /* shut up compiler */
	  break;
	}
    }

  *res = val;
  return 0;
}

static void
free_expr(struct expr *e)
{
  if (e)
    {
      switch (e->op)
	{
	case E_INT:
	  break;

	case E_SYM:
	  FREE(e->u.sym.id);
	  free_expr(e->u.sym.sub);
	  free_expr(e->u.sym.memb);
	  break;

	case E_EQ:
	case E_NE:
	case E_LT:
	case E_LE:
	case E_GT:
	case E_GE:
	case E_ADD:
	case E_SUB:
	case E_MUL:
	case E_DIV:
	case E_MOD:
	case E_NEG:
	case E_SL:
	case E_SR:
	case E_BITAND:
	case E_BITOR:
	case E_BITXOR:
	case E_BITNOT:
	case E_AND:
	case E_OR:
	case E_NOT:
	case E_COND:
	  free_expr(e->u.f.p1);
	  free_expr(e->u.f.p2);
	  free_expr(e->u.f.p3);
	  break;

	case E_AT:
	  free_expr(e->u.at.off);
	  break;
	}

      FREE(e);
    }
}

static void
free_ast(struct ast *ast)
{
  if (ast)
    {
      switch (ast->op)
	{
	case A_TERMINAL:
	case A_STRUCT:
	  FREE(ast->u.annot.id);
	  free_expr(ast->u.annot.param);
	  free_ast(ast->u.annot.membs);
	  free_expr(ast->u.annot.rep);
	  break;

	case A_IF:
	case A_WHILE:
	  free_expr(ast->u.control.cond);
	  free_ast(ast->u.control.s1);
	  free_ast(ast->u.control.s2);
	  break;

	case A_SET:
	  FREE(ast->u.set.id);
	  free_expr(ast->u.set.val);

	case A_SETSEX:
	  break;
	}

      free_ast(ast->next);
      FREE(ast);
    }
}

static int
add_annot(struct annot *c, struct ast *ast, struct env *penv, struct annot *p,
	  size_t off)
{
  struct env env;
  env.tab = penv->tab;
  env.self = c;
  env.selfoff = penv->selfoff + off;
  env.curoff = penv->curoff;

  c->name = xstrdup(ast->u.annot.id);
  c->nth = ++ast->u.annot.nth;
  c->reloff = off;
  c->parent = p;
  c->type = ast->u.annot.type;

  if (ast->u.annot.defsex)
    c->type |= defsex;

  if (ast->u.annot.membs)
    {
      dbg("starting structure '%s'", c->name);
      if (add_ast(ast->u.annot.membs, c, &env, 0, &c->len))
	return -1;
    }
  else
    {
      c->len = ast->u.annot.len;
      if (c->len == 0)
	{
	  set_msg("%zi: '%s' cannot have zero width", ast->line, c->name);
	  return -1;
	}
      else if (ANNOT_TERMINAL(c) && p->type != T_BITFIELD
	       && sizeof(valbits_t) < c->len)
	{
	  set_msg("%zi: '%s' exceeds maximum width", ast->line, c->name);
	  return -1;
	}
      else if (ANNOT_TERMINAL(c) && p->type == T_BITFIELD
	       && CHAR_BIT * sizeof(valbits_t) < c->len)
	{
	  set_msg("%zi: '%s' exceeds maximum bitfield width", ast->line,
		  c->name);
	  return -1;	  
	}
    }

  if (ast->u.annot.rep)
    {
      struct val v;

      if (! c->allow_repeat)
	{
	  set_msg("%zi: object referenced with $ cannot repeat", ast->line);
	  return -1;
	}

      c->repeats = TRUE;
      if (evaluate(ast->u.annot.rep, &env, &v))
	return -1;

      if (is_neg(&v))
	{
	  set_msg("%zi: array size cannot be negative (%s)", ast->line,
		  valstr(&v));
	}
      c->repeat = v.bits;
    }
  else
    {
      c->repeats = FALSE;
      c->repeat = 1;
    }

  if (ast->u.annot.param)
    {
      if (evaluate(ast->u.annot.param, &env, &c->param))
	return -1;

      c->has_param = TRUE;
    }

  dbg("%zi: added %s '%s'[%s](%s) @0X%zX", ast->line,
      typestr(c->type, c->len), c->name, c->repeats ? nstr(c->repeat) : "",
      c->has_param ? valstr(&c->param) : "", penv->curoff);
  c->next = p->child;
  p->child = c;
  return 0;
}

static int
add_ast(struct ast *ast, struct annot *p, struct env *penv, size_t off,
	size_t *len)
{
  struct env env = *penv;
  size_t start = off;

  while (ast)
    {
      if (stop_parser)
	{
	  set_msg("user interrupt");
	  return -1;
	}

      switch (ast->op)
	{
	case A_TERMINAL:
	case A_STRUCT:
	  {
	    struct annot *c = make_annot();
	    if (add_annot(c, ast, &env, p, off))
	      {
		free_annot(c);
		return -1;
	      }
	    else
	      {
		off += c->len * c->repeat;
		env.curoff += c->len * c->repeat;
	      }
	  }
	  break;

	case A_IF:
	case A_WHILE:
	  {
	    struct val cond;

	    do
	      {
		if (evaluate(ast->u.control.cond, &env, &cond))
		  return -1;
		else
		  {
		    size_t n = 0;
		    if (cond.bits)
		      {
			if (add_ast(ast->u.control.s1, p, &env, off, &n))
			  return -1;
		      }
		    else if (ast->u.control.s2)
		      {
			if (add_ast(ast->u.control.s2, p, &env, off, &n))
			  return -1;
		      }
		    off += n;
		    env.curoff += n;
		  }
	      } while (ast->op == A_WHILE && cond.bits);
	  }
	  break;

	case A_SET:
	  {
	    struct val v;
	    if (evaluate(ast->u.set.val, &env, &v))
	      return -1;

	    dbg("%zi: setting '%s' to %s (0X%zX)", ast->line, ast->u.set.id,
		valstr(&v), v.bits);

	    if (strcmp(ast->u.set.id, "_") == 0)
	      {
		size_t o = 0;

		if (is_neg(&v))
		  {
		    set_msg("%zi: cannot assign '_' a negative value (%s)",
			    valstr(&v));
		    return -1;
		  }
		else if (p != top)
		  {
		    set_msg("%zi: set '_' is only legal at the top level",
			    ast->line);
		    return -1;
		  }
		else if (top != find_annot(top, v.bits, &o))
		  {
		    set_msg("%zi: set '_' value must be at the top level",
			    ast->line);
		    return -1;
		  }
		else
		  {
		    off = v.bits;
		    env.curoff = v.bits;
		  }
	      }
	    else
	      insert(env.tab, ast->u.set.id, &v);
	  }
	  break;

	case A_SETSEX:
	  defsex = ast->u.setsex;
	  break;
	}

      ast = ast->next;
    }

  if (len)
    *len = off - start;

  return 0;
}

static void
add_desc(struct ast *ast)
{
  if (ANNOT_TERMINAL(cursan))
    {
      set_msg("cannot add members to a terminal annotation");
    }
  else
    {
      struct env env;
      struct annot *ok = cursan->child;

      defsex = T_LSBFIRST;
      env.tab = make_table();
      env.self = cursan;
      env.selfoff = absoff;
      env.curoff = offset + cursor;
      if (add_ast(ast, cursan, &env, offset + cursor - absoff, NULL))
	{
	  while (ok != cursan->child)
	    {
	      struct annot *bad = cursan->child;
	      cursan->child = bad->next;
	      bad->next = NULL;
	      free_annot(bad);
	    }
	}
      free_table(env.tab);
    }
  free_ast(ast);
}

static struct ast *
make_ast(void)
{
  struct ast *ast = xmalloc(sizeof *ast);
  ast->line = line;
  ast->op = -1;
  memset(&ast->u, 0, sizeof ast->u);
  ast->next = NULL;
  return ast;
}

static struct ast *
make_terminal(char *id, struct an_type *at, struct expr *rep,
	      struct expr *param)
{
  struct ast *ast = make_ast();
  ast->op = A_TERMINAL;
  ast->u.annot.id = id;
  ast->u.annot.nth = 0;
  ast->u.annot.param = param;
  ast->u.annot.type = at->type;
  ast->u.annot.len = at->len;
  ast->u.annot.defsex = at->defsex;
  ast->u.annot.membs = NULL;
  ast->u.annot.rep = rep;
  return ast;
}

static struct ast *
make_struct(char *id, struct ast *membs, struct expr *rep, struct expr *param)
{
  struct ast *ast = make_ast();
  ast->op = A_STRUCT;
  ast->u.annot.id = id;
  ast->u.annot.nth = 0;
  ast->u.annot.param = param;
  ast->u.annot.type = T_GROUP;
  ast->u.annot.defsex = FALSE;
  ast->u.annot.membs = membs;
  ast->u.annot.rep = rep;
  return ast;
}

static struct ast *
make_if(struct expr *cond, struct ast *s1, struct ast *s2)
{
  struct ast *ast = make_ast();
  ast->op = A_IF;
  ast->u.control.cond = cond;
  ast->u.control.s1 = s1;
  ast->u.control.s2 = s2;
  return ast;
}

static struct ast *
make_while(struct expr *cond, struct ast *s)
{
  struct ast *ast = make_ast();
  ast->op = A_WHILE;
  ast->u.control.cond = cond;
  ast->u.control.s1 = s;
  ast->u.control.s2 = NULL;
  return ast;
}

static struct ast *
make_set(char *id, struct expr *val)
{
  struct ast *ast = make_ast();
  ast->op = A_SET;
  ast->u.set.id = id;
  ast->u.set.val = val;
  return ast;
}

static struct ast *
make_setsex(unsigned sex)
{
  struct ast *ast = make_ast();
  ast->op = A_SETSEX;
  ast->u.setsex = sex;
  return ast;
}

static struct expr *
make_expr(void)
{
  struct expr *e = xmalloc(sizeof *e);
  e->line = line;
  e->op = -1;
  memset(&e->u, 0, sizeof e->u);
  return e;
}

static struct expr *
make_val(int val)
{
  struct expr *e = make_expr();
  e->op = E_INT;
  e->u.val = val;
  return e;
}

static struct expr *
make_sym(char *id, struct expr *sub)
{
  struct expr *e = make_expr();
  e->op = E_SYM;
  e->u.sym.id = id;
  e->u.sym.nth = FALSE;
  e->u.sym.self = 0;
  e->u.sym.sub = sub;
  e->u.sym.memb = NULL;
  return e;
}

static struct expr *
make_self(size_t self, struct expr *sub)
{
  struct expr *e = make_expr();
  e->op = E_SYM;
  e->u.sym.id = NULL;
  e->u.sym.nth = FALSE;
  e->u.sym.self = self;
  e->u.sym.sub = sub;
  e->u.sym.memb = NULL;
  return e;
}

static struct expr *
make_nth(struct expr *sub)
{
  struct expr *e = make_expr();
  e->op = E_SYM;
  e->u.sym.id = NULL;
  e->u.sym.nth = TRUE;
  e->u.sym.self = 0;
  e->u.sym.sub = sub;
  e->u.sym.memb = NULL;
  return e;
}

static struct expr *
make_f1(enum expr_op op, struct expr *p1)
{
  return make_f3(op, p1, NULL, NULL);
}

static struct expr *
make_f2(enum expr_op op, struct expr *p1, struct expr *p2)
{
  return make_f3(op, p1, p2, NULL);
}

static struct expr *
make_f3(enum expr_op op, struct expr *p1, struct expr *p2, struct expr *p3)
{
  struct expr *e = make_expr();
  e->op = op;
  e->u.f.p1 = p1;
  e->u.f.p2 = p2;
  e->u.f.p3 = p3;
  return e;
}

static struct expr *
make_at(struct an_type *t, struct expr *off)
{
  struct expr *e = make_expr();
  e->op = E_AT;
  e->u.at.type = t->type;
  e->u.at.len = t->len;
  e->u.at.off = off;
  return e;
}

