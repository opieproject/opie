/* ---------------------------------------------------------------------- */
/*                                                                        */
/* [main.C]                        Konsole                                */
/*                                                                        */
/* ---------------------------------------------------------------------- */
/*                                                                        */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>        */
/*                                                                        */
/* This file is part of Konsole, an X terminal.                           */
/*                                                                        */
/* The material contained in here more or less directly orginates from    */
/* kvt, which is copyright (c) 1996 by Matthias Ettrich <ettrich@kde.org> */
/*                                                                        */
/* ---------------------------------------------------------------------- */
/*                        */
/* Ported Konsole to Qt/Embedded                                              */
/*                        */
/* Copyright (C) 2000 by John Ryland <jryland@trolltech.com>                  */
/*                        */
/* -------------------------------------------------------------------------- */

#include "konsole.h"

#ifdef QT_QWS_OPIE

#include <opie2/oapplicationfactory.h>

/* --| main |------------------------------------------------------ */
using namespace Opie::Core;
OPIE_EXPORT_APP( OApplicationFactory<Konsole> )

#else //for non opie builds

#include <qpe/qpeapplication.h>

#include <qfile.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <pwd.h>
#include <sys/types.h>


/* --| main |------------------------------------------------------ */
		int main(int argc, char* argv[]) {
		if(setuid(getuid()) !=0) qDebug("setuid failed");
		if(setgid(getgid()) != 0) qDebug("setgid failed"); // drop privileges

		QPEApplication a( argc, argv );
#ifdef FAKE_CTRL_AND_ALT
    qDebug("Fake Ctrl and Alt defined");
		QPEApplication::grabKeyboard(); // for CTRL and ALT
#endif

		QStrList tmp;
		const char* shell = getenv("SHELL");

		if (shell == NULL || *shell == '\0') {
				struct passwd *ent = 0;
				uid_t me = getuid();
				shell = "/bin/sh";
    
				while ( (ent = getpwent()) != 0 ) {
						if (ent->pw_uid == me) {
								if (ent->pw_shell != "")
										shell = ent->pw_shell;
								break;
						}
				}
				endpwent();
		}

		if( putenv((char*)"COLORTERM=") !=0)
				qDebug("putenv failed"); // to trigger mc's color detection

		Konsole m( "test", shell, tmp, TRUE  );
		m.setCaption( Konsole::tr("Terminal") );
		a.showMainWidget( &m );

		return a.exec();
}

#endif
