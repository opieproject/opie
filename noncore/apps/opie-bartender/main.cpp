/**      by:  L.J. Potter <ljp@llornkcor.com>
**     copyright            : (C) 2002 by ljp
    email                : ljp@llornkcor.com
    *   This program is free software; you can redistribute it and/or modify  *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    ***************************************************************************/

#include "bartender.h"
#include <qpe/qpeapplication.h>

//Bartender  *bart;

int main( int argc, char ** argv )
{
    QPEApplication a( argc, argv );

    Bartender bart;
    a.showMainWidget( &bart );

    return a.exec();
}
