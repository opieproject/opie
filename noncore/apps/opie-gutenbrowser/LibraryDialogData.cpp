/***************************************************************************
LibraryDialogData.cpp  -  description
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
 ***************************************************************************/
#include "LibraryDialog.h"
#include <qpe/config.h>

#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>

#include <qimage.h>
#include <qpixmap.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qdialog.h>
#include <qcheckbox.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qheader.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qprogressbar.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>


void  LibraryDialog::initDialog(){


    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing(2);
    layout->setMargin(4);

    tabWidget = new QTabWidget( this, "tabWidget" );
    layout->addMultiCellWidget( tabWidget, 0, 0, 0, 3);

    widget_1 = new QWidget( tabWidget, "widget" );
    ListView1 = new QListView( widget_1, "ListView1" );
    QGridLayout *layout1 = new QGridLayout(widget_1 );

    widget_2 = new QWidget( tabWidget, "widget_2" );
    ListView2 = new QListView( widget_2, "ListView2" );
    QGridLayout *layout2 = new QGridLayout(widget_2 );

    widget_3 = new QWidget( tabWidget, "widget_3" );
    ListView3 = new QListView( widget_3, "ListView3" );
    QGridLayout *layout3 = new QGridLayout(widget_3 );

    widget_4 = new QWidget( tabWidget, "widget_4" );
    ListView4 = new QListView( widget_4, "ListView4" );
    QGridLayout *layout4 = new QGridLayout(widget_4 );

    widget_5 = new QWidget( tabWidget, "widget_5" );
    ListView5 = new QListView( widget_5, "ListView5" );
    QGridLayout *layout5 = new QGridLayout(widget_5 );


    ListView1->addColumn( tr( "Title" ) );
    ListView1->setColumnWidthMode( 0, QListView::Manual );
    ListView1->setColumnWidth(0, 330);
    ListView1->setColumnAlignment( 0, 1 );

    ListView1->addColumn( tr( "Author" ) );
    ListView1->setColumnWidthMode( 1, QListView::Manual );
    ListView1->setColumnWidth(1, 170);
    ListView1->setColumnAlignment( 1, 1 );

    ListView1->addColumn( tr( "Year" ) );
    ListView1->setColumnWidthMode( 2, QListView::Manual );
    ListView1->setColumnWidth(2, 50);
    ListView1->setColumnAlignment( 2, 1 );

    ListView1->addColumn( tr( "File" ) );
    ListView1->setColumnWidthMode( 2, QListView::Manual );
    ListView1->setColumnWidth(2, 100);
    ListView1->setColumnAlignment( 2, 1 );

    ListView2->addColumn( tr( "Title" ) );
    ListView2->setColumnWidthMode( 0, QListView::Manual );
    ListView2->setColumnWidth(0, 330);
    ListView2->setColumnAlignment( 0, 1 );

    ListView2->addColumn( tr( "Author" ) );
    ListView2->setColumnWidthMode( 1, QListView::Manual );
    ListView2->setColumnWidth(1, 170);
    ListView2->setColumnAlignment( 1, 1 );

    ListView2->addColumn( tr( "Year" ) );
    ListView2->setColumnWidthMode( 2, QListView::Manual );
    ListView2->setColumnWidth(2, 50);
    ListView2->setColumnAlignment( 2, 1 );

    ListView2->addColumn( tr( "File" ) );
    ListView2->setColumnWidthMode( 3, QListView::Manual );
    ListView2->setColumnWidth(3, 100);
    ListView2->setColumnAlignment( 3, 1 );

    ListView3->addColumn( tr( "Title" ) );
    ListView3->setColumnWidthMode( 0, QListView::Manual );
    ListView3->setColumnWidth(0, 330);
    ListView3->setColumnAlignment( 0, 1 );

    ListView3->addColumn( tr( "Author" ) );
    ListView3->setColumnWidthMode( 1, QListView::Manual );
    ListView3->setColumnWidth(1, 170);
    ListView3->setColumnAlignment( 1, 1 );

    ListView3->addColumn( tr( "Year" ) );
    ListView3->setColumnWidthMode( 2, QListView::Manual );
    ListView3->setColumnWidth(2, 50);
    ListView3->setColumnAlignment( 2, 1 );

    ListView3->addColumn( tr( "File" ) );
    ListView3->setColumnWidthMode( 3, QListView::Manual );
    ListView3->setColumnWidth(3, 100);
    ListView3->setColumnAlignment( 3, 1 );


    ListView4->addColumn( tr( "Title" ) );
    ListView4->setColumnWidthMode( 0, QListView::Manual );
    ListView4->setColumnWidth(0, 330);
    ListView4->setColumnAlignment( 0, 1 );

    ListView4->addColumn( tr( "Author" ) );
    ListView4->setColumnWidthMode( 1, QListView::Manual );
    ListView4->setColumnWidth(1, 170);
    ListView4->setColumnAlignment( 1, 1 );

    ListView4->addColumn( tr( "Year" ) );
    ListView4->setColumnWidthMode( 2, QListView::Manual );
    ListView4->setColumnWidth(2, 50);
    ListView4->setColumnAlignment( 2, 1 );

    ListView4->addColumn( tr( "File" ) );
    ListView4->setColumnWidthMode( 3, QListView::Manual );
    ListView4->setColumnWidth(3, 100);
    ListView4->setColumnAlignment( 3, 1 );

    ListView5->addColumn( tr( "Title" ) );
    ListView5->setColumnWidthMode( 0, QListView::Manual );
    ListView5->setColumnWidth(0, 330);
    ListView5->setColumnAlignment( 0, 1 );

    ListView5->addColumn( tr( "Author" ) );
    ListView5->setColumnWidthMode( 1, QListView::Manual );
    ListView5->setColumnWidth(1, 170);
    ListView5->setColumnAlignment( 1, 1 );

    ListView5->addColumn( tr( "Year" ) );
    ListView5->setColumnWidthMode( 2, QListView::Manual );
    ListView5->setColumnWidth(2, 50);
    ListView5->setColumnAlignment( 2, 1 );

    ListView5->addColumn( tr( "File" ) );
    ListView5->setColumnWidthMode( 3, QListView::Manual );
    ListView5->setColumnWidth(3, 100);
    ListView5->setColumnAlignment( 3, 1 );

    tabWidget->insertTab( widget_1, tr( "A-F" ) );
    tabWidget->insertTab( widget_2, tr( "G-M" ) );
    tabWidget->insertTab( widget_3, tr( "N-R" ) );
    tabWidget->insertTab( widget_4, tr( "S-Z" ) );
    tabWidget->insertTab( widget_5, tr( "   " ) );

    ListView1->setMultiSelection(TRUE);
    ListView2->setMultiSelection(TRUE);
    ListView3->setMultiSelection(TRUE);
    ListView4->setMultiSelection(TRUE);
    ListView5->setMultiSelection(TRUE);

    widget_6 = new QWidget( tabWidget, "widget_6" );
    tabWidget->insertTab(widget_6,tr("Options"));

    ListView1->setSorting( 2, TRUE);
    ListView2->setSorting( 2, TRUE);
    ListView3->setSorting( 2, TRUE);
    ListView4->setSorting( 2, TRUE);
    ListView5->setSorting( 2, TRUE);
    ListView1->setAllColumnsShowFocus( TRUE );
    ListView2->setAllColumnsShowFocus( TRUE );
    ListView3->setAllColumnsShowFocus( TRUE );
    ListView4->setAllColumnsShowFocus( TRUE );
    ListView5->setAllColumnsShowFocus( TRUE );

    QGridLayout *layout6 = new QGridLayout(widget_6 );
    QComboBox * sortingCombo;
    buttonCancel = new QPushButton( widget_6, "buttonCancel" );
    checkBox = new QCheckBox( ( tr("Open Automatically")), widget_6);
    checkBox->setChecked( FALSE);
//      httpBox = new QCheckBox( ( tr("Use http")),widget_6);
//      httpBox->setChecked( FALSE);
//      QToolTip::add( httpBox, ( tr("Use http to download \nproxy users should probably use this.")) );
    authBox= new QCheckBox( ( tr("Last name first \n(requires library restart)")),widget_6);
    authBox->setChecked( FALSE);

    layout1->addMultiCellWidget( ListView1, 0, 0, 0, 4);
    layout2->addMultiCellWidget( ListView2, 0, 0, 0, 4);
    layout3->addMultiCellWidget( ListView3, 0, 0, 0, 4);
    layout4->addMultiCellWidget( ListView4, 0, 0, 0, 4);
    layout5->addMultiCellWidget( ListView5, 0, 0, 0, 4);

    buttonSearch = new QPushButton(this,"buttonSearch");
    buttonLibrary=new QPushButton(this,"buttenLibrary");
    buttonNewList=new QPushButton(this,"NewList");
    moreInfoButton= new QPushButton(this,"moreInfo");
    sortingCombo=new QComboBox(widget_6,"sort by combo");


    layout6->addMultiCellWidget(buttonCancel, 0, 0, 4, 4);
    layout6->addMultiCellWidget(checkBox, 1, 1, 0, 0);
    layout6->addMultiCellWidget(sortingCombo, 2, 2, 0, 0);
    layout6->addMultiCellWidget(authBox, 3, 3, 0, 0);

    layout->addMultiCellWidget( buttonSearch, 1, 1, 0, 0);
    layout->addMultiCellWidget( buttonLibrary, 1, 1, 1, 1);
    layout->addMultiCellWidget( moreInfoButton, 1, 1, 2, 2);
    layout->addMultiCellWidget( buttonNewList, 1, 1, 3, 3);

    moreInfoButton->setText("Info");
    moreInfoButton->setDisabled(TRUE);
    buttonSearch->setDisabled(TRUE);

    buttonSearch->setText(tr("Search"));

    buttonCancel->setText(tr("Close"));
    buttonLibrary->setText("Load");
    buttonLibrary->setDefault(TRUE);
    buttonNewList->setText("New List");

    QStrList sortingList;
    sortingList.append( "Sort by Number");
    sortingList.append("Sort by Title");
    sortingList.append( "Sort by Author");
    sortingList.append("Sort by Year");

    sortingCombo->insertStrList(sortingList,0);
    sortingCombo->setCurrentItem(2);
    sortingCombo->setMaximumWidth(180);

      // signals and slots connections
    connect(buttonSearch,SIGNAL(clicked()),this,SLOT(onButtonSearch()));
    connect(buttonLibrary,SIGNAL(clicked()),this,SLOT(FindLibrary()));
    connect(buttonCancel,SIGNAL(clicked()),this,SLOT(reject()));
    connect(moreInfoButton,SIGNAL(clicked()),this,SLOT(moreInfo()));
    connect(buttonNewList,SIGNAL(clicked()),this,SLOT(newList()));

    connect(ListView1,SIGNAL(doubleClicked(QListViewItem*)),SLOT(select_title(QListViewItem*)));
    connect(ListView1,SIGNAL(returnPressed(QListViewItem*)),SLOT(select_title(QListViewItem*)));
    connect(ListView2,SIGNAL(doubleClicked(QListViewItem*)),SLOT(select_title(QListViewItem*)));
    connect(ListView2,SIGNAL(returnPressed(QListViewItem*)),SLOT(select_title(QListViewItem*)));
    connect(ListView3,SIGNAL(doubleClicked(QListViewItem*)),SLOT(select_title(QListViewItem*)));
    connect(ListView3,SIGNAL(returnPressed(QListViewItem*)),SLOT(select_title(QListViewItem*)));
    connect(ListView4,SIGNAL(doubleClicked(QListViewItem*)),SLOT(select_title(QListViewItem*)));
    connect(ListView4,SIGNAL(returnPressed(QListViewItem*)),SLOT(select_title(QListViewItem*)));
    connect(ListView5,SIGNAL(doubleClicked(QListViewItem*)),SLOT(select_title(QListViewItem*)));
    connect(ListView5,SIGNAL(returnPressed(QListViewItem*)),SLOT(select_title(QListViewItem*)));

    connect(sortingCombo,SIGNAL(activated(int)),SLOT(comboSelect(int)));

}

void LibraryDialog::doListView() {

}
