/*
                             This file is part of the Opie Project
              =.             (C) 2000-2002 Trolltech AS
            .=l.             (C) 2002-2005 The Opie Team <opie-devel@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
    :`=1 )Y*s>-.--   :       the terms of the GNU Library General Public
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
#include "wait.h"

#include <opie2/owait.h>
#include <opie2/oresource.h>
using namespace Opie::Core;

#include <qtopia/config.h>
#include <qtopia/applnk.h>

Wait *lastWaitObject = NULL;


using namespace Opie::Ui;
Wait::Wait( QWidget *parent ) : QWidget( parent ),
                                waiting( FALSE )
{

    QSize size( AppLnk::smallIconSize(), AppLnk::smallIconSize() );
    setFixedSize( size );
    pm = OResource::loadPixmap( "wait", OResource::SmallIcon );

    lastWaitObject = this;
    m_centralWait = new OWait( 0l );
    m_centralWait->hide();
    hide();
}

Wait::~Wait()
{
    delete m_centralWait;
}


Wait *Wait::getWaitObject()
{
    return lastWaitObject;
}


void Wait::setWaiting( bool w )
{
    Config cfg ( "Launcher" );
    cfg.setGroup("GUI");


    waiting = w;
    if ( w ) {
        if ( cfg. readBoolEntry( "BigBusy" ) )
            m_centralWait->show();
        else
            show();
    }else{
        m_centralWait->hide();
	hide();
    }
}


void Wait::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    p.drawPixmap( 0, 0, pm );
}


