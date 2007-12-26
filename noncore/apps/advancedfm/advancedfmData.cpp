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

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oresource.h>
#include <qpe/applnk.h>
#include <qpe/storage.h>
#include <qpe/qpeapplication.h>
#include <qpe/menubutton.h>
using namespace Opie::Core;
using namespace Opie::Ui;

/* QT */
#include <qframe.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qmenubar.h>
#include <qcombobox.h>
#include <qtoolbutton.h>
#include <qlineedit.h>
#include <qlistview.h>

/* STD */
#include <sys/utsname.h>

void AdvancedFm::init() {
    b = false;
    setCaption( tr( "AdvancedFm" ) );

//    QFrame* frame = new QFrame(this);
//    setCentralWidget(frame);
//    QVBoxLayout *layout = new QVBoxLayout( frame );

    QVBoxLayout *layout = new QVBoxLayout( this);
    layout->setSpacing( 2);
    layout->setMargin( 0); // squeeze

    QMenuBar *menuBar = new QMenuBar(this);
    menuBar->setMargin( 0 ); // squeeze
    fileMenu = new QPopupMenu( this );
    viewMenu  = new QPopupMenu( this );
//    customDirMenu  = new QPopupMenu( this );

    layout->addWidget( menuBar );

    menuBar->insertItem( tr( "File" ), fileMenu);
    menuBar->insertItem( tr( "View" ), viewMenu);
    connect(fileMenu,SIGNAL(aboutToShow()),this,SLOT(enableDisableMenu()));

    bool useBigIcon = qApp->desktop()->size().width() > 330;

    cdUpButton = new QToolButton( 0,"cdUpButton");
    cdUpButton->setUsesBigPixmap( useBigIcon );
    cdUpButton->setPixmap( Opie::Core::OResource::loadPixmap( "up", Opie::Core::OResource::SmallIcon ) );
    cdUpButton->setAutoRaise( true );
    menuBar->insertItem( cdUpButton );

    qpeDirButton= new QToolButton( 0,"QPEButton");
    qpeDirButton->setUsesBigPixmap( useBigIcon );
    qpeDirButton->setPixmap( Opie::Core::OResource::loadPixmap( "logo/opielogo", Opie::Core::OResource::SmallIcon ) );
    qpeDirButton->setAutoRaise( true );
    menuBar->insertItem( qpeDirButton );

    cfButton = new QToolButton( 0, "CFButton");
    cfButton->setUsesBigPixmap( useBigIcon );
    cfButton->setPixmap( Opie::Core::OResource::loadPixmap( "pcmcia", Opie::Core::OResource::SmallIcon ) );
    cfButton->setAutoRaise( true );
    menuBar->insertItem( cfButton );

    sdButton = new QToolButton( 0, "SDButton");
    sdButton->setUsesBigPixmap( useBigIcon );
    sdButton->setPixmap( Opie::Core::OResource::loadPixmap( "advancedfm/sdcard", Opie::Core::OResource::SmallIcon ) );
    sdButton->setAutoRaise( true );
    menuBar->insertItem( sdButton );

    docButton = new QToolButton( 0,"docsButton");
    docButton->setUsesBigPixmap( useBigIcon );
    docButton->setPixmap( Opie::Core::OResource::loadPixmap( "DocsIcon", Opie::Core::OResource::SmallIcon ) );
    docButton->setAutoRaise( true );
    menuBar->insertItem( docButton );

    homeButton = new QToolButton( 0, "homeButton");
    homeButton->setUsesBigPixmap( useBigIcon );
    homeButton->setPixmap( Opie::Core::OResource::loadPixmap( "home", Opie::Core::OResource::SmallIcon ) );
    homeButton->setAutoRaise( true );
    menuBar->insertItem( homeButton );

//    fileMenu->insertItem( tr( "File Search" ), this, SLOT( openSearch() ));
//    fileMenu->insertSeparator();
    fileMenu->insertItem( tr( "Make Directory" ), this, SLOT( mkDir() ), 0, 101);
    fileMenu->insertItem( tr( "Select All" ), this, SLOT( selectAll() ), 0, 102);
    fileMenu->insertSeparator();
    fileMenu->insertItem( tr( "Make Symlink" ), this, SLOT( mkSym() ), 0, 103);
    fileMenu->insertItem( tr( "Add To Documents" ), this, SLOT( addToDocs() ), 0, 104);
    fileMenu->insertItem( tr( "Run Command" ), this, SLOT( runCommandStd() ), 0, 105);
    fileMenu->insertItem( tr( "Run Command with Output" ), this, SLOT( runCommand() ), 0, 106);
    fileMenu->insertItem( tr( "Rename" ), this, SLOT( rn() ), 0, 107);
    fileMenu->insertItem( tr( "Delete" ), this, SLOT( del() ), 0, 108);

    viewMenu->insertItem( tr( "Switch to View 1" ), this, SLOT( switchToLocalTab()));
    viewMenu->insertItem( tr( "Switch to View 2" ), this, SLOT( switchToRemoteTab()));
    viewMenu->insertSeparator();
    viewMenu->insertItem( tr( "Refresh" ), this, SLOT( refreshCurrentTab()));
    viewMenu->insertSeparator();
    viewMenu->insertItem( tr( "Show Hidden Files" ), this,  SLOT( showMenuHidden() ));
//    viewMenu->insertSeparator();
//    viewMenu->insertItem( tr( "About" ), this, SLOT( doAbout() ));
    viewMenu->setCheckable(true);
    viewMenu->setItemChecked( viewMenu->idAt(0), true);
    viewMenu->setItemChecked( viewMenu->idAt(1), false);
    viewMenu->setItemChecked( viewMenu->idAt(5), false);

    s_addBookmark = tr("Bookmark Directory");
    s_removeBookmark = tr("Remove Current Directory from Bookmarks");

//    menuButton->insertItem("");

//    customDirMenu->insertItem(tr("Add This Directory"));
//    customDirMenu->insertItem(tr("Remove This Directory"));
//    customDirMenu->insertSeparator();

    QHBoxLayout *CBHB = new QHBoxLayout(); // parent layout will be set later
    CBHB->setMargin( 0 );
    CBHB->setSpacing( 1 );

    menuButton = new MenuButton( this );

    menuButton->setUseLabel(false);
    menuButton->setMaximumWidth( 20 );
    menuButton->insertItem( s_addBookmark);
    menuButton->insertItem( s_removeBookmark);
    menuButton->insertSeparator();
//    menuButton->setFocusPolicy(NoFocus);
    CBHB->addWidget( menuButton );

    customDirsToMenu();

    currentPathCombo = new QComboBox( FALSE, this, "currentPathCombo" );
    currentPathCombo->setEditable(TRUE);
    currentPathCombo->lineEdit()->setText( currentDir.canonicalPath());
//    currentPathCombo->setFocusPolicy(NoFocus);
    CBHB->addWidget( currentPathCombo );

    layout->addLayout( CBHB );

    TabWidget = new OSplitter( Horizontal, this, "TabWidget" );
//    TabWidget = new QTabWidget( this, "TabWidget" );
    layout->addWidget( TabWidget, 4 );

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
    Local_View->setFocusPolicy(StrongFocus);
    Local_View->installEventFilter( this );

    QPEApplication::setStylusOperation( Local_View->viewport() , QPEApplication::RightOnHold);

    tabLayout->addWidget( Local_View, 0, 0 );

    TabWidget->addWidget( tab,"advancedfm/smFileBrowser.png", tr("1"));
//    TabWidget->insertTab( tab, tr("1"));

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
    Remote_View->setFocusPolicy(StrongFocus);
    Remote_View->installEventFilter( this );

    QPEApplication::setStylusOperation( Remote_View->viewport(), QPEApplication::RightOnHold);

    tabLayout_2->addWidget( Remote_View, 0, 0 );

    TabWidget->addWidget( tab_2, "advancedfm/smFileBrowser.png",tr( "2"));
    TabWidget->setSizeChange( 370 );
//    TabWidget->insertTab( tab_2, tr( "2"));

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

    if( !StorageInfo::hasSd() || !StorageInfo::hasMmc()) {
        sdButton->hide();
    }
    if( !StorageInfo::hasCf() ) {
        cfButton->hide();
    }
    currentDir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
    currentDir.setPath( QDir::currentDirPath());

    currentRemoteDir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden | QDir::All);
    currentRemoteDir.setPath( QDir::currentDirPath());

    filterStr="*";
    showHidden();
    TabWidget->setCurrentWidget(0);
}

