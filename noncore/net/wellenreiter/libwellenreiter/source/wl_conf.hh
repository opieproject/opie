/* $Id: wl_conf.hh,v 1.1 2003-01-05 11:18:27 mjm Exp $ */

#ifndef WLCONF_HH
#define WLCONF_HH

#include <string.h>
#include <errno.h>

int wl_checkcfg(void);
int wl_cfgvalue(const char *, char *, int);

#endif /* WLCONF_HH */
