/***************************************************************************
   advancedfmData.cpp
                             -------------------
** Created: Mon 09-23-2002 13:24:11
    copyright            : (C) 2002 by ljp
    email                : ljp@llornkcor.com
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "advancedfm.h"

#include <opie/otabwidget.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qpe/resource.h>

#include <qlayout.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qpopupmenu.h>
#include <qtabwidget.h>
#include <qtoolbutton.h>
#include <qlineedit.h>
#include <qlistview.h>

#include <sys/utsname.h>


void AdvancedFm::init() {
#if defined(QT_QWS_OPIE)
    qDebug("<<<<<<<<<<<<<<<<<<<<<<<< OPIE!!!");
#endif
    setCaption( tr( "AdvancedFm" ) );

  QGridLayout *layout = new QGridLayout( this );
  layout->setSpacing( 2);
  layout->setMargin( 2);


  QPEMenuBar *menuBar = new QPEMenuBar(this);
  fileMenu = new QPopupMenu( this );
  viewMenu  = new QPopupMenu( this );

  layout->addMultiCellWidget( menuBar, 0, 0, 0, 1 );

  menuBar->insertItem( tr( "File" ), fileMenu);
  menuBar->insertItem( tr( "View" ), viewMenu);

  qpeDirButton= new QToolButton(this,"QPEButton");
  qpeDirButton->setPixmap( Resource::loadPixmap("launcher/opielogo16x16"));//,"",this,"QPEButton");
  qpeDirButton ->setFixedSize( QSize( 20, 20 ) );
  layout->addMultiCellWidget( qpeDirButton , 0, 0, 2, 2);

  cfButton = new QToolButton( this,"CFButton");
  cfButton->setPixmap(Resource::loadPixmap("cardmon/pcmcia"));
  cfButton ->setFixedSize( QSize( 20, 20 ) );
  layout->addMultiCellWidget( cfButton , 0, 0, 3, 3);

  sdButton = new QToolButton( this,"SDButton");
  sdButton->setPixmap(Resource::loadPixmap("advancedfm/sdcard"));
  sdButton->setFixedSize( QSize( 20, 20 ) );
  layout->addMultiCellWidget( sdButton , 0, 0, 4, 4);

  cdUpButton = new QToolButton( this,"cdUpButton");
  cdUpButton->setPixmap(Resource::loadPixmap("up"));
  cdUpButton ->setFixedSize( QSize( 20, 20 ) );
  layout->addMultiCellWidget( cdUpButton , 0, 0, 5, 5);

  docButton = new QToolButton( this,"docsButton");
  docButton->setPixmap(Resource::loadPixmap("DocsIcon"));
  docButton->setFixedSize( QSize( 20, 20 ) );
  layout->addMultiCellWidget( docButton, 0, 0, 6, 6);

  homeButton = new QToolButton( this,"homeButton");
  homeButton->setPixmap(Resource::loadPixmap("home"));
  homeButton->setFixedSize( QSize( 20, 20 ) );
  layout->addMultiCellWidget( homeButton, 0, 0, 7, 7);

  fileMenu->insertItem( tr( "Show Hidden Files" ), this,  SLOT( showMenuHidden() ));
  fileMenu->setItemChecked( fileMenu->idAt(0),TRUE);
  fileMenu->insertSeparator();
  fileMenu->insertItem( tr( "Make Directory" ), this, SLOT( mkDir() ));
  fileMenu->insertItem( tr( "Rename" ), this, SLOT( rn() ));
  fileMenu->insertItem( tr( "Run Command" ), this, SLOT( runCommandStd() ));
  fileMenu->insertItem( tr( "Run Command with Output" ), this, SLOT( runCommand() ));
  fileMenu->insertSeparator();
  fileMenu->insertItem( tr( "Select All" ), this, SLOT( selectAll() ));
  fileMenu->insertItem( tr( "Add To Documents" ), this, SLOT( addToDocs() ));
  fileMenu->insertItem( tr( "Delete" ), this, SLOT( del() ));
  fileMenu->setCheckable(TRUE);

  viewMenu->insertItem( tr( "Switch to Local" ), this, SLOT( switchToLocalTab() ));
  viewMenu->insertItem( tr( "Switch to Remote" ), this, SLOT( switchToRemoteTab() ));
  viewMenu->insertSeparator();
  viewMenu->insertItem( tr( "About" ), this, SLOT( doAbout() ));
  viewMenu->setCheckable(TRUE);


  currentPathCombo = new QComboBox( FALSE, this, "currentPathCombo" );
  currentPathCombo->setEditable(TRUE);
  layout->addMultiCellWidget( currentPathCombo, 1, 1, 0, 7);
  currentPathCombo->lineEdit()->setText( currentDir.canonicalPath());

  currentPathCombo->lineEdit()->setText( currentDir.canonicalPath());

  layout->addMultiCellWidget( currentPathCombo, 1, 1, 0, 7);


  TabWidget = new OTabWidget( this, "TabWidget",/* OTabWidget::Global | */OTabWidget::IconTab);
