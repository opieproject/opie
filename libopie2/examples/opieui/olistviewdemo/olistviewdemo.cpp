/*
                             This file is part of the Opie Project

                             Copyright (C) 2003 Michael 'Mickey' Lauer
                             <mickey@tm.informatik.uni-frankfurt.de>
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

#include "olistviewdemo.h"
#include <opie2/olistview.h>

#include <qstring.h>
#include <qpixmap.h>
#include <qlistview.h>

OListViewDemo::OListViewDemo( QWidget* parent, const char* name, WFlags f )
                   :QVBox( parent, name, f )
{
    lv = new ONamedListView( this );
    lv->addColumns( QStringList::split( ' ', "Column1 Column2 Column3 Column4" ) );

    ONamedListViewItem* item = new ONamedListViewItem( lv, QStringList::split( ' ', "Text1 Text2 Text3 Text4" ) );
    item->setText( "Column2", "ModifiedText" );
    item->setText( "Column5", "ThisColumnDoesNotExits" );
}

OListViewDemo::~OListViewDemo()
{
}

