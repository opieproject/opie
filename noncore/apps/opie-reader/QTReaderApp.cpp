/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  Allrights reserved.
**
** This file is part of Qt Palmtop Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "useqpe.h"
#include "QTReader.h"
#include "GraphicWin.h"
#include "Bkmks.h"
#include "cbkmkselector.h"
#include "infowin.h"
#include "ToolbarPrefs.h"
#include "Prefs.h"
#include "CAnnoEdit.h"
#include "QFloatBar.h"
#include "FixedFont.h"
#include "URLDialog.h"
#include "QTReaderApp.h"
#include "CDrawBuffer.h"
#include "Filedata.h"
#include "opie.h"
#include "names.h"
#include "CEncoding_tables.h"
#include "CloseDialog.h"

/* OPIE */
#include <opie2/odebug.h>
#ifdef USEQPE
#include <qpe/menubutton.h>
#include <qpe/fontdatabase.h>
#include <qpe/global.h>
#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
#endif

/* QT */
#ifdef USEQPE
#include <qmenubar.h>
#include <qtoolbar.h>
#endif
#include <qregexp.h>
#include <qclipboard.h>
#include <qwidgetstack.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qcombobox.h>
#include <qpopupmenu.h>
#include <qaction.h>
#include <qapplication.h>
#include <qlineedit.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qobjectlist.h>
#include <qfileinfo.h>
#include <qprogressbar.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>

/* STD */
#include <stdlib.h> //getenv


#ifdef USEQPE
#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include "fileBrowser.h"
#else
#include "qfiledialog.h"
#endif

bool CheckVersion(int&, int&, char&);

#ifdef _WINDOWS
#define PICDIR "c:\\uqtreader\\pics\\"
#else
#ifdef USEQPE
#define PICDIR "opie-reader/"
#else
#define PICDIR "/home/tim/uqtreader/pics/"
#endif
#endif

unsigned long QTReaderApp::m_uid = 0;

void QTReaderApp::setScrollState(bool _b) { m_scrollButton->setOn(_b); }

#ifdef USEQPE
#define geticon(iconname) Resource::loadPixmap( iconname )
#define getmyicon(iconname) Resource::loadPixmap( PICDIR iconname )
#else
#define geticon(iconname) QPixmap(PICDIR iconname ".png")
#define getmyicon(iconname) geticon(iconname)
//#define geticon(iconname) QIconSet( QPixmap(PICDIR iconname) )
#endif

#ifndef _WINDOWS
#include <unistd.h>
#endif
#include <stddef.h>
#ifndef _WINDOWS
#include <dirent.h>
#endif

void QTReaderApp::listBkmkFiles()
{
    bkmkselector->clear();
    bkmkselector->setText("Cancel");
#ifndef USEQPE
    int cnt = 0;

    QDir d = QDir::home();                      // "/"
    if ( !d.cd(APPDIR) ) {                       // "/tmp"
        owarn << "Cannot find the \"~/" APPDIR "\" directory" << oendl;
        d = QDir::home();
        d.mkdir(APPDIR);
        d.cd(APPDIR);
    }




        d.setFilter( QDir::Files | QDir::NoSymLinks );
//        d.setSorting( QDir::Size | QDir::Reversed );

        const QFileInfoList *list = d.entryInfoList();
        QFileInfoListIterator it( *list );      // create list iterator
        QFileInfo *fi;                          // pointer for traversing

        while ( (fi=it.current()) ) {           // for each file...

            bkmkselector->insertItem(fi->fileName());
            cnt++;

            //odebug << "" << fi->size() << " " << fi->fileName().data() << "" << oendl;
            ++it;                               // goto next list element
        }

#else /* USEQPE */
    int cnt = 0;
    DIR *d;
    d = opendir((const char *)Global::applicationFileName(APPDIR,""));

    while(1)
    {
    struct dirent* de;
    struct stat buf;
    de = readdir(d);
    if (de == NULL) break;

    if (lstat((const char *)Global::applicationFileName(APPDIR,de->d_name),&buf) == 0 && S_ISREG(buf.st_mode))
    {
        bkmkselector->insertItem(de->d_name);
        cnt++;
    }
    }

    closedir(d);
#endif
    if (cnt > 0)
      {
//tjw        menu->hide();
        editorStack->raiseWidget( bkmkselector );
        hidetoolbars();
    m_nBkmkAction = cRmBkmkFile;
      }
    else
      QMessageBox::information(this, PROGNAME, "No bookmark files");
}

void QTReaderApp::hidetoolbars()
{
    menubar->hide();
    if (fileBar != NULL) fileBar->hide();
    if (viewBar != NULL) viewBar->hide();
    if (navBar != NULL) navBar->hide();
    if (markBar != NULL) markBar->hide();
    if (m_fontVisible) m_fontBar->hide();
    if (regVisible)
    {
#ifdef USEQPE
        Global::hideInputMethod();
#endif
    regBar->hide();
    }
    if (searchVisible)
    {
#ifdef USEQPE
        Global::hideInputMethod();
#endif
    searchBar->hide();
    }
}

