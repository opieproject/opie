/*
                            This file is part of the Opie Project
             =.             Copyright (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
           .=l.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
          .>+-=
_;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=         redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :          the terms of the GNU General Public
.="- .-=="i,     .._        License as published by the Free Software
- .   .-<_>     .<>         Foundation; version 2 of the License.
    ._= =}       :
   .%`+i>       _;_.
   .i_,=:_.      -<s.       This program is distributed in the hope that
    +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
   : ..    .:,     . . .    without even the implied warranty of
   =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;     General Public License for more
++=   -.     .`     .:      details.
:     =  ...= . :.=-
-.   .:....=;==+<;          You should have received a copy of the GNU
 -_. . .   )=.  =           General Public License along with
   --        :-=`           this application; see the file LICENSE.GPL.
                            If not, write to the Free Software Foundation,
                            Inc., 59 Temple Place - Suite 330,
                            Boston, MA 02111-1307, USA.
*/

#include "devicesinfo.h"

/* OPIE */
#include <opie2/olistview.h>
#include <qpe/qpeapplication.h>

/* QT */
#include <qcombobox.h>
#include <qfile.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtextview.h>
#include <qtimer.h>
#include <qwhatsthis.h>

using namespace Opie::Ui;

DevicesView::DevicesView( QWidget* parent,  const char* name, WFlags fl )
        : OListView( parent, name, fl )
{
    addColumn( tr( "Module" ) );
    setAllColumnsShowFocus( true );
    setRootIsDecorated( true );
    QWhatsThis::add( this, tr( "This is a list of all the devices currently recognized on this device." ) );
}

DevicesView::~DevicesView()
{
}

DevicesInfo::DevicesInfo( QWidget* parent,  const char* name, WFlags fl )
        : QWidget( parent, name, fl )
{
    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 4 );
    layout->setMargin( 4 );

    view = new DevicesView( this );
    
    layout->addMultiCellWidget( view, 0, 0, 0, 1 );
}

DevicesInfo::~DevicesInfo()
{
}
