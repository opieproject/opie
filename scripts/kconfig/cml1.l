%option nounput backup nostdinit noyywrap full ecs
%option 8bit backup nodefault perf-report perf-report
%x STRING
%{
/*
 * Copyright (C) 2002 Roman Zippel <zippel@linux-m68k.org>
 * Released under the terms of the GNU GPL v2.0.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cml1.h"

char *text;
static char *text_ptr;
static int text_size, text_asize;

struct buffer {
	struct buffer *parent;
	YY_BUFFER_STATE state;
};

struct buffer *current_buf;

int lineno(void)
{
	if (current_buf)
		return current_file->lineno;
	else
		return 0;
}

void scan_init(char *file)
{
	struct buffer *buf = malloc(sizeof(*buf));
	memset(buf, 0, sizeof(*buf));

	yyin = fopen(file, "r");
	if (!yyin) {
		printf("can't find file %s\n", file);
		exit(1);
	}
	current_buf = buf;

	current_file = lookup_file(file);
	current_file->lineno = 0;
	current_file->flags = FILE_BUSY;
}

void scan_nextfile(char *name)
{
	struct file *file = lookup_file(name);
	struct buffer *buf = malloc(sizeof(*buf));
	memset(buf, 0, sizeof(*buf));

	current_buf->state = YY_CURRENT_BUFFER;
	yyin = fopen(name, "r");
	if (!yyin) {
		printf("can't find file %s\n", name);
		exit(1);
	}
	yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
	buf->parent = current_buf;
	current_buf = buf;

	if (file->flags & FILE_BUSY) {
		printf("recursive scan (%s)?\n", name);
		exit(1);
	}
	file->flags |= FILE_BUSY;
	file->lineno = 0;
	file->parent = current_file;
	current_file = file;
}

#define START_STRSIZE	16

void new_string(void)
{
	text = malloc(START_STRSIZE);
	text_asize = START_STRSIZE;
	text_ptr = text;
	text_size = 0;
	*text_ptr = 0;
}

void append_string(const char *str, int size)
{
	int new_size = text_size + size + 1;
	if (new_size > text_asize) {
		text = realloc(text, new_size);
		text_asize = new_size;
		text_ptr = text + text_size;
	}
	memcpy(text_ptr, str, size);
	text_ptr += size;
	text_size += size;
	*text_ptr = 0;
}

void alloc_string(const char *str, int size)
{
	text = malloc(size + 1);
	memcpy(text, str, size);
	text[size] = 0;
}

%}
%%
	int str = 0;

\"|\'	{
	str = yytext[0];
	new_string();
	BEGIN(STRING);
}
<STRING>{
	[^'"\n\\]+	{
		append_string(yytext, yyleng);
	}
	\'|\"	{
		if (str == yytext[0]) {
			BEGIN(INITIAL);
			cml1lval.string = text;
			//printf("s:%s", text);
			return str == '"' ? T_WORD_DQUOTE : T_WORD_QUOTE;
		} else
			append_string(yytext, 1);
	}
	\\[ \t]*\n	append_string(yytext+yyleng-1, 1); current_file->lineno++;
	\\[ \t]*	append_string(yytext+1, yyleng-1);
	\\.		append_string(yytext+1, 1);
	\n	{
		printf(":%d: open string!\n", current_file->lineno+1);
		exit(0);
	}
	<<EOF>>	{
		printf(":%d: open string!\n", current_file->lineno+1);
		exit(0);
	}
}
mainmenu_name[ \t]	return T_MAINMENU_NAME;
mainmenu_option[ \t]	return T_MAINMENU_OPTION;
next_comment/[ \t\n]	return T_NEXT_COMMENT;
endmenu/[ \t\n]		return T_ENDMENU;
comment[ \t]		return T_COMMENT;
bool[ \t]		return T_BOOL;
hex[ \t]		return T_HEX;
int[ \t]		return T_INT;
string[ \t]		return T_STRING;
tristate[ \t]		return T_TRISTATE;
define_bool[ \t]	return T_DEFINE_BOOL;
define_hex[ \t]		return T_DEFINE_HEX;
define_int[ \t]		return T_DEFINE_INT;
define_string[ \t]	return T_DEFINE_STRING;
define_tristate[ \t]	return T_DEFINE_TRISTATE;
dep_bool[ \t]		return T_DEP_BOOL;
dep_mbool[ \t]		return T_DEP_MBOOL;
dep_tristate[ \t]	return T_DEP_TRISTATE;
unset[ \t]		return T_UNSET;
choice[ \t]		return T_CHOICE;
source[ \t]		return T_SOURCE;
if[ \t]			return T_IF;
else/[ \t\n]		return T_ELSE;
fi/[ \t\n]		return T_FI;
then/[ \t\n]		return T_THEN;
\[			return '[';
\]			return ']';
;			return ';';
!=			return T_UNEQUAL;
=			return '=';
!			return '!';
-a			return T_AND;
-o			return T_OR;
#[^\n]*	{
	/* comment */
	current_file->lineno++;
	alloc_string(yytext, yyleng);
	cml1lval.string = text;
	return T_SH_COMMENT;
}
[ \t]+			/* space */
\\\n			current_file->lineno++;	/* ignore */
\n	{
	current_file->lineno++;
	return '\n';
}
[[:alnum:]$/][[:graph:]]*	{
	alloc_string(yytext, yyleng);
	cml1lval.string = text;
	//printf("w:%s", text);
	return T_WORD;
}
.	{
		printf("%s:%d: invalid character 0x%x(%c)\n", current_file->name, current_file->lineno+1, yytext[0], yytext[0]);
		exit(1);
}
<<EOF>>	{
	if (current_buf->parent) {
		struct buffer *buf = current_buf;
		current_buf = buf->parent;
		current_file->flags |= FILE_SCANNED;
		current_file->flags &= ~FILE_BUSY;
		current_file = current_file->parent;
		yy_delete_buffer(YY_CURRENT_BUFFER);
		yy_switch_to_buffer(current_buf->state);
		free(buf);
	} else
		yyterminate();
}
%%
