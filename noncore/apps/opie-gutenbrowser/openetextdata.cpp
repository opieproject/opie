/***************************************************************************
                          openetextdata.cpp  -  description
                             -------------------
    begin                : Sat Dec 4 1999
    begin                : Tue Jul 25 2000
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "openetext.h"
#include <qtooltip.h>
#include <qlayout.h>
#include <qpe/config.h>

void  OpenEtext::initDialog()
{
  this->setCaption("gutenbrowser local library");

    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 4 );
    layout->setMargin( 4 );

//  local_index = local_library +"PGWHOLE.TXT";
    local_index = local_library +"GUTINDEX.ALL";

    QListBox_1= new QListBox(this,"ListBox");
    QListBox_1->setAutoUpdate( TRUE );

    OpenButton = new QPushButton( this, "OpenButton" );
    OpenButton->setFocusPolicy( QWidget::TabFocus );
    OpenButton->setText("&Open");
    OpenFileButton = new QPushButton( this, "addButton" );
    OpenFileButton->setText("&Import");

    RemoveButton = new QPushButton( this, "removeButton" );
    RemoveButton->setFocusPolicy( QWidget::TabFocus );
    RemoveButton->setText("&Delete");

    QPushButton * scanButton;
    scanButton= new QPushButton( this, "scanButton" );
    scanButton->setFocusPolicy( QWidget::TabFocus );

    scanButton->setText("&Sort");
    editButton= new QPushButton(this,"EditBtn");
    editButton->setText("&Edit");

    QLabel_1= new QLabel(this,"label");

    layout->addMultiCellWidget( OpenButton, 0, 0, 0, 0 );
    layout->addMultiCellWidget( OpenFileButton, 0, 0, 1, 1 );
    layout->addMultiCellWidget( RemoveButton, 0, 0, 2, 2 );
    layout->addMultiCellWidget( scanButton, 0, 0, 3, 3 );
    layout->addMultiCellWidget( editButton, 0, 0, 5, 5 );

    layout->addMultiCellWidget( QLabel_1, 1, 1, 0, 5 );
    layout->addMultiCellWidget( QListBox_1, 2, 2, 0, 5 );

    OpenButton->setFixedHeight(22);
    OpenFileButton->setFixedHeight(22);
    RemoveButton->setFixedHeight(22);
    scanButton->setFixedHeight(22);
    editButton->setFixedHeight(22);

	connect( OpenButton, SIGNAL(released()), SLOT( OpenTitle()) );
    connect( OpenFileButton, SIGNAL(released()), SLOT(open()) );
    connect( RemoveButton, SIGNAL(released()), SLOT(remove()) );
    connect( scanButton, SIGNAL(released()), SLOT( scan()) );
    connect( editButton, SIGNAL(released()),SLOT(editTitle()));
}