QTReaderApp::QTReaderApp( QWidget *parent, const char *name, WFlags f )
    : QMainWindow( parent, name, f ), bFromDocView( FALSE ), m_dontSave(false),
      fileBar(NULL), navBar(NULL), viewBar(NULL), markBar(NULL)
{
    m_url_clipboard = false;
    m_url_localfile = false;
    m_url_globalfile = false;
        ftime(&m_lastkeytime);
////  odebug << "Application directory = " << (const tchar *)QPEApplication::documentDir() << "" << oendl;
////  odebug << "Application directory = " << (const tchar *)Global::applicationFileName("uqtreader" << "" << oendl;

    m_bcloseDisabled = true;
    m_disableesckey = false;
    pBkmklist = NULL;
    pOpenlist = NULL;
//    doc = 0;

    m_fBkmksChanged = false;

  QString lang = getenv( "LANG" );
  QString rot = getenv( "QWS_DISPLAY" );

/*
  int m_rot = 0;
  if (rot.contains("Rot90"))
  {
      m_rot = 90;
  }
  else if (rot.contains("Rot180"))
  {
      m_rot = 180;
  }
  else if (rot.contains("Rot270"))
  {
      m_rot = 270;
  }

//  odebug << "Initial Rotation(" << m_rot << "):" << rot << "" << oendl;
*/
  m_autogenstr = "^ *[A-Z].*[a-z] *$";

#ifdef USEQPE
    setIcon( Resource::loadPixmap( PICDIR  "uqtreader") );
#else
    setIcon( QPixmap (PICDIR  "uqtreader.png") );
#endif /* USEQPE */

//    QToolBar *bar = new QToolBar( this );
//    menubar = new QToolBar( this );
#ifdef USEQPE
  Config config( APPDIR );
#else
    QDir d = QDir::home();                      // "/"
    if ( !d.cd(APPDIR) ) {                       // "/tmp"
        owarn << "Cannot find the \"~/" APPDIR "\" directory" << oendl;
        d = QDir::home();
        d.mkdir(APPDIR);
        d.cd(APPDIR);
    }
    QFileInfo fi(d, INIFILE);
//    odebug << "Path:" << fi.absFilePath() << "" << oendl;
    Config config(fi.absFilePath());
#endif
    config.setGroup("Toolbar");
    m_tbmovesave = m_tbmove = config.readBoolEntry("Movable", false);
    m_tbpolsave = m_tbpol = (ToolbarPolicy)config.readNumEntry("Policy", 1);
    m_tbposition = (ToolBarDock)config.readNumEntry("Position", 2);
    menubar = new QToolBar("Menus", this, m_tbposition);

//    fileBar = new QToolBar("File", this);
//    QToolBar* viewBar = new QToolBar("File", this);
//    QToolBar* navBar = new QToolBar("File", this);
//    QToolBar* markBar = new QToolBar("File", this);

#ifdef USEQPE
    mb = new QMenuBar( menubar );
#else
    mb = new QMenuBar( menubar );
#endif

//#ifdef USEQPE
    QPopupMenu* tmp = new QPopupMenu(mb);
    mb->insertItem( geticon( "AppsIcon" ), tmp );
//#else
//    QMenuBar* tmp = mb;
//#endif

    QPopupMenu *file = new QPopupMenu( mb );
    tmp->insertItem( tr( "File" ), file );

    QPopupMenu *navigation = new QPopupMenu(mb);
    tmp->insertItem( tr( "Navigation" ), navigation );

    QPopupMenu *view = new QPopupMenu( mb );
    tmp->insertItem( tr( "View" ), view );

    QPopupMenu *marks = new QPopupMenu( this );
    tmp->insertItem( tr( "Marks" ), marks );

    QPopupMenu *settings = new QPopupMenu( this );
    tmp->insertItem( tr( "Settings" ), settings );

//    addToolBar(menubar, "Menus",QMainWindow::Top);
//    addToolBar(fileBar, "Toolbar",QMainWindow::Top);

    //    QPopupMenu *edit = new QPopupMenu( this );

    /*
    QAction *a = new QAction( tr( "New" ), Resource::loadPixmap( "new" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( fileNew() ) );
    a->addTo( bar );
    a->addTo( file );
    */

    editorStack = new QWidgetStack( this );
    setCentralWidget( editorStack );

    searchVisible = FALSE;
    regVisible = FALSE;
    m_fontVisible = false;

    m_annoWin = new CAnnoEdit(editorStack);
    editorStack->addWidget(m_annoWin, get_unique_id());
    connect( m_annoWin, SIGNAL( finished(const QString&,const QString&) ), this, SLOT( addAnno(const QString&,const QString&) ) );
    connect( m_annoWin, SIGNAL( cancelled() ), this, SLOT( infoClose() ) );

    m_infoWin = new infowin(editorStack);
    editorStack->addWidget(m_infoWin, get_unique_id());
    connect( m_infoWin, SIGNAL( Close() ), this, SLOT( infoClose() ) );

    m_graphicwin = new GraphicWin(editorStack);
    editorStack->addWidget(m_graphicwin, get_unique_id());
    connect( m_graphicwin, SIGNAL( Closed() ), this, SLOT( infoClose() ) );

//    bkmkselector = new QListBox(editorStack, "Bookmarks");
    bkmkselector = new CBkmkSelector(editorStack, "Bookmarks");
    //    connect(bkmkselector, SIGNAL( selected(const QString&) ), this, SLOT( gotobkmk(const QString&) ) );
    connect(bkmkselector, SIGNAL( selected(int) ), this, SLOT( gotobkmk(int) ) );
    connect(bkmkselector, SIGNAL( cancelled() ), this, SLOT( cancelbkmk() ) );
    editorStack->addWidget( bkmkselector, get_unique_id() );

/*
    importSelector = new FileSelector( "*", editorStack, "importselector", false );
    connect( importSelector, SIGNAL( fileSelected(const DocLnk&) ), this, SLOT( importFile(const DocLnk&) ) );

    editorStack->addWidget( importSelector, get_unique_id() );

    // don't need the close visible, it is redundant...
    importSelector->setCloseVisible( FALSE );
*/
//    odebug << "Reading file list" << oendl;
    readfilelist();

    reader = new QTReader( editorStack );

    reader->bDoUpdates = false;

#ifdef USEQPE
    ((QPEApplication*)qApp)->setStylusOperation(reader, QPEApplication::RightOnHold);
#endif

//    odebug << "Reading config" << oendl;
//  Config config( APPDIR );
  config.setGroup( "View" );
  m_debounce = config.readNumEntry("Debounce", 0);
#ifdef USEQPE
  m_bFloatingDialog = config.readBoolEntry("FloatDialogs", false);
#else
  m_bFloatingDialog = config.readBoolEntry("FloatDialogs", true);
#endif
  reader->bstripcr = config.readBoolEntry( "StripCr", true );
  reader->bfulljust = config.readBoolEntry( "FullJust", false );
  reader->setextraspace(config.readNumEntry( "ExtraSpace", 0 ));
  reader->setlead(config.readNumEntry( "ExtraLead", 0 ));
  reader->btextfmt = config.readBoolEntry( "TextFmt", false );
  reader->bautofmt = config.readBoolEntry( "AutoFmt", true );
  reader->bstriphtml = config.readBoolEntry( "StripHtml", false );
  reader->bpeanut = config.readBoolEntry( "Peanut", false );
  reader->bdehyphen = config.readBoolEntry( "Dehyphen", false );
  reader->bdepluck = config.readBoolEntry( "Depluck", false );
  reader->bdejpluck = config.readBoolEntry( "Dejpluck", false );
  reader->bonespace = config.readBoolEntry( "OneSpace", false );
  reader->bunindent = config.readBoolEntry( "Unindent", false );
  reader->brepara = config.readBoolEntry( "Repara", false );
  reader->bdblspce = config.readBoolEntry( "DoubleSpace", false );
  reader->bindenter = config.readNumEntry( "Indent", 0 );
  reader->m_textsize = config.readNumEntry( "FontSize", 12 );
  reader->m_delay = config.readNumEntry( "ScrollDelay", 5184);
  reader->m_lastfile = config.readEntry( "LastFile", QString::null );
  reader->m_lastposn = config.readNumEntry( "LastPosn", 0 );
  reader->m_bpagemode = config.readBoolEntry( "PageMode", true );
  reader->m_bMonoSpaced = config.readBoolEntry( "MonoSpaced", false);
  reader->m_swapmouse = config.readBoolEntry( "SwapMouse", false);
  reader->m_fontname = config.readEntry( "Fontname", "helvetica" );
  reader->m_encd = config.readNumEntry( "Encoding", 0 );
  reader->m_charpc = config.readNumEntry( "CharSpacing", 100 );
  reader->m_overlap = config.readNumEntry( "Overlap", 0 );
  reader->m_border = config.readNumEntry( "Margin", 6 );
#ifdef REPALM
  reader->brepalm = config.readBoolEntry( "Repalm", true );
#endif
  reader->bremap = config.readBoolEntry( "Remap", true );
  reader->bmakebold = config.readBoolEntry( "MakeBold", false );
  reader->setContinuous(config.readBoolEntry( "Continuous", true ));
  m_targetapp = config.readEntry( "TargetApp", QString::null );
  m_targetmsg = config.readEntry( "TargetMsg", QString::null );
#ifdef _SCROLLPIPE
  reader->m_pipetarget = config.readEntry( "PipeTarget", QString::null );
  reader->m_pauseAfterEachPara = config.readBoolEntry( "PauseAfterPara", true );
#endif
  m_twoTouch = config.readBoolEntry( "TwoTouch", false);
  m_doAnnotation = config.readBoolEntry( "Annotation", false);
  m_doDictionary = config.readBoolEntry( "Dictionary", false);
  m_doClipboard = config.readBoolEntry( "Clipboard", false);
  m_spaceTarget = (ActionTypes)config.readNumEntry("SpaceTarget", cesAutoScroll);
  m_escapeTarget = (ActionTypes)config.readNumEntry("EscapeTarget", cesNone);
  m_returnTarget = (ActionTypes)config.readNumEntry("ReturnTarget", cesFullScreen);
  m_leftTarget = (ActionTypes)config.readNumEntry("LeftTarget", cesZoomOut);
  m_rightTarget = (ActionTypes)config.readNumEntry("RightTarget", cesZoomIn);
  m_upTarget = (ActionTypes)config.readNumEntry("UpTarget", cesPageUp);
  m_downTarget = (ActionTypes)config.readNumEntry("DownTarget", cesPageDown);

  m_leftScroll = config.readBoolEntry("LeftScroll", false);
  m_rightScroll = config.readBoolEntry("RightScroll", false);
  m_upScroll = config.readBoolEntry("UpScroll", true);
  m_downScroll = config.readBoolEntry("DownScroll", true);

  m_propogatefontchange = config.readBoolEntry( "RequestorFontChange", false);
  reader->setBaseSize(config.readNumEntry( "Basesize", 10 ));

#ifndef USEQPE
    config.setGroup( "Geometry" );
    setGeometry(0,0,
    config.readNumEntry( "width", QApplication::desktop()->width()/2 ),
    config.readNumEntry( "height", QApplication::desktop()->height()/2 ));
    move(
    config.readNumEntry( "x", 20 ),
    config.readNumEntry( "y", 20 ));
#endif



  setTwoTouch(m_twoTouch);

    connect( reader, SIGNAL( OnShowPicture(QImage&) ), this, SLOT( showgraphic(QImage&) ) );

    connect( reader, SIGNAL( OnRedraw() ), this, SLOT( OnRedraw() ) );
    connect( reader, SIGNAL( OnWordSelected(const QString&,size_t,const QString&) ), this, SLOT( OnWordSelected(const QString&,size_t,const QString&) ) );
    connect( reader, SIGNAL( OnURLSelected(const QString&) ), this, SLOT( OnURLSelected(const QString&) ) );
    editorStack->addWidget( reader, get_unique_id() );

    m_preferences_action = new QAction( tr( "Configuration" ), geticon( "SettingsIcon" ), QString::null, 0, this, NULL);
    connect( m_preferences_action, SIGNAL( activated() ), this, SLOT( showprefs() ) );
    m_preferences_action->addTo( settings );

    m_saveconfig_action = new QAction( tr( "Save Config" ), QString::null, 0, this, NULL);
    connect( m_saveconfig_action, SIGNAL( activated() ), this, SLOT( SaveConfig() ) );
    m_saveconfig_action->addTo( settings );

    m_loadconfig_action = new QAction( tr( "Load Config" ), QString::null, 0, this, NULL);
    connect( m_loadconfig_action, SIGNAL( activated() ), this, SLOT( LoadConfig() ) );
    m_loadconfig_action->addTo( settings );

    m_tidyconfig_action = new QAction( tr( "Delete Config" ), QString::null, 0, this, NULL);
    connect( m_tidyconfig_action, SIGNAL( activated() ), this, SLOT( TidyConfig() ) );
    m_tidyconfig_action->addTo( settings );

    settings->insertSeparator();
    m_toolbarprefs_action = new QAction( tr( "Toolbars" ), QString::null, 0, this, NULL);
    connect( m_toolbarprefs_action, SIGNAL( activated() ), this, SLOT( showtoolbarprefs() ) );
    m_toolbarprefs_action->addTo( settings );

    m_open_action = new QAction( tr( "Open" ), geticon( "fileopen" ), QString::null, 0, this, 0 );
    connect( m_open_action, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    m_open_action->addTo( file );

    m_close_action = new QAction( tr( "Close" ), geticon( "close" ), QString::null, 0, this, 0 );
    connect( m_close_action, SIGNAL( activated() ), this, SLOT( fileClose() ) );
    m_close_action->addTo( file );

#ifdef _SCRIPT
    a = new QAction( tr( "Run Script" ), QString::null, 0, this, NULL);
    connect( a, SIGNAL( activated() ), this, SLOT( RunScript() ) );
    a->addTo( file );
#endif
    /*
    a = new QAction( tr( "Revert" ), geticon( "close" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( fileRevert() ) );
    a->addTo( file );

    a = new QAction( tr( "Cut" ), geticon( "cut" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editCut() ) );
    a->addTo( filebar() );
    a->addTo( edit );
    */

    m_info_action = new QAction( tr( "Info" ), geticon( "UtilsIcon" ), QString::null, 0, this, NULL);
    connect( m_info_action, SIGNAL( activated() ), this, SLOT( showinfo() ) );
    m_info_action->addTo( file );

    m_touch_action = new QAction( tr( "Two/One Touch" ), geticon( "1to1" ), QString::null, 0, this, NULL, true );
    connect( m_touch_action, SIGNAL( toggled(bool) ), this, SLOT( setTwoTouch(bool) ) );
    m_touch_action->setOn(m_twoTouch);
    m_touch_action->addTo( file );

    m_find_action = new QAction( tr( "Find..." ), geticon( "find" ), QString::null, 0, this, NULL);
     connect( m_find_action, SIGNAL( activated() ), this, SLOT( editFind() ) );
     file->insertSeparator();
//     a->addTo( bar );
     m_find_action->addTo( file );

    m_exportlinks_action = new QAction( tr( "Export Links" ), QString::null, 0, this, NULL);
    connect( m_exportlinks_action, SIGNAL( activated() ), this, SLOT( ExportLinks() ) );
    m_exportlinks_action->addTo( file );

    m_scrollButton = new QAction( tr( "Scroll" ), getmyicon( "panel-arrow-down" ), QString::null, 0, this, 0, true );
    connect( m_scrollButton, SIGNAL( toggled(bool) ), this, SLOT( autoScroll(bool) ) );
    m_scrollButton->addTo(navigation);
    m_scrollButton->setOn(false);

    m_start_action = new QAction( tr( "Goto Start" ), geticon( "start" ), QString::null, 0, this, NULL);
    connect( m_start_action, SIGNAL( activated() ), this, SLOT( gotoStart() ) );
    m_start_action->addTo(navigation);

    m_end_action = new QAction( tr( "Goto End" ), geticon( "finish" ), QString::null, 0, this, NULL);
    connect( m_end_action, SIGNAL( activated() ), this, SLOT( gotoEnd() ) );
    m_end_action->addTo(navigation);

    m_jump_action = new QAction( tr( "Jump" ), geticon( "rotate" ), QString::null, 0, this, NULL);
    connect( m_jump_action, SIGNAL( activated() ), this, SLOT( jump() ) );
    m_jump_action->addTo(navigation);

    m_pageline_action = new QAction( tr( "Page/Line Scroll" ), geticon( "pass" ), QString::null, 0, this, NULL, true );
    connect( m_pageline_action, SIGNAL( toggled(bool) ), this, SLOT( pagemode(bool) ) );
    m_pageline_action->addTo(navigation);
    m_pageline_action->setOn(reader->m_bpagemode);

    m_pageup_action = new QAction( tr( "Up" ), geticon( "up" ), QString::null, 0, this, 0 );
    connect( m_pageup_action, SIGNAL( activated() ), this, SLOT( pageup() ) );
    m_pageup_action->addTo( navigation );

    m_pagedn_action = new QAction( tr( "Down" ), geticon( "down" ), QString::null, 0, this, 0 );
    connect( m_pagedn_action, SIGNAL( activated() ), this, SLOT( pagedn() ) );
    m_pagedn_action->addTo( navigation );

    m_back_action = new QAction( tr( "Back" ), geticon( "back" ), QString::null, 0, this, 0 );
    connect( m_back_action, SIGNAL( activated() ), reader, SLOT( goBack() ) );
    m_back_action->addTo( navigation );

    m_home_action = new QAction( tr( "Home" ), geticon( "home" ), QString::null, 0, this, 0 );
    connect( m_home_action, SIGNAL( activated() ), reader, SLOT( goHome() ) );
    m_home_action->addTo( navigation );

    m_forward_action = new QAction( tr( "Forward" ), geticon( "forward" ), QString::null, 0, this, 0 );
    connect( m_forward_action, SIGNAL( activated() ), reader, SLOT( goForward() ) );
    m_forward_action->addTo( navigation );

    /*
    a = new QAction( tr( "Find" ), QString::null, 0, this, NULL, true );
    //    connect( a, SIGNAL( activated() ), this, SLOT( pagedn() ) );
    a->addTo( file );

    a = new QAction( tr( "Find Again" ), QString::null, 0, this, NULL, true );
    //    connect( a, SIGNAL( activated() ), this, SLOT( pagedn() ) );
    a->addTo( file );
    */

//    file->insertSeparator();

#ifdef _SCROLLPIPE

    QActionGroup* ag = new QActionGroup(this);
    ag->setExclusive(false);
    spacemenu = new QPopupMenu(this);
    file->insertItem( tr( "Scrolling" ), spacemenu );

    a = new QAction( tr( "Set Target" ), QString::null, 0, ag, NULL);
    connect( a, SIGNAL( activated() ), this, SLOT( setpipetarget() ) );

    a = new QAction( tr( "Pause Paras" ), QString::null, 0, ag, NULL, true );
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( setpause(bool) ) );
    a->setOn(reader->m_pauseAfterEachPara);

    ag->addTo(spacemenu);
//    file->insertSeparator();

#endif

/*
    a = new QAction( tr( "Import" ), QString::null, 0, this, NULL );
    connect( a, SIGNAL( activated() ), this, SLOT( importFiles() ) );
    a->addTo( file );
*/

    /*
    a = new QAction( tr( "Paste" ), geticon( "paste" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editPaste() ) );
    a->addTo( fileBar );
    a->addTo( edit );
    */

//     a = new QAction( tr( "Find..." ), geticon( "find" ), QString::null, 0, this, 0 );

     m_fullscreen = false;
    m_actFullscreen = new QAction( tr( "Fullscreen" ), geticon( "fullscreen" ), QString::null, 0, this, NULL, true );
    connect( m_actFullscreen, SIGNAL( toggled(bool) ), this, SLOT( setfullscreen(bool) ) );
    m_actFullscreen->setOn(m_fullscreen);
    m_actFullscreen->addTo( view );

    view->insertSeparator();

    m_zoomin_action = new QAction( tr( "Zoom In" ), geticon( "zoom" ), QString::null, 0, this);
    connect( m_zoomin_action, SIGNAL( activated() ), this, SLOT( zoomin() ) );
    m_zoomin_action->addTo( view );

    m_zoomout_action = new QAction( tr( "Zoom Out" ), geticon( "mag" ), QString::null, 0, this);
    connect( m_zoomout_action, SIGNAL( activated() ), this, SLOT( zoomout() ) );
    m_zoomout_action->addTo( view );

    view->insertSeparator();
    m_setfont_action = new QAction( tr( "Set Font" ), getmyicon( "font" ), QString::null, 0, this);
    connect( m_setfont_action, SIGNAL( activated() ), this, SLOT( setfont() ) );
    m_setfont_action->addTo( view );

    view->insertSeparator();
    m_setenc_action = new QAction( tr( "Set Encoding" ), getmyicon( "charset" ), QString::null, 0, this);
    connect( m_setenc_action, SIGNAL( activated() ), this, SLOT( chooseencoding() ) );
    m_setenc_action->addTo( view );

    m_setmono_action = new QAction( tr( "Ideogram" ), getmyicon( "ideogram" ), QString::null, 0, this, NULL, true);
    connect( m_setmono_action, SIGNAL( toggled(bool) ), this, SLOT( monospace(bool) ) );
    m_setmono_action->addTo( view );
    m_setmono_action->setOn(reader->m_bMonoSpaced);


    //    a = new QAction( tr( "Zoom" ), QString::null, 0, this, NULL, true );
    //    a = new QAction( tr( "Zoom" ), geticon( "mag" ), QString::null, 0, this, 0 );



    //    a->addTo( filebar() );
//     view->insertSeparator();

/*
    a = new QAction( tr( "Ideogram/Word" ), QString::null, 0, this, NULL, true );
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( monospace(bool) ) );
    a->setOn(reader->m_bMonoSpaced);
    a->addTo( view );
*/
/*
    a = new QAction( tr( "Set Width" ), QString::null, 0, this, NULL);
    connect( a, SIGNAL( activated() ), this, SLOT( setspacing() ) );
    a->addTo( view );
*/

    m_mark_action = new QAction( tr( "Bookmark" ), getmyicon( "bookmark" ), QString::null, 0, this, NULL);
    connect( m_mark_action, SIGNAL( activated() ), this, SLOT( addbkmk() ) );
    m_mark_action->addTo( marks );

    m_annotate_action = new QAction( tr( "Annotate" ), getmyicon( "annotate" ), QString::null, 0, this, NULL);
    connect( m_annotate_action, SIGNAL( activated() ), this, SLOT( addanno() ) );
    m_annotate_action->addTo( marks );

    m_goto_action = new QAction( tr( "Goto" ), getmyicon( "bookmark_goto" ), QString::null, 0, this, NULL, false );
    connect( m_goto_action, SIGNAL( activated() ), this, SLOT( do_gotomark() ) );
    m_goto_action->addTo( marks );

    m_delete_action = new QAction( tr( "Delete" ), getmyicon( "bookmark_delete" ), QString::null, 0, this, NULL);
    connect( m_delete_action, SIGNAL( activated() ), this, SLOT( do_delmark() ) );
    m_delete_action->addTo( marks );

    m_autogen_action = new QAction( tr( "Autogen" ), geticon( "exec" ), QString::null, 0, this, NULL, false );
    connect( m_autogen_action, SIGNAL( activated() ), this, SLOT( do_autogen() ) );
     marks->insertSeparator();
    m_autogen_action->addTo( marks );

    m_clear_action = new QAction( tr( "Clear" ), getmyicon( "bookmark_clear" ), QString::null, 0, this, NULL);
    connect( m_clear_action, SIGNAL( activated() ), this, SLOT( clearBkmkList() ) );
    m_clear_action->addTo( marks );

    m_save_action = new QAction( tr( "Save" ), getmyicon( "bookmark_save" ), QString::null, 0, this, NULL );
    connect( m_save_action, SIGNAL( activated() ), this, SLOT( savebkmks() ) );
    m_save_action->addTo( marks );

    m_tidy_action = new QAction( tr( "Tidy" ), getmyicon( "bookmark_tidy" ), QString::null, 0, this, NULL);
    connect( m_tidy_action, SIGNAL( activated() ), this, SLOT( listBkmkFiles() ) );
     marks->insertSeparator();
    m_tidy_action->addTo( marks );

    m_startBlock_action = new QAction( tr( "Start Block" ), geticon( "new" ), QString::null, 0, this, NULL);
    connect( m_startBlock_action, SIGNAL( activated() ), this, SLOT( editMark() ) );
    marks->insertSeparator();
    m_startBlock_action->addTo( marks );

    m_endBlock_action = new QAction( tr( "Copy Block" ), geticon( "copy" ), QString::null, 0, this, NULL);
    connect( m_endBlock_action, SIGNAL( activated() ), this, SLOT( editCopy() ) );
    m_endBlock_action->addTo( marks );

    m_bkmkAvail = NULL;


    setToolBarsMovable(m_tbmove);
    addtoolbars(&config);

    pbar = new QProgressBar(this);
    pbar->hide();

    searchBar = new QFloatBar( "Search", this, QMainWindow::Top, TRUE );

    searchBar->setHorizontalStretchable( TRUE );

    connect(searchBar, SIGNAL( OnHide() ), this, SLOT( restoreFocus() ));

    searchEdit = new QLineEdit( searchBar, "searchEdit" );
//      QFont f("unifont", 16 /*, QFont::Bold*/);
//      searchEdit->setFont( f );
    searchBar->setStretchableWidget( searchEdit );


#ifdef __ISEARCH
    connect( searchEdit, SIGNAL( textChanged(const QString&) ),
         this, SLOT( search(const QString&) ) );
#else
    connect( searchEdit, SIGNAL( returnPressed() ),
         this, SLOT( search() ) );
#endif
     QAction*a = new QAction( tr( "Find Next" ), geticon( "next" ), QString::null, 0, this, 0 );
     connect( a, SIGNAL( activated() ), this, SLOT( findNext() ) );
     a->addTo( searchBar );

    a = new QAction( tr( "Close Find" ), geticon( "close" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( findClose() ) );
    a->addTo( searchBar );

    searchBar->hide();

    regBar = new QFloatBar( "Autogen", this, QMainWindow::Top, TRUE );
    connect(regBar, SIGNAL( OnHide() ), this, SLOT( restoreFocus() ));

    regBar->setHorizontalStretchable( TRUE );

    regEdit = new QLineEdit( regBar, "regEdit" );
//    regEdit->setFont( f );

    regBar->setStretchableWidget( regEdit );

    connect( regEdit, SIGNAL( returnPressed() ),
         this, SLOT( do_regaction() ) );

    a = new QAction( tr( "Do Reg" ), geticon( "enter" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( do_regaction() ) );
    a->addTo( regBar );

    a = new QAction( tr( "Close Edit" ), geticon( "close" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( regClose() ) );
    a->addTo( regBar );

    regBar->hide();

    m_fontBar = new QToolBar( "Autogen", this, QMainWindow::Top, TRUE );

    m_fontBar->setHorizontalStretchable( TRUE );

//    odebug << "Font selector" << oendl;
    m_fontSelector = new QComboBox(false, m_fontBar);
    m_fontBar->setStretchableWidget( m_fontSelector );
    {
#ifndef USEQPE
    QFontDatabase f;
#else
    FontDatabase f;
#endif
    QStringList flist = f.families();
    bool realfont = false;
    for (QStringList::Iterator nm = flist.begin(); nm != flist.end(); nm++)
    {
        if (reader->m_fontname == *nm)
        {
        realfont = true;
        }
        if ((*nm).contains(FIXEDFONT,false)) reader->m_fontControl.hasCourier(true, *nm);
    }
    if (!realfont) reader->m_fontname = flist[0];
    } // delete the FontDatabase!!!
    connect( m_fontSelector, SIGNAL( activated(const QString&) ),
         this, SLOT( do_setfont(const QString&) ) );
    connect( m_fontSelector, SIGNAL( activated(int) ),
         this, SLOT( do_setencoding(int) ) );

    m_fontBar->hide();
    m_fontVisible = false;
#ifdef USEMSGS
    connect(qApp, SIGNAL( appMessage(const QCString&,const QByteArray&) ),
         this, SLOT( msgHandler(const QCString&,const QByteArray&) ) );
#endif
//    odebug << "Initing" << oendl;
  reader->init();
//    odebug << "Inited" << oendl;
//    m_buttonAction[m_spaceTarget]->setOn(true);
//    odebug << "fonting" << oendl;
    do_setfont(reader->m_fontname);
    if (!reader->m_lastfile.isEmpty())
    {
//  odebug << "doclnk" << oendl;
//  doc = new DocLnk(reader->m_lastfile);
//  odebug << "doclnk done" << oendl;
    if (pOpenlist != NULL)
    {

/*
        int ind = 0;
        Bkmk* p = (*pOpenlist)[ind];
        while (p != NULL && toQString(CFiledata(p->anno()).name()) != reader->m_lastfile)
        {
        p = (*pOpenlist)[++ind];
        }
*/
        Bkmk* p = NULL;
        for (CList<Bkmk>::iterator iter = pOpenlist->begin(); iter != pOpenlist->end(); iter++)
        {
        p = iter.pContent();
        if (toQString(CFiledata(p->anno()).name()) == reader->m_lastfile)
        {
            break;
        }
//      odebug << "Item:" << toQString(CFiledata(p->anno()).name()) << "" << oendl;
        p = NULL;
        }
        if (p != NULL)
        {
//  odebug << "openfrombkmk" << oendl;
        if (!openfrombkmk(p))
            showEditTools();
        }
        else
        {
//  odebug << "openfile" << oendl;
        openFile( reader->m_lastfile );
        }
    }
    else
    {
//      odebug << "Openfile 2" << oendl;
        if (!reader->m_lastfile.isEmpty())
        openFile( reader->m_lastfile );
    }
    }
    else
    {
      showEditTools();
    }
//    qApp->processEvents();
    reader->bDoUpdates = true;
    reader->update();
    config.setGroup("Version");
    int major = config.readNumEntry("Major", 0);
    int bkmktype = config.readNumEntry("BkmkType", 0);
    char minor = config.readNumEntry("Minor", 0);
    if (CheckVersion(major, bkmktype, minor))
    {
    config.writeEntry("Major", major);
    config.writeEntry("BkmkType", bkmktype);
    config.writeEntry("Minor", (int)minor);
    }
//    odebug << "finished update" << oendl;
}

void QTReaderApp::addtoolbars(Config* config)
{
    config->setGroup("Toolbar");

    if (fileBar != NULL)
    {
    if (fileBar != menubar)
    {
        fileBar->clear();
    }
    else
    {
        m_preferences_action->removeFrom( filebar() );
        m_open_action->removeFrom( filebar() );
        m_close_action->removeFrom( filebar() );
        m_info_action->removeFrom( filebar() );
        m_touch_action->removeFrom( filebar() );
        m_find_action->removeFrom( filebar() );
    }
    }

    m_preferences_action->addTo( filebar() );
    addfilebar(config, "Open", m_open_action);
    addfilebar(config, "Close", m_close_action);
    addfilebar(config, "Info", m_info_action);
    addfilebar(config, "Two/One Touch", m_touch_action);
    addfilebar(config, "Find", m_find_action);

    if (navBar != NULL)
    {
    if ((navBar == fileBar) && (fileBar == menubar))
    {
        m_scrollButton->removeFrom( navbar() );
        m_start_action->removeFrom( navbar() );
        m_end_action->removeFrom( navbar() );
        m_jump_action->removeFrom( navbar() );
        m_pageline_action->removeFrom( navbar() );
        m_pageup_action->removeFrom( navbar() );
        m_pagedn_action->removeFrom( navbar() );
        m_back_action->removeFrom( navbar() );
        m_home_action->removeFrom( navbar() );
        m_forward_action->removeFrom( navbar() );
    }
    else if (navBar != fileBar)
    {
        navBar->clear();
    }
    }

    addnavbar(config, "Scroll", m_scrollButton);
    addnavbar(config, "Goto Start", m_start_action);
    addnavbar(config, "Goto End", m_end_action);

    addnavbar(config, "Jump", m_jump_action);
    addnavbar(config, "Page/Line Scroll", m_pageline_action);

    addnavbar(config, "Page Up", m_pageup_action);
    addnavbar(config, "Page Down", m_pagedn_action);

    addnavbar(config, "Back", m_back_action);
    addnavbar(config, "Home", m_home_action);
    addnavbar(config, "Forward", m_forward_action);

    if (viewBar != NULL)
    {
    if ((viewBar == fileBar) && (fileBar == menubar))
    {
        m_actFullscreen->removeFrom( filebar() );
        m_zoomin_action->removeFrom( viewbar() );
        m_zoomout_action->removeFrom( viewbar() );
        m_setfont_action->removeFrom( viewbar() );
        m_setenc_action->removeFrom( viewbar() );
        m_setmono_action->removeFrom( viewbar() );
    }
    else if (viewBar != fileBar)
    {
        viewBar->clear();
    }
    }

    addviewbar(config, "Fullscreen", m_actFullscreen);
    addviewbar(config, "Zoom In", m_zoomin_action);
    addviewbar(config, "Zoom Out", m_zoomout_action);
    addviewbar(config, "Set Font", m_setfont_action);
    addviewbar(config, "Encoding Select", m_setenc_action);
    addviewbar(config, "Ideogram Mode", m_setmono_action);

    if (markBar != NULL)
    {
    if ((markBar == fileBar) && (fileBar == menubar))
    {
        m_mark_action->removeFrom( markbar() );
        m_annotate_action->removeFrom( markbar());
        m_goto_action->removeFrom( markbar() );
        m_delete_action->removeFrom( markbar() );
        m_autogen_action->removeFrom( markbar() );
        m_clear_action->removeFrom( markbar() );
        m_save_action->removeFrom( markbar() );
        m_tidy_action->removeFrom( markbar() );
        m_startBlock_action->removeFrom( markbar() );
        m_endBlock_action->removeFrom( markbar() );
    }
    else if (markBar != fileBar)
    {
        markBar->clear();
    }
    }
    addmarkbar(config, "Mark", m_mark_action);
    addmarkbar(config, "Annotate", m_annotate_action);
    addmarkbar(config, "Goto", m_goto_action);
    addmarkbar(config, "Delete", m_delete_action);
    addmarkbar(config, "Autogen", m_autogen_action);
    addmarkbar(config, "Clear", m_clear_action);
    addmarkbar(config, "Save", m_save_action);
    addmarkbar(config, "Tidy", m_tidy_action);
    addmarkbar(config, "Start Block", m_startBlock_action);
    addmarkbar(config, "Copy Block", m_endBlock_action);
    if (checkbar(config, "Annotation indicator"))
    {
    if (m_bkmkAvail == NULL)
    {
        m_bkmkAvail = new QAction( tr( "Annotation" ), geticon( "find" ), QString::null, 0, this, 0 );
        connect( m_bkmkAvail, SIGNAL( activated() ), this, SLOT( showAnnotation() ) );

        m_bkmkAvail->setEnabled(false);
    }
    QLabel *spacer = new QLabel(markBar, "");
    markbar()->setStretchableWidget(spacer);
    m_bkmkAvail->removeFrom( markbar() );
    m_bkmkAvail->addTo( markbar() );
    }
    else
    {
    if (m_bkmkAvail != NULL)
    {
        m_bkmkAvail->removeFrom( markbar() );
        delete m_bkmkAvail;
        m_bkmkAvail = NULL;
    }
    }
}

bool QTReaderApp::checkbar(Config* _config, const QString& key)
{
    return _config->readBoolEntry(key, false);
}


QToolBar* QTReaderApp::filebar()
{
    if (fileBar == NULL)
    {
    switch (m_tbpol)
    {
        case cesSingle:
//      odebug << "Setting filebar to menubar" << oendl;
        fileBar = menubar;
        break;
        default:
        odebug << "Incorrect toolbar policy set" << oendl;
        case cesMenuTool:
        case cesMultiple:
//      odebug << "Creating new file bar" << oendl;
        fileBar = new QToolBar("File", this, m_tbposition);
        break;
    }
//  fileBar->setHorizontalStretchable( true );
    }
    return fileBar;
}
QToolBar* QTReaderApp::viewbar()
{
    if (viewBar == NULL)
    {
    switch (m_tbpol)
    {
        case cesMultiple:
        viewBar = new QToolBar("View", this, m_tbposition);
        break;
        default:
        odebug << "Incorrect toolbar policy set" << oendl;
        case cesSingle:
        case cesMenuTool:
        viewBar = fileBar;
        break;
    }
    }
    return viewBar;
}
QToolBar* QTReaderApp::navbar()
{
    if (navBar == NULL)
    {
    switch (m_tbpol)
    {
        case cesMultiple:
//      odebug << "Creating new nav bar" << oendl;
        navBar = new QToolBar("Navigation", this, m_tbposition);
        break;
        default:
        odebug << "Incorrect toolbar policy set" << oendl;
        case cesSingle:
        case cesMenuTool:
        navBar = fileBar;
//      odebug << "Setting navbar to filebar" << oendl;
        break;
    }
    }
    return navBar;
}
QToolBar* QTReaderApp::markbar()
{
    if (markBar == NULL)
    {
    switch (m_tbpol)
    {
        case cesMultiple:
        markBar = new QToolBar("Marks", this, m_tbposition);
        break;
        default:
        odebug << "Incorrect toolbar policy set" << oendl;
        case cesSingle:
        case cesMenuTool:
        markBar = fileBar;
        break;
    }
    }
    return markBar;
}

void QTReaderApp::addfilebar(Config* _config, const QString& key, QAction* a)
{
    if (_config->readBoolEntry(key, false)) a->addTo( filebar() );
}
void QTReaderApp::addnavbar(Config* _config, const QString& key, QAction* a)
{
    if (_config->readBoolEntry(key, false)) a->addTo( navbar() );
}
void QTReaderApp::addmarkbar(Config* _config, const QString& key, QAction* a)
{
    if (_config->readBoolEntry(key, false)) a->addTo( markbar() );
}
void QTReaderApp::addviewbar(Config* _config, const QString& key, QAction* a)
{
    if (_config->readBoolEntry(key, false)) a->addTo( viewbar() );
}

void QTReaderApp::suspend() { reader->suspend(); }

#ifdef USEMSGS
void QTReaderApp::msgHandler(const QCString& _msg, const QByteArray& _data)
{
    QString msg = QString::fromUtf8(_msg);

////    odebug << "Received:" << msg << "" << oendl;

    QDataStream stream( _data, IO_ReadOnly );
    if ( msg == "info(QString)" )
    {
    QString info;
    stream >> info;
    QMessageBox::information(this, PROGNAME, info);
    }
    else if ( msg == "Update(int)" )
    {
    int info;
    stream >> info;
    if (info)
    {
        reader->bDoUpdates = true;
        reader->refresh();
    }
    else
    {
        reader->bDoUpdates = false;
    }
    }
    else if ( msg == "warn(QString)" )
    {
    QString info;
    stream >> info;
    QMessageBox::warning(this, PROGNAME, info);
    }
    else if ( msg == "exit()" )
    {
    m_dontSave = true;
    close();
    }
    else if ( msg == "pageDown()" )
    {
    reader->dopagedn();
    }
    else if ( msg == "pageUp()" )
    {
    reader->dopageup();
    }
    else if ( msg == "lineDown()" )
    {
    reader->lineDown();
    }
    else if ( msg == "lineUp()" )
    {
    reader->lineUp();
    }
    else if ( msg == "showText()" )
    {
    showEditTools();
    }
    else if ( msg == "home()" )
    {
    reader->goHome();
    }
    else if ( msg == "back()" )
    {
    reader->goBack();
    }
    else if ( msg == "forward()" )
    {
    reader->goForward();
    }
    else if ( msg == "File/Open(QString)" )
    {
    QString info;
    stream >> info;
    openFile( info );
    }
    else if ( msg == "File/Info()" )
    {
    showinfo();
    }
    else if ( msg == "File/Action(QString)" )
    {
    QString info;
    stream >> info;
    m_spaceTarget = ActNameToInt(info);
    }
    else if ( msg == "Navigation/Scroll(int)" )
    {
    int info;
    stream >> info;
    autoScroll(info);
    }

    else if ( msg == "Navigation/GotoStart()" )
    {
    gotoStart();
    }
    else if ( msg == "Navigation/GotoEnd()" )
    {
    gotoEnd();
    }
    else if ( msg == "Navigation/Jump(int)" )
    {
    int info;
    stream >> info;
    reader->locate(info);
    }
    else if ( msg == "Navigation/Page/LineScroll(int)" )
    {
    int info;
    stream >> info;
    pagemode(info);
    }
    else if ( msg == "Navigation/SetOverlap(int)" )
    {
    int info;
    stream >> info;
    reader->m_overlap = info;
    }
    else if ( msg == "Navigation/SetMargin(int)" )
    {
    int info;
    stream >> info;
    do_margin(info);
    }
    else if ( msg == "File/SetDictionary(QString)" )
    {
    QString info;
    stream >> info;
    do_settarget(info);
    }
#ifdef _SCROLLPIPE
    else if ( msg == "File/SetScrollTarget(QString)" )
    {
    QString info;
    stream >> info;
    reader->m_pipetarget = info;
    }
#endif
    else if ( msg == "File/Two/OneTouch(int)" )
    {
    int info;
    stream >> info;
    setTwoTouch(info);
    }
    else if ( msg == "Target/Annotation(int)" )
    {
    int info;
    stream >> info;
    OnAnnotation(info);
    }
    else if ( msg == "Target/Dictionary(int)" )
    {
    int info;
    stream >> info;
    OnDictionary(info);
    }
    else if ( msg == "Target/Clipboard(int)" )
    {
    int info;
    stream >> info;
    OnClipboard(info);
    }
    else if ( msg == "File/Find(QString)" )
    {
    QString info;
    stream >> info;
    QRegExp arg(info);
    size_t pos = reader->pagelocate();
    size_t start = pos;
    CDrawBuffer test(&(reader->m_fontControl));
    reader->getline(&test);
    while (arg.match(toQString(test.data())) == -1)
    {
        pos = reader->locate();
        if (!reader->getline(&test))
        {
        QMessageBox::information(this, PROGNAME, QString("Can't find\n")+info);
        pos = start;
        break;
        }
    }
    reader->locate(pos);
    }
    else if ( msg == "File/Fullscreen(int)" )
    {
    int info;
    stream >> info;
    setfullscreen(info);
    }
    else if ( msg == "File/Continuous(int)" )
    {
    int info;
    stream >> info;
    setcontinuous(info);
    }
    else if ( msg == "Markup(QString)" )
    {
    QString info;
    stream >> info;
    if (info == "Auto")
    {
        autofmt(true);
    }
    if (info == "None")
    {
        autofmt(false);
        textfmt(false);
        striphtml(false);
        peanut(false);
    }
    if (info == "Text")
    {
        textfmt(true);
    }
    if (info == "HTML")
    {
        striphtml(true);
    }
    if (info == "Peanut/PML")
    {
        peanut(true);
    }
    }
    else if ( msg == "Layout/StripCR(int)" )
    {
    int info;
    stream >> info;
    stripcr(info);
    }
    else if ( msg == "Layout/Dehyphen(int)" )
    {
    int info;
    stream >> info;
    dehyphen(info);
    }
    else if ( msg == "Layout/Depluck(int)" )
    {
    int info;
    stream >> info;
    depluck(info);
    }
    else if ( msg == "Layout/Dejpluck(int)" )
    {
    int info;
    stream >> info;
    dejpluck(info);
    }
    else if ( msg == "Layout/SingleSpace(int)" )
    {
    int info;
    stream >> info;
    onespace(info);
    }
#ifdef REPALM
    else if ( msg == "Layout/Repalm(int)" )
    {
    int info;
    stream >> info;
    repalm(info);
    }
#endif
    else if ( msg == "Layout/Unindent(int)" )
    {
    int info;
    stream >> info;
    unindent(info);
    }
    else if ( msg == "Layout/Re-paragraph(int)" )
    {
    int info;
    stream >> info;
    repara(info);
    }
    else if ( msg == "Layout/DoubleSpace(int)" )
    {
    int info;
    stream >> info;
    dblspce(info);
    }
    else if ( msg == "Layout/Indent(int)" )
    {
    int info;
    stream >> info;
    reader->bindenter = info;
    reader->setfilter(reader->getfilter());
    }
    else if ( msg == "Layout/Remap(int)" )
    {
    int info;
    stream >> info;
    remap(info);
    }
    else if ( msg == "Layout/Embolden(int)" )
    {
    int info;
    stream >> info;
    embolden(info);
    }
    else if ( msg == "Format/Ideogram/Word(int)" )
    {
    int info;
    stream >> info;
    monospace(info);
    }
    else if ( msg == "Format/SetWidth(int)" )
    {
    int info;
    stream >> info;
    reader->m_charpc = info;
    reader->setfont();
    reader->refresh();
    }
    else if ( msg == "Format/SetFont(QString,int)" )
    {
    QString fontname;
    int size;
    stream >> fontname;
    stream >> size;
    setfontHelper(fontname, size);
    }
    else if ( msg == "Marks/Autogen(QString)" )
    {
    QString info;
    stream >> info;
    do_autogen(info);
    }
    else if ( msg == "File/StartBlock()" )
    {
    editMark();
    }
    else if ( msg == "File/CopyBlock()" )
    {
    editCopy();
    }
}
#endif
ActionTypes QTReaderApp::ActNameToInt(const QString& _enc)
{
    for (int i = 0; i < MAX_ACTIONS; i++)
    {
    if (m_buttonAction[i]->text() == _enc) return (ActionTypes)i;
    }
    return cesAutoScroll;
}

void QTReaderApp::setfullscreen(bool sfs)
{
    reader->bDoUpdates = false;
    m_fullscreen = sfs;
    showEditTools();
//    qApp->processEvents();
    reader->bDoUpdates = true;
    reader->update();
}

void QTReaderApp::buttonActionSelected(QAction* _a)
{
////    odebug << "es:" << _a << " : " << (const char *)(_a->text()) << " (" << ActNameToInt(_a->text()) << ")" << oendl;
    m_spaceTarget = ActNameToInt(_a->text());
}

QTReaderApp::~QTReaderApp()
{
}

void QTReaderApp::autoScroll(bool _b)
{
    reader->setautoscroll(_b);
    setScrollState(reader->m_autoScroll);
}

void QTReaderApp::zoomin()
{
    reader->zoomin();
}

void QTReaderApp::zoomout()
{
    reader->zoomout();
}

void QTReaderApp::clearBkmkList()
{
    delete pBkmklist;
    pBkmklist = NULL;
    m_fBkmksChanged = false;
}

void QTReaderApp::fileClose()
{
    CCloseDialog* cd = new CCloseDialog(reader->m_string, false, this);
    if (cd->exec())
    {
    if (pOpenlist != NULL)
    {
        int ind = 0;
        Bkmk* p = (*pOpenlist)[ind];
        while (p != NULL && toQString(CFiledata(p->anno()).name()) != reader->m_lastfile)
        {
        p = (*pOpenlist)[++ind];
        }
        if (p != NULL) pOpenlist->erase(ind);
        if (cd->delFile())
        {
        unlink((const char*)reader->m_lastfile);
        }
        if (cd->delMarks())
        {
#ifndef USEQPE
        QDir d = QDir::home();                      // "/"
        d.cd(APPDIR);
        d.remove(reader->m_string);
#else /* USEQPE */
        unlink((const char *)Global::applicationFileName(APPDIR,reader->m_string));
#endif /* USEQPE */
        }
        if (cd->delConfig())
        {
#ifndef USEQPE
        QDir d = QDir::home();                      // "/"
        d.cd(APPDIR "/configs");
        d.remove(reader->m_string);
#else /* USEQPE */
        unlink((const char *)Global::applicationFileName(APPDIR "/configs",reader->m_string));
#endif /* USEQPE */
        }
    }

    fileOpen2();
    }
    delete cd;
}

void QTReaderApp::updatefileinfo()
{
    if (reader->m_string.isEmpty()) return;
    if (reader->m_lastfile.isEmpty()) return;
    tchar* nm = fromQString(reader->m_string);
    tchar* fl = fromQString(reader->m_lastfile);
//    odebug << "Lastfile:" << fl << "" << oendl;
    bool notadded = true;
    if (pOpenlist == NULL) pOpenlist = new CList<Bkmk>;
    else
    {
    for (CList<Bkmk>::iterator iter = pOpenlist->begin(); iter != pOpenlist->end(); iter++)
    {
        if (ustrcmp(CFiledata(iter->anno()).name(), fl) == 0)
        {
        iter->value(reader->pagelocate());
        unsigned short dlen;
        unsigned char* data;
        CFiledata fd(iter->anno());
        reader->setSaveData(data, dlen, fd.content(), fd.length());
//      odebug << "Filedata(1):" << fd.length() << ", " << dlen << "" << oendl;
//      getstate(data, dlen);
        iter->setAnno(data, dlen);
        notadded = false;
        delete [] data;
        break;
        }
    }
    }
//    odebug << "Added?:" << notadded << "" << oendl;
    if (notadded)
    {
    struct stat fnstat;
    stat((const char *)reader->m_lastfile, &fnstat);
    CFiledata fd(fnstat.st_mtime, fl);
    unsigned short dlen;
    unsigned char* data;
    reader->setSaveData(data, dlen, fd.content(), fd.length());
    pOpenlist->push_front(Bkmk(nm, data, dlen, reader->pagelocate()));
//  odebug << "Filedata(2):" << fd.length() << ", " << dlen << "" << oendl;
    delete [] data;
    }
    delete [] nm;
    delete [] fl;
}

void QTReaderApp::fileOpen()
{
/*
    menu->hide();
    fileBar->hide();
    if (regVisible) regBar->hide();
    if (searchVisible) searchBar->hide();
*/
//    odebug << "fileOpen" << oendl;
//    if (!reader->m_lastfile.isEmpty())
    updatefileinfo();
    fileOpen2();
}

void QTReaderApp::fileOpen2()
{
    if (pBkmklist != NULL)
    {
    if (m_fBkmksChanged)
    {
        if (QMessageBox::warning(this, PROGNAME, "Save bookmarks?", "Save", "Don't bother") == 0)
        savebkmks();
    }
    delete pBkmklist;
    pBkmklist = NULL;
    m_fBkmksChanged = false;
    }
    reader->disableAutoscroll();
/*
    editorStack->raiseWidget( fileSelector );
    fileSelector->reread();
*/
    bool usebrowser = true;
    if (pOpenlist != NULL)
    {
    m_nBkmkAction = cOpenFile;
    if (listbkmk(pOpenlist, "Browse")) usebrowser = false;
    }
    if (usebrowser)
    {
    QString fn = usefilebrowser();
//  qApp->processEvents();
    if (!fn.isEmpty() && QFileInfo(fn).isFile())
    {
        openFile(fn);
    }
    reader->setFocus();
    }
//    reader->refresh();
//    odebug << "HEIGHT:" << reader->m_lastheight << "" << oendl;
}

QString QTReaderApp::usefilebrowser()
{
#ifndef USEQPE
    QString s( QFileDialog::getOpenFileName( reader->m_lastfile, QString::null, this ) );
    return s;
#else
    fileBrowser* fb = new fileBrowser(false, this,"OpieReader",!m_bFloatingDialog,
                      0,
//                    WStyle_Customize | WStyle_NoBorderEx,
                      "*", QFileInfo(reader->m_lastfile).dirPath(true));


    QString fn;
    if (fb->exec())
    {
    fn = fb->getCurrentFile();
    }
//    odebug << "Selected " << fn << "" << oendl;
    delete fb;
    showEditTools();
    return fn;
#endif
}

void QTReaderApp::showgraphic(QImage& pm)
{
    QPixmap pc;
    pc.convertFromImage(pm);
    m_graphicwin->setPixmap(pc);
    editorStack->raiseWidget( m_graphicwin );
    m_graphicwin->setFocus();
}


void QTReaderApp::showprefs()
{
    CPrefs* prefwin = new CPrefs(!m_bFloatingDialog, this);

    prefwin->twotouch(m_twoTouch);
    prefwin->propfontchange(m_propogatefontchange);
    prefwin->StripCR(reader->bstripcr);
    prefwin->Dehyphen(reader->bdehyphen);
    prefwin->SingleSpace(reader->bonespace);
    prefwin->Unindent(reader->bunindent);
    prefwin->Reparagraph(reader->brepara);
    prefwin->DoubleSpace(reader->bdblspce);
    prefwin->Remap(reader->bremap);
    prefwin->Embolden(reader->bmakebold);
    prefwin->FullJustify(reader->bfulljust);
    prefwin->ParaLead(reader->getextraspace());
    prefwin->LineLead(reader->getlead());
    prefwin->Margin(reader->m_border);
    prefwin->Indent(reader->bindenter);
    if (reader->bautofmt)
    {
    prefwin->Markup(0);
    }
    else if (reader->btextfmt)
    {
    prefwin->Markup(2);
    }
    else if (reader->bstriphtml)
    {
    prefwin->Markup(3);
    }
    else if (reader->bpeanut)
    {
    prefwin->Markup(4);
    }
    else
    {
    prefwin->Markup(1);
    }
    prefwin->Depluck(reader->bdepluck);
    prefwin->Dejpluck(reader->bdejpluck);
    prefwin->Continuous(reader->m_continuousDocument);

    prefwin->dictApplication(m_targetapp);
    prefwin->dictMessage(m_targetmsg);

    prefwin->spaceAction(m_spaceTarget);
    prefwin->escapeAction(m_escapeTarget);
    prefwin->returnAction(m_returnTarget);
    prefwin->leftAction(m_leftTarget);
    prefwin->rightAction(m_rightTarget);
    prefwin->upAction(m_upTarget);
    prefwin->downAction(m_downTarget);

    prefwin->leftScroll(m_leftScroll);
    prefwin->rightScroll(m_rightScroll);
    prefwin->upScroll(m_upScroll);
    prefwin->downScroll(m_downScroll);

    prefwin->miscannotation(m_doAnnotation);
    prefwin->miscdictionary(m_doDictionary);
    prefwin->miscclipboard(m_doClipboard);

    prefwin->SwapMouse(reader->m_swapmouse);

    prefwin->Font(reader->m_fontname);

    prefwin->gfxsize(reader->getBaseSize());

    prefwin->pageoverlap(reader->m_overlap);

    prefwin->ideogram(reader->m_bMonoSpaced);

    prefwin->encoding(reader->m_encd);

    prefwin->ideogramwidth(reader->m_charpc);

    if (prefwin->exec())
    {
    m_twoTouch = prefwin->twotouch();
    reader->setTwoTouch(m_twoTouch);
    m_touch_action->setOn(m_twoTouch);

    reader->bstripcr = prefwin->StripCR();
    reader->bdehyphen = prefwin->Dehyphen();
    reader->bonespace = prefwin->SingleSpace();
    reader->bunindent = prefwin->Unindent();
    reader->brepara = prefwin->Reparagraph();
    reader->bdblspce = prefwin->DoubleSpace();
    reader->bremap = prefwin->Remap();
    reader->bmakebold = prefwin->Embolden();
    reader->bfulljust = prefwin->FullJustify();
    reader->setextraspace(prefwin->ParaLead());
    reader->setlead(prefwin->LineLead());
    reader->m_border = prefwin->Margin();
    reader->bindenter = prefwin->Indent();
    reader->bautofmt = reader->btextfmt = reader->bstriphtml = reader->bpeanut = false;
    switch (prefwin->Markup())
    {
        case 0:
        reader->bautofmt = true;
        break;
        case 1:
        break;
        case 2:
        reader->btextfmt = true;
        break;
        case 3:
        reader->bstriphtml = true;
        break;
        case 4:
        reader->bpeanut = true;
        break;
        default:
        odebug << "Format out of range" << oendl;
    }
    reader->bdepluck = prefwin->Depluck();
    reader->bdejpluck = prefwin->Dejpluck();
    reader->setContinuous(prefwin->Continuous());

    m_spaceTarget = (ActionTypes)prefwin->spaceAction();
    m_escapeTarget = (ActionTypes)prefwin->escapeAction();
    m_returnTarget = (ActionTypes)prefwin->returnAction();
    m_leftTarget = (ActionTypes)prefwin->leftAction();
    m_rightTarget = (ActionTypes)prefwin->rightAction();
    m_upTarget = (ActionTypes)prefwin->upAction();
    m_downTarget = (ActionTypes)prefwin->downAction();
    m_leftScroll = prefwin->leftScroll();
    m_rightScroll = prefwin->rightScroll();
    m_upScroll = prefwin->upScroll();
    m_downScroll = prefwin->downScroll();

    m_targetapp = prefwin->dictApplication();
    m_targetmsg = prefwin->dictMessage();

    m_doAnnotation = prefwin->miscannotation();
    m_doDictionary = prefwin->miscdictionary();
    m_doClipboard = prefwin->miscclipboard();
    reader->m_swapmouse = prefwin->SwapMouse();
    reader->setBaseSize(prefwin->gfxsize());
    reader->m_overlap = prefwin->pageoverlap();
    reader->m_bMonoSpaced = prefwin->ideogram();
    m_setmono_action->setOn(reader->m_bMonoSpaced);
    reader->m_encd = prefwin->encoding();
    reader->m_charpc = prefwin->ideogramwidth();

    if (
        reader->m_fontname != prefwin->Font()
        ||
        m_propogatefontchange != prefwin->propfontchange())
    {
        m_propogatefontchange = prefwin->propfontchange();
        setfontHelper(prefwin->Font());
    }
    delete prefwin;
    reader->setfilter(reader->getfilter());
    reader->refresh();

    }
    else
    {
    delete prefwin;
    }
}

void QTReaderApp::showtoolbarprefs()
{
#ifdef USEQPE
    CBarPrefs* prefwin = new CBarPrefs(APPDIR, !m_bFloatingDialog, this);
#else
    QFileInfo fi;
    QDir d = QDir::home();                      // "/"
    if ( !d.cd(APPDIR) )
    {                       // "/tmp"
        owarn << "Cannot find the \"~/" << APPDIR << "\" directory" << oendl;
        d = QDir::home();
        d.mkdir(APPDIR);
        d.cd(APPDIR);
    }
    fi.setFile(d, INIFILE);
    CBarPrefs* prefwin = new CBarPrefs(fi.absFilePath(), !m_bFloatingDialog, this);
#endif
    prefwin->tbpolicy(m_tbpolsave);
    prefwin->tbposition(m_tbposition-2);
    prefwin->tbmovable(m_tbmovesave);
    prefwin->floating(m_bFloatingDialog);
    if (prefwin->exec())
    {
    m_bFloatingDialog = prefwin->floating();
    if (
        m_tbpolsave != (ToolbarPolicy)prefwin->tbpolicy()
        ||
        m_tbposition != (ToolBarDock)(prefwin->tbposition()+2)
        ||
        m_tbmovesave != prefwin->tbmovable()
        )
    {
        QMessageBox::warning(this, PROGNAME, "Some changes won't take effect\nuntil the next time the\napplication is started");
    }
    m_tbpolsave = (ToolbarPolicy)prefwin->tbpolicy();
    m_tbposition = (ToolBarDock)(prefwin->tbposition()+2);
    m_tbmovesave = prefwin->tbmovable();
    bool isChanged = prefwin->isChanged();
    delete prefwin;
#ifdef USEQPE
    Config config( APPDIR );
#else
    QFileInfo fi;
    QDir d = QDir::home();                      // "/"
    if ( !d.cd(APPDIR) )
    {                       // "/tmp"
        owarn << "Cannot find the \"~/" << APPDIR << "\" directory" << oendl;
        d = QDir::home();
        d.mkdir(APPDIR);
        d.cd(APPDIR);
    }
    fi.setFile(d, INIFILE);
    Config config( fi.absFilePath() );
#endif
    if (isChanged) addtoolbars(&config);
    }
    else
    {
    delete prefwin;
    }
}

void QTReaderApp::showinfo()
{
    unsigned long fs, ts, pl;
    if (reader->empty())
    {
    QMessageBox::information(this, PROGNAME, "No file loaded", 1);
    }
    else
    {
    reader->sizes(fs,ts);
    pl = reader->pagelocate();
    m_infoWin->setFileSize(fs);
    m_infoWin->setTextSize(ts);
    m_infoWin->setRatio(100-(100*fs + (ts >> 1))/ts);
    m_infoWin->setLocation(pl);
    m_infoWin->setRead((100*pl + (ts >> 1))/ts);
    editorStack->raiseWidget( m_infoWin );
    m_infoWin->setFocus();
    }
}

void QTReaderApp::addAnno(const QString& name, const QString& text, size_t posn)
{
    if (pBkmklist == NULL) pBkmklist = new CList<Bkmk>;
#ifdef _UNICODE
    CBuffer buff(name.length()+1);
    int i;
    for (i = 0; i < name.length(); i++)
    {
    buff[i] = name[i].unicode();
    }
    buff[i] = 0;
    CBuffer buff2(text.length()+1);
    for (i = 0; i < text.length(); i++)
    {
    buff2[i] = text[i].unicode();
    }
    buff2[i] = 0;
    pBkmklist->push_front(Bkmk(buff.data(), buff2.data(), posn));
#else
    pBkmklist->push_front(Bkmk((const tchar*)text,posn));
#endif
    m_fBkmksChanged = true;
    pBkmklist->sort();
}

void QTReaderApp::addAnno(const QString& name, const QString& text)
{
    if (m_annoIsEditing)
    {
    if (name.isEmpty())
    {
        QMessageBox::information(this, PROGNAME, "Need a name for the bookmark\nPlease try again", 1);
    }
    else
    {
        addAnno(name, text, m_annoWin->getPosn());
    }
    showEditTools();
    }
    else
    {
    if (m_annoWin->edited())
    {
        CBuffer buff(text.length()+1);
        int i;
        for (i = 0; i < text.length(); i++)
        {
        buff[i] = text[i].unicode();
        }
        buff[i] = 0;
        m_fBkmksChanged = true;
        m_anno->setAnno(buff.data());
    }
    bool found = findNextBookmark(m_anno->value()+1);
    if (found)
    {
        m_annoWin->setName(toQString(m_anno->name()));
        m_annoWin->setAnno(toQString(m_anno->anno()));
    }
    else
    {
        showEditTools();
    }
    }
}

bool QTReaderApp::findNextBookmark(size_t start)
{
    bool found = false;
    for (CList<Bkmk>::iterator iter = pBkmklist->begin(); iter != pBkmklist->end(); iter++)
    {
    if (iter->value() >= start)
    {
        if (iter->value() < reader->locate())
        {
        found = true;
        m_anno = iter.pContent();
        }
        break;
    }
    }
    return found;
}

void QTReaderApp::addanno()
{
    if (reader->empty())
    {
    QMessageBox::information(this, PROGNAME, "No file loaded", 1);
    }
    else
    {
    m_annoWin->setName("");
    m_annoWin->setAnno("");
    m_annoWin->setPosn(reader->pagelocate());
    m_annoIsEditing = true;
    editorStack->raiseWidget( m_annoWin );
#ifdef USEQPE
    Global::showInputMethod();
#endif
    m_annoWin->setFocus();
    }
}

void QTReaderApp::infoClose()
{
    showEditTools();
}

/*
void QTReaderApp::fileRevert()
{
    clear();
    fileOpen();
}

void QTReaderApp::editCut()
{
#ifndef QT_NO_CLIPBOARD
    editor->cut();
#endif
}
*/
void QTReaderApp::editMark()
{
    m_savedpos = reader->pagelocate();
}

void QTReaderApp::editCopy()
{
    QClipboard* cb = QApplication::clipboard();
    QString text;
    int ch;
    unsigned long currentpos = reader->pagelocate();
    unsigned long endpos = reader->locate();
    if (m_savedpos == 0xffffffff)
    {
        m_savedpos = currentpos;
    }
    reader->jumpto(m_savedpos);
    while (reader->explocate() < endpos && (ch = reader->getch()) != UEOF)
    {
        text += ch;
    }
    cb->setText(text);
    reader->locate(currentpos);
    m_savedpos = 0xffffffff;
}

void QTReaderApp::gotoStart()
{
    reader->locate(reader->buffdoc.startSection());
}

void QTReaderApp::gotoEnd()
{
    reader->dopageup(reader->buffdoc.endSection());
}

void QTReaderApp::pageup()
{
    reader->NavUp();
}

void QTReaderApp::pagedn()
{
    reader->NavDown();
}

void QTReaderApp::pagemode(bool _b)
{
    reader->setpagemode(_b);
}

/*
void QTReaderApp::setspacing()
{
    m_nRegAction = cMonoSpace;
    char lcn[20];
    sprintf(lcn, "%lu", reader->m_charpc);
    regEdit->setText(lcn);
    do_regedit();
}
*/
void QTReaderApp::settarget()
{
    m_nRegAction = cSetTarget;
    QString text = ((m_targetapp.isEmpty()) ? QString("") : m_targetapp)
    + "/"
    + ((m_targetmsg.isEmpty()) ? QString("") : m_targetmsg);
    regEdit->setText(text);
    do_regedit();
}

/*
void QTReaderApp::do_mono(const QString& lcn)
{
    bool ok;
    unsigned long ulcn = lcn.toULong(&ok);
    if (ok)
    {
    reader->m_charpc = ulcn;
    reader->setfont();
    reader->refresh();
//  reader->setmono(true);
    }
    else
    QMessageBox::information(this, PROGNAME, "Must be a number");
}
*/
/*
void QTReaderApp::editPaste()
{
#ifndef QT_NO_CLIPBOARD
    editor->paste();
#endif
}
*/

void QTReaderApp::editFind()
{
  searchStart = reader->pagelocate();
#ifdef __ISEARCH
  searchStack = new QStack<searchrecord>;
#endif
#ifdef USEQPE
  Global::showInputMethod();
#endif
  searchBar->show();
  searchVisible = TRUE;
  searchEdit->setFocus();
#ifdef __ISEARCH
  searchStack->push(new searchrecord("",reader->pagelocate()));
#endif
}

void QTReaderApp::findNext()
{
//  //  odebug << "findNext called\n" << oendl;
#ifdef __ISEARCH
  QString arg = searchEdit->text();
#else
  QRegExp arg = searchEdit->text();
#endif
  CDrawBuffer test(&(reader->m_fontControl));
  size_t start = reader->pagelocate();
  reader->jumpto(start);
  reader->getline(&test);
  dosearch(start, test, arg);
}

void QTReaderApp::findClose()
{
  searchVisible = FALSE;
  searchEdit->setText("");
#ifdef USEQPE
  Global::hideInputMethod();
#endif
  searchBar->hide();
#ifdef __ISEARCH
//  searchStack = new QStack<searchrecord>;
  while (!searchStack->isEmpty())
    {
      delete searchStack->pop();
    }
  delete searchStack;
#endif
  reader->setFocus();
}

void QTReaderApp::regClose()
{
  regVisible = FALSE;
  regEdit->setText("");
  regBar->hide();
#ifdef USEQPE
  Global::hideInputMethod();
#endif
  reader->setFocus();
}

#ifdef __ISEARCH
bool QTReaderApp::dosearch(size_t start, CDrawBuffer& test, const QString& arg)
#else
bool QTReaderApp::dosearch(size_t start, CDrawBuffer& test, const QRegExp& arg)
#endif
{
  bool ret = true;
    unsigned long fs, ts;
    reader->sizes(fs,ts);
  size_t pos = reader->locate();
  pbar->setGeometry(searchBar->x(),searchBar->y(),searchBar->width(), searchBar->height());
  pbar->show();
  pbar->raise();
  pbar->reset();
  int offset;
  int lastpc = (100*pos)/ts;
  pbar->setProgress(lastpc);
//  qApp->processEvents();
  if (reader->buffdoc.getpara(test) >= 0)
  {
      reader->setFocus();
#ifdef __ISEARCH
      while (strstr(test.data(),(const tchar*)arg) == NULL)
#else
#ifdef _UNICODE
      while ((offset = arg.match(toQString(test.data()))) == -1)
#else
      while (arg.match(test.data()) == -1)
#endif
#endif
      {
      pos = reader->locate();
      int pc = (100*pos)/ts;
      if (pc != lastpc)
      {
          pbar->setProgress(pc);
          qApp->processEvents();
          reader->setFocus();
          lastpc = pc;
      }

      if (reader->buffdoc.getpara(test) < 0)
      {
          if (QMessageBox::warning(this, "Can't find", searchEdit->text(), 1, 2) == 2)
          pos = searchStart;
          else
          pos = start;
          findClose();
          pbar->hide();
          reader->locate(pos);
          return false;
      }
      }
//      odebug << "Found it at " << pos << ":" << offset << "" << oendl;
      pbar->hide();
//      odebug << "Hid" << oendl;
      reader->locate(pos+offset);
//      odebug << "Loacted" << oendl;
//      odebug << "page up" << oendl;
      ret = true;
  }
  else
  {
          if (QMessageBox::warning(this, "Can't find", searchEdit->text(), 1, 2) == 2)
          pos = searchStart;
          else
          pos = start;
          ret = false;
          findClose();
  }
  return ret;
}

#ifdef __ISEARCH
void QTReaderApp::search(const QString & arg)
{
  searchrecord* ss = searchStack->top();
  CBuffer test;
  size_t start = reader->pagelocate();
  bool haspopped = false;
  while (arg.left(ss->s.length()) != ss->s)
  {
      haspopped = true;
      start = ss->pos;
//      reader->locate(start);
      searchStack->pop();
      delete ss;
  }
  if (haspopped) reader->locate(start);
/*
  if (arg.length() < ss->len)
    {
      start = ss->pos;
      reader->locate(start);
      searchStack->pop();
      delete ss;
    }
*/
  else
    {
      start = reader->pagelocate();
      reader->jumpto(start);
      searchStack->push(new searchrecord(arg,start));
    }
    dosearch(start, test, arg);
}
#else
void QTReaderApp::search()
{
    findNext();
}
#endif

void QTReaderApp::openFile( const QString &f )
{
//    odebug << "File:" << f << "" << oendl;
//    openFile(DocLnk(f));
//}
//
//void QTReaderApp::openFile( const DocLnk &f )
//{
  clear();
  QFileInfo fm(f);
  if ( fm.exists() )
    {
//      QMessageBox::information(0, "Progress", "Calling fileNew()");
#ifdef USEQPE
    if (fm.extension( FALSE ) == "desktop")
    {
        DocLnk d(f);
        QFileInfo fnew(d.file());
        fm = fnew;
        if (!fm.exists()) return;
    }
#endif
      clear();

      reader->setText(fm.baseName(), fm.absFilePath());
      m_loadedconfig = readconfig(reader->m_string, false);
      showEditTools();
      readbkmks();
      m_savedpos = 0xffffffff;
    }
  else
    {
      QMessageBox::information(this, PROGNAME, "File does not exist");
      reader->m_lastfile = QString::null;
    }

}
/*
void QTReaderApp::resizeEvent(QResizeEvent* e)
{
    if (m_fullscreen)
    {
    showNormal();
    showFullScreen();
    }
}
*/
void QTReaderApp::handlekey(QKeyEvent* e)
{
//    odebug << "Keypress event" << oendl;
    timeb now;
    ftime(&now);
    unsigned long etime = (1000*(now.time - m_lastkeytime.time) + now.millitm)-m_lastkeytime.millitm;
    if (etime < m_debounce)
    {
    return;
    }
    m_lastkeytime = now;
    switch(e->key())
    {
    case Key_Escape:
//      odebug << "escape event" << oendl;
        if (m_disableesckey)
        {
        m_disableesckey = false;
        }
        else
        {
        m_bcloseDisabled = true;
        if (m_fullscreen)
        {
            m_actFullscreen->setOn(false);
            e->accept();
        }
        else
        {
//      odebug << "escape action" << oendl;
            doAction(m_escapeTarget, e);
        }
        }
        break;
    case Key_Space:
    {
        doAction(m_spaceTarget, e);
    }
    break;
    case Key_Return:
    {
        doAction(m_returnTarget, e);
    }
    break;
    case Key_Left:
    {
        if (reader->m_autoScroll && m_leftScroll)
        {
        reader->reduceScroll();
        }
        else
        {
        doAction(m_leftTarget, e);
        }
    }
    break;
    case Key_Right:
    {
        if (reader->m_autoScroll && m_rightScroll)
        {
        reader->increaseScroll();
        }
        else
        {
        doAction(m_rightTarget, e);
        }
    }
    break;
    case Key_Up:
    {
        if (reader->m_autoScroll && m_upScroll)
        {
        reader->increaseScroll();
        }
        else
        {
        doAction(m_upTarget, e);
        }
    }
    break;
    case Key_Down:
    {
        if (reader->m_autoScroll && m_downScroll)
        {
        reader->reduceScroll();
        }
        else
        {
        doAction(m_downTarget, e);
        }
    }
    break;
    default:
    {
        e->ignore();
    }

/*
    QString msg("Key press was:");
    QString key;
    msg += key.setNum(e->key());
    QMessageBox::information(this, PROGNAME, msg);
*/
    }
}

void QTReaderApp::showEditTools()
{
//    if ( !doc )
//  close();
    if (m_fullscreen)
    {
    if (menubar != NULL) menubar->hide();
    if (fileBar != NULL) fileBar->hide();
    if (viewBar != NULL) viewBar->hide();
    if (navBar != NULL) navBar->hide();
    if (markBar != NULL) markBar->hide();
    searchBar->hide();
    regBar->hide();
#ifdef USEQPE
    Global::hideInputMethod();
#endif
    m_fontBar->hide();
//  showNormal();
    showFullScreen();
    }
    else
    {
//  odebug << "him" << oendl;
#ifdef USEQPE
    Global::hideInputMethod();
#endif
//  odebug << "eb" << oendl;
    menubar->show();
    if (fileBar != NULL) fileBar->show();
    if (viewBar != NULL) viewBar->show();
    if (navBar != NULL) navBar->show();
    if (markBar != NULL) markBar->show();
    mb->show();
    if ( searchVisible )
    {
#ifdef USEQPE
        Global::showInputMethod();
#endif
        searchBar->show();
    }
    if ( regVisible )
    {
#ifdef USEQPE
        Global::showInputMethod();
#endif
        regBar->show();
    }
    if (m_fontVisible) m_fontBar->show();
//  odebug << "sn" << oendl;
    showNormal();
//  odebug << "sm" << oendl;
#ifdef USEQPE
    showMaximized();
#endif
//  setCentralWidget(reader);
    }

//    odebug << "uc" << oendl;
    updateCaption();
//    odebug << "rw" << oendl;
    editorStack->raiseWidget( reader );
//    odebug << "sf" << oendl;
    reader->setFocus();
    reader->refresh();
}
/*
void QTReaderApp::save()
{
    if ( !doc )
    return;
    if ( !editor->edited() )
    return;

    QString rt = editor->text();
    QString pt = rt;

    if ( doc->name().isEmpty() ) {
    unsigned ispace = pt.find( ' ' );
    unsigned ienter = pt.find( '\n' );
    int i = (ispace < ienter) ? ispace : ienter;
    QString docname;
    if ( i == -1 ) {
        if ( pt.isEmpty() )
        docname = "Empty Text";
        else
        docname = pt;
    } else {
        docname = pt.left( i );
    }
    doc->setName(docname);
    }
    FileManager fm;
    fm.saveFile( *doc, rt );
}
*/

void QTReaderApp::clear()
{
//    if (doc != 0)
//      {
//  QMessageBox::information(this, PROGNAME, "Deleting doc", 1);
//  delete doc;
//  QMessageBox::information(this, PROGNAME, "Deleted doc", 1);
//  doc = 0;
    //     }
    reader->clear();
}

void QTReaderApp::updateCaption()
{
//    if ( !doc )
//  setCaption( tr("QTReader") );
//    else {
//  QString s = doc->name();
//  if ( s.isEmpty() )
//      s = tr( "Unnamed" );
    setCaption( reader->m_string + " - " + tr("Reader") );
//    }
}

void QTReaderApp::setDocument(const QString& fileref)
{
    bFromDocView = TRUE;
//QMessageBox::information(0, "setDocument", fileref);
    openFile(fileref);
//    showEditTools();
}

void QTReaderApp::closeEvent( QCloseEvent *e )
{
//    odebug << "Close event" << oendl;
    if (m_fullscreen)
    {
    m_fullscreen = false;
    showEditTools();
    e->accept();
    }
    else if (m_dontSave)
    {
    e->accept();
    }
    else
    {
    if (editorStack->visibleWidget() == reader)
    {
        if ((m_escapeTarget != cesNone) && m_bcloseDisabled)
        {
//  odebug << "Close disabled" << oendl;
        m_bcloseDisabled = false;
        e->ignore();
        }
        else
        {
        if (m_fontVisible)
        {
            m_fontBar->hide();
            m_fontVisible = false;
        }
        if (regVisible)
        {
            regBar->hide();
#ifdef USEQPE
            Global::hideInputMethod();
#endif
            regVisible = false;
            return;
        }
        if (searchVisible)
        {
            searchBar->hide();
#ifdef USEQPE
            Global::hideInputMethod();
#endif
            searchVisible = false;
            return;
        }
        if (m_fBkmksChanged && pBkmklist != NULL)
        {
            if (QMessageBox::warning(this, PROGNAME, "Save bookmarks?", "Save", "Don't bother") == 0)
            savebkmks();
            delete pBkmklist;
            pBkmklist = NULL;
            m_fBkmksChanged = false;
        }
        bFromDocView = FALSE;
        updatefileinfo();
        saveprefs();
        e->accept();
        }
    }
    else
    {
        showEditTools();
        m_disableesckey = true;
    }
    }
}

void QTReaderApp::do_gotomark()
{
    m_nBkmkAction = cGotoBkmk;
    if (!listbkmk(pBkmklist))
    QMessageBox::information(this, PROGNAME, "No bookmarks in memory");
}

void QTReaderApp::do_delmark()
{
    m_nBkmkAction = cDelBkmk;
    if (!listbkmk(pBkmklist))
    QMessageBox::information(this, PROGNAME, "No bookmarks in memory");
}

bool QTReaderApp::listbkmk(CList<Bkmk>* plist, const QString& _lab)
{
    bkmkselector->clear();
    if (_lab.isEmpty())
    bkmkselector->setText("Cancel");
    else
    bkmkselector->setText(_lab);
    int cnt = 0;
    if (plist != NULL)
      {
    for (CList<Bkmk>::iterator i = plist->begin(); i != plist->end(); i++)
      {
#ifdef _UNICODE
//        odebug << "Item:" << toQString(i->name()) << "" << oendl;
        bkmkselector->insertItem(toQString(i->name()));
#else
        bkmkselector->insertItem(i->name());
#endif
        cnt++;
      }
      }
    if (cnt > 0)
      {
      hidetoolbars();
        editorStack->raiseWidget( bkmkselector );
    return true;
      }
    else
    return false;
}

void QTReaderApp::do_autogen()
{
  m_nRegAction = cAutoGen;
  regEdit->setText(m_autogenstr);
  do_regedit();
}

void QTReaderApp::do_regedit()
{
//    fileBar->hide();
    reader->bDoUpdates = false;
//    odebug << "Showing regbar" << oendl;
  regBar->show();
//    odebug << "Showing kbd" << oendl;
#ifdef USEQPE
  Global::showInputMethod();
#endif
  regVisible = true;
  regEdit->setFocus();
//  qApp->processEvents();
    reader->bDoUpdates = true;
    reader->update();
}

bool QTReaderApp::openfrombkmk(Bkmk* bk)
{
    QString fn = toQString(
    CFiledata(bk->anno()).name()
    );
//  odebug << "fileinfo" << oendl;
    if (!fn.isEmpty() && QFileInfo(fn).isFile())
    {
//  odebug << "Opening" << oendl;
    openFile(fn);
    struct stat fnstat;
    stat((const char *)reader->m_lastfile, &fnstat);

    if (CFiledata(bk->anno()).date()
        != fnstat.st_mtime)
    {
        CFiledata fd(bk->anno());
        fd.setdate(fnstat.st_mtime);
        bk->value(0);
    }
    else
    {
        unsigned short svlen = bk->filedatalen();
        unsigned char* svdata = bk->filedata();
        reader->putSaveData(svdata, svlen);
//      setstate(svdata, svlen);
        if (svlen != 0)
        {
        QMessageBox::warning(this, PROGNAME, "Not all file data used\nNew version?");
        }
//      odebug << "updating" << oendl;
//      showEditTools();
        reader->locate(bk->value());
    }
    return true;
    }
    else
    {
    return false;
    }
}

void QTReaderApp::gotobkmk(int ind)
{
    showEditTools();
    switch (m_nBkmkAction)
    {
    case cOpenFile:
    {
//      qApp->processEvents();
        if (!openfrombkmk((*pOpenlist)[ind]))
        {
        pOpenlist->erase(ind);
        QMessageBox::information(this, PROGNAME, "Can't find file");
        }
    }
    break;
    case cGotoBkmk:
        reader->locate((*pBkmklist)[ind]->value());
        break;
    case cDelBkmk:
////        odebug << "Deleting:" << (*pBkmklist)[ind]->name() << "\n" << oendl;
        pBkmklist->erase(ind);
        m_fBkmksChanged = true;
//      pBkmklist->sort();
        break;
    case cRmBkmkFile:
        {
#ifndef USEQPE
        QDir d = QDir::home();                      // "/"
        d.cd(APPDIR);
        d.remove(bkmkselector->text(ind));
#else /* USEQPE */
        unlink((const char *)Global::applicationFileName(APPDIR,bkmkselector->text(ind)));
#endif /* USEQPE */
        }
        break;
    case cLdConfig:
        readconfig(bkmkselector->text(ind), false);
        break;
    case cRmConfig:
        {
#ifndef USEQPE
        QDir d = QDir::home();                      // "/"
        d.cd(APPDIR "/configs");
        d.remove(bkmkselector->text(ind));
#else /* USEQPE */
        unlink((const char *)Global::applicationFileName(APPDIR "/configs",bkmkselector->text(ind)));
#endif /* USEQPE */
        }
        break;
    case cExportLinks:
    {
#ifndef USEQPE
        QDir d = QDir::home();                      // "/"
        d.cd(APPDIR "/urls");
        QFileInfo fi(d, bkmkselector->text(ind));
        if (fi.exists())
        {
        QString outfile( QFileDialog::getSaveFileName( QString::null, QString::null, this ) );
        if (!outfile.isEmpty())
        {
            FILE* fout = fopen((const char *)outfile, "w");
            if (fout != NULL)
            {
            FILE* fin = fopen((const char *)fi.absFilePath(), "r");
            if (fin != NULL)
            {
                fprintf(fout, "<html><body>\n");
                int ch = 0;
                while ((ch = fgetc(fin)) != EOF)
                {
                fputc(ch, fout);
                }
                fclose(fin);
                fprintf(fout, "</html></body>\n");
                d.remove(bkmkselector->text(ind));
            }
            fclose(fout);
            }
            else
            QMessageBox::information(this, PROGNAME, "Couldn't open output");
        }
        }
#else /* USEQPE */
        FILE* fin = fopen((const char *)Global::applicationFileName(APPDIR "/urls",bkmkselector->text(ind)), "r");
        if (fin != NULL)
        {
        bool allok = false;
        fileBrowser* fb = new fileBrowser(true, this,"OpieReader",!m_bFloatingDialog, 0, "*", QString::null);
        if (fb->exec())
        {
            QString outfile = fb->getCurrentFile();
            FILE* fout = fopen((const char *)outfile, "w");
            if (fout != NULL)
            {
            fprintf(fout, "<html><body>\n");
            int ch = 0;
            while ((ch = fgetc(fin)) != EOF)
            {
                fputc(ch, fout);
            }
            fprintf(fout, "</html></body>\n");
            fclose(fout);
            allok = true;
            }
            else
            QMessageBox::information(this, PROGNAME, "Couldn't open output");
        }
        delete fb;
        fclose(fin);
        if (allok) unlink((const char *)Global::applicationFileName(APPDIR "/urls",bkmkselector->text(ind)));
        }
        else
        {
        QMessageBox::information(this, PROGNAME, "Couldn't open input");
        }

/*
        CFileSelector *f = new CFileSelector("text/html", this, NULL, !m_bFloatingDialog, TRUE, TRUE );
        int ret = f->exec();
        odebug << "Return:" << ret << "" << oendl;
        DocLnk* doc = f->getDoc();
        if (doc != NULL)
        {
        FILE* fin = fopen((const char *)Global::applicationFileName(APPDIR "/urls",bkmkselector->text(ind)), "r");
        QString rt;
        rt = "<html><body>\n";
        int ch = 0;
        while ((ch = fgetc(fin)) != EOF)
        {
            rt += (char)ch;
        }
        fclose(fin);
        rt += "</html></body>\n";
        if ( doc->name().isEmpty() )
        {
            doc->setName(bkmkselector->text(ind));
        }
        FileManager fm;
        fm.saveFile( *doc, rt );
        odebug << "YES" << oendl;
        }
        else
        {
        odebug << "NO" << oendl;
        }
        delete f;
*/

#endif /* USEQPE */
    }
    break;
    }
}

void QTReaderApp::cancelbkmk()
{
    if (m_nBkmkAction == cOpenFile)
    {
    QString fn = usefilebrowser();
    if (!fn.isEmpty() && QFileInfo(fn).isFile()) openFile(fn);
    }
  showEditTools();
}

void QTReaderApp::jump()
{
  m_nRegAction = cJump;
  char lcn[20];
  sprintf(lcn, "%lu", reader->pagelocate());
  regEdit->setText(lcn);
  do_regedit();
}

void QTReaderApp::do_jump(const QString& lcn)
{
    bool ok;
    unsigned long ulcn = lcn.toULong(&ok);
    if (ok)
    reader->locate(ulcn);
    else
    QMessageBox::information(this, PROGNAME, "Must be a number");
}

void QTReaderApp::do_regaction()
{
    reader->bDoUpdates = false;
  regBar->hide();
#ifdef USEQPE
  Global::hideInputMethod();
#endif
  regVisible = false;
  switch(m_nRegAction)
  {
      case cAutoGen:
      do_autogen(regEdit->text());
      break;
      case cAddBkmk:
      do_addbkmk(regEdit->text());
      break;
      case cJump:
      do_jump(regEdit->text());
      break;
/*
      case cMonoSpace:
      do_mono(regEdit->text());
      break;
*/
      case cSetTarget:
      do_settarget(regEdit->text());
      break;
#ifdef _SCROLLPIPE
      case cSetPipeTarget:
      do_setpipetarget(regEdit->text());
      break;
#endif
      case cSetConfigName:
//    odebug << "Saving config" << oendl;
      do_saveconfig(regEdit->text(), false);
      break;
  }
//  reader->restore();
//    fileBar->show();
  reader->setFocus();
//    qApp->processEvents();
    reader->bDoUpdates = true;
    reader->update();
}

void QTReaderApp::do_settarget(const QString& _txt)
{
    int ind = _txt.find('/');
    if (ind == -1)
    {
    m_targetapp = "";
    m_targetmsg = "";
    QMessageBox::information(this, PROGNAME, "Format is\nappname/messagename");
    }
    else
    {
    m_targetapp = _txt.left(ind);
    m_targetmsg = _txt.right(_txt.length()-ind-1);
    }
}

void QTReaderApp::chooseencoding()
{
    m_fontSelector->clear();
    m_fontSelector->insertItem("Ascii");
    m_fontSelector->insertItem("UTF-8");
    m_fontSelector->insertItem("UCS-2(BE)");
    m_fontSelector->insertItem("USC-2(LE)");
    m_fontSelector->insertItem("Palm");
    for (unicodetable::iterator iter = unicodetable::begin(); iter != unicodetable::end(); iter++)
    {
    m_fontSelector->insertItem(iter->mime);
    } // delete the FontDatabase!!!
    m_fontSelector->setCurrentItem (reader->m_encd);
    m_fontAction = cChooseEncoding;
    m_fontBar->show();
    m_fontVisible = true;
}

void QTReaderApp::setfont()
{
    m_fontSelector->clear();
    {
#ifdef USEQPE
    FontDatabase f;
#else
    QFontDatabase f;
#endif
    QStringList flist = f.families();
    m_fontSelector->insertStringList(flist);
    } // delete the FontDatabase!!!

    for (int i = 1; i <= m_fontSelector->count(); i++)
    {
    if (m_fontSelector->text(i) == reader->m_fontname)
    {
        m_fontSelector->setCurrentItem(i);
        break;
    }
    }
    m_fontAction = cChooseFont;
    m_fontBar->show();
    m_fontVisible = true;
}

void QTReaderApp::setfontHelper(const QString& lcn, int size)
{
    if (size == 0) size = reader->m_fontControl.currentsize();
    if (m_propogatefontchange)
    {
    QFont f(lcn, 10);
    bkmkselector->setFont( f );
    regEdit->setFont( f );
    searchEdit->setFont( f );
    m_annoWin->setFont( f );
    }
    reader->m_fontname = lcn;
    if (!reader->ChangeFont(size))
    {
    reader->ChangeFont(size);
    }
}

void QTReaderApp::do_setencoding(int i)
{
//    odebug << "setencoding:" << i << "" << oendl;
    if (m_fontAction == cChooseEncoding)
    {
    reader->setencoding(i);
    }
    reader->refresh();
    m_fontBar->hide();
    m_fontVisible = false;
//    odebug << "showedit" << oendl;
    if (reader->isVisible()) showEditTools();
//    odebug << "showeditdone" << oendl;
}

void QTReaderApp::do_setfont(const QString& lcn)
{
    if (m_fontAction == cChooseFont)
    {
    setfontHelper(lcn);
    }
    reader->refresh();
    m_fontBar->hide();
    m_fontVisible = false;
//    odebug << "showedit" << oendl;
    //if (reader->isVisible())
    showEditTools();
//    odebug << "showeditdone" << oendl;
}

void QTReaderApp::do_autogen(const QString& regText)
{
    unsigned long fs, ts;
    reader->sizes(fs,ts);
//  //  odebug << "Reg:" << (const tchar*)(regEdit->text()) << "\n" << oendl;
  m_autogenstr = regText;
  QRegExp re(regText);
  CBuffer buff;
  if (pBkmklist != NULL) delete pBkmklist;
  pBkmklist = new CList<Bkmk>;
  m_fBkmksChanged = true;

  pbar->setGeometry(regBar->x(),regBar->y(),regBar->width(), regBar->height());
  pbar->show();
  pbar->raise();
  pbar->reset();
  reader->update();
  qApp->processEvents();
  reader->setFocus();
  reader->jumpto(0);
  int lastpc = 0;
  int i = 0;
  while (i >= 0)
    {
      unsigned int lcn = reader->locate();
      int pc = (100*lcn)/ts;
      if (pc != lastpc)
      {
        pbar->setProgress(pc);
    qApp->processEvents();
    if (reader->locate() != lcn) reader->jumpto(lcn);
    reader->setFocus();
        lastpc = pc;
      }
      i = reader->buffdoc.getpara(buff);
#ifdef _UNICODE
      if (re.match(toQString(buff.data())) != -1)
#else
      if (re.match(buff.data()) != -1)
#endif
    pBkmklist->push_back(Bkmk(buff.data(), NULL, lcn));
    }
  pBkmklist->sort();
  pbar->setProgress(100);
  qApp->processEvents();
  pbar->hide();
  reader->refresh();
}

void QTReaderApp::saveprefs()
{
//    odebug << "saveprefs" << oendl;
//  reader->saveprefs("uqtreader");
//    if (!m_loadedconfig)
    do_saveconfig( APPDIR, true );

/*
    Config config( APPDIR );
    config.setGroup( "View" );

    reader->m_lastposn = reader->pagelocate();

    config.writeEntry("FloatDialogs", m_bFloatingDialog);
    config.writeEntry( "StripCr", reader->bstripcr );
    config.writeEntry( "AutoFmt", reader->bautofmt );
    config.writeEntry( "TextFmt", reader->btextfmt );
    config.writeEntry( "StripHtml", reader->bstriphtml );
    config.writeEntry( "Dehyphen", reader->bdehyphen );
    config.writeEntry( "Depluck", reader->bdepluck );
    config.writeEntry( "Dejpluck", reader->bdejpluck );
    config.writeEntry( "OneSpace", reader->bonespace );
    config.writeEntry( "Unindent", reader->bunindent );
    config.writeEntry( "Repara", reader->brepara );
    config.writeEntry( "DoubleSpace", reader->bdblspce );
    config.writeEntry( "Indent", reader->bindenter );
    config.writeEntry( "FontSize", (int)(reader->m_fontControl.currentsize()) );
    config.writeEntry( "ScrollDelay", reader->m_delay);
    config.writeEntry( "LastFile", reader->m_lastfile );
    config.writeEntry( "LastPosn", (int)(reader->pagelocate()) );
    config.writeEntry( "PageMode", reader->m_bpagemode );
    config.writeEntry( "MonoSpaced", reader->m_bMonoSpaced );
    config.writeEntry( "SwapMouse", reader->m_swapmouse);
    config.writeEntry( "Fontname", reader->m_fontname );
    config.writeEntry( "Encoding", reader->m_encd );
    config.writeEntry( "CharSpacing", reader->m_charpc );
    config.writeEntry( "Overlap", (int)(reader->m_overlap) );
    config.writeEntry( "Margin", (int)reader->m_border );
    config.writeEntry( "TargetApp", m_targetapp );
    config.writeEntry( "TargetMsg", m_targetmsg );
#ifdef _SCROLLPIPE
    config.writeEntry( "PipeTarget", reader->m_pipetarget );
    config.writeEntry( "PauseAfterPara", reader->m_pauseAfterEachPara );
#endif
    config.writeEntry( "TwoTouch", m_twoTouch );
    config.writeEntry( "Annotation", m_doAnnotation);
    config.writeEntry( "Dictionary", m_doDictionary);
    config.writeEntry( "Clipboard", m_doClipboard);
    config.writeEntry( "SpaceTarget", m_spaceTarget);
    config.writeEntry( "EscapeTarget", m_escapeTarget);
    config.writeEntry( "ReturnTarget", m_returnTarget);
    config.writeEntry( "LeftTarget", m_leftTarget);
    config.writeEntry( "RightTarget", m_rightTarget);
    config.writeEntry( "UpTarget", m_upTarget);
    config.writeEntry( "DownTarget", m_downTarget);
    config.writeEntry("LeftScroll", m_leftScroll);
    config.writeEntry("RightScroll", m_rightScroll);
    config.writeEntry("UpScroll", m_upScroll);
    config.writeEntry("DownScroll", m_downScroll);
#ifdef REPALM
    config.writeEntry( "Repalm", reader->brepalm );
#endif
    config.writeEntry( "Remap", reader->bremap );
    config.writeEntry( "Peanut", reader->bpeanut );
    config.writeEntry( "MakeBold", reader->bmakebold );
    config.writeEntry( "Continuous", reader->m_continuousDocument );
    config.writeEntry( "FullJust", reader->bfulljust );
    config.writeEntry( "ExtraSpace", reader->getextraspace() );
    config.writeEntry( "ExtraLead", reader->getlead() );
    config.writeEntry( "Basesize",  (int)reader->getBaseSize());
    config.writeEntry( "RequestorFontChange", m_propogatefontchange);

    config.setGroup( "Toolbar" );
    config.writeEntry("Movable", m_tbmovesave);
    config.writeEntry("Policy", m_tbpolsave);
    config.writeEntry("Position", m_tbposition);
*/
    savefilelist();
}

/*
void QTReaderApp::oldFile()
{
//  odebug << "oldFile called" << oendl;
  reader->setText(true);
//  odebug << "settext called" << oendl;
  showEditTools();
//  odebug << "showedit called" << oendl;
}
*/

/*
void info_cb(Fl_Widget* o, void* _data)
{

    if (infowin == NULL)
    {

    infowin = new Fl_Window(160,240);
    filename = new Fl_Output(45,5,110,14,"Filename");
    filesize = new Fl_Output(45,25,110,14,"Filesize");
    textsize = new Fl_Output(45,45,110,14,"Textsize");
    comprat = new CBar(45,65,110,14,"Ratio %");
    posn = new Fl_Output(45,85,110,14,"Location");
    frcn = new CBar(45,105,110,14,"% Read");
    about = new Fl_Multiline_Output(5,125,150,90);
    about->value("TWReader - $Name:  $\n\nA file reader program for the Agenda\n\nReads text, PalmDoc and ppms format files");
    Fl_Button *jump_accept = new Fl_Button(62,220,35,14,"Okay");
    infowin->set_modal();
    }
    if (((reader_ui *)_data)->g_filename[0] != '\0')
    {
    unsigned long fs,ts;
    tchar sz[20];
    ((reader_ui *)_data)->input->sizes(fs,ts);
    unsigned long pl = ((reader_ui *)_data)->input->locate();

    filename->value(((reader_ui *)_data)->g_filename);

    sprintf(sz,"%u",fs);
    filesize->value(sz);

    sprintf(sz,"%u",ts);
    textsize->value(sz);

    comprat->value(100-(100*fs + (ts >> 1))/ts);

    sprintf(sz,"%u",pl);
    posn->value(sz);

    frcn->value((100*pl + (ts >> 1))/ts);
    }
    infowin->show();
}
*/

void QTReaderApp::savebkmks()
{
    if (pBkmklist != NULL)
    {
#ifndef USEQPE
        QDir d = QDir::home();                      // "/"
        d.cd(APPDIR);
        QFileInfo fi(d, reader->m_string);
    BkmkFile bf((const char *)fi.absFilePath(), true);
#else /* USEQPE */
    BkmkFile bf((const char *)Global::applicationFileName(APPDIR,reader->m_string), true);
#endif /* USEQPE */
    bf.write(*pBkmklist);
    }
    m_fBkmksChanged = false;
}

void QTReaderApp::readfilelist()
{
#ifndef USEQPE
        QDir d = QDir::home();                      // "/"
        d.cd(APPDIR);
        QFileInfo fi(d, ".openfiles");
    BkmkFile bf((const char *)fi.absFilePath());
#else /* USEQPE */
    BkmkFile bf((const char *)Global::applicationFileName(APPDIR,".openfiles"));
#endif /* USEQPE */
//    odebug << "Reading open files" << oendl;
    pOpenlist = bf.readall();
//    if (pOpenlist != NULL) odebug << "...with success" << oendl;
//    else odebug << "...without success!" << oendl;
}

void QTReaderApp::savefilelist()
{
    if (pOpenlist != NULL)
    {
#ifndef USEQPE
        QDir d = QDir::home();                      // "/"
        d.cd(APPDIR);
        QFileInfo fi(d, ".openfiles");
    BkmkFile bf((const char *)fi.absFilePath(), true);
#else /* USEQPE */
    BkmkFile bf((const char *)Global::applicationFileName(APPDIR,".openfiles"), true);
#endif /* USEQPE */
//  odebug << "Writing open files" << oendl;
    bf.write(*pOpenlist);
    }
}

void QTReaderApp::readbkmks()
{
    if (pBkmklist != NULL)
    {
        delete pBkmklist;
    }
    struct stat fnstat;
    struct stat bkstat;
#ifndef USEQPE
        QDir d = QDir::home();                      // "/"
        d.cd(APPDIR);
        QFileInfo fi(d, reader->m_string);
#endif /* ! USEQPE */
    if (
    stat((const char *)reader->m_lastfile, &fnstat) == 0
    &&
#ifndef USEQPE
    stat((const char *)fi.absFilePath(), &bkstat) == 0
#else /* USEQPE */
    stat((const char *)Global::applicationFileName(APPDIR,reader->m_string), &bkstat) == 0
#endif /* USEQPE */
    )
    {
    if (bkstat.st_mtime < fnstat.st_mtime)
    {
#ifndef USEQPE
        unlink((const char *)fi.absFilePath());
#else /* USEQPE */
        unlink((const char *)Global::applicationFileName(APPDIR,reader->m_string));
#endif /* USEQPE */
    }
    }

#ifndef USEQPE
    BkmkFile bf((const char *)fi.absFilePath());
#else /* USEQPE */
    BkmkFile bf((const char *)Global::applicationFileName(APPDIR,reader->m_string));
#endif /* USEQPE */

    pBkmklist = bf.readall();
    m_fBkmksChanged = bf.upgraded();
    if (pBkmklist == NULL)
    {
    pBkmklist = reader->getbkmklist();
    }
    if (pBkmklist != NULL)
    pBkmklist->sort();
}

void QTReaderApp::addbkmk()
{
  m_nRegAction = cAddBkmk;
  regEdit->setText(reader->firstword());
  do_regedit();
}

void QTReaderApp::do_addbkmk(const QString& text)
{
    if (text.isEmpty())
    {
      QMessageBox::information(this, PROGNAME, "Need a name for the bookmark\nSelect add again", 1);
    }
    else
    {
        if (pBkmklist == NULL) pBkmklist = new CList<Bkmk>;
#ifdef _UNICODE
        CBuffer buff;
        int i = 0;
        for (i = 0; i < text.length(); i++)
        {
            buff[i] = text[i].unicode();
        }
        buff[i] = 0;
        pBkmklist->push_front(Bkmk(buff.data(), NULL, reader->pagelocate()));
#else
        pBkmklist->push_front(Bkmk((const tchar*)text, reader->pagelocate()));
#endif
        m_fBkmksChanged = true;
        pBkmklist->sort();
    }
}

void QTReaderApp::OnRedraw()
{
    if ((pBkmklist != NULL) && (m_bkmkAvail != NULL))
    {
    bool found = findNextBookmark(reader->pagelocate());
    m_bkmkAvail->setEnabled(found);
    }
}

void QTReaderApp::showAnnotation()
{
    m_annoWin->setName(toQString(m_anno->name()));
    m_annoWin->setAnno(toQString(m_anno->anno()));
    m_annoIsEditing = false;
#ifdef USEQPE
    Global::showInputMethod();
#endif
    editorStack->raiseWidget( m_annoWin );
    m_annoWin->setFocus();
}

void QTReaderApp::OnWordSelected(const QString& wrd, size_t posn, const QString& line)
{
////    odebug << "OnWordSelected(" << posn << "):" << wrd << "" << oendl;

    if (m_doClipboard)
    {
    QClipboard* cb = QApplication::clipboard();
    cb->setText(wrd);
#ifdef USEQPE
    if (wrd.length() > 10)
    {
        Global::statusMessage(wrd.left(8) + "..");
    }
    else
    {
        Global::statusMessage(wrd);
    }
#endif
    }
    if (m_doAnnotation)
    {
//  addAnno(wrd, "Need to be able to edit this", posn);
    m_annoWin->setName(line);
    m_annoWin->setAnno("");
    m_annoWin->setPosn(posn);
    m_annoIsEditing = true;
#ifdef USEQPE
    Global::showInputMethod();
#endif
    editorStack->raiseWidget( m_annoWin );
    }
#ifdef USEQPE
    if (m_doDictionary)
    {
    if (!m_targetapp.isEmpty() && !m_targetmsg.isEmpty())
    {
        QCopEnvelope e(("QPE/Application/"+m_targetapp).utf8(), (m_targetmsg+"(QString)").utf8());
        e << wrd;
    }
    }
#endif
}

void QTReaderApp::doAction(ActionTypes a, QKeyEvent* e)
{
    if (a == 0)
    {
    e->ignore();
    }
    else
    {
    e->accept();
//  odebug << "Accepted" << oendl;
    switch (a)
    {
        case cesOpenFile:
        {
        fileOpen();
        }
        break;
        case cesAutoScroll:
        {
        reader->setautoscroll(!reader->m_autoScroll);
        setScrollState(reader->m_autoScroll);
        }
        break;
        case cesActionMark:
        {
        addbkmk();
        }
        break;
        case cesFullScreen:
        {
        m_actFullscreen->setOn(!m_fullscreen);
        }
        break;
        case cesActionAnno:
        {
        addanno();
        }
        break;
        case cesZoomIn:
        zoomin();
        break;
        case cesZoomOut:
        zoomout();
        break;
        case cesBack:
        reader->goBack();
        break;
        case cesForward:
        reader->goForward();
        break;
        case cesHome:
        reader->goHome();
        break;
        case cesPageUp:
        reader->dopageup();
        break;
        case cesPageDown:
        reader->dopagedn();
        break;
        case cesLineUp:
        reader->lineUp();
        break;
        case cesLineDown:
        reader->lineDown();
        break;
        case cesStartDoc:
        gotoStart();
        break;
        case cesEndDoc:
        gotoEnd();
        break;
        default:
        odebug << "Unknown ActionType:" << a << "" << oendl;
        break;
    }
    }
}

void QTReaderApp::setTwoTouch(bool _b) { reader->setTwoTouch(_b); }
void QTReaderApp::restoreFocus() { reader->setFocus(); }

void QTReaderApp::SaveConfig()
{
    m_nRegAction = cSetConfigName;
    regEdit->setText(reader->m_string);
    do_regedit();
}

void QTReaderApp::do_saveconfig(const QString& _txt, bool full)
{
//    odebug << "do_saveconfig:" << _txt << "" << oendl;
#ifdef USEQPE
    QString configname;
    Config::Domain dom;

    if (full)
    {
    configname = _txt;
    dom = Config::User;
    }
    else
    {
    configname = Global::applicationFileName(APPDIR "/configs", _txt);
    dom = Config::File;
    }

    Config config(configname, dom);
    config.setGroup( "View" );

#else
    QFileInfo fi;
    if (full)
    {
//      odebug << "full:" << _txt << "" << oendl;
        QDir d = QDir::home();                      // "/"
        if ( !d.cd(_txt) )
        {                       // "/tmp"
            owarn << "Cannot find the \"~/" << _txt << "\" directory" << oendl;
            d = QDir::home();
            d.mkdir(_txt);
            d.cd(_txt);
        }
        fi.setFile(d, INIFILE);
    }
    else
    {
        QDir d = QDir::home();                      // "/"
        if ( !d.cd(APPDIR) )
        {                       // "/tmp"
            owarn << "Cannot find the \"~/" APPDIR "\" directory" << oendl;
            d = QDir::home();
            d.mkdir(APPDIR);
            d.cd(APPDIR);
        }
        if ( !d.cd("configs") )
        {                       // "/tmp"
            owarn << "Cannot find the \"~/" APPDIR "/configs\" directory" << oendl;
            d = QDir::home();
            d.cd(APPDIR);
            d.mkdir("configs");
            d.cd("configs");
        }
        fi.setFile(d, _txt);
    }
//  odebug << "Path:" << fi.absFilePath() << "" << oendl;
    Config config(fi.absFilePath());
#endif


    config.writeEntry( "StripCr", reader->bstripcr );
    config.writeEntry( "AutoFmt", reader->bautofmt );
    config.writeEntry( "TextFmt", reader->btextfmt );
    config.writeEntry( "StripHtml", reader->bstriphtml );
    config.writeEntry( "Dehyphen", reader->bdehyphen );
    config.writeEntry( "Depluck", reader->bdepluck );
    config.writeEntry( "Dejpluck", reader->bdejpluck );
    config.writeEntry( "OneSpace", reader->bonespace );
    config.writeEntry( "Unindent", reader->bunindent );
    config.writeEntry( "Repara", reader->brepara );
    config.writeEntry( "DoubleSpace", reader->bdblspce );
    config.writeEntry( "Indent", reader->bindenter );
    config.writeEntry( "FontSize", (int)(reader->m_fontControl.currentsize()) );
    config.writeEntry( "ScrollDelay", reader->m_delay);
    if (full)
    {
    config.writeEntry("Debounce", m_debounce);
    config.writeEntry("FloatDialogs", m_bFloatingDialog);
    reader->m_lastposn = reader->pagelocate();
    config.writeEntry( "LastFile", reader->m_lastfile );
    config.writeEntry( "LastPosn", (int)(reader->pagelocate()) );
    }
    config.writeEntry( "PageMode", reader->m_bpagemode );
    config.writeEntry( "MonoSpaced", reader->m_bMonoSpaced );
    config.writeEntry( "SwapMouse", reader->m_swapmouse);
    config.writeEntry( "Fontname", reader->m_fontname );
    config.writeEntry( "Encoding", reader->m_encd );
    config.writeEntry( "CharSpacing", reader->m_charpc );
    config.writeEntry( "Overlap", (int)(reader->m_overlap) );
    config.writeEntry( "Margin", (int)reader->m_border );
    config.writeEntry( "TargetApp", m_targetapp );
    config.writeEntry( "TargetMsg", m_targetmsg );
#ifdef _SCROLLPIPE
    config.writeEntry( "PipeTarget", reader->m_pipetarget );
    config.writeEntry( "PauseAfterPara", reader->m_pauseAfterEachPara );
#endif
    config.writeEntry( "TwoTouch", m_twoTouch );
    config.writeEntry( "Annotation", m_doAnnotation);
    config.writeEntry( "Dictionary", m_doDictionary);
    config.writeEntry( "Clipboard", m_doClipboard);
    config.writeEntry( "SpaceTarget", m_spaceTarget);
    config.writeEntry( "EscapeTarget", m_escapeTarget);
    config.writeEntry( "ReturnTarget", m_returnTarget);
    config.writeEntry( "LeftTarget", m_leftTarget);
    config.writeEntry( "RightTarget", m_rightTarget);
    config.writeEntry( "UpTarget", m_upTarget);
    config.writeEntry( "DownTarget", m_downTarget);
    config.writeEntry("LeftScroll", m_leftScroll);
    config.writeEntry("RightScroll", m_rightScroll);
    config.writeEntry("UpScroll", m_upScroll);
    config.writeEntry("DownScroll", m_downScroll);
#ifdef REPALM
    config.writeEntry( "Repalm", reader->brepalm );
#endif
    config.writeEntry( "Remap", reader->bremap );
    config.writeEntry( "Peanut", reader->bpeanut );
    config.writeEntry( "MakeBold", reader->bmakebold );
    config.writeEntry( "Continuous", reader->m_continuousDocument );
    config.writeEntry( "FullJust", reader->bfulljust );
    config.writeEntry( "ExtraSpace", reader->getextraspace() );
    config.writeEntry( "ExtraLead", reader->getlead() );
    config.writeEntry( "Basesize",  (int)reader->getBaseSize());
    config.writeEntry( "RequestorFontChange", m_propogatefontchange);
    if (full)
    {
    config.setGroup( "Toolbar" );
    config.writeEntry("Movable", m_tbmovesave);
    config.writeEntry("Policy", m_tbpolsave);
    config.writeEntry("Position", m_tbposition);
#ifndef USEQPE
    config.setGroup( "Geometry" );
    config.writeEntry( "x", x() );
    config.writeEntry( "y", y() );
    config.writeEntry( "width", width() );
    config.writeEntry( "height", height() );
#endif
    }
}

/*
void QTReaderApp::setstate(unsigned char* _sd, unsigned short _sdlen)
{
    unsigned short sdlen;
    memcpy(&sdlen, _sd, sizeof(sdlen));
    sdlen -= sizeof(sdlen);
    _sd += sizeof(sdlen);
    statedata* sd;
    char* data;
    if (sdlen < sizeof(statedata)+1)
    {
    sdlen = sizeof(statedata)+1;
    }
    data = new char[sdlen];
    sd = (statedata*)data;
    memcpy(sd, _sd, sdlen);
    data[sdlen] = 0;
    reader->setstate(*sd);
    delete [] data;
}

void QTReaderApp::getstate(unsigned char*& data, unsigned short& len)
{
    unsigned char* olddata = data;
    unsigned short oldlen = len;
    len = oldlen+sizeof(unsigned short)+sizeof(statedata)+reader->m_fontname.length();
    data = new unsigned char[len];
    memcpy(data, olddata, oldlen);
    delete [] olddata;
    memcpy(data+oldlen, &len, sizeof(len));
    statedata* sd = (statedata*)(data+oldlen+sizeof(unsigned short));

    sd->bstripcr = reader->bstripcr;
    sd->btextfmt = reader->btextfmt;
    sd->bautofmt = reader->bautofmt;
    sd->bstriphtml = reader->bstriphtml;
    sd->bpeanut = reader->bpeanut;
    sd->bdehyphen = reader->bdehyphen;
    sd->bdepluck = reader->bdepluck;
    sd->bdejpluck = reader->bdejpluck;
    sd->bonespace = reader->bonespace;
    sd->bunindent = reader->bunindent;
    sd->brepara = reader->brepara;
    sd->bdblspce = reader->bdblspce;
    sd->m_bpagemode = reader->m_bpagemode;
    sd->m_bMonoSpaced = reader->m_bMonoSpaced;
    sd->bremap = reader->bremap;
    sd->bmakebold = reader->bmakebold;
    sd->Continuous = reader->m_continuousDocument;
#ifdef REPALM
    sd->brepalm = reader->brepalm;
#endif
    sd->bindenter = reader->bindenter;
    sd->m_textsize = reader->m_textsize; //reader->m_fontControl.currentsize()
    sd->m_encd = reader->m_encd;
    sd->m_charpc = reader->m_charpc;
    strcpy(sd->m_fontname, reader->m_fontname.latin1());
}
*/
#ifdef _SCRIPT
void QTReaderApp::RunScript()
{
    fileBrowser* fb = new fileBrowser(this,"OpieReader",!m_bFloatingDialog,
                      0,
//                    WStyle_Customize | WStyle_NoBorderEx,
                      "*", Global::applicationFileName(APPDIR "/scripts", ""));

    QString fn;
    if (fb->exec())
    {
    fn = fb->fileList[0];
    }
    delete fb;
    if ( !fn.isEmpty() && fork() == 0 )
    {
    execlp((const char *)fn,(const char *)fn,NULL);
    }
}

void QTReaderApp::SaveScript(const char* sname)
{
    FILE* f = fopen(sname,"w");
    if (f != NULL)
    {
#ifdef OPIE
    fprintf(f, "#!/bin/sh\nmsg() {\n\tqcop QPE/Application/reader \"$1\" \"$2\" \"$3\"\n}\n");
#else
    fprintf(f, "#!/bin/bash\nmsg() {\n\tqcop QPE/Application/uqtreader \"$1\" \"$2\" \"$3\"\n}\n");
#endif
    fprintf(f, "msg \"Update(int)\" 0\n");
    fprintf(f, "msg \"Layout/StripCR(int)\" %d\n", (reader->bstripcr) ? 1:0);
    if (reader->btextfmt) fprintf(f, "msg \"Markup(QString)\" \"Text\"\n");
    else if (reader->bautofmt) fprintf(f, "msg \"Markup(QString)\" \"Auto\"\n");
    else if (reader->bstriphtml) fprintf(f, "msg \"Markup(QString)\" \"HTML\"\n");
    else if (reader->bpeanut) fprintf(f, "msg \"Markup(QString)\" \"Peanut/PML\"\n");
    else fprintf(f, "msg \"Markup(QString)\" \"None\"\n");
    fprintf(f, "msg \"Layout/Dehyphen(int)\" %d\n", (reader->bdehyphen) ? 1:0);
    fprintf(f, "msg \"Layout/Depluck(int)\" %d\n", (reader->bdepluck) ? 1:0);
    fprintf(f, "msg \"Layout/Dejpluck(int)\" %d\n", (reader->bdejpluck) ? 1:0);
    fprintf(f, "msg \"Layout/SingleSpace(int)\" %d\n", (reader->bonespace) ? 1:0);
    fprintf(f, "msg \"Layout/Unindent(int)\" %d\n", (reader->bunindent) ? 1:0);
    fprintf(f, "msg \"Layout/Re-paragraph(int)\" %d\n", (reader->brepara) ? 1:0);
    fprintf(f, "msg \"Layout/DoubleSpace(int)\" %d\n", (reader->bdblspce) ? 1:0);
    fprintf(f, "msg \"Layout/Indent(int)\" %d\n", reader->bindenter);
    fprintf(f, "msg \"Format/SetFont(QString,int)\" \"%s\" %d\n", (const char*)reader->m_fontname, reader->m_textsize);
    fprintf(f, "msg \"Navigation/Page/LineScroll(int)\" %d\n", (reader->m_bpagemode) ? 1:0);
    fprintf(f, "msg \"Format/Ideogram/Word(int)\" %d\n", (reader->m_bMonoSpaced) ? 1:0);
    fprintf(f, "msg \"Format/Encoding(QString)\" \"%s\"\n", (const char*)m_EncodingAction[reader->m_encd]->text());
    fprintf(f, "msg \"Format/SetWidth(int)\" %d\n", reader->m_charpc);
    fprintf(f, "msg \"Navigation/SetOverlap(int)\" %d\n", reader->m_overlap);
    fprintf(f, "msg \"Layout/Remap(int)\" %d\n", (reader->bremap) ? 1:0);
    fprintf(f, "msg \"Layout/Embolden(int)\" %d\n", (reader->bmakebold) ? 1:0);
    fprintf(f, "msg \"File/Continuous(int)\" %d\n", (reader->m_continuousDocument) ? 1:0);
    fprintf(f, "msg \"File/SetDictionary(QString)\" \"%s/%s\"\n", (const char *)m_targetapp, (const char *)m_targetmsg);
#ifdef _SCROLLPIPE
    fprintf(f, "msg \"File/SetScrollTarget(QString)\" \"%s\"\n", (const char *)reader->m_pipetarget);
#endif
    fprintf(f, "msg \"File/Two/OneTouch(int)\" %d\n", (m_twoTouch) ? 1:0);
    fprintf(f, "msg \"Target/Annotation(int)\" %d\n", (m_doAnnotation) ? 1:0);
    fprintf(f, "msg \"Target/Dictionary(int)\" %d\n", (m_doDictionary) ? 1:0);
    fprintf(f, "msg \"Target/Clipboard(int)\" %d\n", (m_doClipboard) ? 1:0);
    fprintf(f, "msg \"File/Action(QString)\" \"%s\"\n", (const char *)m_buttonAction[m_spaceTarget]->text());
    fprintf(f, "msg \"Update(int)\" 1\n");
    fprintf(f, "msg \"info(QString)\" \"All Done\"\n");
    fclose(f);
    chmod(sname, S_IXUSR | S_IXGRP | S_IXOTH);
    }
}

void QTReaderApp::SaveConfig()
{
    m_nRegAction = cSetConfigName;
    regEdit->setText("");
    do_regedit();
}

void QTReaderApp::do_saveconfig(const QString& _txt)
{
    SaveScript(Global::applicationFileName(APPDIR "/scripts", _txt));
}
#endif

#ifdef _SCROLLPIPE
void QTReaderApp::setpipetarget()
{
    m_nRegAction = cSetPipeTarget;
    QString text = (reader->m_pipetarget.isEmpty()) ? QString("") : reader->m_pipetarget;
    regEdit->setText(text);
    do_regedit();
}

void QTReaderApp::do_setpipetarget(const QString& _txt)
{
    reader->m_pipetarget = _txt;
}

void QTReaderApp::setpause(bool sfs)
{
    reader->m_pauseAfterEachPara = sfs;
}
#endif

void QTReaderApp::monospace(bool _b)
{
    reader->setmono(_b);
}

bool QTReaderApp::readconfig(const QString& _txt, bool full=false)
{
#ifdef USEQPE
    QString configname;
    Config::Domain dom;

    if (full)
    {
    configname = _txt;
    dom = Config::User;
    }
    else
    {
    configname = Global::applicationFileName(APPDIR "/configs", _txt);
    QFileInfo fm(configname);
    if ( !fm.exists() ) return false;
    dom = Config::File;
    }

    Config config(configname, dom);
    config.setGroup( "View" );

#else
    QFileInfo fi;
    if (full)
    {
        QDir d = QDir::home();                      // "/"
        if ( !d.cd(_txt) )
        {                       // "/tmp"
            owarn << "Cannot find the \"~/" << _txt << "\" directory" << oendl;
            d = QDir::home();
            d.mkdir(_txt);
            d.cd(_txt);
        }
        fi.setFile(d, INIFILE);
    }
    else
    {
        QDir d = QDir::home();                      // "/"
        if ( !d.cd(APPDIR) )
        {                       // "/tmp"
            owarn << "Cannot find the \"~/" APPDIR "\" directory" << oendl;
            d = QDir::home();
            d.mkdir(APPDIR);
            d.cd(APPDIR);
        }
        if ( !d.cd("configs") )
        {                       // "/tmp"
            owarn << "Cannot find the \"~/" APPDIR "/configs\" directory" << oendl;
            d = QDir::home();
            d.mkdir("configs");
            d.cd("configs");
        }
        fi.setFile(d, _txt);
    }
#ifdef _WINDOWS
    struct stat fnstat;
    if (stat((const char *)reader->m_lastfile, &fnstat) == 0) return false; // get round fileinfo bug on windows
#else
    if (!fi.exists()) return false;
#endif
    Config config(fi.absFilePath());
#endif
    if (full)
    {
    config.setGroup("Toolbar");
    m_tbmovesave = m_tbmove = config.readBoolEntry("Movable", false);
    m_tbpolsave = m_tbpol = (ToolbarPolicy)config.readNumEntry("Policy", 1);
    m_tbposition = (ToolBarDock)config.readNumEntry("Position", 2);
    }
    config.setGroup( "View" );
    m_bFloatingDialog = config.readBoolEntry("FloatDialogs", false);
    reader->bstripcr = config.readBoolEntry( "StripCr", true );
    reader->bfulljust = config.readBoolEntry( "FullJust", false );
    reader->setextraspace(config.readNumEntry( "ExtraSpace", 0 ));
    reader->setlead(config.readNumEntry( "ExtraLead", 0 ));
    reader->btextfmt = config.readBoolEntry( "TextFmt", false );
    reader->bautofmt = config.readBoolEntry( "AutoFmt", true );
    reader->bstriphtml = config.readBoolEntry( "StripHtml", false );
    reader->bpeanut = config.readBoolEntry( "Peanut", false );
    reader->bdehyphen = config.readBoolEntry( "Dehyphen", false );
    reader->bdepluck = config.readBoolEntry( "Depluck", false );
    reader->bdejpluck = config.readBoolEntry( "Dejpluck", false );
    reader->bonespace = config.readBoolEntry( "OneSpace", false );
    reader->bunindent = config.readBoolEntry( "Unindent", false );
    reader->brepara = config.readBoolEntry( "Repara", false );
    reader->bdblspce = config.readBoolEntry( "DoubleSpace", false );
    reader->bindenter = config.readNumEntry( "Indent", 0 );
    reader->m_textsize = config.readNumEntry( "FontSize", 12 );
    reader->m_delay = config.readNumEntry( "ScrollDelay", 5184);
    if (full)
    {
    reader->m_lastfile = config.readEntry( "LastFile", QString::null );
    reader->m_lastposn = config.readNumEntry( "LastPosn", 0 );
    }
    reader->m_bpagemode = config.readBoolEntry( "PageMode", true );
    reader->m_bMonoSpaced = config.readBoolEntry( "MonoSpaced", false);
    reader->m_swapmouse = config.readBoolEntry( "SwapMouse", false);
     reader->m_fontname = config.readEntry( "Fontname", "helvetica" );
    reader->m_encd = config.readNumEntry( "Encoding", 0 );
    reader->m_charpc = config.readNumEntry( "CharSpacing", 100 );
    reader->m_overlap = config.readNumEntry( "Overlap", 0 );
    reader->m_border = config.readNumEntry( "Margin", 6 );
#ifdef REPALM
    reader->brepalm = config.readBoolEntry( "Repalm", true );
#endif
    reader->bremap = config.readBoolEntry( "Remap", true );
    reader->bmakebold = config.readBoolEntry( "MakeBold", false );
    reader->setContinuous(config.readBoolEntry( "Continuous", true ));
    m_targetapp = config.readEntry( "TargetApp", QString::null );
    m_targetmsg = config.readEntry( "TargetMsg", QString::null );
#ifdef _SCROLLPIPE
    reader->m_pipetarget = config.readEntry( "PipeTarget", QString::null );
    reader->m_pauseAfterEachPara = config.readBoolEntry( "PauseAfterPara", true );
#endif
    m_twoTouch = config.readBoolEntry( "TwoTouch", false);
    m_doAnnotation = config.readBoolEntry( "Annotation", false);
    m_doDictionary = config.readBoolEntry( "Dictionary", false);
    m_doClipboard = config.readBoolEntry( "Clipboard", false);
    m_spaceTarget = (ActionTypes)config.readNumEntry("SpaceTarget", cesAutoScroll);
    m_escapeTarget = (ActionTypes)config.readNumEntry("EscapeTarget", cesNone);
    m_returnTarget = (ActionTypes)config.readNumEntry("ReturnTarget", cesFullScreen);
    m_leftTarget = (ActionTypes)config.readNumEntry("LeftTarget", cesZoomOut);
    m_rightTarget = (ActionTypes)config.readNumEntry("RightTarget", cesZoomIn);
    m_upTarget = (ActionTypes)config.readNumEntry("UpTarget", cesPageUp);
    m_downTarget = (ActionTypes)config.readNumEntry("DownTarget", cesPageDown);

    m_leftScroll = config.readBoolEntry("LeftScroll", false);
    m_rightScroll = config.readBoolEntry("RightScroll", false);
    m_upScroll = config.readBoolEntry("UpScroll", true);
    m_downScroll = config.readBoolEntry("DownScroll", true);
    m_propogatefontchange = config.readBoolEntry( "RequestorFontChange", false);
    reader->setBaseSize(config.readNumEntry( "Basesize", 10 ));
    reader->setTwoTouch(m_twoTouch);

    m_touch_action->setOn(m_twoTouch);
    m_setmono_action->setOn(reader->m_bMonoSpaced);
    setfontHelper(reader->m_fontname);
    if (full)
    {
    addtoolbars(&config);
    }
    reader->setfilter(reader->getfilter());
    reader->refresh();
    return true;
}

bool QTReaderApp::PopulateConfig(const char* tgtdir)
{
    bkmkselector->clear();
    bkmkselector->setText("Cancel");
#ifndef USEQPE
    int cnt = 0;

    QDir d = QDir::home();                      // "/"
    if ( !d.cd(APPDIR) ) {                       // "/tmp"
        owarn << "Cannot find the \"~/" APPDIR "\" directory" << oendl;
    d = QDir::home();
    d.mkdir(APPDIR);
    d.cd(APPDIR);
    }
    if ( !d.cd(tgtdir) ) {                       // "/tmp"
        owarn << "Cannot find the \"~/" APPDIR "/" << tgtdir << "\" directory" << oendl;
    d = QDir::home();
    d.mkdir(tgtdir);
    d.cd(tgtdir);
    }
    d.setFilter( QDir::Files | QDir::NoSymLinks );
//        d.setSorting( QDir::Size | QDir::Reversed );

    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator it( *list );      // create list iterator
    QFileInfo *fi;                          // pointer for traversing

    while ( (fi=it.current()) ) {           // for each file...

    bkmkselector->insertItem(fi->fileName());
    cnt++;

    //odebug << "" << fi->size() << " " << fi->fileName().data() << "" << oendl;
    ++it;                               // goto next list element
    }

#else /* USEQPE */
    int cnt = 0;
    DIR *d;
    char* finaldir;
    finaldir = new char[strlen(APPDIR)+1+strlen(tgtdir)+1];
    strcpy(finaldir, APPDIR);
    strcat(finaldir, "/");
    strcat(finaldir, tgtdir);
    d = opendir((const char *)Global::applicationFileName(finaldir,""));

    while(1)
    {
    struct dirent* de;
    struct stat buf;
    de = readdir(d);
    if (de == NULL) break;

    if (lstat((const char *)Global::applicationFileName(finaldir,de->d_name),&buf) == 0 && S_ISREG(buf.st_mode))
    {
        bkmkselector->insertItem(de->d_name);
        cnt++;
    }
    }
    delete [] finaldir;
    closedir(d);
#endif
    return (cnt > 0);
}

void QTReaderApp::LoadConfig()
{
    if (PopulateConfig("configs"))
      {
        editorStack->raiseWidget( bkmkselector );
        hidetoolbars();
    m_nBkmkAction = cLdConfig;
      }
    else
      QMessageBox::information(this, PROGNAME, "No config files");
}

void QTReaderApp::TidyConfig()
{
    if (PopulateConfig("configs"))
      {
        editorStack->raiseWidget( bkmkselector );
        hidetoolbars();
    m_nBkmkAction = cRmConfig;
      }
    else
      QMessageBox::information(this, PROGNAME, "No config files");
}

void QTReaderApp::ExportLinks()
{
    if (PopulateConfig("urls"))
      {
        editorStack->raiseWidget( bkmkselector );
        hidetoolbars();
    m_nBkmkAction = cExportLinks;
      }
    else
      QMessageBox::information(this, PROGNAME, "No url files");
}

void QTReaderApp::OnURLSelected(const QString& href)
{
    CURLDialog* urld = new CURLDialog(href, false, this);
    urld->clipboard(m_url_clipboard);
    urld->localfile(m_url_localfile);
    urld->globalfile(m_url_globalfile);
    if (urld->exec())
    {
    m_url_clipboard = urld->clipboard();
    m_url_localfile = urld->localfile();
    m_url_globalfile = urld->globalfile();
    if (m_url_clipboard)
    {
        QClipboard* cb = QApplication::clipboard();
        cb->setText(href);
        odebug << "<a href=\"" << href << "\">" << href << "</a>" << oendl;
    }
    if (m_url_localfile)
    {
        writeUrl(reader->m_string, href);
    }
    if (m_url_globalfile)
    {
        writeUrl("GlobalURLFile", href);
    }
    }
    delete urld;
}

void QTReaderApp::writeUrl(const QString& file, const QString& href)
{
    QString filename;
#ifdef USEQPE
    filename = Global::applicationFileName(APPDIR "/urls", file);
#else
    QFileInfo fi;
    QDir d = QDir::home();                      // "/"
    if ( !d.cd(APPDIR) )
    {                       // "/tmp"
    owarn << "Cannot find the \"~/" APPDIR "\" directory" << oendl;
    d = QDir::home();
    d.mkdir(APPDIR);
    d.cd(APPDIR);
    }
    if ( !d.cd("urls") )
    {                       // "/tmp"
    owarn << "Cannot find the \"~/" APPDIR "/urls\" directory" << oendl;
    d = QDir::home();
    d.cd(APPDIR);
    d.mkdir("urls");
    d.cd("urls");
    }
    fi.setFile(d, file);
    filename = fi.absFilePath();
#endif
    FILE* fout = fopen(filename, "a");
    if (fout != NULL)
    {
    fprintf(fout, "<p><a href=\"%s\">%s</a>\n", (const char*)href, (const char*)href);
    fclose(fout);
    }
    else
    {
    QMessageBox::warning(this, PROGNAME, "Problem with writing URL");
    }
}
