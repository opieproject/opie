/* $Id: wl_proto.hh,v 1.4 2002-12-31 12:36:07 mjm Exp $ */

#ifndef WLPROTO_HH
#define WLPROTO_HH

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Type definitions, to be continued */
#define NETFOUND 01
#define NETLOST  02
#define STARTSNIFF 98
#define STOPSNIFF  99

int add_field(char *, const char *, int);
int get_field(const char *, char *, int);
int send_network_found (const char *, int, void *);
int get_network_found (void *, const char *);

#endif /* WLPROTO_HH */
