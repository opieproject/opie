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

using namespace Opie::Ui;

OListViewDemo::OListViewDemo( QWidget* parent, const char* name, WFlags f )
                   :QVBox( parent, name, f )
{
    lv = new ONamedListView( this );
    lv->setRootIsDecorated( true );
    lv->addColumns( QStringList::split( ' ', "Column1 Column2 Column3 Column4" ) );

    ONamedListViewItem* item = new ONamedListViewItem( lv, QStringList::split( ' ', "Text1 Text2 Text3 Text4" ) );
    item->setText( "Column2", "ModifiedText" );
    item->setText( "Column5", "ThisColumnDoesNotExits" );

    new ONamedListViewItem( lv, QStringList::split( ' ', "Text1 Text2 Text3 Text4" ) );
    new ONamedListViewItem( lv, QStringList::split( ' ', "Text1 Text2 Text3 Text4" ) );
    new ONamedListViewItem( lv, QStringList::split( ' ', "Text1 Text2 Text3 Minni" ) );
    item = new ONamedListViewItem( lv, QStringList::split( ' ', "XXX YYY ZZZ ***" ) );
    new ONamedListViewItem( lv, QStringList::split( ' ', "Text1 Text2 Text3 Text4" ) );
    new ONamedListViewItem( lv, QStringList::split( ' ', "Text1 Text2 Text3 Text4" ) );

    new ONamedListViewItem( item, QStringList::split( ' ', "SubText1 Text2 Text3 Text4" ) );
    new ONamedListViewItem( item, QStringList::split( ' ', "SubText1 Text2 Text3 Text4" ) );
    new ONamedListViewItem( item, QStringList::split( ' ', "SubText1 Text2 Text3 Text4" ) );
    item = new ONamedListViewItem( item, QStringList::split( ' ', "Text1 Text2 Text3 HereItComes" ) );
    item = new ONamedListViewItem( item, QStringList::split( ' ', "Text1 Text2 Text3 HereItComesSoon" ) );
    item = new ONamedListViewItem( item, QStringList::split( ' ', "Text1 Text2 Text3 Mickey" ) );

    if ( lv->find( 3, "Mickey", 3 ) )
        qDebug( "found Mickey :-)" );
    else
        qDebug( "did not found Mickey :-(" );

    if ( lv->find( 3, "Minni", 0 ) )
        qDebug( "found Minni :-)" );
    else
        qDebug( "did not found Minni :-(" );

}

OListViewDemo::~OListViewDemo()
{
}

