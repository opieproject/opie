/* orinoco_hopper.c
 * orinoco wireless nic channel scanning utility
 *
 * By Snax <snax@shmoo.com>
 * Copyright (c) 2002 Snax
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * For a copy of the GNU General Public License write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include <unistd.h>
#include <getopt.h>

#ifndef SIOCIWFIRSTPRIV
#define SIOCIWFIRSTPRIV SIOCDEVPRIVATE
#endif

void changeChannel(int);
int setChannel( unsigned char channel );

int max = 11;
int mode = 2;
char dev[32];

void changeChannel(int x) {
   static int chan = 0;
   chan = (chan % max) + 1;
   setChannel(chan);
}

int setChannel( unsigned char channel )
{
   int result = 0;
   int fd;
   struct iwreq ireq;  //for Orinoco
   int *ptr;

   /* get a socket */
   fd = socket(AF_INET, SOCK_STREAM, 0);

   if ( fd == -1 ) {
      return -1;
   }
   ptr = (int *) ireq.u.name;
   ptr[0] = mode;
   ptr[1] = channel;
   strcpy(ireq.ifr_ifrn.ifrn_name, dev);
   result = ioctl( fd, SIOCIWFIRSTPRIV + 0x8, &ireq);
   close(fd);
   return result;
}

void usage(char *cmd) {
    fprintf(stderr, 
        "Usage: %s <iface> [-p] [-i <interval millisec>] [-n]\n   -n = international channels\n   -p = keep prism headers\n", cmd);
    exit(1);
}

int main (int argc, char *argv[])
{
  struct itimerval tval;
  int ms, r;

  //this will be the channel scanning interval, currently 0.2 sec
  struct timeval interval = {0, 200000};

   if (argc < 2) usage(argv[0]);
   strncpy(dev, argv[1], 32);
   dev[31] = 0;

   while (1) {
      r = getopt(argc,argv,"i:np");
      if (r < 0) break;
      switch (r) {
      case 'n':
         max = 14;
         break;
      case 'p':
         mode = 1;
         break;
      case 'i':
         ms = atoi(optarg);
         interval.tv_sec = ms / 1000;
         interval.tv_usec = (ms % 1000) * 1000;
         break;
      default:
         usage(argv[0]);
      }
   } 

  //this sets up the kchannel scanning stuff
  signal(SIGALRM, changeChannel);
  tval.it_interval = interval;
  tval.it_value = interval;
  setitimer(ITIMER_REAL, &tval, NULL);
  while (1) pause();

  return 0;
}

