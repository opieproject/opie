/***************************************************************************
 main.cpp  -  main routine
 ***************************************************************************/
//// main.cpp
////  copyright 2001, 2002, by L. J. Potter <ljp@llornkcor.com>
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "qtrec.h"
#include <qpe/qpeapplication.h>

#ifdef PDAUDIO
int main(int argc, char* argv[]) {
    QPEApplication a(argc, argv);
    QtRec qtrec;
    a.showMainWidget( &qtrec);
 return a.exec();
}


#else
#include <opie/oapplicationfactory.h>
OPIE_EXPORT_APP( OApplicationFactory<QtRec> )

#endif