//  TabWidget = new QTabWidget( this, "TabWidget" );
  layout->addMultiCellWidget( TabWidget, 2, 2, 0, 7);

  tab = new QWidget( TabWidget, "tab" );
  tabLayout = new QGridLayout( tab );
  tabLayout->setSpacing( 2);
  tabLayout->setMargin( 2);

  Local_View = new QListView( tab, "Local_View" );
  Local_View->addColumn( tr("File"),130);
  Local_View->addColumn( tr("Size"),-1);
  Local_View->setColumnAlignment(1,QListView::AlignRight);
  Local_View->addColumn( tr("Date"),-1);
  Local_View->setColumnAlignment(2,QListView::AlignRight);
  Local_View->setAllColumnsShowFocus(TRUE);
  Local_View->setMultiSelection( TRUE );
  Local_View->setSelectionMode(QListView::Extended);

  QPEApplication::setStylusOperation( Local_View->viewport(),QPEApplication::RightOnHold);

  tabLayout->addWidget( Local_View, 0, 0 );

  TabWidget->addTab( tab,"advancedfm/smFileBrowser.png", tr("1"));
//  TabWidget->insertTab( tab, tr("1"));

  tab_2 = new QWidget( TabWidget, "tab_2" );
  tabLayout_2 = new QGridLayout( tab_2 );
  tabLayout_2->setSpacing( 2);
  tabLayout_2->setMargin( 2);

  Remote_View = new QListView( tab_2, "Remote_View" );
  Remote_View->addColumn( tr("File"),130);
  Remote_View->addColumn( tr("Size"),-1);
  Remote_View->setColumnAlignment(1,QListView::AlignRight);
  Remote_View->addColumn( tr("Date"),-1);
  Remote_View->setColumnAlignment(2,QListView::AlignRight);
  Remote_View->setAllColumnsShowFocus(TRUE);
  Remote_View->setMultiSelection( TRUE );
  Remote_View->setSelectionMode(QListView::Extended);


  QPEApplication::setStylusOperation( Remote_View->viewport(),QPEApplication::RightOnHold);

  tabLayout_2->addWidget( Remote_View, 0, 0 );

  TabWidget->addTab( tab_2, "advancedfm/smFileBrowser.png",tr( "2"));
//  TabWidget->insertTab( tab_2, tr( "2"));

  /*     tab_3 = new QWidget( TabWidget, "tab_3" );
         tabLayout_3 = new QGridLayout( tab_3 );
         tabLayout_3->setSpacing( 2);
         tabLayout_3->setMargin( 2);


         //     OFileDialog fileDialog;
         // fileDialog;
         //    fileSelector = new FileSelector( "*",tab_3, "fileselector" , FALSE, FALSE); //buggy
         //     fileDialog = new OFileDialog("bangalow", tab_3, 4, 2, "Bungalow");
         //      OFileSelector fileDialog = new OFileSelector( tab_3, 4, 2,"/");

         QListView *fileTree;
         fileTree = new QListView( tab_3, "tree" );

      
         tabLayout_3->addMultiCellWidget( fileTree, 0, 0, 0, 3 );

         TabWidget->insertTab( tab_3, tr( "Remote" ) );
  */
         
  ///////////////

  struct utsname name; /* check for embedix kernel running on the zaurus*/
  if (uname(&name) != -1) {
      QString release=name.release;
      if(release.find("embedix",0,TRUE) !=-1) {
          zaurusDevice=TRUE;
     } else {
          zaurusDevice=FALSE;
          sdButton->hide();
      }
  }
  
  currentDir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
  currentDir.setPath( QDir::currentDirPath());

  currentRemoteDir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
  currentRemoteDir.setPath( QDir::currentDirPath());

  b = TRUE;

  filterStr="*";
  b=FALSE;
  TabWidget->setCurrentTab(0);
   
}

void AdvancedFm::initConnections() {
  connect( qApp,SIGNAL( aboutToQuit()),SLOT( cleanUp()) );
  connect( qpeDirButton ,SIGNAL(released()),this,SLOT( QPEButtonPushed()) );
  connect( cfButton ,SIGNAL(released()),this,SLOT( CFButtonPushed()) );
  connect( sdButton ,SIGNAL(released()),this,SLOT( SDButtonPushed()) );
  connect( cdUpButton ,SIGNAL(released()),this,SLOT( upDir()) );
  connect( docButton,SIGNAL(released()),this,SLOT( docButtonPushed()) );
  connect(homeButton,SIGNAL(released()),this,SLOT(homeButtonPushed()) );
  connect( currentPathCombo, SIGNAL( activated( const QString & ) ),
           this, SLOT(  currentPathComboActivated( const QString & ) ) );

  connect( currentPathCombo->lineEdit(),SIGNAL(returnPressed()),
           this,SLOT(currentPathComboChanged()));

  connect( Local_View, SIGNAL( clicked( QListViewItem*)),
           this,SLOT( localListClicked(QListViewItem *)) );
  connect( Local_View, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint&, int)),
           this,SLOT( localListPressed(int, QListViewItem *, const QPoint&, int)) );

  connect( Local_View, SIGNAL( selectionChanged() ), SLOT( cancelMenuTimer() ) );

  connect( Remote_View, SIGNAL( clicked( QListViewItem*)),
           this,SLOT( remoteListClicked(QListViewItem *)) );
  connect( Remote_View, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint&, int)),
           this,SLOT( remoteListPressed(int, QListViewItem *, const QPoint&, int)) );

  connect(TabWidget,SIGNAL(currentChanged(QWidget *)),
          this,SLOT(tabChanged(QWidget*)));

  connect( &menuTimer, SIGNAL( timeout() ), SLOT( showFileMenu() ) );

}
