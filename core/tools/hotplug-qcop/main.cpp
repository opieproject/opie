/*
                             This file is part of the Opie Project
                             (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

/* OPIE */
#include <qpe/qcopenvelope_qws.h>

/* QT */
#include <qapplication.h>
#include <qstring.h>
#include <qstringlist.h>

/* STD */
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

int main( int argc, char** argv )
{
    qDebug( "NOTE: hotplug-qcop started" );
    QString event( argc > 1 ? QString( argv[1] ) : QString::null );

    QStringList list;
    int i = 0;
    while( environ[i] )
    {
        qDebug( "NOTE: hotplug-qcop adding '%s'", environ[i] );
        list += environ[i++];
    }

    QApplication app( argc, argv );

	if ( 1 )
    {
        QCopEnvelope e( "QPE/System", "HotPlugEvent(QString, QStringList)" );
        e << event << list;
    }

    qDebug( "NOTE: hotplug-qcop ended" );
	return 0;
}
