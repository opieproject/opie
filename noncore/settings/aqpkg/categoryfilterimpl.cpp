/***************************************************************************
                          categoryfilterimpl.cpp  -  description
                             -------------------
    begin                : Sun Nov 17 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
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
