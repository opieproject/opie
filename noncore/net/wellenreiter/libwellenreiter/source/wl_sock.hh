/* $Id */

#ifndef WLSOCK_HH
#define WLSOCK_HH

#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

int wl_setupsock(const char *, int, struct sockaddr_in);
int wl_send(const char *, int, const char *, ...);
int wl_recv(int *, struct sockaddr_in, char *, int);

#endif /* WLSOCK_HH */
