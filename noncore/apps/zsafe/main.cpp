/*
** Author: Carsten Schneider <CarstenSchneider@t-online.de>
**
** $Id: main.cpp,v 1.8 2004-11-15 17:09:35 zecke Exp $
**
** Homepage: http://home.t-online.de/home/CarstenSchneider/zsafe/index.html
*/


#include "zsafe.h"

/* OPIE */
#include <opie2/odebug.h>

/* QT */
#include <opie2/oapplicationfactory.h>

void suspend (int signum)
{
   printf ("Suspend signal %d received\n", signum);
}

void resume (int signum)
{
   printf ("Resume signal %d received\n", signum);
   zs->resume(signum);
}


OPIE_EXPORT_APP( Opie::Core::OApplicationFactory<ZSafe> )

