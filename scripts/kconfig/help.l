%option nounput nostdinit noyywrap 8bit
%x ENTRY
%{
/*
 * Copyright (C) 2002 Roman Zippel <zippel@linux-m68k.org>
 * Released under the terms of the GNU GPL v2.0.
 */

#include <string.h>
#include <unistd.h>
#include "cml1.h"

#define YY_DECL void helplex(char *help)
#undef YY_NULL
#define YY_NULL
%}

%%
	struct symbol *sym = NULL;
	helpin = fopen(help, "r");
	if (!helpin)
		return;
	//help_flex_debug = 1;
	yyrestart(helpin);
	BEGIN(INITIAL);

^#.*\n	/* skip */

^CONFIG_[[:alnum:]_]+ {
	char name[256];
	int ch;
	while ((ch = input()) != '\n' && ch != EOF)
		;
	memcpy(name, yytext, yyleng);
	name[yyleng] = 0;
	sym = lookup_symbol(name, SYMBOL_UNKNOWN);
	new_string();
	BEGIN(ENTRY);
}

<ENTRY>{
	[ \t].*\n+/[^ \t\n] |
	<<EOF>> {
		append_string(yytext, yyleng);
		if (sym) {
			if (sym->help) {
				//printf("symbol %s has duplicated help?\n", sym->name);
				free(text);
			} else
				sym->help = text;
			sym = NULL;
		} else
			printf("no symbol?\n");
		BEGIN(INITIAL);
	}
	\n/[^ \t\n] {
		append_string(yytext, yyleng);
		if (sym) {
			if (sym->help) {
				//printf("symbol %s has duplicated help?\n", sym->name);
				free(text);
			} else
				sym->help = text;
			sym = NULL;
		} else
			printf("no symbol?\n");
		BEGIN(INITIAL);
	}
	[ \t].*\n |
	.	|
	\n	{
		append_string(yytext, yyleng);
	}
}

.
\n
%%
