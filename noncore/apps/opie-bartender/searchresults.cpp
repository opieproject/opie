/****************************************************************************
** Created: Sat Jul 20 08:23:27 2002
**      by:  L.J. Potter <ljp@llornkcor.com>
**     copyright            : (C) 2002 by ljp
    email                : ljp@llornkcor.com
    *   This program is free software; you can redistribute it and/or modify  *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    ***************************************************************************/
#include "searchresults.h"


#include <qlistbox.h>
#include <qlayout.h>

Search_Results::Search_Results( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ) {
    if ( !name )
    setName( drinkName);

    drinkName = name;
    setCaption( drinkName );

    Layout5 = new QGridLayout( this ); 
    Layout5->setSpacing( 6 );
    Layout5->setMargin( 4 );

    ListBox1 = new QListBox( this, "ListBox1" );
    
    Layout5->addMultiCellWidget(  ListBox1, 0, 1, 0, 3 );
    connect( ListBox1, SIGNAL( clicked(QListBoxItem*)), SLOT( listSelected(QListBoxItem*)) );
}

Search_Results::~Search_Results() {
}


void Search_Results::listSelected(QListBoxItem *) {
    
    QDialog::accept();
}

