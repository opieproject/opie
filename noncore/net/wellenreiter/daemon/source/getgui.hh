/* $id */

#ifndef GETGUI_HH
#define GETGUI_HH

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int commsock(int *);
int commstring(const char *);

#endif /* GETGUI_HH */
