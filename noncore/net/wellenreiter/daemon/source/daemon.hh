/* $Id: daemon.hh,v 1.5 2002-12-16 17:59:21 mjm Exp $ */

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

#include "../../libwellenreiter/source/sock.hh"
#include "../../libwellenreiter/source/log.hh"
#include "../../libwellenreiter/source/proto.hh"
#include "../../libwellenreiter/source/cardmode.hh"
#include "../../libwellenreiter/source/sniff.hh"

void usage(void);

#endif /* DAEMON_HH */ 
