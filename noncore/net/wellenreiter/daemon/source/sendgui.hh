/* $Id: sendgui.hh,v 1.2 2002-11-13 21:57:19 mjm Exp $ */

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

int sendgui(const char *string, ...);

#endif /* SENDGUI_HH */
