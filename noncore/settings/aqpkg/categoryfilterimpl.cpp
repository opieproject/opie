/*
                             This file is part of the OPIE Project
                             
               =.            Copyright (c)  2002 Andy Qua <andy.qua@blueyonder.co.uk>
             .=l.                                Dan Williams <drw@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <iostream>
using namespace std;

#include <qgroupbox.h>
#include <qlayout.h> 
#include <qlistbox.h>
#include <qstring.h>

#include "categoryfilterimpl.h"

CategoryFilterImpl :: CategoryFilterImpl(const QString &categories, const QString &selectedCategories, QWidget *parent, const char *name )
    : QDialog( parent, name, true )
{
    setCaption( tr( "Category Filter" ) );

    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setMargin( 2 );
    layout->setSpacing( 4 );

    QGroupBox *grpbox = new QGroupBox( 0, Qt::Vertical, tr( "Select one or more groups" ), this );
    grpbox->layout()->setSpacing( 2 );
    grpbox->layout()->setMargin( 4 );
    layout->addWidget( grpbox );

    QVBoxLayout *grplayout = new QVBoxLayout( grpbox->layout() );
    
    lstCategories = new QListBox( grpbox );
    lstCategories->setSelectionMode( QListBox::Multi );
    grplayout->addWidget( lstCategories );
    
    // Split up categories and add them to the listbox
    int start = 1;

    QString item;
    int end;
    QString finditem;
    do
    {
        end = categories.find( "#", start );
        item = categories.mid( start, end - start );
        if ( item != "" )
        {
            lstCategories->insertItem( item );
            finditem = QString( "#%1#" ).arg( item );
            if ( selectedCategories.find( finditem ) != -1 )
                lstCategories->setSelected( lstCategories->count()-1, true );
        }

        start = end + 1;
    } while ( start < (int)categories.length() );

    lstCategories->sort( true );

    showMaximized();
}

CategoryFilterImpl :: ~CategoryFilterImpl()
{
}

QString CategoryFilterImpl :: getSelectedFilter()
{
    // Grab cetegories from listbox
    QString ret = "#";

    for ( int i = 0 ; i < (int)lstCategories->count() ; ++i )
    {
        if ( lstCategories->isSelected( i ) )
        {
            ret.append( lstCategories->text( i ) );
            ret.append( "#" );
        }
    }

    if ( ret == "#" )
        ret = "";
    return ret;
}
