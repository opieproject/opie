/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 <>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
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
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "taskeditoralarms.h"

#include <qpe/resource.h>

#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qwhatsthis.h>

TaskEditorAlarms::TaskEditorAlarms( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    QGridLayout *layout = new QGridLayout( this, 2, 3, 4, 4 ); 

    lstAlarms = new QListView( this );
    layout->addMultiCellWidget( lstAlarms, 0, 0, 0, 2 );

    QPushButton *btn = new QPushButton( Resource::loadPixmap( "new" ), tr( "New" ), this );
    //QWhatsThis::add( btn, tr( "Click here to add a new transaction." ) );
    //connect( btn, SIGNAL( clicked() ), this, SLOT( slotNew() ) );
    layout->addWidget( btn, 1, 0 );

    btn = new QPushButton( Resource::loadPixmap( "edit" ), tr( "Edit" ), this );
    //QWhatsThis::add( btn, tr( "Select a transaction and then click here to edit it." ) );
    //connect( btn, SIGNAL( clicked() ), this, SLOT( slotEdit() ) );
    layout->addWidget( btn, 1, 1 );

    btn = new QPushButton( Resource::loadPixmap( "trash" ), tr( "Delete" ), this );
    //QWhatsThis::add( btn, tr( "Select a checkbook and then click here to delete it." ) );
    //connect( btn, SIGNAL( clicked() ), this, SLOT( slotDelete() ) );
    layout->addWidget( btn, 1, 2 );
}

TaskEditorAlarms::~TaskEditorAlarms()
{
}

void TaskEditorAlarms::slotNew()
{
}

void TaskEditorAlarms::slotEdit()
{
}

void TaskEditorAlarms::slotDelete()
{
}
