/*
 * Copyright (C) 2002 Roman Zippel <zippel@linux-m68k.org>
 * Released under the terms of the GNU GPL v2.0.
 */

#ifndef CML1_H
#define CML1_H

#include <stdio.h>
#ifndef YYBISON
#include "cml1.tab.h"
#endif
#define CML1
#define WORD T_WORD
#define prop stmt
#define property statement
#include "expr.h"

struct symbol *lookup_symbol(char *name, int type);
struct symbol *lookup_symbol_ref(char *name);

void new_string(void);
void append_string(const char *str, int size);
extern char *text;

extern int cml1debug;
extern int cml1parse(void);
extern void helplex(char *help);

extern void scan_init(char *name);
extern void print_files(FILE *out, const char *name);
extern void optimize_config(void);

void fprint_expr(FILE *out, struct expr *e, int prevtoken);

#endif /* CML1_H */
