/*
 * Copyright (C) 2002 Roman Zippel <zippel@linux-m68k.org>
 * Released under the terms of the GNU GPL v2.0.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cml1.h"

struct file *file_list;
struct file *current_file;

struct symbol symbol_yes = {
	name: "y",
	curr: { "y", yes },
	flags: SYMBOL_YES|SYMBOL_VALID,
}, symbol_mod = {
	name: "m",
	curr: { "m", mod },
	flags: SYMBOL_MOD|SYMBOL_VALID,
}, symbol_no = {
	name: "n",
	curr: { "n", no },
	flags: SYMBOL_NO|SYMBOL_VALID,
}, symbol_empty = {
	name: "",
	curr: { "", no },
	flags: SYMBOL_VALID,
};

struct expr *expr_alloc_symbol(struct symbol *sym)
{
	struct expr *e = malloc(sizeof(*e));
	memset(e, 0, sizeof(*e));
	e->token = T_WORD;
	e->left.sym = sym;
	return e;
}

struct expr *expr_alloc_one(int token, struct expr *ce)
{
	struct expr *e = malloc(sizeof(*e));
	memset(e, 0, sizeof(*e));
	e->token = token;
	e->left.expr = ce;
	return e;
}

struct expr *expr_alloc_two(int token, struct expr *e1, struct expr *e2)
{
	struct expr *e = malloc(sizeof(*e));
	memset(e, 0, sizeof(*e));
	e->token = token;
	e->left.expr = e1;
	e->right.expr = e2;
	return e;
}

struct expr *expr_alloc_comp(int token, struct symbol *s1, struct symbol *s2)
{
	struct expr *e = malloc(sizeof(*e));
	memset(e, 0, sizeof(*e));
	e->token = token;
	e->left.sym = s1;
	e->right.sym = s2;
	return e;
}

struct expr *expr_alloc_and(struct expr *e1, struct expr *e2)
{
	if (!e1)
		return e2;
	return e2 ? expr_alloc_two(T_AND, e1, e2) : e1;
}

struct expr *expr_copy(struct expr *org)
{
	struct expr *e;

	if (!org)
		return NULL;

	e = malloc(sizeof(*org));
	memcpy(e, org, sizeof(*org));
	switch (org->token) {
	case T_WORD:
		e->left = org->left;
		break;
	case '!':
		e->left.expr = expr_copy(org->left.expr);
		break;
	case '=':
	case T_UNEQUAL:
		e->left.sym = org->left.sym;
		e->right.sym = org->right.sym;
		break;
	case T_OR:
	case T_AND:
	case '&':
	case '|':
	case '^':
	case T_IF:
	case T_ELSE:
		e->left.expr = expr_copy(org->left.expr);
		e->right.expr = expr_copy(org->right.expr);
		break;
	default:
		printf("can't copy token %d\n", e->token);
		free(e);
		e = NULL;
		break;
	}

	return e;
}

void expr_free(struct expr *e)
{
	if (!e)
		return;

	switch (e->token) {
	case T_WORD:
		break;
	case '!':
		expr_free(e->left.expr);
		return;
	case '=':
	case T_UNEQUAL:
		break;
	case '|':
	case T_OR:
	case T_AND:
	case '&':
	case T_IF:
	case T_ELSE:
		expr_free(e->left.expr);
		expr_free(e->right.expr);
		break;
	default:
		printf("how to free token %d?\n", e->token);
		break;
	}
	free(e);
}

static inline int token_gt(int t1, int t2)
{
#if 0
	return 1;
#else
	switch (t1) {
	case '=':
	case T_UNEQUAL:
		if (t2 == '!')
			return 1;
	case '!':
		if (t2 == T_AND || t2 == T_IF || t2 == T_ELSE || t2 == '&')
			return 1;
	case '&':
	case T_IF:
	case T_ELSE:
	case T_AND:
		if (t2 == T_OR || t2 == '|')
			return 1;
	case '|':
	case T_OR:
		if (t2 == '^')
			return 1;
	case '^':
		if (t2 == 0)
			return 1;
	case 0:
		return 0;
	}
	printf("[%dgt%d?]", t1, t2);
	return 0;
#endif
}

int print_type = 1;

static void print_sym(FILE *out, struct symbol *sym)
{
	fprintf(out, "%s", sym->name);
	if (print_type)
		fprintf(out, "%s",
			sym->type == SYMBOL_BOOLEAN ? "?" :
			sym->type == SYMBOL_TRISTATE ? "??" :
			sym->type == SYMBOL_HEX ? "h" :
			sym->type == SYMBOL_INT ? "i" :
			sym->type == SYMBOL_STRING ? "s" :
		       	"");
}

void fprint_expr(FILE *out, struct expr *e, int prevtoken)
{
	if (!e) {
		fprintf(out, "<none>");
		return;
	}

	switch (e->token) {
	case T_WORD:
		print_sym(out, e->left.sym);
		break;
	case '!':
		if (token_gt(prevtoken, '!'))
			fprintf(out, "(");
		fprintf(out, "!");
		fprint_expr(out, e->left.expr, '!');
		if (token_gt(prevtoken, '!'))
			fprintf(out, ")");
		break;
	case '=':
		if (token_gt(prevtoken, '='))
			fprintf(out, "(");
		print_sym(out, e->left.sym);
		fprintf(out, "=");
		print_sym(out, e->right.sym);
		if (token_gt(prevtoken, '='))
			fprintf(out, ")");
		break;
	case T_UNEQUAL:
		if (token_gt(prevtoken, T_UNEQUAL))
			fprintf(out, "(");
		print_sym(out, e->left.sym);
		fprintf(out, "!=");
		print_sym(out, e->right.sym);
		if (token_gt(prevtoken, T_UNEQUAL))
			fprintf(out, ")");
		break;
	case T_OR:
		if (token_gt(prevtoken, T_OR))
			fprintf(out, "(");
		fprint_expr(out, e->left.expr, T_OR);
		fprintf(out, " || ");
		fprint_expr(out, e->right.expr, T_OR);
		if (token_gt(prevtoken, T_OR))
			fprintf(out, ")");
		break;
	case T_AND:
		if (token_gt(prevtoken, T_AND))
			fprintf(out, "(");
		fprint_expr(out, e->left.expr, T_AND);
		fprintf(out, " && ");
		fprint_expr(out, e->right.expr, T_AND);
		if (token_gt(prevtoken, T_AND))
			fprintf(out, ")");
		break;
	case '|':
		if (token_gt(prevtoken, '|'))
			fprintf(out, "(");
		fprint_expr(out, e->left.expr, '|');
		fprintf(out, " || ");
		fprint_expr(out, e->right.expr, '|');
		if (token_gt(prevtoken, '|'))
			fprintf(out, ")");
		break;
	case '&':
		if (token_gt(prevtoken, '&'))
			fprintf(out, "(");
		fprint_expr(out, e->left.expr, '&');
		if (print_type)
			fprintf(out, " & ");
		else
			fprintf(out, " && ");
		fprint_expr(out, e->right.expr, '&');
		if (token_gt(prevtoken, '&'))
			fprintf(out, ")");
		break;
	case '^':
		if (e->left.expr) {
			fprint_expr(out, e->left.expr, '^');
			fprintf(out, " ^ ");
		}
		fprintf(out, "%s", e->right.sym->name);
		break;
	case T_IF:
		if (token_gt(prevtoken, T_IF))
			fprintf(out, "[");
		if (e->right.expr) {
			fprint_expr(out, e->right.expr, T_IF);
			fprintf(out, " && ");
		}
		fprint_expr(out, e->left.expr, T_IF);
		if (token_gt(prevtoken, T_IF))
			fprintf(out, "]");
		break;
	case T_ELSE:
		if (token_gt(prevtoken, T_ELSE))
			fprintf(out, "[");
		//fprintf(out, "[");
		if (e->right.expr) {
			fprint_expr(out, e->right.expr, T_ELSE);
			fprintf(out, " && ");
		}
		fprintf(out, "!");
		fprint_expr(out, e->left.expr, '!');
		if (token_gt(prevtoken, T_ELSE))
			fprintf(out, "]");
		break;
	default:
		fprintf(out, "<unknown token %d>", e->token);
		break;
	}
}

void print_expr(int mask, struct expr *e, int prevtoken)
{
	if (!(cdebug & mask))
		return;
	fprint_expr(stdout, e, prevtoken);
}

struct file *lookup_file(const char *name)
{
	struct file *file;

	for (file = file_list; file; file = file->next) {
		if (!strcmp(name, file->name))
			return file;
	}

	file = malloc(sizeof(*file));
	memset(file, 0, sizeof(*file));
	file->name = strdup(name);
	file->next = file_list;
	file_list = file;
	return file;
}
