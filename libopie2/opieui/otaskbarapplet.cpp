/*
                             This file is part of the Opie Project

                             Copyright (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
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
#include <opie2/otaskbarapplet.h>

#include <qpe/qpeapplication.h>
#include <qframe.h>

using namespace Opie::Ui;


/**
 * \todo no example yet!!!
 * If you want to implement an Applet for the Opie Taskbar
 * use this interface.
 * The only specail thing about applets is that you need to build
 * it as plugin/library and do EXPORT_OPIE_APPLET_v1( YourApplet )
 * at the bottom of your application. This takes care of
 * the activation and implementing the TaskbarAppletInterface.
 * You also need to add a static int position() functions to your
 * application.
 * \code
 *    class MyApplet : public OTaskBarApplet {
 *     public:
 *      static int position() { return 3: }
 *      void doStuff() {
 *          popup( myWidget );
 *      }
 *    };
 *    EXPORT_OPIE_APPLET_v1( MyApplet )
 * \endcode
 *
 * @author Michael Lauer
 * @version 0.5
 * @see TaskbarAppletInterface
 */
OTaskbarApplet::OTaskbarApplet( QWidget* parent, const char* name  )
    :QWidget( parent, name ){
    setFixedHeight( 18 );
    setFixedWidth( 14 );
}

OTaskbarApplet::~OTaskbarApplet(){
}

void OTaskbarApplet::popup( QWidget* widget ){
    QPoint curPos = mapToGlobal( QPoint( 0, 0 ) );
    int w = widget->sizeHint().width();
    int x = curPos.x() - (w/2 );
    if ( (x+w) > QPEApplication::desktop()->width() )
        x = QPEApplication::desktop()->width()-w;
    widget->move( x, curPos.y()-widget->sizeHint().height() );
    widget->show();
}

