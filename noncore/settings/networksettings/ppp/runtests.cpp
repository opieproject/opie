/*
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: runtests.cpp,v 1.1.2.1 2003-07-15 15:29:08 tille Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 * This file was contributed by Mario Weilguni <mweilguni@sime.com>
 * Thanks Mario !
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qdir.h>
#include "runtests.h"
#include <ctype.h>
#include <unistd.h>
#include <qmessagebox.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <netinet/in.h>

#ifdef HAVE_RESOLV_H
#include <arpa/nameser.h>
#include <resolv.h>
#endif

#ifndef _PATH_RESCONF
#define _PATH_RESCONF "/etc/resolv.conf"
#endif

//#include <klocale.h>
#define i18n QObject::tr
#include "pppdata.h"

// initial effective uid (main.cpp)
extern uid_t euid;

// secure pppd location (opener.cpp)
extern const char* pppdPath();

// shamelessly stolen from pppd-2.3.5
/********************************************************************
 *
 * Internal routine to decode the version.modification.patch level
 */

static void decode_version (const char *_buf, int *version,
			    int *modification, int *patch)
  {
    char *buffer = qstrdup(_buf);
    char *buf = buffer;
    *version      = (int) strtoul (buf, &buf, 10);
    *modification = 0;
    *patch        = 0;

    if (*buf == '.')
      {
	++buf;
	*modification = (int) strtoul (buf, &buf, 10);
	if (*buf == '.')
	  {
	    ++buf;
	    *patch = (int) strtoul (buf, &buf, 10);
	  }
      }

    if (*buf != '\0')
      {
	*version      =
	*modification =
	*patch        = 0;
      }

    delete [] buffer;
  }


void pppdVersion(int *version, int *modification, int *patch) {
  char buffer[30];
  const char *pppd;
  char *query;

  *version = *modification = *patch = 0;

  // locate pppd
  if(!(pppd = pppdPath()))
    return;

  // call pppd with --version option
  if(!(query = new char[strlen(pppd)+25]))
    return;
  strcpy(query, pppd);
  // had to add a dummy device to prevent a "no device specified
  // and stdin is not a tty" error from newer pppd versions.
  strcat(query, " --version /dev/tty 2>&1");
  fflush(0L);
  FILE *output = popen(query, "r");
  delete [] query;
  if(!output)
    return;

  // read output
  int size = fread(buffer, sizeof(char), 29, output);

  if(ferror(output)) {
    pclose(output);
    return;
  }
  pclose(output);
  buffer[size] = '\0';

  // find position of version number x.y.z
  char *p = buffer;
  while(*p && !isdigit(*p))
    p++;
  if (*p == 0)
    return;
  char *p2 = p;
  while(*p2 == '.' || isdigit(*p2))
    p2++;
  *p2 = '\0';

  decode_version(p, version, modification, patch);
}


int uidFromName(const char *uname) {
  struct passwd *pw;

  setpwent();
  while((pw = getpwent()) != NULL) {
    if(strcmp(uname, pw->pw_name) == 0) {
      int uid = pw->pw_uid;
      endpwent();
      return uid;
    }
  }

  endpwent();
  return -1;
}


const char *homedirFromUid(uid_t uid) {
  struct passwd *pw;
  char *d = 0;

  setpwent();
  while((pw = getpwent()) != NULL) {
    if(pw->pw_uid == uid) {
      d = strdup(pw->pw_dir);
      endpwent();
      return d;
    }
  }

  endpwent();
  return d;
}


const char* getHomeDir() {
  static const char *hd = 0;
  static bool ranTest = false;
  if(!ranTest) {
    hd = homedirFromUid(getuid());
    ranTest = true;
  }

  return hd;
}


int runTests() {
  int warning = 0;

  // Test pre-1: check if the user is allowed to dial-out
  if(access("/etc/kppp.allow", R_OK) == 0 && getuid() != 0) {
    bool access = FALSE;
    FILE *f;
    if((f = fopen("/etc/kppp.allow", "r")) != NULL) {
      char buf[2048]; // safe
      while(f != NULL && !feof(f)) {
	if(fgets(buf, sizeof(buf), f) != NULL) {
	  QString s(buf);

	  s = s.stripWhiteSpace();
	  if(s[0] == '#' || s.length() == 0)
	    continue;

	  if((uid_t)uidFromName(QFile::encodeName(s)) == getuid()) {
	    access = TRUE;
	    fclose(f);
	    f = NULL;
	  }
	}
      }
      if(f)
	fclose(f);
    }

    if(!access) {
        QMessageBox::warning(0,"error",
		 i18n("You're not allowed to dial out with "
		      "kppp.\nContact your system administrator."));
      return TEST_CRITICAL;
    }
  }

  // Test 1: search the pppd binary
  const char *f = pppdPath();

  if(!f) {
      QMessageBox::warning(0,"error",
		 i18n("Cannot find the PPP daemon!\n"
                      "Make sure that pppd is installed."));
    warning++;
  }

  // Test 2: check access to the pppd binary
  if(f) {
#if 0
    if(access(f, X_OK) != 0 /* && geteuid() != 0 */) {
      KMessageBox::warning(0,
		   i18n("You do not have the permission "
			"to start pppd!\n"
			"Contact your system administrator "
			"and ask to get access to pppd."));
      return TEST_CRITICAL;
    }
#endif

    if(geteuid() != 0) {
      struct stat st;
      stat(f, &st);
      if(st.st_uid != 0 || (st.st_mode & S_ISUID) == 0) {
          QMessageBox::warning(0,"error",
                     i18n("You don't have sufficient permission to run\n"
                          "%1\n"
                          "Please make sure that kppp is owned by root "
                          "and has the SUID bit set.").arg(f));
        warning++;
      }
    }
  }

  // Test 5: check for existence of /etc/resolv.conf
  if (access(_PATH_RESCONF, R_OK) != 0) {
    QString file = _PATH_RESCONF" ";
    QString msgstr = i18n("%1 is missing or can't be read!\n"
                   "Ask your system administrator to create "
                   "this file (can be empty) with appropriate "
                   "read and write permissions.").arg(file);
    QMessageBox::warning(0, "errror", msgstr);
    warning ++;
  }

  if(warning == 0)
    return TEST_OK;
  else
    return TEST_WARNING;
}

