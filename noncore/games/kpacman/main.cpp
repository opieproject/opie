/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Sam Jan 19 13:37:57 CET 2002
    copyright            : (C) 2002 by Jörg Thönnissen
    email                : joe@dsite.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "portable.h"

#if defined( KDE2_PORT )
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#elif defined( QPE_PORT )
#include <qpe/qpeapplication.h>
#endif

#include "kpacman.h"

#ifdef KDE2_PORT
static const char *description =
        I18N_NOOP("Kpacman");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE


static KCmdLineOptions options[] =
{
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};
#endif

int main(int argc, char *argv[])
{
#if defined( KDE2_PORT )
  KAboutData aboutData( "kpacman", I18N_NOOP("Kpacman"),
    VERSION, description, KAboutData::License_GPL,
    "(c) 2002, Jörg Thönnissen", 0, 0, "joe@dsite.de");
  aboutData.addAuthor("Jörg Thönnissen",0, "joe@dsite.de");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.
  KApplication a;
#elif defined( QPE_PORT )
  QPEApplication a( argc, argv );
#endif
  Kpacman *kpacman = new Kpacman( NULL, "Kpacman" );
  a.setMainWidget(kpacman);
#if defined( KDE2_PORT )
  a.setTopWidget(kpacman);
  kpacman->show();
#elif defined( QPE_PORT )
  kpacman->showMaximized();
#endif

  return a.exec();
}
