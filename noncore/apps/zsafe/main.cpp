/*
** Author: Carsten Schneider <CarstenSchneider@t-online.de>
**
** $Id: main.cpp,v 1.7 2004-07-21 20:44:12 llornkcor Exp $
**
** Homepage: http://home.t-online.de/home/CarstenSchneider/zsafe/index.html
*/


#include "zsafe.h"

#ifndef NO_OPIE
/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;
#endif

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
#ifndef Q_WS_WIN
    // install signal handler
//    signal (SIGSTOP, suspend);
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
#ifndef Q_WS_WIN
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

#ifndef NO_OPIE
    owarn << "Width: " << DeskW << " Height: " << DeskH << oendl;
#endif
#ifdef JPATCH_HDE
    // nothings
#else
    if (DeskW > 240)
    {
       DeskW -= 20;
       DeskH += 25;
#ifndef NO_OPIE
       owarn << "Changed width: " << DeskW << " Height: " << DeskH << oendl;
#endif
    }
#endif

#endif

    ZSafe *mw = new ZSafe( 0, 0, QWidget::WDestructiveClose );
//    ZSafe mw;
//    zs = &mw;
    zs = mw;
		
#ifndef Q_WS_WIN
//    signal (SIGCONT, resume);
#endif
#ifdef DESKTOP
    a.setMainWidget(mw);
    mw->show();
#else
    // a.showMainWidget( &mw );
    a.showMainDocumentWidget( mw );
#endif
    int ret = a.exec();

    return ret;
}
