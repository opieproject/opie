/*
** Author: Carsten Schneider <CarstenSchneider@t-online.de>
**
** $Id: main.cpp,v 1.5 2004-05-31 15:26:51 ar Exp $
**
** Homepage: http://home.t-online.de/home/CarstenSchneider/zsafe/index.html
*/


#include "zsafe.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#ifdef DESKTOP
#include <qapplication.h>
#else
#include <qpe/qpeapplication.h>
#endif

/* STD */
#include <stdio.h>
#include <signal.h>

ZSafe *zs;
#ifdef DESKTOP
QApplication   *appl;
#else
QPEApplication *appl;
#endif

int DeskW;
int DeskH;

void suspend (int signum)
{
   printf ("Suspend signal %d received\n", signum);
}

void resume (int signum)
{
   printf ("Resume signal %d received\n", signum);
   zs->resume(signum);
}

int main( int argc, char ** argv )
{
#ifndef WIN32
    // install signal handler
    signal (SIGSTOP, suspend);
#endif

#ifdef DESKTOP
    QApplication a( argc, argv );
#else
    QPEApplication a( argc, argv );
#endif
    appl = &a;

#ifdef DESKTOP
    if (argc >= 3)
    {
#ifndef WIN32
       DeskW = atoi(argv[1]);
       DeskH = atoi(argv[2]);
#else
       int w, h;
       sscanf (argv[1], "%d", &w);
       sscanf (argv[2], "%d", &h);
       DeskW = w;
       DeskH = h;
#endif
    }
    else
    {
       DeskW = 600;
       DeskH = 400;
    }
#else
    DeskW = a.desktop()->width();
    DeskH = a.desktop()->height();

    owarn << "Width: " << DeskW << " Height: " << DeskH << oendl;

#ifdef JPATCH_HDE
    // nothings
#else
    if (DeskW > 240)
    {
       DeskW -= 20;
       DeskH += 25;
       owarn << "Changed width: " << DeskW << " Height: " << DeskH << oendl;
    }
#endif

#endif

    ZSafe mw;
    zs = &mw;

#ifndef WIN32
    signal (SIGCONT, resume);
#endif
#ifdef DESKTOP
    a.setMainWidget(&mw);
    mw.show();
#else
    // a.showMainWidget( &mw );
    a.showMainDocumentWidget( &mw );
#endif
    int ret = a.exec();

    return ret;
}
