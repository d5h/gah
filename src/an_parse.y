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

%}

%union
{
  struct an_type type;
  struct expr *expr;
  struct ast *ast;
  size_t n;
  int val;
  char *id;
}

%token <val> K_INT
%token <id> K_ID
%token <type> K_TYPE
%token K_S K_IF K_ELSE K_WHILE K_SET K_LSBFIRST K_MSBFIRST

%right '?' ':'
%left K_OR
%left K_AND
%left '|'
%left '^'
%left '&'
%left K_EQ K_NE
%left '<' K_LE '>' K_GE
%left K_SL K_SR
%left '+' '-'
%left '*' '/' '%'
%right '@'
%right K_NEG '!' '~'

%type <type> type
%type <expr> opt_sub opt_rep expr symex sym opt_param
%type <ast> start desc ifdesc whiledesc thendesc desc_list
%type <id> id
%type <n> selfs

%%

start
	: desc_list	{ add_desc($1); }
	;

desc
	: type
	  id opt_rep opt_param ';'	{ $$ = make_terminal($2, &$1, $3, $4); }
	| K_S '{' desc_list '}'
	  id opt_rep opt_param ';'	{ $$ = make_struct($5, $3, $6, $7); }
	| ifdesc
	| whiledesc
	| K_SET id '=' expr ';'	{ $$ = make_set($2, $4); }
	| K_LSBFIRST ';'	{ $$ = make_setsex(T_LSBFIRST); }
	| K_MSBFIRST ';'	{ $$ = make_setsex(0); }
	;

opt_param
	: /* empty */		{ $$ = NULL; }
	| '(' expr ')'		{ $$ = $2; }
	;

ifdesc
	: K_IF '(' expr ')' thendesc	{ $$ = make_if($3, $5, NULL); }
	| K_IF '(' expr ')' thendesc
	  K_ELSE thendesc		{ $$ = make_if($3, $5, $7); }
	;

whiledesc
	: K_WHILE '(' expr ')' thendesc	{ $$ = make_while($3, $5); }
	;

thendesc
	: '{' desc_list '}'	{ $$ = $2; }
	| desc
	;

desc_list
	: desc desc_list	{ $$ = $1; $$->next = $2; }
	| desc
	;

type
	: K_TYPE
	;

id
	: K_ID
	;

opt_sub
	: /* empty */	{ $$ = NULL; }
	| '[' expr ']'	{ $$ = $2; }
	;

opt_rep
	: /* empty */	{ $$ = NULL; }
	| '[' expr ']'	{ $$ = $2; }
	;

expr
	: K_INT				{ $$ = make_val($1); }
	| symex
	| '(' expr ')'			{ $$ = $2; }
	| '-' expr	%prec K_NEG	{ $$ = make_f1(E_NEG, $2); }
	| '!' expr			{ $$ = make_f1(E_NOT, $2); }
	| '~' expr			{ $$ = make_f1(E_BITNOT, $2); }
	| type '@' expr			{ $$ = make_at(&$1, $3); }
	| expr K_EQ expr		{ $$ = make_f2(E_EQ, $1, $3); }
	| expr K_NE expr		{ $$ = make_f2(E_NE, $1, $3); }
	| expr '<' expr			{ $$ = make_f2(E_LT, $1, $3); }
	| expr K_LE expr		{ $$ = make_f2(E_LE, $1, $3); }
	| expr '>' expr			{ $$ = make_f2(E_GT, $1, $3); }
	| expr K_GE expr		{ $$ = make_f2(E_GE, $1, $3); }
	| expr '+' expr			{ $$ = make_f2(E_ADD, $1, $3); }
	| expr '-' expr			{ $$ = make_f2(E_SUB, $1, $3); }
	| expr '*' expr			{ $$ = make_f2(E_MUL, $1, $3); }
	| expr '/' expr			{ $$ = make_f2(E_DIV, $1, $3); }
	| expr '%' expr			{ $$ = make_f2(E_MOD, $1, $3); }
	| expr K_SL expr		{ $$ = make_f2(E_SL, $1, $3); }
	| expr K_SR expr		{ $$ = make_f2(E_SR, $1, $3); }
	| expr '&' expr			{ $$ = make_f2(E_BITAND, $1, $3); }
	| expr '|' expr			{ $$ = make_f2(E_BITOR, $1, $3); }
	| expr '^' expr			{ $$ = make_f2(E_BITXOR, $1, $3); }
	| expr K_AND expr		{ $$ = make_f2(E_AND, $1, $3); }
	| expr K_OR expr		{ $$ = make_f2(E_OR, $1, $3); }
	| expr '?' expr ':' expr	{ $$ = make_f3(E_COND, $1, $3, $5); }
	;

symex
	: sym '.' symex		{ $$ = $1; $$->u.sym.memb = $3; }
	| sym
	;

sym
	: id opt_sub	{ $$ = make_sym($1, $2); }
	| selfs opt_sub	{ $$ = make_self($1, $2); }
	| '#' opt_sub	{ $$ = make_nth($2); }
	;

selfs
	: selfs '$'	{ $$ = $1 + 1; }
	| '$'		{ $$ = 1; }
	;

%%

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
