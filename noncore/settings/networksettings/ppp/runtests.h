/*
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: runtests.h,v 1.1.2.1 2003-07-15 15:29:08 tille Exp $
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

#ifndef __RUNTESTS__H__
#define __RUNTESTS__H__

#define SYSOPTIONS "/etc/ppp/options"

const int TEST_OK = 0;
const int TEST_WARNING = 1;
const int TEST_NOCONNECT = 2;
const int TEST_CRITICAL = 3;

// Various tests to be run at starttime
int runTests();
const char* getHomeDir();
void pppdVersion(int *version, int *modification, int *patch);

#ifdef __linux__
bool ppp_available(void);
#endif

#endif

