/***************************************************************************
                          gutenbrowser.cpp  -  description
                             -------------------
    begin                : Mon Jul 24 22:33:12 MDT 2000
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
***************************************************************************/
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
//#include "gutenbrowserData.h"
#include "gutenbrowser.h"

#include <qpe/config.h>
#include <qpe/fontdatabase.h>
#include <qpe/menubutton.h>
#include <qpe/resource.h>
#include <qpe/qpeapplication.h>

#include <qfontinfo.h>
#include <qtoolbutton.h>

void Gutenbrowser::initSlots() {

    connect(LibraryButton,SIGNAL(released()),SLOT(LibraryBtn()));
    connect(OpenButton,SIGNAL(released()),SLOT(OpenBtn()));
    connect(SearchButton,SIGNAL(released()),SLOT(SearchBtn()));
    connect(ForwardButton,SIGNAL(clicked()),SLOT(ForwardBtn()));
    connect(BackButton,SIGNAL(clicked()),SLOT(BackBtn()));
    connect(setBookmarkButton,SIGNAL(released()),SLOT(setBookmark()));
    connect(dictionaryButton,SIGNAL(released()),SLOT(LookupBtn()));
    connect(InfoBar,SIGNAL(clicked()),SLOT(InfoBarClick()));
    connect(qApp,SIGNAL(aboutToQuit()),SLOT(cleanUp()));
    connect(mainList,SIGNAL(clicked(QListBoxItem *)),SLOT(listClickedSlot(QListBoxItem *)));
    connect(bookmarksMenu,SIGNAL(activated(int)),SLOT(Bookmark(int)));    
}

void Gutenbrowser::initConfig() {
//qDebug("Starting configuration.");
    QDir library(local_library);
    if ( !library.exists()) {
        library.mkdir(local_library, TRUE);
        QString msg;
// #ifndef Q_WS_WIN
        msg = "chmod 755 " + local_library;
        system(msg);
//     QDir pixdir(local_library+"pix");
//     if ( !pixdir.exists()) {
//         pixdir.mkdir(local_library+"pix", TRUE);
//         QString msg;
//         msg = "chmod 755 " + local_library+"pix";
//         system(msg);
//     }
// #endif

    }
      //    qDebug( "init file is %s",iniFile.latin1());

#ifdef Q_WS_QWS
    useSplitter=FALSE;
    Config config("Gutenbrowser");

    config.setGroup( "General" );
//useWordWrap_CheckBox
    useIcons=TRUE;
#else

    config.setGroup( "General" );
    QString s_Split=config.readEntry("Splitter", "TRUE" );
    QWidget *d = QApplication::desktop();
    int width=d->width();
    int height=d->height();
    int w=0, h=0;
    QString str, str2;

    this->setMaximumWidth(width);

    if(s_Split=="TRUE")
        useSplitter=TRUE;
    else
        useSplitter=FALSE;

    config.setGroup( "Geometry" );
    if(width < 1030) {
        str = config.readEntry("Frame", "700,500");
        if(  !str.isEmpty() && str.find(',')>=0) {
            sscanf(str,"%d,%d",&w,&h);
            resize(w,h);
            str2.sprintf("%d,%d", (width-w)/2,(height-h)/2);
            str = config.readEntry( "Position", str2);
        } else {
            resize( 740,510 );
            str2.sprintf("%d,%d", (width-w)/2,(height-h)/2);
            str = config.readEntry( "Position",str2);
        }
    } else {// desktop is high res
        str = config.readEntry("Frame", "990,640");
        if(  !str.isEmpty() && str.find(',')>=0) {
            sscanf(str,"%d,%d",&w,&h);
            resize(w,h);
            str2.sprintf("%d,%d", (width-w)/2,(height-h)/2);
            str = config.readEntry( "Position", str2);
        } else {
            resize( 990,640 );
            str2.sprintf("%d,%d", (width-w)/2,(height-h)/2);
            str = config.readEntry( "Position", str2);
        }
    }
    int posX, posY;
    bool ok;
    QString goober;
    goober=str.left( str.find(",", 0, TRUE) );
    posX=goober.toInt( &ok, 10);
    goober=str.right( str.findRev(",", -1, TRUE) );
    posY= goober.toInt( &ok, 10);
//     move( posX, posY);
    str = config.readEntry("Icons", "TRUE");
    if(str == "TRUE")
        useIcons=TRUE;
    else
        useIcons=FALSE;
#endif
// #if defined(_WS_WIN_)
//     move( posX-4, posY-20);
// #endif

    config.setGroup( "Browser" );
    brow = config.readEntry( "Preferred", "Opera");
    config.setGroup( "FTPsite" );  // ftp server config
    ftp_host=config.readEntry("SiteName", "sailor.gutenberg.org");
//    ftp_host=ftp_host.right(ftp_host.length()-(ftp_host.find(") ",0,TRUE)+1) );
    ftp_host=ftp_host.right(ftp_host.length()-(ftp_host.find(")        ",0,TRUE)+1) );
    ftp_host=ftp_host.stripWhiteSpace();

    ftp_base_dir= config.readEntry("base",  "/pub/gutenberg");

			//bool ok2;
    QString temp;
    QString copying;
    copying="";

    config.setGroup("General");
    QString qExit;
    qExit=config.readEntry("queryExit","TRUE");
    if(qExit=="TRUE") {
        b_queryExit=TRUE;
//    qDebug("Please query before leaving the library.");
    } else {
//    qDebug("Please DO NOT query before leaving the library.");
        b_queryExit=FALSE;
    }
// bookmarks
//       config.setGroup("Titles");
//      QString tmpTitle=config.readEntry(file_name,"");
  

}// end initConfig()

