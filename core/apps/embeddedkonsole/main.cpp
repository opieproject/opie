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
/*									      */
/* Ported Konsole to Qt/Embedded                                              */
/*									      */
/* Copyright (C) 2000 by John Ryland <jryland@trolltech.com>                  */
/*									      */
/* -------------------------------------------------------------------------- */

#include "konsole.h"

#include <qpe/qpeapplication.h>

#include <qfile.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


/* --| main |------------------------------------------------------ */
int main(int argc, char* argv[])
{
  setuid(getuid()); setgid(getgid()); // drop privileges

  QPEApplication a( argc, argv );

#ifdef FAKE_CTRL_AND_ALT
    qDebug("Fake Ctrl and Alt defined");
  QPEApplication::grabKeyboard(); // for CTRL and ALT
#endif

  QStrList tmp;
  const char* shell = getenv("SHELL");
  if (shell == NULL || *shell == '\0')
    shell = "/bin/sh";

  // sh is completely broken on familiar. Let's try to get something better
  if ( qstrcmp( shell, "/bin/shell" ) == 0 && QFile::exists( "/bin/bash" ) )
      shell = "/bin/bash";
  
  putenv((char*)"COLORTERM="); // to trigger mc's color detection

  Konsole m( "test", shell, tmp, TRUE  );
  m.setCaption( Konsole::tr("Terminal") );
  a.showMainWidget( &m );

  return a.exec();
}
