/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef __OPIE_LOGINAPPLICATION_H__
#define __OPIE_LOGINAPPLICATION_H__

#include <qstringlist.h>

#include <qpe/qpeapplication.h>

#ifdef USEPAM
struct pam_message;
struct pam_response;
#endif

class LoginApplication : public QPEApplication {
public:
	LoginApplication ( int &argc, char **argv, pid_t parentpid );

	static bool checkPassword ( const char *user, const char *password );

	static const char *loginAs ( );
	static void setLoginAs ( const char *user );

	static bool changeIdentity ( );
	static bool login ( );

	static QStringList allUsers ( );

	void quitToConsole ( );

private:
	static const char *s_username;

#ifdef USEPAM
	static int pam_helper ( int num_msg, const struct pam_message **msg, struct pam_response **resp, void * );
	static const char *s_pam_password;
#endif

private:
	pid_t m_parentpid;
};

extern LoginApplication *lApp;

#endif