void Gutenbrowser::initMenuBar()
{
//    qDebug("Starting menu init.");
      // menuBar entry fileMenu
    menubar = new QPEMenuBar(this);

    fileMenu=new QPopupMenu();
    fileMenu->insertItem(Resource::loadPixmap("gutenbrowser/openbook"),
                         "Open Local Library...", this, SLOT( OpenBtn()) );
//    fileMenu->insertItem("Download FTPSite", this, SLOT( downloadFtpList()) );
    fileMenu->insertItem( Resource::loadPixmap("home"),
                          "Download Library Index", this, SLOT( downloadLibIndex()) );
    fileMenu->insertItem( Resource::loadPixmap("quit"),
                          "Quit Gutenbrowser...", this, SLOT( ByeBye()) );
      // menuBar entry editMenu

    editMenu=new QPopupMenu();
    
    editMenu->insertItem(Resource::loadPixmap("up"), "Top",
												 this, SLOT(TopBtn()) );
    editMenu->insertItem( Resource::loadPixmap("back"), "Beginning",
                          this, SLOT(doBeginBtn()) );
    editMenu->insertItem( Resource::loadPixmap("gutenbrowser/search"), "Search",
                          this, SLOT(SearchBtn()) );
    
    editMenu->insertItem("Clear", this, SLOT(ClearEdit()) );

    optionsMenu= new QPopupMenu();
    optionsMenu->insertItem( Resource::loadPixmap("gutenbrowser/configure"),
                             "Configure", this, SLOT(doOptions()) );

    donateMenu = new QPopupMenu();
//     donateMenu->insertItem("Gutenberg", this, SLOT(donateGutenberg()) );
    donateMenu->insertItem( Resource::loadPixmap("gutenbrowser/gutenbrowser_sm"),
                            "Gutenbrowser Developer", this, SLOT(infoGutenbrowser()) );

    menubar->insertItem("File", fileMenu);
    menubar->insertItem("Page", editMenu);
    menubar->insertItem("Options", optionsMenu);
//    menubar->insertItem("More Info", donateMenu);

    menu->addWidget( menubar,0);

    topLayout->addLayout( menu, 0);
}

