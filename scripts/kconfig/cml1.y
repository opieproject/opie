%{
/*
 * Copyright (C) 2002 Roman Zippel <zippel@linux-m68k.org>
 * Released under the terms of the GNU GPL v2.0.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRINTD		0x0001
#define DEBUG_PARSE	0x0002
#define DEBUG_OPT	0x0004
#define DEBUG_OPT1	0x0008
#define DEBUG_OPT2	0x0010
#define DEBUG_OPT3	0x0020
#define DEBUG_OPT4	0x0040
#define DEBUG_OPT5	0x0080
#define DEBUG_DEP	0x0100

int cdebug = PRINTD;//DEBUG_OPT|-1;

#define YYERROR_VERBOSE
%}

%union
{
	char *string;
	struct expr *expr;
	struct symbol *symbol;
	struct statement *stmt;
}

%token <string> T_WORD
%token <string> T_WORD_QUOTE
%token <string> T_WORD_DQUOTE
%token T_MAINMENU_NAME
%token T_MAINMENU_OPTION
%token T_NEXT_COMMENT
%token T_ENDMENU
%token T_COMMENT
%token T_BOOL
%token T_HEX
%token T_INT
%token T_STRING
%token T_TRISTATE
%token T_DEFINE_BOOL
%token T_DEFINE_HEX
%token T_DEFINE_INT
%token T_DEFINE_STRING
%token T_DEFINE_TRISTATE
%token T_DEP_BOOL
%token T_DEP_MBOOL
%token T_DEP_TRISTATE
%token T_UNSET
%token T_CHOICE
%token T_SOURCE
%token T_IF
%token T_ELSE
%token T_FI
%token T_THEN
%token T_UNEQUAL
%token <string> T_SH_COMMENT

%type <string> prompt
%type <string> word
%type <string> menu_comment
%type <symbol> symbol
%type <symbol> symbol_ref
%type <symbol> symbol_other
%type <stmt> statement
%type <expr> expr
%type <expr> dep
%type <expr> deps
%type <stmt> comment_opt

%left T_OR
%left T_AND
%nonassoc '!'
%left '=' T_UNEQUAL

%{
#include "cml1.h"

extern int cml1lex(void);
static void cml1error(const char *err);
extern int lineno(void);
extern void scan_nextfile(char *file);

struct statement *create_stmt(int token);
struct statement *add_stmt(struct symbol *sym, int token,
			   char *prompt, struct symbol *def);
void print_expr(int mask, struct expr *e, int prevtoken);
void print_dep(int mask, struct statement *stmt);

void eliminate_eq(int token, struct expr **ep1, struct expr **ep2);
static inline int expr_is_yes(struct expr *e);
static inline int expr_is_no(struct expr *e);
struct expr *trans_dep_bool(struct expr *e);
struct expr *trans_bool_expr(struct expr *e);
struct expr *fixup_expr(struct expr *e);
struct expr *trans_expr(struct expr *e);
void extract_eq(int token, struct expr **ep, struct expr **ep1, struct expr **ep2);
struct expr *extract_eq_and(struct expr **ep1, struct expr **ep2);
struct expr *extract_eq_or(struct expr **ep1, struct expr **ep2);
struct expr *optimize_expr_or(struct expr *e);
struct expr *eliminate_dups(struct expr *e);
struct expr *eliminate_yn(struct expr *e);
void eliminate_dep(struct symbol *sym, struct expr *dep);
int contains_expr(struct expr *dep, struct expr *e);

#define printd(mask, fmt...) if (cdebug & (mask)) printf(fmt)
#define fprintd(f, mask, fmt...) if (cdebug & (mask)) fprintf(f, fmt)

static struct expr *if_expr;
static struct menu root_menu;
static struct menu *current_menu = &root_menu;

struct symbol *symbol_hash[257];
%}
%%
input:	line
;

line:	/* empty */
;

line:	line statement
;

line:	line '\n'
;

statement: T_SH_COMMENT
{
	$$ = create_stmt(T_SH_COMMENT);
	$$->text = $1;
};

statement: T_MAINMENU_NAME prompt '\n'
{
	$$ = create_stmt(T_MAINMENU_NAME);
	$$->text = $2;
	printd(DEBUG_PARSE, "%d:mainmenu_name: %s\n", lineno(), $2);
};

statement: T_COMMENT prompt '\n'
{
	$$ = create_stmt(T_COMMENT);
	$$->text = $2;
	$$->dep = trans_expr(fixup_expr(expr_copy(if_expr)));
	printd(DEBUG_PARSE, "%d:comment: %s\n", lineno(), $2);
};

statement: T_BOOL prompt symbol dep '\n'
{
	$$ = add_stmt($3, T_BOOL, $2, NULL);
	printd(DEBUG_PARSE, "%d:bool: %s %s\n", lineno(), $2, $3->name);
	if ($4) {
		printf("%s:%d: warning ignoring dependencies\n", current_file->name, current_file->lineno);
		expr_free($4);
	}
	print_dep(DEBUG_PARSE, $$);
};

statement: T_HEX prompt symbol symbol_other '\n'
{
	$$ = add_stmt($3, T_HEX, $2, NULL);
	add_stmt($3, T_DEFINE_HEX, NULL, $4);
	printd(DEBUG_PARSE, "%d:hex: %s %s %s\n", lineno(), $2, $3->name, $4->name);
	print_dep(DEBUG_PARSE, $$);
};

statement: T_INT prompt symbol symbol_other '\n'
{
	$$ = add_stmt($3, T_INT, $2, NULL);
	add_stmt($3, T_DEFINE_INT, NULL, $4);
	printd(DEBUG_PARSE, "%d:int: %s %s %s\n", lineno(), $2, $3->name, $4->name);
	print_dep(DEBUG_PARSE, $$);
};

statement: T_STRING prompt symbol symbol_other '\n'
{
	$$ = add_stmt($3, T_STRING, $2, NULL);
	add_stmt($3, T_DEFINE_STRING, NULL, $4);
	printd(DEBUG_PARSE, "%d:string: %s %s %s\n", lineno(), $2, $3->name, $4->name);
	print_dep(DEBUG_PARSE, $$);
};

statement: T_TRISTATE prompt symbol dep '\n'
{
	$$ = add_stmt($3, T_TRISTATE, $2, NULL);
	printd(DEBUG_PARSE, "%d:tristate: %s %s\n", lineno(), $2, $3->name);
	if ($4) {
		printf("%s:%d: warning ignoring dependencies\n", current_file->name, current_file->lineno);
		expr_free($4);
	}
	print_dep(DEBUG_PARSE, $$);
};

statement: T_DEFINE_BOOL symbol symbol_ref comment_opt '\n'
{
	$$ = add_stmt($2, T_DEFINE_BOOL, NULL, $3);
	printd(DEBUG_PARSE, "%d:define_bool: %s %s\n", lineno(), $2->name, $3->name);
	print_dep(DEBUG_PARSE, $$);
};

statement: T_DEFINE_HEX symbol symbol_other '\n'
{
	$$ = add_stmt($2, T_DEFINE_HEX, NULL, $3);
	printd(DEBUG_PARSE, "%d:define_hex: %s %s\n", lineno(), $2->name, $3->name);
	print_dep(DEBUG_PARSE, $$);
};

statement: T_DEFINE_INT symbol symbol_other comment_opt '\n'
{
	$$ = add_stmt($2, T_DEFINE_INT, NULL, $3);
	printd(DEBUG_PARSE, "%d:define_int: %s %s\n", lineno(), $2->name, $3->name);
	print_dep(DEBUG_PARSE, $$);
};

statement: T_DEFINE_STRING symbol symbol_other '\n'
{
	$$ = add_stmt($2, T_DEFINE_STRING, NULL, $3);
	printd(DEBUG_PARSE, "%d:define_string: %s %s\n", lineno(), $2->name, $3->name);
	print_dep(DEBUG_PARSE, $$);
};

statement: T_DEFINE_TRISTATE symbol symbol_ref '\n'
{
	$$ = add_stmt($2, T_DEFINE_TRISTATE, NULL, $3);
	printd(DEBUG_PARSE, "%d:define_tristate: %s %s\n", lineno(), $2->name, $3->name);
	print_dep(DEBUG_PARSE, $$);
};

statement: T_DEP_BOOL prompt symbol dep '\n'
{
	$$ = add_stmt($3, T_DEP_BOOL, $2, NULL);
	if (!($$->dep2 = $4))
		printf("%s:%d: warning dep_bool without dependencies\n", current_file->name, current_file->lineno);
	printd(DEBUG_PARSE, "%d:dep_bool: %s %s\n", lineno(), $2, $3->name);
	print_dep(DEBUG_PARSE, $$);
};

statement: T_DEP_MBOOL prompt symbol dep '\n'
{
	$$ = add_stmt($3, T_DEP_MBOOL, $2, NULL);
	if (!($$->dep2 = $4))
		printf("%s:%d: warning dep_mbool without dependencies\n", current_file->name, current_file->lineno);
	printd(DEBUG_PARSE, "%d:dep_mbool: %s %s\n", lineno(), $2, $3->name);
	print_dep(DEBUG_PARSE, $$);
};

statement: T_DEP_TRISTATE prompt symbol dep '\n'
{
	$$ = add_stmt($3, T_DEP_TRISTATE, $2, NULL);
	if (!($$->dep2 = $4))
		printf("%s:%d: warning dep_tristate without dependencies\n", current_file->name, current_file->lineno);
	printd(DEBUG_PARSE, "%d:dep_tristate: %s %s\n", lineno(), $2, $3->name);
	print_dep(DEBUG_PARSE, $$);
};

statement: T_UNSET deps '\n'
{
	//add_stmt($2, T_UNSET, NULL, NULL);
	printd(DEBUG_PARSE, "%d:unset\n", lineno());
	print_dep(DEBUG_PARSE, NULL);
};

statement: T_CHOICE prompt word word '\n'
{
	struct statement *stmt;
	struct symbol *sym;
	struct expr **depp;
	char *ptr, *name, *def, end;

	printd(DEBUG_PARSE, "%d:choice: %s %s %s\n", lineno(), $2, $3, $4);
	$$ = create_stmt('^');
	$$->text = $2;
	ptr = $3;
	depp = &$$->dep;
	do {
		while (isspace(*ptr))
			ptr++;
		if (!*ptr)
			break;
		def = ptr;
		while (*ptr && !isspace(*ptr))
			ptr++;
		*ptr++ = 0;
		//def = lookup_symbol(name, SYMBOL_OTHER);
		while (isspace(*ptr))
			ptr++;
		if (!*ptr)
			break;
		name = ptr;
		while (*ptr && !isspace(*ptr))
			ptr++;
		end = *ptr;
		*ptr++ = 0;
		sym = lookup_symbol(name, SYMBOL_BOOLEAN);
		if (!$$->def) {
			if (!strncmp(def, $4, strlen($4)))
				$$->def = sym;
		}
		sym->flags |= SYMBOL_CHOICE;
		stmt = add_stmt(sym, T_BOOL, strdup(def), NULL);
		if (!stmt->next) {
			stmt->next = $$;
		} else
			printf("warning: unexpected stmt for choice symbol %s\n", sym->name);
		*depp = expr_alloc_one('^', NULL);
		(*depp)->right.sym = sym;
		depp = &(*depp)->left.expr;
		printd(DEBUG_PARSE, "added symbol: %s %s\n", sym->name, stmt->text);
	} while (end);
	print_dep(DEBUG_PARSE, NULL);
};

statement: T_IF '[' expr ']' semi_or_nl T_THEN comment_opt '\n'
{
	struct expr *e;
	printd(DEBUG_PARSE, "%d:if ", lineno());
	print_expr(DEBUG_PARSE, $3, 0);
	printd(DEBUG_PARSE, "\n");
	e = expr_alloc_one(T_IF, $3);
	e->right.expr = if_expr;
	if_expr = e;
	$$ = NULL;
};

semi_or_nl: ';' | '\n'
;

statement: T_ELSE comment_opt '\n'
{
	printd(DEBUG_PARSE, "%d:else\n", lineno());
	if_expr->token = T_ELSE;
	$$ = NULL;
};

statement: T_FI comment_opt '\n'
{
	struct expr *e;
	printd(DEBUG_PARSE, "%d:fi\n", lineno());
	e = if_expr;
	if (e) {
		if_expr = e->right.expr;
		expr_free(e->left.expr);
		free(e);
	} else
		YYABORT;
	$$ = NULL;
};

statement: T_MAINMENU_OPTION T_NEXT_COMMENT menu_comment '\n'
{
	printd(DEBUG_PARSE, "%d:mainmenu_option: %s\n", lineno(), $3);
	$$ = create_stmt(T_MAINMENU_OPTION);
	$$->text = $3;
	$$->dep = trans_expr(fixup_expr(expr_copy(if_expr)));
	current_menu = $$->menu;
};

menu_comment: '\n' T_COMMENT prompt { $$ = $3; }
	    | '\n' menu_comment { $$ = $2; }
;

statement: T_ENDMENU '\n'
{
	$$ = create_stmt(T_ENDMENU);
	printd(DEBUG_PARSE, "%d:endmenu\n", lineno());
	current_menu = current_menu->parent;
};

statement: T_SOURCE word '\n'
{
	static char helppath[256];
	char *p;
	$$ = create_stmt(T_SOURCE);
	$$->text = $2;
	printd(DEBUG_PARSE, "%d:source: %s\n", lineno(), $2);
	strcpy(helppath, $2);
	p = strrchr(helppath, '.');
	if (p) {
		strcpy(p, ".help");
		helplex(helppath);
	}
	scan_nextfile($2);
};

comment_opt:	/* empty */	{ $$ = NULL; }
	|	T_SH_COMMENT
{
	$$ = create_stmt(T_SH_COMMENT);
	$$->text = $1;
};

prompt:   T_WORD_QUOTE
	| T_WORD_DQUOTE
;

word:	  T_WORD
	| T_WORD_QUOTE
	| T_WORD_DQUOTE
;

dep:	  /* empty */		{ $$ = NULL; }
	| deps			{ $$ = $1; }
;

deps:	  symbol_ref		{ $$ = expr_alloc_symbol($1); }
	| deps symbol_ref	{ $$ = expr_alloc_two('&', $1, expr_alloc_symbol($2)); }
;

symbol:	T_WORD			{ $$ = lookup_symbol($1, SYMBOL_UNKNOWN); free($1); }
;

expr:	  symbol_ref '=' symbol_ref		{ $$ = expr_alloc_comp('=', $1, $3); }
	| symbol_ref T_UNEQUAL symbol_ref	{ $$ = expr_alloc_comp(T_UNEQUAL, $1, $3); }
	| '!' expr		{ $$ = expr_alloc_one('!', $2); }
	| expr T_AND expr	{ $$ = expr_alloc_two(T_AND, $1, $3); }
	| expr T_OR expr	{ $$ = expr_alloc_two(T_OR, $1, $3); }
;

symbol_ref: T_WORD		{ $$ = lookup_symbol_ref($1); free($1); }
	  | T_WORD_QUOTE	{ $$ = lookup_symbol_ref($1); free($1); }
	  | T_WORD_DQUOTE	{ $$ = lookup_symbol_ref($1); free($1); }
;

symbol_other: T_WORD		{ $$ = lookup_symbol($1, SYMBOL_OTHER); free($1); }
	    | T_WORD_QUOTE	{ $$ = lookup_symbol($1, SYMBOL_OTHER); free($1); }
	    | T_WORD_DQUOTE	{ $$ = lookup_symbol($1, SYMBOL_OTHER); free($1); }
;

%%

static void yyerror(const char *err)
{
	printf("%s:%d: %s\n", current_file->name, current_file->lineno + 1, err);
	exit(1);
}

void print_dep(int mask, struct statement *stmt)
{
	printd(mask, "dep: ");
	if (stmt) {
		if (stmt->dep2)
			printd(mask, "[");
		print_expr(mask, stmt->dep, 0);
		if (stmt->dep2) {
			printd(mask, " & ");
			print_expr(mask, stmt->dep2, 0);
			printd(mask, "]");
		}
	} else
		print_expr(mask, if_expr, 0);
	printd(mask, "\n");
}

struct symbol *lookup_symbol(char *name, int type)
{
	struct symbol *symbol;
	char *ptr;
	int hash = 0;

	//printf("lookup: %s -> ", name);
	if (type != SYMBOL_OTHER && !strncmp(name, "CONFIG_", 7)) {
		name += 7;
	} else {
		//printf("Warning: Invalid symbol name '%s'\n", name);
		type = SYMBOL_OTHER;
	}

	for (ptr = name; *ptr; ptr++)
		hash += *ptr;

	for (symbol = symbol_hash[hash&255]; symbol; symbol = symbol->next) {
		if (!strcmp(symbol->name, name)) {
			//printf("h:%p\n", symbol);
			if (type == SYMBOL_BOOLEAN)
				symbol->type = SYMBOL_BOOLEAN;
			return symbol;
		}
	}

	symbol = malloc(sizeof(*symbol));
	memset(symbol, 0, sizeof(*symbol));
	symbol->name = strdup(name);
	symbol->type = type;

	symbol->next = symbol_hash[hash&255];
	symbol_hash[hash&255] = symbol;

	//printf("n:%p\n", symbol);
	return symbol;
}

struct symbol *lookup_symbol_ref(char *name)
{
	if (name[0] == 'y' && !name[1])
		return &symbol_yes;
	if (name[0] == 'm' && !name[1])
		return &symbol_mod;
	if (name[0] == 'n' && !name[1])
		return &symbol_no;
	if (name[0] == '$')
		return lookup_symbol(name+1, SYMBOL_UNKNOWN);
	return lookup_symbol(name, SYMBOL_OTHER);
}

struct statement *create_stmt(int token)
{
	struct statement *stmt;
	struct menu *menu, **menup;

	stmt = malloc(sizeof(*stmt));
	memset(stmt, 0, sizeof(*stmt));
	stmt->token = token;
	stmt->file = current_file;
	stmt->lineno = lineno();

	if (!current_file->stmt)
		current_file->stmt = stmt;
	if (current_file->last_stmt)
		current_file->last_stmt->next_pos = stmt;
	current_file->last_stmt = stmt;

	menu = malloc(sizeof(*menu));
	memset(menu, 0, sizeof(*menu));
	menu->prompt = stmt;
	menu->parent = current_menu;
	for (menup = &current_menu->list; *menup; menup = &(*menup)->next)
		;
	*menup = menu;

	stmt->menu = menu;

	return stmt;
}

struct statement *add_stmt(struct symbol *sym, int token, char *prompt, struct symbol *def)
{
	struct statement *stmt = create_stmt(token);

	stmt->dep = expr_copy(if_expr);
	stmt->text = prompt;
	stmt->def = def;

	stmt->sym = sym;
	stmt->next = sym->stmt;
	sym->stmt = stmt;

	return stmt;
}

void print_quoted_string(FILE *out, const char *str)
{
	const char *p;
	int len;

	putc('"', out);
	while ((p = strchr(str, '"'))) {
		len = p - str;
		if (len)
			fprintf(out, "%.*s", len, str);
		fputs("\\\"", out);
		str += len + 1;
	}
	fputs(str, out);
	putc('"', out);
}

const char *skip_space(const char *p)
{
	while (*p == ' ')
		p++;
	return p;
}

void do_print_symbol(FILE *out, struct symbol *sym)
{
	struct statement *zero_stmt, *stmt, *stmt2;
	struct expr *dep, *dep2;

#define VALID_STMT(s) (s->token && s->token != '^' && s->menu && s->menu->parent == current_menu)

	dep = NULL;
	for (stmt = sym->stmt; stmt; stmt = stmt->next) {
		if (!VALID_STMT(stmt))
			continue;
		if (!dep) {
			if (!stmt->dep)
				break;
			dep = stmt->dep;
			stmt->dep = NULL;
		} else {
			struct expr *e = dep;
			if (stmt->dep) {
				dep = extract_eq_and(&e, &stmt->dep);
				if (expr_is_yes(e)) {
					expr_free(e);
					continue;
				}
			} else
				dep = NULL;
			for (stmt2 = sym->stmt; stmt != stmt2; stmt2 = stmt2->next) {
				if (!VALID_STMT(stmt2))
					continue;
				stmt2->dep = !stmt2->dep ? expr_copy(e) :
					expr_alloc_two(T_AND, expr_copy(e), stmt2->dep);
			}
			expr_free(e);
			if (!dep)
				break;
		}
	}

	zero_stmt = NULL;
	for (stmt = sym->stmt; stmt; stmt = stmt->next) {
		if (!stmt->token)
			zero_stmt = stmt;
		if (VALID_STMT(stmt))
			break;
	}
	if (!stmt) {
		if (sym->flags & SYMBOL_PRINTED)
			return;
		if (!zero_stmt)
			return;
		for (stmt = sym->stmt; stmt; stmt = stmt->next) {
			if (stmt->token)
				return;
		}
		stmt = zero_stmt;
	}

	fprintf(out, "config %s\n", sym->name);

	switch (sym->type) {
	case SYMBOL_BOOLEAN: fputs("\tbool", out); break;
	case SYMBOL_TRISTATE: fputs("\ttristate", out); break;
	case SYMBOL_INT: fputs("\tint", out); break;
	case SYMBOL_HEX: fputs("\thex", out); break;
	case SYMBOL_STRING: fputs("\tstring", out); break;
	default: fputs("\t#unknown type?", out); break;
	}

	if (stmt->text) {
		fputc(' ', out);
		print_quoted_string(out, skip_space(stmt->text));
		if (!expr_is_yes(stmt->dep)) {
			fputs(" if ", out);
			fprint_expr(out, stmt->dep, 0);
		}
		stmt->menu = NULL;
		stmt = stmt->next;
	}
	fputc('\n', out);

	if (sym->dep || dep) {
		dep = expr_alloc_and(expr_copy(sym->dep), dep);
		dep = eliminate_yn(dep);
		if (current_menu->prompt && current_menu->prompt->dep) {
			dep2 = expr_copy(current_menu->prompt->dep);
			switch (sym->type) {
			case SYMBOL_BOOLEAN: case SYMBOL_INT: case SYMBOL_HEX: case SYMBOL_STRING:
				dep2 = trans_bool_expr(dep2);
				break;
			}
			eliminate_eq(T_AND, &dep, &dep2);
			dep = eliminate_yn(dep);
#if 0
			dep2 = eliminate_yn(dep2);
			if (!expr_is_yes(dep2)) {
				printf("Warning, I'm still confused (%s)?\n", sym->name);
				print_expr(PRINTD, dep, 0);
				printf(" - ");
				print_expr(PRINTD, dep2, 0);
				printf("\n");
			}
#endif
			expr_free(dep2);
		}
		if (!expr_is_yes(dep)) {
			fputs("\tdepends on ", out);
			fprint_expr(out, eliminate_yn(dep), 0);
			fputc('\n', out);
		}
	}
	for (; stmt; stmt = stmt->next) {
		if (!VALID_STMT(stmt))
			continue;
		if (stmt->text) {
			;
		} else if (stmt->def) {
			if (sym->flags & SYMBOL_CHOICE)
				printf("Choice value %s has default value!\n", sym->name);
			fputs( "\tdefault ", out);
			switch (sym->type) {
			case SYMBOL_BOOLEAN:
			case SYMBOL_TRISTATE:
				fputs(stmt->def->name, out);
				break;
			default:
				print_quoted_string(out, stmt->def->name);
				break;
			}
			if (!expr_is_yes(stmt->dep)) {
				fputs(" if ", out);
				fprint_expr(out, eliminate_yn(stmt->dep), 0);
			}
			fputc('\n', out);
			stmt->menu = NULL;
		}
	}
	if (sym->help && !(sym->flags & SYMBOL_PRINTED)) {
		int len = strlen(sym->help);
		char *p, *p2;
		int cnt;

		while (sym->help[--len] == '\n')
			sym->help[len] = 0;
		for (p = sym->help, cnt = 0; (p = strchr(p, '\n')); p++, cnt++)
			;
		if (cnt >= 9)
			fprintf(out, "\t---help---\n");
		else
			fprintf(out, "\thelp\n");
		for (p = sym->help;; p = p2 + 1) {
			while (*p == ' ')
				p++;
			if (*p == '\n') {
				fputc('\n', out);
				p2 = p;
				continue;
			}
			p2 = strchr(p, '\n');
			if (!p2)
				break;
			fprintf(out, "\t  %.*s\n", p2 - p, p);
		}
		fprintf(out, "\t  %s\n", p);
	}
	fputc('\n', out);
	for (stmt = sym->stmt; stmt; stmt = stmt->next) {
		if (!VALID_STMT(stmt))
			continue;
		if (stmt->text) {
			fprintf(out, "config %s\n", sym->name);
			fputs( "\tprompt ", out);
			print_quoted_string(out, skip_space(stmt->text));
			fputc('\n', out);
			dep2 = expr_alloc_and(expr_copy(dep), expr_copy(stmt->dep));
			dep2 = eliminate_dups(dep2);
			if (!expr_is_yes(dep2)) {
				fputs("\tdepends on ", out);
				fprint_expr(out, dep2, 0);
				fputc('\n', out);
			}
			expr_free(dep2);
			fputc('\n', out);
			stmt->menu = NULL;
		}
	}
	sym->flags |= SYMBOL_PRINTED;
	expr_free(dep);
}

void print_choice(FILE *out, struct symbol *sym)
{
	struct statement *stmt;
	struct expr *lst, *dep, *tmp, *tmp2;

	if (sym->flags & SYMBOL_PRINTED)
		return;

	for (stmt = sym->stmt; stmt; stmt = stmt->next)
		if (stmt->token == '^')
			break;
	if (!stmt)
		//warn?
		return;

	fputs("choice\n", out);
	if (stmt->text) {
		fputs("\tprompt ", out);
		print_quoted_string(out, skip_space(stmt->text));
		fputc('\n', out);
	}

	dep = NULL;
	for (lst = stmt->dep; lst; lst = lst->left.expr) {
		sym = lst->right.sym;
		if (sym == &symbol_no) {
			fputs("\toptional\n", out);
			continue;
		}
		if (!dep) {
			if (!sym->dep)
				break;
			dep = sym->dep;
			sym->dep = NULL;
		} else {
			tmp = dep;
			if (sym->dep) {
				dep = extract_eq_and(&tmp, &sym->dep);
				if (expr_is_yes(tmp)) {
					expr_free(tmp);
					continue;
				}
			} else
				dep = NULL;
			for (tmp2 = stmt->dep; tmp2 != lst; tmp2 = tmp2->left.expr) {
				sym = tmp2->right.sym;
				if (sym == &symbol_no)
					continue;
				sym->dep = !sym->dep ? expr_copy(tmp) :
					expr_alloc_two(T_AND, expr_copy(tmp), sym->dep);
				sym->dep = eliminate_yn(sym->dep);
			}
			expr_free(tmp);
			if (!dep)
				break;
		}
	}
	for (;lst; lst = lst->left.expr) {
		sym = lst->right.sym;
		if (sym == &symbol_no) {
			fputs("\toptional\n", out);
			continue;
		}
	}
	if (!expr_is_yes(dep)) {
		fputs("\tdepends on ", out);
		fprint_expr(out, dep, 0);
		fputc('\n', out);
	}
	if (stmt->def)
		fprintf(out, "\tdefault %s\n", stmt->def->name);

	fputc('\n', out);

	for (lst = stmt->dep; lst; lst = lst->left.expr) {
		sym = lst->right.sym;
		if (sym == &symbol_no)
			continue;
		do_print_symbol(out, sym);
		sym->flags |= SYMBOL_PRINTED;
	}

	fputs("endchoice\n\n", out);
}

void print_symbols(FILE *out, struct symbol *sym)
{
	//if (sym->flags & SYMBOL_PRINTED)
	//	return;
	//sym->flags |= SYMBOL_PRINTED;

	if (sym->flags & SYMBOL_CHOICE) {
		print_choice(out, sym);
		return;
	}
	do_print_symbol(out, sym);
}

char *gen_filename(const char *name)
{
	static char newname[128];
	char *p;

	strcpy(newname, name);
	p = strrchr(newname, '/');
	if (!p)
		p = newname;
	else
		p++;
#if 0
	p = strrchr(p, '.');
	if (!p)
		p = newname + strlen(newname);
	strcpy(p, ".new");
#else
#if 0
	memcpy(p, "Build", 5);
	memmove(p + 5, p + 6, strlen(p) - 5);
	p = strrchr(p, '.');
	if (!p)
		p = newname + strlen(newname);
	strcpy(p, ".conf");
#else
	memmove(p + 1, p, strlen(p) + 1);
	p[0] = 'K';
	p[1] = 'c';
	p = strrchr(p, '.');
	*p = 0;
#endif
#endif
	return newname;
}

void print_files(FILE *all_out, const char *name)
{
	struct file *file;
	FILE *out = all_out;
	struct symbol *sym;
	struct statement *stmt;
	struct expr *dep1, *dep2;
	char *p;

	print_type = 0;

	file = lookup_file(name);
	if (!file) {
		fprintf(stderr, "# file %s not found!\n\n", file->name);
		return;
	} else if (file->flags & FILE_PRINTED) {
		fprintf(stderr, "# file %s skipped\n\n", file->name);
		return;
	}

	if (!out) {
		p = gen_filename(file->name);
		strcat(p, ".lkc");
		out = fopen(p, "w");
		if (!out) {
			printf("unable to open %s for writing!\n", p);
			exit(1);
		}
	}

	if (all_out)
		fprintf(all_out, "# file %s\n\n", file->name);
	file->flags |= FILE_PRINTED;

	for (stmt = file->stmt; stmt; stmt = stmt->next_pos) {
		sym = stmt->sym;
		if (!sym) {
			switch (stmt->token) {
			case T_MAINMENU_NAME:
				fputs("\nmainmenu ", out);
				print_quoted_string(out, skip_space(stmt->text));
				fputs("\n\n", out);
				break;
			case T_COMMENT:
				fputs("comment ", out);
				print_quoted_string(out, skip_space(stmt->text));
				stmt->dep = eliminate_dups(trans_expr(stmt->dep));
				dep1 = expr_copy(stmt->dep);
				if (dep1 && current_menu->prompt && current_menu->prompt->dep) {
					dep2 = expr_copy(current_menu->prompt->dep);
					eliminate_eq(T_AND, &dep1, &dep2);
					dep1 = eliminate_yn(dep1);
					dep2 = eliminate_yn(dep2);
					if (!expr_is_yes(dep2)) {
						printf("Warning, I'm confused!\n");
					}
					expr_free(dep2);
				}
				if (!expr_is_yes(dep1)) {
					dep1 = trans_bool_expr(dep1);
					fputs("\n\tdepends on ", out);
					fprint_expr(out, dep1, 0);
				}
				expr_free(dep1);
				fputs("\n\n", out);
				break;
			case T_SH_COMMENT:
				fprintf(out, "%s\n", stmt->text);
				break;
			case T_MAINMENU_OPTION:
				fputs("\nmenu ", out);
				print_quoted_string(out, skip_space(stmt->text));
				stmt->dep = eliminate_dups(trans_expr(stmt->dep));
				dep1 = expr_copy(stmt->dep);
				if (dep1 && current_menu->prompt && current_menu->prompt->dep) {
					dep2 = expr_copy(current_menu->prompt->dep);
					eliminate_eq(T_AND, &dep1, &dep2);
					dep1 = eliminate_yn(dep1);
					dep2 = eliminate_yn(dep2);
					if (!expr_is_yes(dep2)) {
						printf("Warning, I'm confused!\n");
					}
					expr_free(dep2);
				}
				if (!expr_is_yes(dep1)) {
					fputs("\n\tdepends on ", out);
					fprint_expr(out, dep1, 0);
				}
				expr_free(dep1);
				fputs("\n\n", out);
				current_menu = stmt->menu;
				break;
			case T_ENDMENU:
				fputs("endmenu\n\n", out);
				current_menu = current_menu->parent;
				break;
			case T_SOURCE:
				fprintf(out, "%ssource ", all_out ? "#" : "");
				print_quoted_string(out, gen_filename(stmt->text));
				fputs("\n\n", out);
				print_files(all_out, stmt->text);
				break;
			}
		} else
			print_symbols(out, sym);
	}

	if (all_out)
		fprintf(out, "# fileend %s\n\n", file->name);
}

static int trans_count;

void eliminate_eq(int token, struct expr **ep1, struct expr **ep2)
{
#define e1 (*ep1)
#define e2 (*ep2)

	if (e1->token == token) {
		eliminate_eq(token, &e1->left.expr, &e2);
		eliminate_eq(token, &e1->right.expr, &e2);
		return;
	}
	if (e2->token == token) {
		eliminate_eq(token, &e1, &e2->left.expr);
		eliminate_eq(token, &e1, &e2->right.expr);
		return;
	}
	if (e1->token == T_WORD && e2->token == T_WORD &&
	    e1->left.sym == e2->left.sym && (e1->left.sym->flags & (SYMBOL_YES|SYMBOL_NO)))
		return;
	if (!expr_eq(e1, e2))
		return;
	trans_count++;
	expr_free(e1); expr_free(e2);
	switch (token) {
	case T_OR:
		e1 = expr_alloc_symbol(&symbol_no);
		e2 = expr_alloc_symbol(&symbol_no);
		break;
	case T_AND:
		e1 = expr_alloc_symbol(&symbol_yes);
		e2 = expr_alloc_symbol(&symbol_yes);
		break;
	}
#undef e1
#undef e2
}

int expr_eq(struct expr *e1, struct expr *e2)
{
	int res, old_count;

	if (e1->token != e2->token)
		return 0;
	switch (e1->token) {
	case '=':
	case T_UNEQUAL:
		return e1->left.sym == e2->left.sym && e1->right.sym == e2->right.sym;
	case T_WORD:
		return e1->left.sym == e2->left.sym;
	case '!':
		return expr_eq(e1->left.expr, e2->left.expr);
	case T_AND:
	case T_OR:
		e1 = expr_copy(e1);
		e2 = expr_copy(e2);
		old_count = trans_count;
		eliminate_eq(e1->token, &e1, &e2);
		e1 = eliminate_yn(e1);
		e2 = eliminate_yn(e2);
		res = (e1->token == T_WORD &&
		       e2->token == T_WORD &&
		       e1->left.sym == e2->left.sym);
		expr_free(e1);
		expr_free(e2);
		trans_count = old_count;
		return res;
	}
	print_expr(PRINTD, e1, 0);
	printf(" = ");
	print_expr(PRINTD, e2, 0);
	printf(" ?\n");
	return 0;
}

/*
 * dep_bool FOO -> FOO=y
 */
struct expr *trans_dep_bool(struct expr *e)
{
	switch (e->token) {
	case '&':
	case T_AND:
		e->left.expr = trans_dep_bool(e->left.expr);
		e->right.expr = trans_dep_bool(e->right.expr);
		break;
	case T_WORD:
		if (e->left.sym->type == SYMBOL_TRISTATE) {
			e->token = '=';
			e->right.sym = &symbol_yes;
		}
		break;
	}
	return e;
}

/*
 * bool FOO!=n => FOO
 */
struct expr *trans_bool_expr(struct expr *e)
{
	if (!e)
		return NULL;
	switch (e->token) {
	case T_AND:
	case T_OR:
	case '!':
		e->left.expr = trans_bool_expr(e->left.expr);
		e->right.expr = trans_bool_expr(e->right.expr);
		break;
	case T_UNEQUAL:
		// FOO!=n -> FOO
		if (e->left.sym->type == SYMBOL_TRISTATE) {
			if (e->right.sym == &symbol_no) {
				e->token = T_WORD;
				e->right.sym = NULL;
			}
		}
		break;
	}
	return e;
}

struct expr *fixup_expr(struct expr *e)
{
	struct expr *tmp;

	if (!e)
		return NULL;

	switch (e->token) {
	case T_IF:
		// if expr if expr ... -> expr && expr ...
		print_expr(DEBUG_OPT1, e, 0);
		printd(DEBUG_OPT1, " -> ");
		tmp = e->left.expr;
		if (!e->right.expr) {
			free(e);
			e = fixup_expr(tmp);
			print_expr(DEBUG_OPT1, e, 0);
			printd(DEBUG_OPT1, "\n");
			return e;
		}
		e->left.expr = e->right.expr;
		e->right.expr = tmp;
		e->token = T_AND;
		print_expr(DEBUG_OPT1, e, 0);
		printd(DEBUG_OPT1, "\n");
		break;
	case T_ELSE:
		// if expr else expr ... -> expr && !expr ...
		print_expr(DEBUG_OPT1, e, 0);
		printd(DEBUG_OPT1, " -> ");
		e->token = '!';
		if (!e->right.expr) {
			print_expr(DEBUG_OPT1, e, 0);
			printd(DEBUG_OPT1, "\n");
			break;
		}
		tmp = e;
		e = expr_alloc_two(T_AND, e->right.expr, e);
		tmp->right.expr = NULL;
		print_expr(DEBUG_OPT1, e, 0);
		printd(DEBUG_OPT1, "\n");
		break;
	case '&':
		// expr & expr -> expr && expr
		print_expr(DEBUG_OPT1, e, 0);
		printd(DEBUG_OPT1, " -> ");
		e->token = T_AND;
		print_expr(DEBUG_OPT1, e, 0);
		printd(DEBUG_OPT1, "\n");
		break;
	case '=':
	case T_UNEQUAL:
		if (!(e->left.sym->flags & SYMBOL_CONST))
			return e;
		if (e->right.sym->flags & SYMBOL_CONST) {
			if (e->left.sym == e->right.sym) {
				e->left.sym = e->token == '=' ? &symbol_yes : &symbol_no;
			} else {
				e->left.sym = e->token != '=' ? &symbol_yes : &symbol_no;
			}
			e->token = T_WORD;
			e->right.sym = NULL;
		} else {
			// y/n/m = sym -> sym = y/n/m
			struct symbol *sym;
			print_expr(DEBUG_OPT1, e, 0);
			printd(DEBUG_OPT1, " -> ");
			sym = e->left.sym;
			e->left.sym = e->right.sym;
			e->right.sym = sym;
			print_expr(DEBUG_OPT1, e, 0);
			printd(DEBUG_OPT1, "\n");
		}
		return e;
	case T_WORD:
	case '^':
		return e;
	}
	e->left.expr = fixup_expr(e->left.expr);
	e->right.expr = fixup_expr(e->right.expr);
	return e;
}

struct expr *join_or(struct expr *e1, struct expr *e2)
{
	struct expr *tmp;
	struct symbol *sym1, *sym2;

	if (expr_eq(e1, e2))
		return expr_copy(e1);
	if (e1->token != '=' && e1->token != T_UNEQUAL && e1->token != T_WORD && e1->token != '!')
		return NULL;
	if (e2->token != '=' && e2->token != T_UNEQUAL && e2->token != T_WORD && e2->token != '!')
		return NULL;
	if (e1->token == '!') {
		tmp = e1->left.expr;
		if (tmp->token != '=' && tmp->token != T_UNEQUAL && tmp->token != T_WORD)
			return NULL;
		sym1 = tmp->left.sym;
	} else
		sym1 = e1->left.sym;
	if (e2->token == '!') {
		if (e2->left.expr->token != T_WORD)
			return NULL;
		sym2 = e2->left.expr->left.sym;
	} else
		sym2 = e2->left.sym;
	if (sym1 != sym2)
		return NULL;
	if (sym1->type != SYMBOL_BOOLEAN && sym1->type != SYMBOL_TRISTATE)
		return NULL;
	if (sym1->type == SYMBOL_TRISTATE) {
		if (e1->token == '=' && e2->token == '=' &&
		    ((e1->right.sym == &symbol_yes && e2->right.sym == &symbol_mod) ||
		     (e1->right.sym == &symbol_mod && e2->right.sym == &symbol_yes))) {
			// (a='y') || (a='m') -> (a!='n')
			return expr_alloc_comp(T_UNEQUAL, sym1, &symbol_no);
		}
		if (e1->token == '=' && e2->token == '=' &&
		    ((e1->right.sym == &symbol_yes && e2->right.sym == &symbol_no) ||
		     (e1->right.sym == &symbol_no && e2->right.sym == &symbol_yes))) {
			// (a='y') || (a='n') -> (a!='m')
			return expr_alloc_comp(T_UNEQUAL, sym1, &symbol_mod);
		}
		if (e1->token == '=' && e2->token == '=' &&
		    ((e1->right.sym == &symbol_mod && e2->right.sym == &symbol_no) ||
		     (e1->right.sym == &symbol_no && e2->right.sym == &symbol_mod))) {
			// (a='m') || (a='n') -> (a!='y')
			return expr_alloc_comp(T_UNEQUAL, sym1, &symbol_yes);
		}
	}
	if (sym1->type == SYMBOL_BOOLEAN && sym1 == sym2) {
		if ((e1->token == '!' && e1->left.expr->token == T_WORD && e2->token == T_WORD) ||
		    (e2->token == '!' && e2->left.expr->token == T_WORD && e1->token == T_WORD))
			return expr_alloc_symbol(&symbol_yes);
	}

	printf("optimize ");
	print_expr(PRINTD, e1, 0);
	printf(" || ");
	print_expr(PRINTD, e2, 0);
	printf(" ?\n");
	return NULL;
}

struct expr *join_and(struct expr *e1, struct expr *e2)
{
	struct expr *tmp;
	struct symbol *sym1, *sym2;

	if (expr_eq(e1, e2))
		return expr_copy(e1);
	if (e1->token != '=' && e1->token != T_UNEQUAL && e1->token != T_WORD && e1->token != '!')
		return NULL;
	if (e2->token != '=' && e2->token != T_UNEQUAL && e2->token != T_WORD && e2->token != '!')
		return NULL;
	if (e1->token == '!') {
		tmp = e1->left.expr;
		if (tmp->token != '=' && tmp->token != T_UNEQUAL && tmp->token != T_WORD)
			return NULL;
		sym1 = tmp->left.sym;
	} else
		sym1 = e1->left.sym;
	if (e2->token == '!') {
		if (e2->left.expr->token != T_WORD)
			return NULL;
		sym2 = e2->left.expr->left.sym;
	} else
		sym2 = e2->left.sym;
	if (sym1 != sym2)
		return NULL;
	if (sym1->type != SYMBOL_BOOLEAN && sym1->type != SYMBOL_TRISTATE)
		return NULL;

	if ((e1->token == T_WORD && e2->token == '=' && e2->right.sym == &symbol_yes) ||
	    (e2->token == T_WORD && e1->token == '=' && e1->right.sym == &symbol_yes))
		// (a) && (a='y') -> (a='y')
		return expr_alloc_comp('=', sym1, &symbol_yes);

	if ((e1->token == T_WORD && e2->token == T_UNEQUAL && e2->right.sym == &symbol_no) ||
	    (e2->token == T_WORD && e1->token == T_UNEQUAL && e1->right.sym == &symbol_no))
		// (a) && (a!='n') -> (a)
		return expr_alloc_symbol(sym1);

	if (sym1->type == SYMBOL_TRISTATE) {
		if (e1->token == '=' && e2->token == T_UNEQUAL) {
			// (a='b') && (a!='c') -> 'b'='c' ? 'n' : a='b'
			sym2 = e1->right.sym;
			if ((e2->right.sym->flags & SYMBOL_CONST) && (sym2->flags & SYMBOL_CONST))
				return sym2 != e2->right.sym ? expr_alloc_comp('=', sym1, sym2)
							     : expr_alloc_symbol(&symbol_no);
		}
		if (e1->token == T_UNEQUAL && e2->token == '=') {
			// (a='b') && (a!='c') -> 'b'='c' ? 'n' : a='b'
			sym2 = e2->right.sym;
			if ((e1->right.sym->flags & SYMBOL_CONST) && (sym2->flags & SYMBOL_CONST))
				return sym2 != e1->right.sym ? expr_alloc_comp('=', sym1, sym2)
							     : expr_alloc_symbol(&symbol_no);
		}
		if (e1->token == T_UNEQUAL && e2->token == T_UNEQUAL &&
			   ((e1->right.sym == &symbol_yes && e2->right.sym == &symbol_no) ||
			    (e1->right.sym == &symbol_no && e2->right.sym == &symbol_yes)))
			// (a!='y') && (a!='n') -> (a='m')
			return expr_alloc_comp('=', sym1, &symbol_mod);

		if (e1->token == T_UNEQUAL && e2->token == T_UNEQUAL &&
			   ((e1->right.sym == &symbol_yes && e2->right.sym == &symbol_mod) ||
			    (e1->right.sym == &symbol_mod && e2->right.sym == &symbol_yes)))
			// (a!='y') && (a!='m') -> (a='n')
			return expr_alloc_comp('=', sym1, &symbol_no);

		if (e1->token == T_UNEQUAL && e2->token == T_UNEQUAL &&
			   ((e1->right.sym == &symbol_mod && e2->right.sym == &symbol_no) ||
			    (e1->right.sym == &symbol_no && e2->right.sym == &symbol_mod)))
			// (a!='m') && (a!='n') -> (a='m')
			return expr_alloc_comp('=', sym1, &symbol_yes);

		if ((e1->token == T_WORD && e2->token == '=' && e2->right.sym == &symbol_mod) ||
		    (e2->token == T_WORD && e1->token == '=' && e1->right.sym == &symbol_mod) ||
		    (e1->token == T_WORD && e2->token == T_UNEQUAL && e2->right.sym == &symbol_yes) ||
		    (e2->token == T_WORD && e1->token == T_UNEQUAL && e1->right.sym == &symbol_yes))
			return NULL;
	}
	printf("optimize ");
	print_expr(PRINTD, e1, 0);
	printf(" && ");
	print_expr(PRINTD, e2, 0);
	printf(" ?\n");
	return NULL;
}

void eliminate_dups1(int token, struct expr **ep1, struct expr **ep2)
{
#define e1 (*ep1)
#define e2 (*ep2)
	struct expr *tmp;

	if (e1->token == token) {
		eliminate_dups1(token, &e1->left.expr, &e2);
		eliminate_dups1(token, &e1->right.expr, &e2);
		return;
	}
	if (e2->token == token) {
		eliminate_dups1(token, &e1, &e2->left.expr);
		eliminate_dups1(token, &e1, &e2->right.expr);
		return;
	}
	if (e1 == e2)
		return;

	switch (e1->token) {
	case T_OR: case T_AND:
		eliminate_dups1(e1->token, &e1, &e1);
	}

	switch (token) {
	case T_OR:
		tmp = join_or(e1, e2);
		if (tmp) {
			expr_free(e1); expr_free(e2);
			e1 = expr_alloc_symbol(&symbol_no);
			e2 = tmp;
			trans_count++;
		}
		break;
	case T_AND:
		tmp = join_and(e1, e2);
		if (tmp) {
			expr_free(e1); expr_free(e2);
			e1 = expr_alloc_symbol(&symbol_yes);
			e2 = tmp;
			trans_count++;
		}
		break;
	}
#undef e1
#undef e2
}

void eliminate_dups2(int token, struct expr **ep1, struct expr **ep2)
{
#define e1 (*ep1)
#define e2 (*ep2)
	struct expr *tmp, *tmp1, *tmp2;

	if (e1->token == token) {
		eliminate_dups2(token, &e1->left.expr, &e2);
		eliminate_dups2(token, &e1->right.expr, &e2);
		return;
	}
	if (e2->token == token) {
		eliminate_dups2(token, &e1, &e2->left.expr);
		eliminate_dups2(token, &e1, &e2->right.expr);
	}
	if (e1 == e2)
		return;

	switch (e1->token) {
	case T_OR:
		eliminate_dups2(e1->token, &e1, &e1);
		// (FOO || BAR) && (!FOO && !BAR) -> n
		tmp1 = trans_expr(expr_alloc_one('!', expr_copy(e1)));
		tmp2 = expr_copy(e2);
		tmp = extract_eq_and(&tmp1, &tmp2);
		if (expr_is_yes(tmp1)) {
			expr_free(e1);
			e1 = expr_alloc_symbol(&symbol_no);
			trans_count++;
		}
		expr_free(tmp2);
		expr_free(tmp1);
		expr_free(tmp);
		break;
	case T_AND:
		eliminate_dups2(e1->token, &e1, &e1);
		// (FOO && BAR) || (!FOO || !BAR) -> y
		tmp1 = trans_expr(expr_alloc_one('!', expr_copy(e1)));
		tmp2 = expr_copy(e2);
		tmp = extract_eq_or(&tmp1, &tmp2);
		if (expr_is_no(tmp1)) {
			expr_free(e1);
			e1 = expr_alloc_symbol(&symbol_yes);
			trans_count++;
		}
		expr_free(tmp2);
		expr_free(tmp1);
		expr_free(tmp);
		break;
	}
#undef e1
#undef e2
}

struct expr *eliminate_dups(struct expr *e)
{
	int oldcount;
	if (!e)
		return e;

	oldcount = trans_count;
	while (1) {
		trans_count = 0;
		switch (e->token) {
		case T_OR: case T_AND:
			eliminate_dups1(e->token, &e, &e);
			eliminate_dups2(e->token, &e, &e);
		}
		if (!trans_count)
			break;
		e = eliminate_yn(e);
	}
	trans_count = oldcount;
	return e;
}

struct expr *eliminate_yn(struct expr *e)
{
	struct expr *tmp;

	if (e) switch (e->token) {
	case T_AND:
		e->left.expr = eliminate_yn(e->left.expr);
		e->right.expr = eliminate_yn(e->right.expr);
		if (e->left.expr->token == T_WORD) {
			if (e->left.expr->left.sym == &symbol_no) {
				expr_free(e->left.expr);
				expr_free(e->right.expr);
				e->token = T_WORD;
				e->left.sym = &symbol_no;
				e->right.expr = NULL;
				return e;
			} else if (e->left.expr->left.sym == &symbol_yes) {
				free(e->left.expr);
				tmp = e->right.expr;
				*e = *(e->right.expr);
				free(tmp);
				return e;
			}
		}
		if (e->right.expr->token == T_WORD) {
			if (e->right.expr->left.sym == &symbol_no) {
				expr_free(e->left.expr);
				expr_free(e->right.expr);
				e->token = T_WORD;
				e->left.sym = &symbol_no;
				e->right.expr = NULL;
				return e;
			} else if (e->right.expr->left.sym == &symbol_yes) {
				free(e->right.expr);
				tmp = e->left.expr;
				*e = *(e->left.expr);
				free(tmp);
				return e;
			}
		}
		break;
	case T_OR:
		e->left.expr = eliminate_yn(e->left.expr);
		e->right.expr = eliminate_yn(e->right.expr);
		if (e->left.expr->token == T_WORD) {
			if (e->left.expr->left.sym == &symbol_no) {
				free(e->left.expr);
				tmp = e->right.expr;
				*e = *(e->right.expr);
				free(tmp);
				return e;
			} else if (e->left.expr->left.sym == &symbol_yes) {
				expr_free(e->left.expr);
				expr_free(e->right.expr);
				e->token = T_WORD;
				e->left.sym = &symbol_yes;
				e->right.expr = NULL;
				return e;
			}
		}
		if (e->right.expr->token == T_WORD) {
			if (e->right.expr->left.sym == &symbol_no) {
				free(e->right.expr);
				tmp = e->left.expr;
				*e = *(e->left.expr);
				free(tmp);
				return e;
			} else if (e->right.expr->left.sym == &symbol_yes) {
				expr_free(e->left.expr);
				expr_free(e->right.expr);
				e->token = T_WORD;
				e->left.sym = &symbol_yes;
				e->right.expr = NULL;
				return e;
			}
		}
		break;
	}
	return e;
}

struct expr *trans_expr(struct expr *e)
{
	struct expr *tmp;

	if (!e)
		return NULL;
	switch (e->token) {
	case '=':
	case T_UNEQUAL:
	case T_WORD:
	case '^':
		break;
	default:
		e->left.expr = trans_expr(e->left.expr);
		e->right.expr = trans_expr(e->right.expr);
	}

	switch (e->token) {
	case '=':
		if (e->left.sym->type != SYMBOL_BOOLEAN)
			break;
		if (e->right.sym == &symbol_no) {
			print_expr(DEBUG_OPT2, e, 0);
			printd(DEBUG_OPT2, " -> ");
			e->token = '!';
			e->left.expr = expr_alloc_symbol(e->left.sym);
			e->right.sym = NULL;
			print_expr(DEBUG_OPT2, e, 0);
			printd(DEBUG_OPT2, "\n");
			break;
		}
		if (e->right.sym == &symbol_mod) {
			printf("boolean symbol %s tested for 'm'? test forced to 'n'\n", e->left.sym->name);
			e->token = T_WORD;
			e->left.sym = &symbol_no;
			e->right.sym = NULL;
			break;
		}
		if (e->right.sym == &symbol_yes) {
			print_expr(DEBUG_OPT2, e, 0);
			printd(DEBUG_OPT2, " -> ");
			e->token = T_WORD;
			e->right.sym = NULL;
			print_expr(DEBUG_OPT2, e, 0);
			printd(DEBUG_OPT2, "\n");
			break;
		}
		break;
	case T_UNEQUAL:
		if (e->left.sym->type != SYMBOL_BOOLEAN)
			break;
		if (e->right.sym == &symbol_no) {
			print_expr(DEBUG_OPT2, e, 0);
			printd(DEBUG_OPT2, " -> ");
			e->token = T_WORD;
			e->right.sym = NULL;
			print_expr(DEBUG_OPT2, e, 0);
			printd(DEBUG_OPT2, "\n");
			break;
		}
		if (e->right.sym == &symbol_mod) {
			printf("boolean symbol %s tested for 'm'? test forced to 'y'\n", e->left.sym->name);
			e->token = T_WORD;
			e->left.sym = &symbol_yes;
			e->right.sym = NULL;
			break;
		}
		if (e->right.sym == &symbol_yes) {
			print_expr(DEBUG_OPT2, e, 0);
			printd(DEBUG_OPT2, " -> ");
			e->token = '!';
			e->left.expr = expr_alloc_symbol(e->left.sym);
			e->right.sym = NULL;
			print_expr(DEBUG_OPT2, e, 0);
			printd(DEBUG_OPT2, "\n");
			break;
		}
		break;
	case '!':
		switch (e->left.expr->token) {
		case '!':
			// !!a -> a
			print_expr(DEBUG_OPT2, e, 0);
			printd(DEBUG_OPT2, " -> ");
			tmp = e->left.expr->left.expr;
			free(e->left.expr);
			free(e);
			e = tmp;
			print_expr(DEBUG_OPT2, e, 0);
			printd(DEBUG_OPT2, "\n");
			e = trans_expr(e);
			break;
		case '=':
		case T_UNEQUAL:
			// !a='x' -> a!='x'
			print_expr(DEBUG_OPT2, e, 0);
			printd(DEBUG_OPT2, " -> ");
			tmp = e->left.expr;
			free(e);
			e = tmp;
			if (e->token == '=')
				e->token = T_UNEQUAL;
			else
				e->token = '=';
			print_expr(DEBUG_OPT2, e, 0);
			printd(DEBUG_OPT2, "\n");
			break;
		case T_OR:
			// !(a || b) -> !a && !b
			print_expr(DEBUG_OPT2, e, 0);
			printd(DEBUG_OPT2, " -> ");
			tmp = e->left.expr;
			e->token = T_AND;
			e->right.expr = expr_alloc_one('!', tmp->right.expr);
			tmp->token = '!';
			tmp->right.expr = NULL;
			print_expr(DEBUG_OPT2, e, 0);
			printd(DEBUG_OPT2, "\n");
			e = trans_expr(e);
			break;
		case T_AND:
			// !(a && b) -> !a || !b
			print_expr(DEBUG_OPT2, e, 0);
			printd(DEBUG_OPT2, " -> ");
			tmp = e->left.expr;
			e->token = T_OR;
			e->right.expr = expr_alloc_one('!', tmp->right.expr);
			tmp->token = '!';
			tmp->right.expr = NULL;
			print_expr(DEBUG_OPT2, e, 0);
			printd(DEBUG_OPT2, "\n");
			e = trans_expr(e);
			break;
		case T_WORD:
			if (e->left.expr->left.sym == &symbol_yes) {
				// !'y' -> 'n'
				print_expr(DEBUG_OPT2, e, 0);
				printd(DEBUG_OPT2, " -> ");
				tmp = e->left.expr;
				free(e);
				e = tmp;
				e->token = T_WORD;
				e->left.sym = &symbol_no;
				print_expr(DEBUG_OPT2, e, 0);
				printd(DEBUG_OPT2, "\n");
				break;
			}
			if (e->left.expr->left.sym == &symbol_no) {
				// !'n' -> 'y'
				print_expr(DEBUG_OPT2, e, 0);
				printd(DEBUG_OPT2, " -> ");
				tmp = e->left.expr;
				free(e);
				e = tmp;
				e->token = T_WORD;
				e->left.sym = &symbol_yes;
				print_expr(DEBUG_OPT2, e, 0);
				printd(DEBUG_OPT2, "\n");
				break;
			}
			break;
		}
		break;
	}
	return e;
}

struct expr *extract_eq_and(struct expr **ep1, struct expr **ep2)
{
	struct expr *tmp = NULL;
	extract_eq(T_AND, &tmp, ep1, ep2);
	if (tmp) {
		*ep1 = eliminate_yn(*ep1);
		*ep2 = eliminate_yn(*ep2);
	}
	return tmp;
}

struct expr *extract_eq_or(struct expr **ep1, struct expr **ep2)
{
	struct expr *tmp = NULL;
	extract_eq(T_OR, &tmp, ep1, ep2);
	if (tmp) {
		*ep1 = eliminate_yn(*ep1);
		*ep2 = eliminate_yn(*ep2);
	}
	return tmp;
}

void extract_eq(int token, struct expr **ep, struct expr **ep1, struct expr **ep2)
{
#define e1 (*ep1)
#define e2 (*ep2)
	if (e1->token == token) {
		extract_eq(token, ep, &e1->left.expr, &e2);
		extract_eq(token, ep, &e1->right.expr, &e2);
		return;
	}
	if (e2->token == token) {
		extract_eq(token, ep, ep1, &e2->left.expr);
		extract_eq(token, ep, ep1, &e2->right.expr);
		return;
	}
	if (expr_eq(e1, e2)) {
		*ep = *ep ? expr_alloc_two(token, *ep, e1) : e1;
		expr_free(e2);
		if (token == T_AND) {
			e1 = expr_alloc_symbol(&symbol_yes);
			e2 = expr_alloc_symbol(&symbol_yes);
		} else if (token == T_OR) {
			e1 = expr_alloc_symbol(&symbol_no);
			e2 = expr_alloc_symbol(&symbol_no);
		}
	}
#undef e1
#undef e2
}

struct expr *optimize_expr_or(struct expr *e)
{
	struct expr *tmp;

	if (!e || e->token != T_OR)
		return e;

	e->left.expr = optimize_expr_or(e->left.expr);
	e->right.expr = optimize_expr_or(e->right.expr);

	tmp = extract_eq_and(&e->left.expr, &e->right.expr);
	if (!tmp)
		return e;
	return expr_alloc_two(T_AND, tmp, e);
}

int contains_expr(struct expr *dep, struct expr *e)
{
	if (!dep)
		return 0;
	if (dep->token == T_AND)
		return contains_expr(dep->left.expr, e) || contains_expr(dep->right.expr, e);
	return expr_eq(dep, e);
}

struct expr *trans_comp_expr(struct expr *e, int token, struct symbol *sym)
{
	struct expr *e1, *e2;

	switch (e->token) {
	case T_AND:
		e1 = trans_comp_expr(e->left.expr, '=', sym);
		e2 = trans_comp_expr(e->right.expr, '=', sym);
		if (sym == &symbol_yes)
			e = expr_alloc_two(T_AND, e1, e2);
		if (sym == &symbol_no)
			e = expr_alloc_two(T_OR, e1, e2);
		if (token == T_UNEQUAL)
			e = expr_alloc_one('!', e);
		return e;
	case T_OR:
		e1 = trans_comp_expr(e->left.expr, '=', sym);
		e2 = trans_comp_expr(e->right.expr, '=', sym);
		if (sym == &symbol_yes)
			e = expr_alloc_two(T_OR, e1, e2);
		if (sym == &symbol_no)
			e = expr_alloc_two(T_AND, e1, e2);
		if (token == T_UNEQUAL)
			e = expr_alloc_one('!', e);
		return e;
	case '!':
		switch (token) {
		case '=':
			return trans_comp_expr(e->left.expr, T_UNEQUAL, sym);
		case T_UNEQUAL:
			return trans_comp_expr(e->left.expr, '=', sym);
		}
		break;
	case T_UNEQUAL:
	case '=':
		if (token == '=') {
			if (sym == &symbol_yes)
				return expr_copy(e);
			if (sym == &symbol_mod)
				return expr_alloc_symbol(&symbol_no);
			if (sym == &symbol_no)
				return expr_alloc_one('!', expr_copy(e));
		} else {
			if (sym == &symbol_yes)
				return expr_alloc_one('!', expr_copy(e));
			if (sym == &symbol_mod)
				return expr_alloc_symbol(&symbol_yes);
			if (sym == &symbol_no)
				return expr_copy(e);
		}
		break;
	case T_WORD:
		return expr_alloc_comp(token, e->left.sym, sym);
	}
	return NULL;
}

void eliminate_dep(struct symbol *sym, struct expr *dep)
{
	struct symbol *sym2;
	struct expr *dep2;

	if (dep->token == T_AND) {
		eliminate_dep(sym, dep->left.expr);
		eliminate_dep(sym, dep->right.expr);
	}
#if 0
	if (dep->token == T_WORD ||
	    (sym->type == SYMBOL_BOOLEAN &&
	     ((dep->token == T_UNEQUAL && dep->right.sym == &symbol_no) ||
	      (dep->token == '=' && dep->right.sym == &symbol_yes)))) {
		sym2 = dep->left.sym;
		if (sym2->dep) {
			if (!sym2->dep2)
				eliminate_dep(sym2, sym2->dep);
			sym->dep2 = !sym->dep2 ? expr_copy(sym2->dep2)
				: expr_alloc_two(T_AND, expr_copy(sym2->dep2), sym->dep2);
		}
	}
#else
	switch (dep->token) {
	case T_WORD:
	case T_UNEQUAL:
	case '=':
		sym2 = dep->left.sym;
		if (sym2->type != SYMBOL_BOOLEAN && sym2->type != SYMBOL_TRISTATE)
			break;
		if (!sym2->dep)
			break;
		if (!sym2->dep2)
			eliminate_dep(sym2, sym2->dep);
		if (dep->token == T_WORD) {
			if (sym2->type == SYMBOL_BOOLEAN)
				dep2 = trans_comp_expr(sym2->dep2, T_UNEQUAL, &symbol_no);
			else
				dep2 = expr_copy(sym2->dep2);
		} else {
			//if ((dep->token == T_UNEQUAL && dep->right.sym == &symbol_no) ||
			//    (sym2->type == SYMBOL_BOOLEAN &&
			//     dep->token == '=' && dep->right.sym == &symbol_yes))
			if ((dep->token == '=' && dep->right.sym == &symbol_yes) ||
			    (sym2->type == SYMBOL_BOOLEAN &&
			     dep->token == T_UNEQUAL && dep->right.sym == &symbol_no))
				dep2 = trans_comp_expr(sym2->dep2, dep->token, dep->right.sym);
			else
				break;
		}
		dep2 = eliminate_yn(trans_expr(fixup_expr(dep2)));
		sym->dep2 = !sym->dep2 ? dep2 : expr_alloc_two(T_AND, dep2, sym->dep2);
	}
#endif
	if (sym->dep == dep) {
		printd(DEBUG_DEP, "el: %s (", sym->name);
		print_expr(DEBUG_DEP, sym->dep, 0);
		printd(DEBUG_DEP, ")\n");
		if (sym->dep2) {
			print_expr(DEBUG_DEP, sym->dep2, 0); printd(DEBUG_DEP, "\n");
			sym->dep2 = eliminate_yn(eliminate_dups(sym->dep2));
			switch (sym->type) {
			case SYMBOL_BOOLEAN: case SYMBOL_INT: case SYMBOL_HEX: case SYMBOL_STRING:
				sym->dep2 = trans_bool_expr(sym->dep2);
				break;
			}
			print_expr(DEBUG_DEP, sym->dep2, 0); printd(DEBUG_DEP, "\n");
			sym->dep = expr_alloc_two(T_AND, expr_copy(sym->dep2), sym->dep);
			sym->dep = eliminate_dups(sym->dep);
			print_expr(DEBUG_DEP, sym->dep, 0); printd(DEBUG_DEP, "\n");
			dep = expr_copy(sym->dep2);
			eliminate_eq(T_AND, &dep, &sym->dep);
			sym->dep = eliminate_yn(sym->dep);
			sym->dep2 = expr_alloc_two(T_AND, sym->dep2, expr_copy(sym->dep));
		} else
			sym->dep2 = expr_copy(sym->dep);
		printd(DEBUG_DEP, " -> ");
		print_expr(DEBUG_DEP, sym->dep2, 0);
		printd(DEBUG_DEP, " -> ");
		print_expr(DEBUG_DEP, sym->dep, 0);
		printd(DEBUG_DEP, "\n");
	}
}

struct symbol * dep_check_symbol(struct symbol *sym);

struct symbol *dep_check_expr(struct expr *e)
{
	struct symbol *sym;
	if (!e)
		return 0;
	switch (e->token) {
	case T_WORD:
	case T_WORD_QUOTE:
	case T_WORD_DQUOTE:
		return dep_check_symbol(e->left.sym);
	case '=':
	case T_UNEQUAL:
		sym = dep_check_symbol(e->left.sym);
		if (sym)
			return sym;
		return dep_check_symbol(e->right.sym);
	case T_OR:
	case T_AND:
		sym = dep_check_expr(e->left.expr);
		if (sym)
			return sym;
		return dep_check_expr(e->right.expr);
	case '!':
		return dep_check_expr(e->left.expr);
	}
	printf("how to check %d?\n", e->token);
	return NULL;
}

struct symbol *dep_check_symbol(struct symbol *sym)
{
	struct statement *stmt, *stmt2;
	struct symbol *sym2;
	struct expr *e1, *e2;

	if (sym->flags & SYMBOL_CHECK) {
		printf("recursive dependency: %s", sym->name);
		return sym;
	}

	sym->flags |= SYMBOL_CHECK;
	sym2 = dep_check_expr(sym->dep);
	if (sym2) {
		printf(" %s", sym->name);
		if (sym->type != sym2->type)
			goto out;
		switch (sym->type) {
		case SYMBOL_TRISTATE:
			e1 = expr_alloc_comp(T_UNEQUAL, sym, &symbol_yes);
			e2 = expr_alloc_comp(T_UNEQUAL, sym2, &symbol_yes);
			break;
		case SYMBOL_BOOLEAN:
			e1 = expr_alloc_one('!', expr_alloc_symbol(sym));
			e2 = expr_alloc_one('!', expr_alloc_symbol(sym2));
			break;
		default:
			goto out;
		}
		if (contains_expr(sym->dep, e2) && contains_expr(sym2->dep, e1)) {
			printf(" (choice(%d) detected)", sym->type);
			sym->flags |= SYMBOL_CHOICE;
			eliminate_eq(T_AND, &sym->dep, &e2);
			sym->dep = eliminate_yn(sym->dep);
			sym2->flags |= SYMBOL_CHOICE;
			eliminate_eq(T_AND, &sym2->dep, &e1);
			sym2->dep = eliminate_yn(sym2->dep);
			stmt = malloc(sizeof(*stmt));
			memset(stmt, 0, sizeof(*stmt));
			stmt->token = '^';
			stmt->text = "change me!";
			stmt->dep = expr_alloc_one('^', NULL);
			stmt->dep->right.sym = sym;
			stmt->dep = expr_alloc_one('^', stmt->dep);
			stmt->dep->right.sym = sym2;
			stmt->dep = expr_alloc_one('^', stmt->dep);
			stmt->dep->right.sym = &symbol_no;
			for (stmt2 = sym->stmt; stmt2->next; stmt2 = stmt2->next)
				;
			if (stmt2->token == '^')
				printf("warning: symbol %s has dup choice statement?\n", sym->name);
			else
				stmt2->next = stmt;
			for (stmt2 = sym2->stmt; stmt2->next; stmt2 = stmt2->next)
				;
			if (stmt2->token == '^')
				printf("warning: symbol %s has dup choice statement?\n", sym->name);
			else
				stmt2->next = stmt;
		}
		expr_free(e1);
		expr_free(e2);
	}
	if (!sym2) for (stmt = sym->stmt; stmt; stmt = stmt->next) {
		if (!stmt->token || stmt->token == '^')
			continue;
		sym2 = dep_check_expr(stmt->dep);
		if (sym2) {
			printf(" %s", sym->name);
			break;
		}
	}
out:
	sym->flags &= ~SYMBOL_CHECK;

	return sym2;
}

const char *sym_typename(int type)
{
	switch (type) {
	case SYMBOL_UNKNOWN: return "unknown";
	case SYMBOL_BOOLEAN: return "bool";
	case SYMBOL_TRISTATE: return "tristate";
	case SYMBOL_INT: return "integer";
	case SYMBOL_HEX: return "hex";
	case SYMBOL_STRING: return "string";
	case SYMBOL_OTHER: return "other";
	default: return "???";
	}
}

void optimize_config(void)
{
	struct symbol *sym;
	struct statement *stmt, *stmt2, **stmtp;
	struct expr *e;
	int i;

	for_all_symbols(i, sym) {
	again:
		for (stmt = sym->stmt; stmt; stmt = stmt->next) {
			switch (stmt->token) {
			case '^':
			case T_BOOL:
			case T_DEP_BOOL:
			case T_DEP_MBOOL:
				if (sym->type == SYMBOL_UNKNOWN)
					sym->type = SYMBOL_BOOLEAN;
				if (sym->type != SYMBOL_BOOLEAN)
					printf("%s:%d:warning: symbol %s has conflicting types (%s,%s)\n",
						stmt->file->name, stmt->lineno, sym->name,
						sym_typename(sym->type), sym_typename(SYMBOL_BOOLEAN));
				if (sym->type == SYMBOL_TRISTATE) {
					printf("%s:%d:Force stmt to dep_tristate\n",
						stmt->file->name, stmt->lineno);
					stmt->token = T_DEP_TRISTATE;
				}
				break;
			case T_DEFINE_BOOL:
				if (stmt->def == &symbol_mod) {
					printf("%s:%d:warning: symbol %s set to 'm', force stmt to define_tristate\n",
						stmt->file->name, stmt->lineno, sym->name);
					stmt->token = T_DEFINE_TRISTATE;
				}
				break;
			case T_HEX:
				if (sym->type == SYMBOL_UNKNOWN)
					sym->type = SYMBOL_HEX;
				if (sym->type != SYMBOL_HEX)
					printf("%s:%d:warning: symbol %s has conflicting types (%s,%s)\n",
						stmt->file->name, stmt->lineno, sym->name,
						sym_typename(sym->type), sym_typename(SYMBOL_HEX));
				break;
			case T_INT:
				if (sym->type == SYMBOL_UNKNOWN)
					sym->type = SYMBOL_INT;
				if (sym->type != SYMBOL_INT)
					printf("%s:%d:warning: symbol %s has conflicting types (%s,%s)\n",
						stmt->file->name, stmt->lineno, sym->name,
						sym_typename(sym->type), sym_typename(SYMBOL_INT));
				break;
			case T_STRING:
				if (sym->type == SYMBOL_UNKNOWN)
					sym->type = SYMBOL_STRING;
				if (sym->type != SYMBOL_STRING)
					printf("%s:%d:warning: symbol %s has conflicting types (%s,%s)\n",
						stmt->file->name, stmt->lineno, sym->name,
						sym_typename(sym->type), sym_typename(SYMBOL_STRING));
				break;
			case T_TRISTATE:
			case T_DEP_TRISTATE:
				if (sym->type == SYMBOL_UNKNOWN)
					sym->type = SYMBOL_TRISTATE;
				if (sym->type != SYMBOL_TRISTATE) {
					printf("%s:%d:warning: symbol %s has conflicting types (%s,%s), force symbol to tristate\n",
						stmt->file->name, stmt->lineno, sym->name,
						sym_typename(sym->type), sym_typename(SYMBOL_TRISTATE));
					sym->type = SYMBOL_TRISTATE;
					goto again;
				}
				break;
			}
		}
		for (stmt = sym->stmt; stmt; stmt = stmt->next) {
			switch (stmt->token) {
			case T_DEFINE_BOOL:
				if (sym->type == SYMBOL_UNKNOWN)
					sym->type = SYMBOL_BOOLEAN;
				if (sym->type != SYMBOL_BOOLEAN)
					printf("%s:%d:warning: symbol %s has conflicting types (%s,%s)\n",
						stmt->file->name, stmt->lineno, sym->name,
						sym_typename(sym->type), sym_typename(SYMBOL_BOOLEAN));
				if (sym->type == SYMBOL_TRISTATE) {
					printf("%s:%d:Force stmt to define_tristate\n",
						stmt->file->name, stmt->lineno);
					stmt->token = T_DEFINE_TRISTATE;
				}
				break;
			case T_DEFINE_TRISTATE:
				if (sym->type == SYMBOL_UNKNOWN)
					sym->type = SYMBOL_TRISTATE;
				if (sym->type != SYMBOL_TRISTATE) {
					printf("%s:%d:warning: symbol %s has conflicting types (%s,%s), force symbol to tristate\n",
						stmt->file->name, stmt->lineno, sym->name,
						sym_typename(sym->type), sym_typename(SYMBOL_TRISTATE));
					sym->type = SYMBOL_TRISTATE;
					goto again;
				}
				break;
			case T_DEFINE_HEX:
				if (sym->type == SYMBOL_UNKNOWN)
					sym->type = SYMBOL_HEX;
				if (sym->type != SYMBOL_HEX)
					printf("%s:%d:warning: symbol %s has conflicting types (%s,%s)\n",
						stmt->file->name, stmt->lineno, sym->name,
						sym_typename(sym->type), sym_typename(SYMBOL_HEX));
				break;
			case T_DEFINE_INT:
				if (sym->type == SYMBOL_UNKNOWN)
					sym->type = SYMBOL_INT;
				if (sym->type != SYMBOL_INT)
					printf("%s:%d:warning: symbol %s has conflicting types (%s,%s)\n",
						stmt->file->name, stmt->lineno, sym->name,
						sym_typename(sym->type), sym_typename(SYMBOL_INT));
				break;
			case T_DEFINE_STRING:
				if (sym->type == SYMBOL_UNKNOWN)
					sym->type = SYMBOL_STRING;
				if (sym->type != SYMBOL_STRING)
					printf("%s:%d:warning: symbol %s has conflicting types (%s,%s)\n",
						stmt->file->name, stmt->lineno, sym->name,
						sym_typename(sym->type), sym_typename(SYMBOL_STRING));
				break;
			}
		}
	}
	for_all_symbols(i, sym) {
		printd(DEBUG_OPT1, "o1: %s\n", sym->name);
		for (stmt = sym->stmt; stmt; stmt = stmt->next) {
			print_expr(DEBUG_OPT1, stmt->dep, 0);
			printd(DEBUG_OPT1, " & ");
			print_expr(DEBUG_OPT1, stmt->dep2, 0);
			printd(DEBUG_OPT1, "\n");
			if (stmt->dep || stmt->dep2) {
				struct expr *e = stmt->dep2;
				if (stmt->token == T_DEP_BOOL) {
					e = trans_dep_bool(e);
					stmt->token = T_DEP_MBOOL;
				}
				if (stmt->dep && e) {
					stmt->dep = expr_alloc_two(T_AND, stmt->dep, e);
				} else if (e)
					stmt->dep = e;
				stmt->dep2 = NULL;
				switch (stmt->token) {
				case T_DEP_MBOOL:	stmt->token = T_BOOL; break;
				case T_DEP_TRISTATE:	stmt->token = T_TRISTATE; break;
				}
			}
			stmt->dep = fixup_expr(stmt->dep);
			print_expr(DEBUG_OPT1, stmt->dep, 0);
			printd(DEBUG_OPT1, "\n");
		}
	}
	for_all_symbols(i, sym) {
		printd(DEBUG_OPT3, "o3: %s\n", sym->name);
		for (stmt = sym->stmt; stmt; stmt = stmt->next) {
			print_expr(DEBUG_OPT3, stmt->dep, 0);
			printd(DEBUG_OPT3, "\n");
			stmt->dep = eliminate_dups(trans_expr(stmt->dep));
			switch (sym->type) {
			case SYMBOL_BOOLEAN: case SYMBOL_INT: case SYMBOL_HEX: case SYMBOL_STRING:
				stmt->dep = trans_bool_expr(stmt->dep);
				break;
			}
			print_expr(DEBUG_OPT3, stmt->dep, 0);
			printd(DEBUG_OPT3, "\n");
		}
	}
	for_all_symbols(i, sym) {
		sym->dep = eliminate_yn(sym->dep);
		for (stmt = sym->stmt; stmt; stmt = stmt->next) {
			stmt->dep = eliminate_yn(stmt->dep);
			if (expr_is_no(stmt->dep)) {
				printf("%s:%d: stmt can never be reached, removed!\n", stmt->file->name, stmt->lineno);
				stmt->token = 0;
			}
		}
	}
	for_all_symbols(i, sym) {
		stmtp = &sym->stmt;
		stmt = *stmtp;
		if (!stmt) {
			printf("undefined symbol %s\n", sym->name);
			continue;
		}
		//if (sym->flags & SYMBOL_CHOICE)
		//	continue;
		while (stmt) {
			if (!stmt->token || stmt->token == '^') {
				stmtp = &stmt->next;
				stmt = *stmtp;
				continue;
			}
#if 1
			if ((stmt->token == T_DEFINE_BOOL || stmt->token == T_DEFINE_TRISTATE) &&
			    (stmt->def == &symbol_no || expr_is_no(stmt->dep))) {
				stmt->token = 0;
				stmt->def = NULL;
				expr_free(stmt->dep);
				stmt->dep = NULL;
				continue;
			}
#endif
			//if (stmt->text && stmt != sym->stmt) {
			//	if (sym->stmt->text)
			//		printf("warning: another stmt with prompt for %s????\n", sym->name);
			//	*stmtp = stmt->next;
			//	stmt->next = sym->stmt;
			//	sym->stmt = stmt;
			//}
			for (stmt2 = stmt->next; stmt2; stmt2 = stmt2->next) {
				if (stmt->token != stmt2->token || stmt->def != stmt2->def ||
				    stmt->menu->parent != stmt2->menu->parent)
					continue;
				if (stmt->text) {
					if (strcmp(skip_space(stmt->text), skip_space(stmt2->text))) {
						printf("warning: prompts differ for %s?\n", sym->name);
						continue;
					}
				}
				if (!stmt->dep)
					stmt->dep = expr_alloc_symbol(&symbol_yes);
				if (!stmt2->dep)
					stmt2->dep = expr_alloc_symbol(&symbol_yes);
				stmt->dep = optimize_expr_or(expr_alloc_two(T_OR, stmt->dep, stmt2->dep));
				stmt2->token = 0;
				free((void *)stmt2->text);
				stmt2->text = NULL;
				stmt2->def = NULL;
				stmt2->dep = NULL;
			}
			if (stmt->text && stmt != sym->stmt) {
				*stmtp = stmt->next;
				stmt->next = sym->stmt;
				sym->stmt = stmt;
			} else
				stmtp = &stmt->next;
			stmt = *stmtp;
		}
		for (stmt = sym->stmt; stmt; stmt = stmt->next) {
			if (!stmt->token || stmt->token == '^')
				continue;
			if (!sym->dep) {
				if (!stmt->dep)
					break;
				sym->dep = stmt->dep;
				stmt->dep = NULL;
			} else {
				e = sym->dep;
				if (stmt->dep) {
					sym->dep = extract_eq_and(&e, &stmt->dep);
					if (expr_is_yes(e)) {
						expr_free(e);
						continue;
					}
				} else
					sym->dep = NULL;
				for (stmt2 = sym->stmt; stmt != stmt2; stmt2 = stmt2->next) {
					if (!stmt2->token || stmt2->token == '^')
						continue;
					stmt2->dep = !stmt2->dep ? expr_copy(e) :
						expr_alloc_two(T_AND, expr_copy(e), stmt2->dep);
				}
				expr_free(e);
				if (!sym->dep)
					break;
			}
		}
#if 0
		for (stmt1 = sym->stmt; stmt1; stmt1 = stmt1->next)
			if (stmt1->token && stmt1->token != '^')
				break;
		if (stmt1 && !expr_is_yes(stmt1->dep)) {
			e = trans_expr(expr_alloc_one('!', stmt1->dep));
			stmt1->dep = NULL;
			for (stmt = stmt1->next; stmt; stmt = stmt->next) {
				if (!stmt->token || stmt->token == '^')
					continue;
				if (stmt->dep) {
					tmp = extract_eq_and(&e, &stmt->dep);
					if (expr_is_yes(e)) {
						expr_free(e);
						e = tmp;
						continue;
					}
					if (tmp) {
						e = expr_alloc_two(T_AND, expr_copy(tmp), e);
						stmt->dep = expr_alloc_two(T_AND, tmp, stmt->dep);
					}
				}
				for (stmt2 = stmt1->next; stmt != stmt2; stmt2 = stmt2->next) {
					if (!stmt2->token || stmt2->token == '^')
						continue;
					stmt2->dep = !stmt2->dep ? expr_copy(e) :
						expr_alloc_two(T_AND, expr_copy(e), stmt2->dep);
				}
				stmt1->dep = trans_expr(expr_alloc_one('!', e));
				break;
			}
			if (!stmt1->dep) {
				e = trans_expr(expr_alloc_one('!', e));
				//sym->dep = sym->dep ? expr_alloc_two(T_AND, sym->dep, e) : e;
				stmt1->dep = e;
			}
		}
		for (stmtp = &sym->stmt; *stmtp; stmtp = &stmt->next) {
			stmt = *stmtp;
			if (!stmt->token || stmt->token == '^')
				continue;
			if (!stmt->dep || stmt->text)
				continue;
			for (stmt2 = stmt->next; stmt2; stmt2 = stmt2->next) {
				trans_count = 0;
				if (!stmt2->dep || stmt2->text)
					continue;
				e = trans_expr(expr_alloc_one('!', expr_copy(stmt->dep)));
				tmp = extract_eq_and(&e, &stmt2->dep);
				if (expr_is_yes(e)) {
					expr_free(e);
					expr_free(tmp);
					continue;
				}
				if (tmp)
					stmt2->dep = expr_alloc_two(T_AND, tmp, stmt2->dep);
				e = trans_expr(expr_alloc_one('!', expr_copy(stmt2->dep)));
				tmp = extract_eq_and(&e, &stmt->dep);
				if (expr_is_yes(e)) {
					expr_free(e);
					expr_free(tmp);
					while (stmt->next != stmt2)
						stmt = stmt->next;
					stmt->next = *stmtp;
					*stmtp = stmt2;
					stmt = stmt->next;
					stmt2 = stmt->next;
					stmt->next = (*stmtp)->next;
					(*stmtp)->next = stmt2;
					continue;
				}
				if (tmp)
					stmt->dep = expr_alloc_two(T_AND, tmp, stmt->dep);
			}
		}
#endif
	}
	for_all_symbols(i, sym) {
		printd(DEBUG_OPT4, "o4: %s\n", sym->name);
		print_expr(DEBUG_OPT4, sym->dep, 0); printd(DEBUG_OPT4, "\n");
		sym->dep = eliminate_dups(sym->dep);
		sym->dep = eliminate_yn(sym->dep);
		print_expr(DEBUG_OPT4, sym->dep, 0); printd(DEBUG_OPT4, "\n");
		for (stmt = sym->stmt; stmt; stmt = stmt->next) {
			print_expr(DEBUG_OPT4, stmt->dep, 0); printd(DEBUG_OPT4, "\n");
			stmt->dep = eliminate_dups(stmt->dep);
			stmt->dep = eliminate_yn(stmt->dep);
			print_expr(DEBUG_OPT4, stmt->dep, 0); printd(DEBUG_OPT4, "\n");
		}
	}
	for_all_symbols(i, sym) {
		if (dep_check_symbol(sym))
			printf("\n");
	}
	for_all_symbols(i, sym) {
		if (sym->dep && !sym->dep2) {
			printd(DEBUG_OPT5, "o5: %s\n", sym->name);
			print_expr(DEBUG_OPT5, sym->dep, 0); printd(DEBUG_OPT5, "\n");
			eliminate_dep(sym, sym->dep);
			print_expr(DEBUG_OPT5, sym->dep, 0); printd(DEBUG_OPT5, "\n");
		}
	}
}

int main(int ac, char **av)
{
	FILE *out = NULL;
	char name[128];
	struct symbol *sym;
	struct statement *stmt;

	if (ac > 2)
		cml1debug = 1;
	helplex("Documentation/Configure.help");
	sprintf(name, "arch/%s/Config.help", av[1]);
	helplex(name);
	sprintf(name, "arch/%s/config.in", av[1]);
	scan_init(name);
	sym = lookup_symbol("CONFIG_ARCH", SYMBOL_STRING);
	stmt = add_stmt(sym, T_DEFINE_STRING, NULL, lookup_symbol(av[1], SYMBOL_OTHER));
	stmt->menu = NULL;
	cml1parse();
	optimize_config();
#if 0
	out = fopen("config.new", "w");
	if (!out) {
		printf("unable to open config.new!\n");
		exit(1);
	}
#endif
	print_files(out, name);

	return 0;
}
