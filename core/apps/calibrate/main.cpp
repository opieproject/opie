/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "calibrate.h"

#include <qfile.h>
#include <qpe/qpeapplication.h>

#ifdef QWS
#include <qwindowsystem_qws.h>
#endif

int main( int argc, char ** argv )
{
    QPEApplication a( argc, argv );
    int retval = 0;

#ifdef QWS
    if ( QWSServer::mouseHandler() &&
         QWSServer::mouseHandler() ->inherits("QCalibratedMouseHandler") ) {
#endif
	// Make sure calibration widget starts on top.
	Calibrate cal;
        a.setMainWidget(&cal);
	a.showMainWidget(&cal);
	return a.exec();
#ifdef QWS
    }
#endif
}
