/* $id */

#ifndef WLSOCK_HH
#define WLSOCK_HH

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

int commsock(const char *, int);
int sendcomm(const char *, int, const char *, ...);

#endif /* WLSOCK_HH */