void AdvancedFm::initConnections()
{
    connect(qApp,SIGNAL(aboutToQuit()),this,SLOT(cleanUp()));
    connect(qpeDirButton,SIGNAL(released()),this,SLOT(QPEButtonPushed()));
    connect(cfButton,SIGNAL(released()),this,SLOT(CFButtonPushed()));
    connect(sdButton,SIGNAL(released()),this,SLOT(SDButtonPushed()));
    connect(cdUpButton,SIGNAL(released()),this,SLOT(upDir()));
    connect(docButton,SIGNAL(released()),this,SLOT(docButtonPushed()));
    connect(homeButton,SIGNAL(released()),this,SLOT(homeButtonPushed()));
    connect(currentPathCombo,SIGNAL(activated(const QString&)),this,SLOT(changeTo(const QString&)));

    connect(currentPathCombo->lineEdit(),SIGNAL(returnPressed()),this,SLOT(currentPathComboChanged()));

    connect(Local_View,SIGNAL(clicked(QListViewItem*)),this,SLOT( ListClicked(QListViewItem*)));

    connect(Local_View,SIGNAL(mouseButtonPressed(int,QListViewItem*,const QPoint&,int)),this,SLOT( ListPressed(int,QListViewItem*,const QPoint&,int)) );

    connect(Remote_View,SIGNAL(clicked(QListViewItem*)),this,SLOT(ListClicked(QListViewItem*)));
    connect(Remote_View,SIGNAL(mouseButtonPressed(int,QListViewItem*,const QPoint&,int)),this,SLOT(ListPressed(int,QListViewItem*,const QPoint&,int)));

    connect(TabWidget,SIGNAL(currentChanged(QWidget*)),this,SLOT(tabChanged(QWidget*)));

    connect(Remote_View,SIGNAL(rightButtonPressed(QListViewItem*,const QPoint&,int)),this,SLOT(showFileMenu()));
    connect(Local_View,SIGNAL(rightButtonPressed(QListViewItem*,const QPoint&,int)),this,SLOT(showFileMenu()));

    connect(menuButton,SIGNAL(selected(const QString&)),SLOT(gotoCustomDir(const QString&)));
//    connect( menuButton, SIGNAL( selected(int)), SLOT( dirMenuSelected(int)));
//    connect( customDirMenu, SIGNAL( activated(int)), this, SLOT( dirMenuSelected(int)));
}