void Gutenbrowser::initButtonBar()
{
//qDebug("Starting buttonbar init.");

    OpenButton = new QPushButton( this, "OpenButton" );
    OpenButton->setFocusPolicy( QWidget::TabFocus );

    LibraryButton = new QPushButton( this, "LibraryButton" );
    LibraryButton->setFocusPolicy( QWidget::TabFocus );

    BackButton = new QPushButton( this, "BackButton" );
    BackButton->setFocusPolicy( QWidget::TabFocus );
    BackButton->setAutoRepeat(TRUE);

    ForwardButton = new QPushButton( this, "ForwardButton" );
    ForwardButton->setFocusPolicy( QWidget::TabFocus );
//    ForwardButton->setAutoRepeat(TRUE);
    
    SearchButton = new QPushButton( this, "SearchButton" );
    SearchButton->setFocusPolicy( QWidget::TabFocus );

    setBookmarkButton = new QPushButton( this, "setBookmark" );
    setBookmarkButton->setFocusPolicy( QWidget::TabFocus );

    lastBmkButton = new MenuButton( this, "lastBmkButton" );
    lastBmkButton->setFocusPolicy( QWidget::TabFocus );

    bookmarksMenu = new QPopupMenu();
    bookmarksMenu->insertItem("Last Set", this, SLOT(Bookmark( int) ));
    
    lastBmkButton->setPopup(bookmarksMenu);

    dictionaryButton = new QPushButton( this, "dictionaryButton" );
    dictionaryButton->setFocusPolicy( QWidget::TabFocus );

    InfoBar = new QPushButton( this, "Info_Bar" );
//    qDebug("Infobar");
//    if(!useSplitter) {

    buttonsHidden=FALSE;
    buttons2->setSpacing(2);
    buttons2->addWidget(OpenButton, 0, AlignCenter);
    buttons2->addWidget(LibraryButton, 0, AlignCenter);
    buttons2->addWidget(BackButton, 0, AlignCenter);
    buttons2->addWidget(ForwardButton, 0, AlignCenter);
    buttons2->addWidget(SearchButton, 0, AlignCenter);
    buttons2->addWidget(setBookmarkButton, 0, AlignCenter);
    buttons2->addWidget(lastBmkButton, 0, AlignCenter);
    buttons2->addWidget(dictionaryButton, 0, AlignCenter);
    buttons2->addWidget(InfoBar, 0, AlignCenter);

    topLayout->setSpacing(0);
    topLayout->addLayout( buttons2,0);

}

/* STATUSBAR*/
void Gutenbrowser::initStatusBar()
{
// #ifndef Q_WS_QWS

//     statusBar = new QStatusBar( this, "Status Bar");
//     statusBar->message(IDS_STATUS_DEFAULT, 2000);
// #endif
}

void Gutenbrowser::initView()
{
      // set the main widget here
//        QFont defaultFont( "charter", 10, 50, 0 );
    Lview = new MultiLine_Ex(this);
      
    Config cfg("Gutenbrowser");
    cfg.setGroup("Font");

    FontDatabase fdb;
    QFont defaultFont=Lview->font();
    QFontInfo fontInfo(defaultFont);

    QString family = cfg.readEntry("Family", fontInfo.family());
    QString style = cfg.readEntry("Style", fdb.styleString(defaultFont));
    int i_size = cfg.readNumEntry("Size", fontInfo.pointSize()/10);
    QString charSet = cfg.readEntry("CharSet", QFont::encodingName( fontInfo.charSet()) );

    defaultFont = fdb.font(family,style,i_size,charSet);

    QString italic=cfg.readEntry("Italic","FALSE");
    if(italic=="TRUE") {
        qDebug("Set Italic font");
        defaultFont = fdb.font(family,"Regular",i_size,charSet); //workaround
        defaultFont.setItalic(TRUE);
    }

    Lview->setFont( defaultFont);
    update();

    cfg.setGroup("General");

    if( cfg.readBoolEntry("WordWrap", 1)) {
        Lview->setWordWrap(QMultiLineEdit::WidgetWidth);
        useWrap=true;
    } else {
        Lview->setWordWrap(QMultiLineEdit::NoWrap);
        useWrap=false;
    }
    mainList = new QListBox(this,"mainlist");

//  QPEApplication::setStylusOperation( mainList->viewport(),QPEApplication::RightOnHold);
      //  mainList->showMaximized();
//  mainList->setGeometry(2,30,230,160);
    Lview->setReadOnly( true);
    edits->addWidget( Lview);
    edits->addWidget(mainList);

    if(!showMainList) {
        Lview->setText( "\nThis is gutenbrowser for the Sharp Zaurus.\nMake your self at home, sit back, relax and read something great. ");
    } else
        Lview->hide();

    topLayout->addLayout( edits, 0);
}
