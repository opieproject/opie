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
 
#include <qlistbox.h>
#include <qstring.h>

#include "categoryfilterimpl.h"

CategoryFilterImpl :: CategoryFilterImpl(const QString &categories, const QString &selectedCategories, QWidget *parent, const char *name )
    : CategoryFilterDlg(parent,name, true)
{
    // Split up categories and add them to the listbox
    int start = 1;

    QString item;
    int end;
    do
    {
        end = categories.find( "#", start );
        item = categories.mid( start, end - start );
        if ( item != "" )
        {
            lstCategories->insertItem( item );

            if ( selectedCategories.find( "#" + item + "#" ) != -1 )
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
            ret += lstCategories->text( i ) + "#";
    }

    if ( ret == "#" )
        ret = "";
    return ret;
}
