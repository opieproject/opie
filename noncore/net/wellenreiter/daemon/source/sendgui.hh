/* $Id: sendgui.hh,v 1.1 2002-11-12 22:12:33 mjm Exp $ */

#ifndef SENDGUI_HH
#define SENDGUI_HH

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int connect_server(int *, unsigned int, int);
int sendgui(const char *string, ...);

#endif /* SENDGUI_HH */
