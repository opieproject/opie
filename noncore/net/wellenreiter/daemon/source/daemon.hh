/* $Id: daemon.hh,v 1.7 2002-12-31 12:38:28 mjm Exp $ */

#ifndef DAEMON_HH
#define DAEMON_HH

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include <libwellenreiter/source/wl_sock.hh>
#include <libwellenreiter/source/wl_log.hh>
#include <libwellenreiter/source/wl_types.hh>
#include <libwellenreiter/source/wl_proto.hh>
#include <libwellenreiter/source/cardmode.hh>
#include <libwellenreiter/source/sniff.hh>

void usage(void);

#endif /* DAEMON_HH */ 
