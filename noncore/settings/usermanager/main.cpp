/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "usermanager.h"
#include <qpe/qpeapplication.h>

int main( int argc, char ** argv )
{
	QPEApplication a( argc, argv );
	UserConfig mw(0,0,0);
	a.showMainWidget( &mw );
	return a.exec();
}
