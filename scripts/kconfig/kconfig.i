%module kconfig
%{
#include "kconfig_load.c"
%}

%init %{
	kconfig_load();
%}

%nodefault;

#ifdef SWIGRUBY
%typemap (out) char * {
	if ($1 == NULL)
		$result = Qnil;
	else
		$result = rb_str_new2($1);
}
%typemap (in) char * {
	if ($input == Qnil)
		$1 = NULL;
	else
		$1 = STR2CSTR($input);
}

%{
static void expr_to_s_help(void *data, const char *str)
{
	rb_str_cat((VALUE)data, str, strlen(str));
}
%}
#endif

%immutable;
%include "expr.h"
#define P(name,type,arg)        extern type name arg
%include "lkc_proto.h"
%mutable;

#ifdef SWIGRUBY
%predicate menu::isVisible;
%predicate symbol::isChangable;
%predicate symbol::isChoice;
%predicate symbol::isChoiceValue;
#endif

%extend menu {
	bool isVisible(void) {
		return menu_is_visible(self);
	}
#ifdef SWIGRUBY
	void each(void) {
		struct menu *child;
		for (child = self->list; child; child = child->next)
			rb_yield(SWIG_NewPointerObj(child, SWIGTYPE_p_menu, 0));
	}
	static void each_menu(void) {
		struct menu *child;
		for (child = rootmenu.list; child; child = child->next)
			rb_yield(SWIG_NewPointerObj(child, SWIGTYPE_p_menu, 0));
	}
#endif
}

%extend symbol {
	void calc_value(void) {
		sym_calc_value(self);
	}
	tristate set_tristate(tristate val) {
		return sym_set_tristate_value(self, val);
	}
	bool set_string(char *val) {
		return sym_set_string_value(self, val);
	}
	const char *get_string(void) {
		return sym_get_string_value(self);
	}
	bool isChangable(void) {
		return sym_is_changable(self);
	}
	bool isChoice(void) {
		return sym_is_choice(self);
	}
	bool isChoiceValue(void) {
		return sym_is_choice_value(self);
	}
	static struct symbol *lookup(const char *name) {
		return sym_lookup(name, 0);
	}
	static struct symbol *find(const char *name) {
		return sym_find(name);
	}
	static const char *type_name(enum symbol_type type) {
		return sym_type_name(type);
	}
#ifdef SWIGRUBY
	void each(void) {
		struct property *prop;
		for (prop = self->prop; prop; prop = prop->next)
			rb_yield(SWIG_NewPointerObj(prop, SWIGTYPE_p_property, 0));
	}
	static void each_sym(void) {
		struct symbol *sym;
		int i;
		for (i = 0; i < SYMBOL_HASHSIZE; i++) {
			for (sym = symbol_hash[i]; sym; sym = sym->next) {
				if (sym->flags & SYMBOL_CONST)
					continue;
				rb_yield(SWIG_NewPointerObj(sym, SWIGTYPE_p_symbol, 0));
			}
		}
	}
#endif
}

%extend property {
	static const char *type_name(enum prop_type type) {
		return prop_get_type_name(type);
	}
}

%extend expr {
	static int comp_type(enum expr_type t1, enum expr_type t2) {
		return expr_compare_type(t1, t2);
	}
#ifdef SWIGRUBY
	VALUE __str__(void) {
		VALUE str = rb_str_new2("");
		expr_print(self, expr_to_s_help, (void*)str, E_NONE);
		return str;
	}
#endif
}
