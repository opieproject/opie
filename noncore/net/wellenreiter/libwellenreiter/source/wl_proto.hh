/* $Id: wl_proto.hh,v 1.5 2003-02-26 22:28:06 mjm Exp $ */

#ifndef WLPROTO_HH
#define WLPROTO_HH

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Type definitions, to be continued */
#define NETFOUND  01
#define NETLOST   02
#define CMD_FAIL  50
#define CMD_OK    51
#define STARTSNIFF 98
#define STOPSNIFF  99

unsigned int add_field(char *, const char *, int);
unsigned int get_field(const char *, char *, int);
int send_ok(const char *, int, int);
int get_ok(const char *);
int send_fail(const char *, int, int, const char *);
int get_fail(char *, const char *, size_t);
int send_network_found (const char *, int, void *);
int get_network_found (void *, const char *);

#endif /* WLPROTO_HH */
