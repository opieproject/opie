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

#include <qregexp.h>
#include <qclipboard.h>
#include <qwidgetstack.h>
#ifdef USEQPE
#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#endif
#include <qmenubar.h>
#include <qtoolbar.h>
#ifdef USEQPE
#include <qpe/menubutton.h>
#endif
#include <qcombobox.h>
#include <qpopupmenu.h>
#include <qaction.h>
#include <qapplication.h>
#include <qlineedit.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qobjectlist.h>
#include <qstatusbar.h>
#ifdef USEQPE
#include <qpe/global.h>
#include <qpe/applnk.h>
#endif
#include <qfileinfo.h>
#include <stdlib.h> //getenv
#include <qprogressbar.h>
#ifdef USEQPE
#include <qpe/config.h>
#endif
#include <qbuttongroup.h>
#include <qradiobutton.h>
#ifdef USEQPE
#include <qpe/qcopenvelope_qws.h>
#endif
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
#include "util.h"
#include <qfontdatabase.h>

#ifdef USEQPE
#include <qpe/resource.h>
#ifdef OPIE
#if defined(OPIEFILEDIALOG)
#include <qpe/applnk.h>
#include <opie2/ofiledialog.h>
using namespace Opie::Ui;
#else
#include "fileBrowser.h"
#endif
#else
#include "fileBrowser.h"
#endif
#else
#include "qfiledialog.h"
#endif

#include "QTReaderApp.h"
#include "CDrawBuffer.h"
#include "Filedata.h"
#include "names.h"
#include "CEncoding_tables.h"
#include "CloseDialog.h"

#include "ButtonPrefs.h"

bool CheckVersion(int&, int&, char&, QWidget*);

#ifdef _WINDOWS
#define PICDIR "c:\\uqtreader\\pics\\"
#else
#ifdef USEQPE
#define USEMSGS
#define PICDIR "opie-reader/"
#else
//#define PICDIR "/home/tim/uqtreader/pics/"
QString picdir()
{
  QString hd(getenv("READERDIR"));
  return hd + "/pics";
}
#define PICDIR picdir()
#endif
#endif

unsigned long QTReaderApp::m_uid = 0;

void QTReaderApp::setScrollState(bool _b) { m_scrollButton->setOn(_b); }

#ifdef USEQPE
#define geticon(iconname) Resource::loadPixmap( iconname )
#define getmyicon(iconname) Resource::loadPixmap( PICDIR iconname )
#else
//#define geticon(iconname) QPixmap(PICDIR iconname ".png")
#define geticon(iconname) QPixmap(PICDIR +"/"+iconname+".png")
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
        qWarning( "Cannot find the \"~/" APPDIR "\" directory" );
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
  
		    bkmkselector->insertItem(fi->fileName(), cnt++);
			
			//qDebug( "%10li %s", fi->size(), fi->fileName().data() );
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
	    bkmkselector->insertItem(de->d_name, cnt++);
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
  if (m_scrollbar != NULL) m_scrollbar->hide();
  if (m_prog != NULL) m_prog->hide();

#if defined(USEQPE)
    menubar->hide();
#endif

    if (m_scrollbar != NULL) m_scrollbar->hide();

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
    : QMainWindow( parent, name, f ), m_dontSave(false),
      fileBar(NULL), navBar(NULL), viewBar(NULL), markBar(NULL), m_scrollbar(NULL), m_localscroll(2), m_hidebars(false), m_kmapchanged(false)
{
  {
    setKeyCompression ( true );
#ifndef USEQPE
  QDir d = QDir::home();                      // "/"
  d.cd(APPDIR);
  QFileInfo fi(d, ".keymap");
  FILE* f = fopen((const char *)fi.absFilePath(), "r");
#else /* USEQPE */
  FILE* f = fopen((const char *)Global::applicationFileName(APPDIR,".keymap"), "r");
#endif /* USEQPE */
  if (f != NULL)
    {
      uint cnt;
      if ((fread(&cnt, sizeof(cnt), 1, f) != 0) && (cnt == KEYMAPVERSION))
	{
	  if (fread(&cnt, sizeof(cnt), 1, f) == 0) cnt = 0;
	  for (uint i = 0; i != cnt; i++)
	    {
	      orKey key;
	      int data;
	      fread(&key, sizeof(key), 1, f);
	      fread(&data, sizeof(data), 1, f);
	      kmap[key] = data;
	    }
	}
      fclose(f);
    }
  }

	m_url_clipboard = false;
	m_url_localfile = false;
	m_url_globalfile = false;
		ftime(&m_lastkeytime);
////  qDebug("Application directory = %s", (const tchar *)QPEApplication::documentDir());
////  qDebug("Application directory = %s", (const tchar *)Global::applicationFileName("uqtreader","bkmks.xml"));

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

//  qDebug("Initial Rotation(%d):%s", m_rot, (const char*)rot);
*/
  m_autogenstr = "^ *[A-Z].*[a-z] *$";

#ifdef USEQPE
    setIcon( Resource::loadPixmap( PICDIR  "uqtreader") );
#else
    setIcon( QPixmap (PICDIR  + "/uqtreader.png") );
#endif /* USEQPE */

//    QPEToolBar *bar = new QPEToolBar( this );
//    menubar = new QPEToolBar( this );
#ifdef USEQPE
  Config config( APPDIR );
#else
    QDir d = QDir::home();                      // "/"
    if ( !d.cd(APPDIR) ) {                       // "/tmp"
        qWarning( "Cannot find the \"~/" APPDIR "\" directory" );
		d = QDir::home();
		d.mkdir(APPDIR);
		d.cd(APPDIR);
    }
    QFileInfo fi(d, INIFILE);
//    qDebug("Path:%s", (const char*)fi.absFilePath());
    Config config(fi.absFilePath());
#endif
    config.setGroup("Toolbar");
    m_tbmovesave = m_tbmove = config.readBoolEntry("Movable", false);
    m_tbpolsave = m_tbpol = (ToolbarPolicy)config.readNumEntry("Policy", 1);
    m_tbposition = (ToolBarDock)config.readNumEntry("Position", 2);
    m_qtscroll = config.readNumEntry("QTScrollBar", false);
    m_localscroll = config.readNumEntry("LocalScrollBar", false);

//    fileBar = new QToolBar("File", this);
//    QToolBar* viewBar = new QToolBar("File", this);
//    QToolBar* navBar = new QToolBar("File", this);
//    QToolBar* markBar = new QToolBar("File", this);

#if defined(USEQPE)
    menubar = new QToolBar("Menus", this, m_tbposition);
    mb = new QPEMenuBar( menubar );
#else
    mb = new QMenuBar( this );
#endif

#if defined(USEQPE)
    QPopupMenu* tmp = new QPopupMenu(mb);
    mb->insertItem( geticon( "AppsIcon" ), tmp );
#else
    QMenuBar* tmp = mb;
#endif

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
    QWidget* widge = new QWidget(this);
    setCentralWidget( widge );
    QVBoxLayout* vlayout = new QVBoxLayout(widge);
    m_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_prog = new QLabel(widge);
    vlayout->addLayout(m_layout, 1);
    vlayout->addWidget(m_prog);

    editorStack = new QWidgetStack( widge );
    //    setCentralWidget( editorStack );

    searchVisible = FALSE;
    regVisible = FALSE;
    m_fontVisible = false;

    m_buttonprefs = new CButtonPrefs(&kmap, this);
    editorStack->addWidget(m_buttonprefs, get_unique_id());
    connect( m_buttonprefs, SIGNAL( Closed() ), this, SLOT( infoClose() ) );


    m_annoWin = new CAnnoEdit(editorStack);
    editorStack->addWidget(m_annoWin, get_unique_id());
    connect( m_annoWin, SIGNAL( finished(const QString&, const QString&) ), this, SLOT( addAnno(const QString&, const QString&) ) );
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
    connect( importSelector, SIGNAL( fileSelected( const DocLnk &) ), this, SLOT( importFile( const DocLnk & ) ) );

    editorStack->addWidget( importSelector, get_unique_id() );

    // don't need the close visible, it is redundant...
    importSelector->setCloseVisible( FALSE );
*/    
//    qDebug("Reading file list");
    readfilelist();

    reader = new QTReader( editorStack );

    reader->setDoUpdates(false);

#ifdef USEQPE
    ((QPEApplication*)qApp)->setStylusOperation(reader, QPEApplication::RightOnHold);
#endif

//    qDebug("Reading config");
//  Config config( APPDIR );
  config.setGroup( "View" );
#if defined(USEQPE) && defined(USENEWFULLSCREEN)
  m_usenewfullscreen = config.readBoolEntry("NewFullScreen", false);
#endif
  m_debounce = config.readNumEntry("Debounce", 0);
  m_buttonprefs->Debounce(m_debounce);
#ifdef USEQPE
  m_bFloatingDialog = config.readBoolEntry("FloatDialogs", false);
#else
  m_bFloatingDialog = config.readBoolEntry("FloatDialogs", true);
#endif
  reader->setStripCR(config.readBoolEntry( "StripCr", true ));
  reader->bfulljust = config.readBoolEntry( "FullJust", false );
  /*
  bool btmp = config.readBoolEntry("Negative", false);
  if (btmp) reader->setNegative();
  */
  reader->bInverse = config.readBoolEntry("Inverse", false);
  reader->m_fontControl.FixGraphics(config.readBoolEntry( "FixGraphics", false ));
  reader->setextraspace(config.readNumEntry( "ExtraSpace", 0 ));
  reader->setlead(config.readNumEntry( "ExtraLead", 0 ));
  reader->btextfmt = config.readBoolEntry( "TextFmt", false );
  reader->bautofmt = config.readBoolEntry( "AutoFmt", true );
  reader->bstriphtml = config.readBoolEntry( "StripHtml", false );
  reader->bNoInlineTables = config.readBoolEntry( "NoInlineTables", false );
  reader->bpeanut = config.readBoolEntry( "Peanut", false );
  reader->bdehyphen = config.readBoolEntry( "Dehyphen", false );
  reader->bdepluck = config.readBoolEntry( "Depluck", false );
  reader->bdejpluck = config.readBoolEntry( "Dejpluck", false );
  reader->bonespace = config.readBoolEntry( "OneSpace", false );
  reader->bunindent = config.readBoolEntry( "Unindent", false );
  reader->brepara = config.readBoolEntry( "Repara", false );
  reader->m_reparastring = config.readEntry( "ReparaString", "\\n{[\\n \\t]}");
  m_bgtype = (bground)config.readNumEntry( "BackgroundType" , 0 );
  m_themename = config.readEntry("Theme", QString::null );
  reader->bdblspce = config.readBoolEntry( "DoubleSpace", false );
  reader->bindenter = config.readNumEntry( "Indent", 0 );
  reader->m_textsize = config.readNumEntry( "FontSize", 12 );
  reader->m_delay = config.readNumEntry( "ScrollDelay", 5184);
  reader->m_scrollstep = config.readNumEntry( "ScrollStep", 1);
  reader->m_outputName = config.readEntry( "OutputCodec", "");


  reader->m_lastfile = config.readEntry( "LastFile", QString::null );
  reader->m_lastposn = config.readNumEntry( "LastPosn", 0 );
  reader->m_bpagemode = config.readBoolEntry( "PageMode", true );
  reader->m_bMonoSpaced = config.readBoolEntry( "MonoSpaced", false);
  reader->m_rotated = config.readBoolEntry( "IsRotated", false );
  reader->m_scrolltype = config.readNumEntry( "ScrollType", 0 );
  m_statusstring = config.readEntry("StatusContent", "%P%% Doc:%d/%D %p%% %z%%");
  m_statusishidden = config.readBoolEntry("StatusHidden", false);
  m_background = config.readNumEntry( "Background", 0 );
  reader->setBackground(getcolour(m_background));
  m_foreground = config.readNumEntry( "Foreground", 1 );
  reader->setForeground(getcolour(m_foreground));
  m_scrollcolor = config.readNumEntry( "ScrollColour", 5 );
  setscrollcolour();
  m_scrollbarcolor = config.readNumEntry( "ScrollBarColour", 5 );
  setscrollbarcolour();
  reader->hyphenate = config.readBoolEntry( "Hyphenate", false );
  reader->m_swapmouse = config.readBoolEntry( "SwapMouse", false);
  reader->m_fontname = config.readEntry( "Fontname", "helvetica" );
  reader->m_encd = config.readNumEntry( "Encoding", 0 );
  reader->m_charpc = config.readNumEntry( "CharSpacing", 100 );
  reader->m_overlap = config.readNumEntry( "Overlap", 0 );
  reader->m_abstopmargin = config.readNumEntry( "Top Margin", 100 );
  reader->m_absbottommargin = config.readNumEntry( "Bottom Margin", 100 );
  reader->m_absleft_border = config.readNumEntry( "Left Margin", 100 );
  reader->m_absright_border = config.readNumEntry( "Right Margin", 100 );

  m_scrollishidden = config.readBoolEntry( "HideScrollBar", false );
  m_hidebars = config.readBoolEntry( "HideToolBar", false );

  reader->brepalm = config.readBoolEntry( "Repalm", false );
  reader->bunderlineLink = config.readBoolEntry( "UnderlineLink", true );
  reader->bkern = config.readBoolEntry( "Kern", false );
  reader->bremap = config.readBoolEntry( "Remap", true );
  reader->bmakebold = config.readBoolEntry( "MakeBold", false );
  reader->setContinuous(config.readBoolEntry( "Continuous", true ));
  reader->setDoubleBuffer(config.readBoolEntry("DoubleBuffer", true));
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
  m_doOutput = config.readBoolEntry( "OutputTgt", false);
  /*
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
  */
  m_propogatefontchange = config.readBoolEntry( "RequestorFontChange", false);
  reader->setBaseSize(config.readNumEntry( "Basesize", 10 ));
  reader->setHyphenThreshold(config.readNumEntry( "HyphenThreshold", 50 ));
  //  reader->buffdoc.setCustomHyphen(config.readBoolEntry( "CustomHyphen", false ));
#ifndef USEQPE
	config.setGroup( "Geometry" );
	setGeometry(0,0,
	config.readNumEntry( "width", QApplication::desktop()->width()/2 ),
	config.readNumEntry( "height", QApplication::desktop()->height()/2 ));
	move(
	config.readNumEntry( "x", 20 ),
	config.readNumEntry( "y", 20 ));
#else
	m_grabkeyboard = config.readBoolEntry( "GrabKeyboard", false);
#endif



  setTwoTouch(m_twoTouch);

    connect( reader, SIGNAL( OnShowPicture(QImage&) ), this, SLOT( showgraphic(QImage&) ) );

    connect( reader, SIGNAL( OnRedraw() ), this, SLOT( OnRedraw() ) );
    connect( reader, SIGNAL( OnWordSelected(const QString&, size_t, size_t, const QString&) ), this, SLOT( OnWordSelected(const QString&, size_t, size_t, const QString&) ) );
    connect( reader, SIGNAL( OnURLSelected(const QString&, const size_t) ), this, SLOT( OnURLSelected(const QString&, const size_t) ) );
    connect( reader, SIGNAL( NewFileRequest(const QString&) ), this, SLOT( forceopen(const QString&) ) );
    connect( reader, SIGNAL( HandleKeyRequest(QKeyEvent*) ), this, SLOT( handlekey(QKeyEvent*) ) );
    connect( reader, SIGNAL( SetScrollState(bool) ), this, SLOT( setScrollState(bool) ) );
    connect( reader, SIGNAL(RefreshBitmap()), this, SLOT(setBackgroundBitmap()));
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

    m_buttonprefs_action = new QAction( tr( "Buttons" ), QString::null, 0, this, NULL);
    connect( m_buttonprefs_action, SIGNAL( activated() ), this, SLOT( showbuttonprefs() ) );
    m_buttonprefs_action->addTo( settings );

    m_loadtheme_action = new QAction( tr( "Load Theme" ), QString::null, 0, this, NULL);
    connect( m_loadtheme_action, SIGNAL( activated() ), this, SLOT( LoadTheme() ) );
    m_loadtheme_action->addTo( settings );

    m_repara_action = new QAction( tr( "EOP Marker" ),  QString::null, 0, this, NULL);
    connect( m_repara_action, SIGNAL( activated() ), this, SLOT( reparastring() ) );
    m_repara_action->addTo(settings);

#ifdef USEQPE
    m_grab_action = new QAction( tr( "Grab Buttons" ), QString::null, 0, this, NULL, true );
    connect( m_grab_action, SIGNAL( toggled(bool) ), this, SLOT( setgrab(bool) ) );
    m_grab_action->setOn(m_grabkeyboard);
    m_grab_action->addTo( settings );
#endif

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

    m_rotate_action = new QAction( tr( "Rotate" ), geticon( "repeat" ), QString::null, 0, this, NULL, true );
    connect( m_rotate_action, SIGNAL( toggled(bool) ), this, SLOT( setrotated(bool) ) );
    m_rotate_action->setOn(reader->m_rotated);
    m_rotate_action->addTo( view );

    m_inverse_action = new QAction( tr( "Invert" ), getmyicon( "invert" ), QString::null, 0, this, NULL, true );
    connect( m_inverse_action, SIGNAL( toggled(bool) ), this, SLOT( setinverted(bool) ) );
    m_inverse_action->setOn(reader->bInverse);
    m_inverse_action->addTo( view );

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
    if (m_hidebars)
      {
#if defined(USEQPE)
	menubar->hide();
#endif
	if (fileBar != NULL) fileBar->hide();
	if (viewBar != NULL) viewBar->hide();
	if (navBar != NULL) navBar->hide();
	if (markBar != NULL) markBar->hide();
	mb->hide();
      }


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
    connect( searchEdit, SIGNAL( textChanged( const QString & ) ),
	     this, SLOT( search( const QString& ) ) );
#else
    connect( searchEdit, SIGNAL( returnPressed( ) ),
	     this, SLOT( search( ) ) );
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

    connect( regEdit, SIGNAL( returnPressed( ) ),
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

//    qDebug("Font selector");
    m_fontSelector = new QComboBox(false, m_fontBar);
    m_fontBar->setStretchableWidget( m_fontSelector );
    {
	QFontDatabase f;
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
    connect( m_fontSelector, SIGNAL( activated(const QString& ) ),
	     this, SLOT( do_setfont(const QString&) ) );
    connect( m_fontSelector, SIGNAL( activated(int ) ),
	     this, SLOT( do_setencoding(int) ) );

    m_fontBar->hide();
    m_fontVisible = false;
#ifdef USEMSGS
    connect(qApp, SIGNAL( appMessage(const QCString&, const QByteArray& ) ),
	     this, SLOT( msgHandler(const QCString&, const QByteArray&) ) );
#endif
//    qDebug("Initing");

    m_layout->addWidget(editorStack);
    if (m_qtscroll != 0)
      {
	/*
	m_scrollbar = new QToolBar( "Autogen", this, (m_localscrollbar) ? QMainWindow::Left : QMainWindow::Right, TRUE );

	m_scrollbar->setVerticalStretchable( TRUE );
	*/
	scrollbar = m_scrollbar = new QScrollBar(QScrollBar::Vertical, widge);
	m_layout->addWidget(scrollbar);
	scrollbar->setTracking(false);
	//	connect(scrollbar, SIGNAL(sliderMoved(int)), this, SLOT(actionscroll(int)) );
	//	m_scrollbar->setStretchableWidget( scrollbar );
	if (m_scrollishidden)
	  {
	    m_scrollbar->hide();
	  }
	else
	  {
	    m_scrollbar->show();
	  }

	setrotated(reader->m_rotated);
      }
    else
      {
	m_scrollbar = NULL;
      }
    setBackgroundBitmap();
    m_inverse_action->setOn(reader->bInverse);
    reader->init();

//    qDebug("Inited");
//    m_buttonAction[m_spaceTarget]->setOn(true);
//    qDebug("fonting");
    do_setfont(reader->m_fontname);
    //    qDebug("fonted");
    QFont progfont(reader->m_fontname, reader->m_fontControl.getsize(0));
    m_prog->setFont( progfont );
    if (m_statusishidden) m_prog->hide();
    showEditTools();
    if (!reader->m_lastfile.isEmpty())
    {
//	qDebug("doclnk");
//	doc = new DocLnk(reader->m_lastfile);
//	qDebug("doclnk done");
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
//		qDebug("Item:%s", (const char*)toQString(CFiledata(p->anno()).name()));
		p = NULL;
	    }
	    if (p != NULL)
	    {
		if (!openfrombkmk(p))
		  {
		    showEditTools();
		  }
	    }
	    else
	    {
		openFile( reader->m_lastfile );
	    }
	}
	else
	{
	    if (!reader->m_lastfile.isEmpty())
	      {
		openFile( reader->m_lastfile );
	      }
	}
    }
    else
    {
      showEditTools();
    }
//    qApp->processEvents();
    if (m_scrollbar == NULL || m_scrollbar->isHidden())
      {
	reader->m_scrollpos = m_localscroll;
      }
    else
      {
	reader->m_scrollpos = (m_qtscroll == 0) ? m_localscroll : 0;
      }
    //    reader->bDoUpdates = true;
    //    reader->update();
    config.setGroup("Version");
    int major = config.readNumEntry("Major", 0);
    int bkmktype = config.readNumEntry("BkmkType", 0);
    char minor = config.readNumEntry("Minor", 0);
    if (CheckVersion(major, bkmktype, minor, this))
    {
	config.writeEntry("Major", major);
	config.writeEntry("BkmkType", bkmktype);
	config.writeEntry("Minor", (int)minor);
    }
//    qDebug("finished update");
    if (kmap.isEmpty())
      {
	QMessageBox::warning(this, PROGNAME, "You haven't mapped any keys yet!\n\nYou will be taken to the key\nmapping dialog.\nA few defaults are provided\nbut you can change then to\nsuit yourself.\n\nYou can change the key\nmapping at any time by\nselecting the Settings/Button\noption on the menu.");

	m_buttonprefs->mapkey(Qt::NoButton, Key_Up, cesPageUp);
	m_buttonprefs->mapkey(Qt::NoButton, Key_Down, cesPageDown);

	m_buttonprefs->mapkey(Qt::NoButton, Key_Right, cesZoomIn);
	m_buttonprefs->mapkey(Qt::NoButton, Key_Left, cesZoomOut);

	showbuttonprefs();
      }
}

void QTReaderApp::addtoolbars(Config* config)
{
    config->setGroup("Toolbar");

    if (fileBar != NULL)
    {
#if defined(USEQPE)
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
#else
	fileBar->clear();
#endif
    }

    m_preferences_action->addTo( filebar() );
    addfilebar(config, "Open", m_open_action);
    addfilebar(config, "Close", m_close_action);
    addfilebar(config, "Info", m_info_action);
    addfilebar(config, "Two/One Touch", m_touch_action);
    addfilebar(config, "Find", m_find_action);

    if (navBar != NULL)
    {
#if defined(USEQPE)
	if ((navBar == fileBar) && (fileBar == menubar))
#else
	if (navBar == fileBar)
#endif
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
#if defined(USEQPE)
	if ((viewBar == fileBar) && (fileBar == menubar))
#else
	if (viewBar == fileBar)
#endif
	{
	    m_actFullscreen->removeFrom( filebar() );
	    m_rotate_action->removeFrom( viewbar() );
	    m_inverse_action->removeFrom( viewbar() );
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
    addviewbar(config, "Rotate", m_rotate_action);
    addviewbar(config, "Invert Action", m_inverse_action);
    addviewbar(config, "Zoom In", m_zoomin_action);
    addviewbar(config, "Zoom Out", m_zoomout_action);
    addviewbar(config, "Set Font", m_setfont_action);
    addviewbar(config, "Encoding Select", m_setenc_action);
    addviewbar(config, "Ideogram Mode", m_setmono_action);

    if (markBar != NULL)
    {
#if defined(USEQPE)
	if ((markBar == fileBar) && (fileBar == menubar))
#else
	if (markBar == fileBar)
#endif
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
#if defined(USEQPE)
	    case cesSingle:
//		qDebug("Setting filebar to menubar");
		fileBar = menubar;
		break;
#endif
	    default:
		qDebug("Incorrect toolbar policy set");
	    case cesMenuTool:
	    case cesMultiple:
//		qDebug("Creating new file bar");
		fileBar = new QToolBar("File", this, m_tbposition);
		break;
	}
//	fileBar->setHorizontalStretchable( true );
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
		qDebug("Incorrect toolbar policy set");
#ifdef USEQPE
	    case cesSingle:
#endif
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
//		qDebug("Creating new nav bar");
		navBar = new QToolBar("Navigation", this, m_tbposition);
		break;
	    default:
		qDebug("Incorrect toolbar policy set");
#ifdef USEQPE
	    case cesSingle:
#endif
	    case cesMenuTool:
		navBar = fileBar;
//		qDebug("Setting navbar to filebar");
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
		qDebug("Incorrect toolbar policy set");
#ifdef USEQPE
	    case cesSingle:
#endif
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

void QTReaderApp::msgHandler(const QCString& _msg, const QByteArray& _data)
{
#ifndef USEMSGS
  return;
#else
    QString msg = QString::fromUtf8(_msg);

    qDebug("Received:%s", (const char*)msg);
    QDataStream stream( _data, IO_ReadOnly );
    if ( msg == "suspend()" )
    {
      qDebug("Suspending");
      suspend();
      qDebug("Suspendedb");
    }
    return;
    /*
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
    else if ( msg == "Layout/Repalm(int)" )
    {
	int info;
	stream >> info;
	repalm(info);
    }
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
    */
#endif
}

ActionTypes QTReaderApp::ActNameToInt(const QString& _enc)
{
    for (int i = 0; i < MAX_ACTIONS; i++)
    {
	if (m_buttonAction[i]->text() == _enc) return (ActionTypes)i;
    }
    return cesAutoScroll;
}

void QTReaderApp::setinverted(bool sfs)
{
  reader->setInverse(sfs);
  reader->setfilter(reader->getfilter());
  reader->refresh();
}

void QTReaderApp::setrotated(bool sfs)
{
    reader->setrotated(sfs);
    if (sfs)
      {
        m_layout->setDirection( (m_qtscroll == 2) ? QBoxLayout::BottomToTop : QBoxLayout::TopToBottom );
	if (m_scrollbar != NULL)
	  {
	    scrollbar->disconnect();
	    m_scrollbar->setOrientation(Qt::Horizontal);
	    connect(scrollbar, SIGNAL(nextLine()), reader, SLOT(lineUp()) );
	    connect(scrollbar, SIGNAL(prevLine()), reader, SLOT(lineDown()) );
	    connect(scrollbar, SIGNAL(nextPage()), reader, SLOT(dopageup()) );
	    connect(scrollbar, SIGNAL(prevPage()), reader, SLOT(dopagedn()) );
	    connect(scrollbar, SIGNAL(sliderMoved(int)), this, SLOT(actionscroll(int)) );
	  }
	//	reader->repaint(0, reader->m_left_border, reader->width(), reader->height()-2*reader->m_border);
      }
    else
      {
        m_layout->setDirection( (m_qtscroll == 2) ? QBoxLayout::RightToLeft : QBoxLayout::LeftToRight );
	if (m_scrollbar != NULL)
	  {
	    scrollbar->disconnect();
	    m_scrollbar->setOrientation(Qt::Vertical);
	    connect(scrollbar, SIGNAL(nextLine()), reader, SLOT(lineDown()) );
	    connect(scrollbar, SIGNAL(prevLine()), reader, SLOT(lineUp()) );
	    connect(scrollbar, SIGNAL(nextPage()), reader, SLOT(dopagedn()) );
	    connect(scrollbar, SIGNAL(prevPage()), reader, SLOT(dopageup()) );
	    connect(scrollbar, SIGNAL(sliderMoved(int)), this, SLOT(actionscroll(int)) );
	  }
	//	reader->repaint(reader->m_border, 0, reader->width()-2*reader->m_border, reader->height());
      }
    reader->refresh();
}

void QTReaderApp::setgrab(bool sfs)
{
#ifdef USEQPE
  m_grabkeyboard = sfs;
  if (m_grabkeyboard)
    {
      ((QPEApplication*)qApp)->grabKeyboard();
    }
  else
    {
      ((QPEApplication*)qApp)->ungrabKeyboard();
    }
#endif
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
/*
void QTReaderApp::buttonActionSelected(QAction* _a)
{
////    qDebug("es:%x : %s (%u)", _a, (const char *)(_a->text()), ActNameToInt(_a->text()));
    m_spaceTarget = ActNameToInt(_a->text());
}
*/
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
    QFont f(reader->m_fontname, reader->m_fontControl.getsize(0));
    m_prog->setFont( f );
}

void QTReaderApp::zoomout()
{
    reader->zoomout();
    QFont f(reader->m_fontname, reader->m_fontControl.getsize(0));
    m_prog->setFont( f );
}

void QTReaderApp::clearBkmkList()
{
	delete pBkmklist;
	reader->pBkmklist = pBkmklist = NULL;
	m_fBkmksChanged = false;
    reader->refresh(true);
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
//    qDebug("Lastfile:%x", fl);
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
//		qDebug("Filedata(1):%u, %u", fd.length(), dlen);
//		getstate(data, dlen);
		iter->setAnno(data, dlen);
		notadded = false;
		delete [] data;
		break;
	    }
	}
    }
//    qDebug("Added?:%x", notadded);
    if (notadded)
    {
	struct stat fnstat;
	stat((const char *)reader->m_lastfile, &fnstat);
	CFiledata fd(fnstat.st_mtime, fl);
	unsigned short dlen;
	unsigned char* data;
	reader->setSaveData(data, dlen, fd.content(), fd.length());
	pOpenlist->push_front(Bkmk(nm, data, dlen, reader->pagelocate()));
//	qDebug("Filedata(2):%u, %u", fd.length(), dlen);
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
//    qDebug("fileOpen");
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
	reader->pBkmklist = pBkmklist = NULL;
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
	if (listbkmk(pOpenlist, "Browse", true)) usebrowser = false;
    }
    if (usebrowser)
    {
	QString fn = usefilebrowser();
//	qApp->processEvents();
	if (!fn.isEmpty() && QFileInfo(fn).isFile())
	{
	    openFile(fn);
	}
	reader->setFocus();
    }
//    reader->refresh();
//    qDebug("HEIGHT:%d", reader->m_lastheight);
}

QString QTReaderApp::usefilebrowser()
{
#ifndef USEQPE
    QString s( QFileDialog::getOpenFileName( reader->m_lastfile, QString::null, this ) );
    return s;
#else
    QString fn;
#if defined(OPIE) && defined(OPIEFILEDIALOG)
    QMap<QString, QStringList> mimeTypes;
    QStringList etypes;
    etypes << "etext/*";
    mimeTypes.insert( tr("eText"), etypes );
    QStringList types;
    types << "text/*";
    mimeTypes.insert( tr("Text"), types );
    mimeTypes.insert( tr("All"), "*/*" );
    fn = OFileDialog::getOpenFileName(OFileSelector::EXTENDED_ALL, QFileInfo(reader->m_lastfile).dirPath(true), QString::null, mimeTypes, 0, "OpieReader");
#else
    fileBrowser* fb = new fileBrowser(false, this,"OpieReader",!m_bFloatingDialog,
				      0,
//				      WStyle_Customize | WStyle_NoBorderEx,
				      "*", QFileInfo(reader->m_lastfile).dirPath(true));


    if (fb->exec())
    {
	fn = fb->getCurrentFile();
    }
//    qDebug("Selected %s", (const char*)fn);
    delete fb;
#endif
    showEditTools();
    return fn;
#endif
}

void QTReaderApp::showgraphic(QImage& pm)
{
    m_graphicwin->setImage(pm);
    editorStack->raiseWidget( m_graphicwin );
    hidetoolbars();
    m_graphicwin->setFocus();
}

void QTReaderApp::showbuttonprefs()
{
    editorStack->raiseWidget( m_buttonprefs );
    hidetoolbars();
    m_buttonprefs->setFocus();
    m_kmapchanged = true;
}

void QTReaderApp::showprefs()
{
    CPrefs* prefwin = new CPrefs(reader->width(), !m_bFloatingDialog, this);

    //    prefwin->Debounce(m_debounce);
    prefwin->bgtype(m_bgtype);
    prefwin->repalm(reader->brepalm);
    prefwin->UnderlineLink(reader->bunderlineLink);
    prefwin->kern(reader->bkern);
    prefwin->hyphenate(reader->hyphenate);
    //    prefwin->customhyphen(reader->buffdoc.getCustomHyphen());
    prefwin->scrolltype(reader->m_scrolltype);
    prefwin->scrollstep(reader->m_scrollstep);
    prefwin->scrollcolor(m_scrollcolor);
    prefwin->minibarcol(m_scrollbarcolor);
    prefwin->foreground(m_foreground);
    prefwin->background(m_background);
    prefwin->twotouch(m_twoTouch); 
    prefwin->propfontchange(m_propogatefontchange);
    prefwin->StripCR(reader->bstripcr);
    prefwin->InlineTables(!reader->bNoInlineTables);
    prefwin->Dehyphen(reader->bdehyphen);
    prefwin->SingleSpace(reader->bonespace);
    prefwin->Unindent(reader->bunindent);
    prefwin->Reparagraph(reader->brepara);
    prefwin->DoubleSpace(reader->bdblspce);
    prefwin->Remap(reader->bremap);
    prefwin->Embolden(reader->bmakebold);
    prefwin->FullJustify(reader->bfulljust);
    //    prefwin->Inverse(reader->bInverse);
    //    prefwin->Negative(reader->bNegative);
    prefwin->FixGraphics(reader->m_fontControl.FixGraphics());
    prefwin->ParaLead(reader->getextraspace());
    prefwin->LineLead(reader->getlead());
    prefwin->TopMargin(reader->m_abstopmargin);
    prefwin->BottomMargin(reader->m_absbottommargin);
    prefwin->LeftMargin(reader->m_absleft_border);
    prefwin->RightMargin(reader->m_absright_border);
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
    prefwin->DoubleBuffer(reader->m_doubleBuffered);

    prefwin->dictApplication(m_targetapp);
    prefwin->dictMessage(m_targetmsg);

    /*
    prefwin->leftScroll(m_leftScroll);
    prefwin->rightScroll(m_rightScroll);
    prefwin->upScroll(m_upScroll);
    prefwin->downScroll(m_downScroll);
    */

    prefwin->miscannotation(m_doAnnotation);
    prefwin->miscdictionary(m_doDictionary);
    prefwin->miscclipboard(m_doClipboard);
    prefwin->miscoutput(m_doOutput);

    prefwin->SwapMouse(reader->m_swapmouse);

    prefwin->Font(reader->m_fontname);

    prefwin->gfxsize(reader->getBaseSize());

    prefwin->pageoverlap(reader->m_overlap);

    prefwin->ideogram(reader->m_bMonoSpaced);

    prefwin->encoding(reader->m_encd);

    prefwin->ideogramwidth(reader->m_charpc);

    prefwin->outcodec(reader->m_outputName);

    if (prefwin->exec())
    {
      //      m_debounce = prefwin->Debounce();
      reader->brepalm = prefwin->repalm();
      reader->bunderlineLink = prefwin->UnderlineLink();

      reader->bkern = prefwin->kern();
      reader->hyphenate = prefwin->hyphenate();
      //      reader->buffdoc.setCustomHyphen(prefwin->customhyphen());
      reader->m_scrolltype = prefwin->scrolltype();
      reader->m_scrollstep = prefwin->scrollstep();
      m_scrollcolor = prefwin->scrollcolor();
      setscrollcolour();
      m_scrollbarcolor = prefwin->minibarcol();
      setscrollbarcolour();
      m_foreground = prefwin->foreground();
      reader->setForeground(getcolour(m_foreground));
      m_background = prefwin->background();
      reader->setBackground(getcolour(m_background));
	m_twoTouch = prefwin->twotouch();
	reader->setTwoTouch(m_twoTouch);
	m_touch_action->setOn(m_twoTouch);

	reader->bstripcr = prefwin->StripCR();
	reader->bNoInlineTables = !prefwin->InlineTables();
	reader->bdehyphen = prefwin->Dehyphen();
	reader->bonespace = prefwin->SingleSpace();
	reader->bunindent = prefwin->Unindent();
	reader->brepara = prefwin->Reparagraph();
	reader->bdblspce = prefwin->DoubleSpace();
	reader->bremap = prefwin->Remap();
	reader->bmakebold = prefwin->Embolden();
	reader->bfulljust = prefwin->FullJustify();
	//	if (reader->bInverse != prefwin->Inverse()) reader->setInverse(prefwin->Inverse());
	//	if (reader->bNegative != prefwin->Negative()) reader->setNegative();
	reader->m_fontControl.FixGraphics(prefwin->FixGraphics());

	reader->setextraspace(prefwin->ParaLead());
	reader->setlead(prefwin->LineLead());
	reader->m_abstopmargin = prefwin->TopMargin();
	reader->m_absbottommargin = prefwin->BottomMargin();
	reader->m_absleft_border = prefwin->LeftMargin();
	reader->m_absright_border = prefwin->RightMargin();
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
		qDebug("Format out of range");
	}
	reader->bdepluck = prefwin->Depluck();
	reader->bdejpluck = prefwin->Dejpluck();
	reader->setContinuous(prefwin->Continuous());
	reader->setDoubleBuffer(prefwin->DoubleBuffer());

	/*
	m_leftScroll = prefwin->leftScroll();
	m_rightScroll = prefwin->rightScroll();
	m_upScroll = prefwin->upScroll();
	m_downScroll = prefwin->downScroll();
	*/
	m_targetapp = prefwin->dictApplication();
	m_targetmsg = prefwin->dictMessage();

	m_doAnnotation = prefwin->miscannotation();
	m_doDictionary = prefwin->miscdictionary();
	m_doClipboard = prefwin->miscclipboard();
	m_doOutput = prefwin->miscoutput();
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
	if (m_bgtype != (bground)prefwin->bgtype())
	  {
	    m_bgtype = (bground)prefwin->bgtype();
	    setBackgroundBitmap();
	  }
	qDebug("OutCodec:%s", (const char*)prefwin->outcodec());
	if (reader->m_outputName != prefwin->outcodec())
	  {
	    if (reader->m_output != NULL)
	      {
		QMessageBox::warning(this, PROGNAME, "Change of output codec\nrequires a restart");
	      }
	    reader->m_outputName = prefwin->outcodec();
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
		qWarning( "Cannot find the \"~/%s\" directory", APPDIR );
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
    prefwin->qtscroll(m_qtscroll);
    prefwin->localscroll(m_localscroll);
    if (prefwin->exec())
    {
	m_bFloatingDialog = prefwin->floating();
	if (
	    m_tbpolsave != (ToolbarPolicy)prefwin->tbpolicy()
	    ||
	    m_tbposition != (ToolBarDock)(prefwin->tbposition()+2)
	    ||
	    m_tbmovesave != prefwin->tbmovable()
	    ||
	    m_qtscroll != prefwin->qtscroll()
	    //	    ||
	    //	    m_localscrollbar != prefwin->scrollonleft()
	    )
	{
	    QMessageBox::warning(this, PROGNAME, "Some changes won't take effect\nuntil the next time the\napplication is started");
	}
	m_tbpolsave = (ToolbarPolicy)prefwin->tbpolicy();
	m_tbposition = (ToolBarDock)(prefwin->tbposition()+2);
	m_tbmovesave = prefwin->tbmovable();
	reader->m_scrollpos = m_localscroll = prefwin->localscroll();
	if (m_qtscroll != prefwin->qtscroll())
	  {
	    m_qtscroll = prefwin->qtscroll();
	    setrotated(reader->m_rotated);
	  }
	if (m_scrollbar == NULL || m_scrollbar->isHidden())
	  {
	    reader->m_scrollpos = m_localscroll;
	  }
	else
	  {
	    reader->m_scrollpos = (m_qtscroll == 0) ? m_localscroll : 0;
	  }
	bool isChanged = prefwin->isChanged();
	delete prefwin;
#ifdef USEQPE
	Config config( APPDIR );
#else
	QFileInfo fi;
    QDir d = QDir::home();                      // "/"
	if ( !d.cd(APPDIR) )
	{                       // "/tmp"
		qWarning( "Cannot find the \"~/%s\" directory", APPDIR );
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
    unsigned long ds, fs, ts, pl, dl;
    if (reader->empty())
    {
	QMessageBox::information(this, PROGNAME, "No file loaded", 1);
    }
    else
    {
	reader->sizes(fs,ts);
	ds = reader->buffdoc.endSection() - reader->buffdoc.startSection();
	pl = reader->pagelocate();
	dl = pl - reader->buffdoc.startSection();
	m_infoWin->setFileSize(fs);
	m_infoWin->setTextSize(ts);
	if (fs > UINT_MAX/100)
	  {
	    unsigned long t1 = (ts+50)/100;
	    m_infoWin->setRatio(100-(fs + (t1 >> 1))/t1);
	  }
	else
	  {
	    m_infoWin->setRatio(100-(100*fs + (ts >> 1))/ts);
	  }
	m_infoWin->setLocation(pl);
	if (pl > UINT_MAX/100)
	  {
	    unsigned long t1 = (ts+50)/100;
	    m_infoWin->setRead((pl + (t1 >> 1))/t1);
	  }
	else
	  {
	    m_infoWin->setRead((100*pl + (ts >> 1))/ts);
	  }
	m_infoWin->setDocSize(ds);
	m_infoWin->setDocLocation(dl);
	if (dl > UINT_MAX/100)
	  {
	    unsigned long d1 = (ds+50)/100;
	    m_infoWin->setDocRead((dl + (d1 >> 1))/d1);
	  }
	else
	  {
	    m_infoWin->setDocRead((100*dl + (ds >> 1))/ds);
	  }
	m_infoWin->setZoom(reader->m_fontControl.currentsize()*10);
	m_infoWin->setAbout(QString("\nApplication (c) Tim Wentford\n")+reader->about());
	editorStack->raiseWidget( m_infoWin );
	hidetoolbars();
	m_infoWin->setFocus();
    }
}

void QTReaderApp::addAnno(const QString& name, const QString& text, size_t posn, size_t posn2)
{
    if (pBkmklist == NULL) reader->pBkmklist = pBkmklist = new CList<Bkmk>;
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
    Bkmk b(buff.data(), buff2.data(), posn, posn2);
	QColor c = m_annoWin->getColor();
	int red,green,blue;
	c.rgb(&red, &green, &blue);
	b.red(red);
	b.green(green);
	b.blue(blue);
    pBkmklist->push_front(b);
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
	    addAnno(name, text, m_annoWin->getPosn(), m_annoWin->getPosn2());
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
	QColor c = m_annoWin->getColor();
	int red,green,blue;
	c.rgb(&red, &green, &blue);
	m_anno->red(red);
	m_anno->green(green);
	m_anno->blue(blue);
	bool found = findNextBookmark(m_anno->value()+1);
	if (found)
	{
	    m_annoWin->setName(toQString(m_anno->name()));
	    m_annoWin->setAnno(toQString(m_anno->anno()));
	    m_annoWin->setColor(QColor(m_anno->red(), m_anno->green(), m_anno->blue()));
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
	hidetoolbars();
#ifdef USEQPE
	Global::showInputMethod();
#endif
	m_annoWin->setFocus();
    }
}

void QTReaderApp::infoClose()
{
  m_debounce = m_buttonprefs->Debounce();
  if (m_kmapchanged)
    {
      m_kmapchanged = false;
#ifndef USEQPE
      QDir d = QDir::home();                      // "/"
      d.cd(APPDIR);
      QFileInfo fi(d, ".keymap");
      FILE* f = fopen((const char *)fi.absFilePath(), "w");
#else /* USEQPE */
      FILE* f = fopen((const char *)Global::applicationFileName(APPDIR,".keymap"), "w");
#endif /* USEQPE */
      if (f != NULL)
	{
	  uint cnt = KEYMAPVERSION;
	  fwrite(&cnt, sizeof(cnt), 1, f);
	  cnt = kmap.count();
	  fwrite(&cnt, sizeof(cnt), 1, f);
	  for (QMap<orKey,int>::Iterator i = kmap.begin(); i != kmap.end(); i++)
	    {
	      orKey key = i.key();
	      int data = i.data();
	      fwrite(&key, sizeof(key), 1, f);
	      fwrite(&data, sizeof(data), 1, f);
	      qDebug("Saved %s as %u", (const char*)key.text(), data);
	    }
	  fclose(f);
	}
    }
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
//	reader->setmono(true);
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
//  //  qDebug("findNext called\n");
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
//      qDebug("Found it at %u:%u", pos, offset);
      pbar->hide();
//      qDebug("Hid");
      reader->locate(pos+offset);
//      qDebug("Loacted");
//      qDebug("page up");
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

void QTReaderApp::openFile( const QString &f, unsigned int loc )
{
//    qDebug("File:%s", (const char*)f);
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
      reader->setText(fm.baseName(), fm.absFilePath(), loc);
      m_loadedconfig = readconfig(APPDIR "/configs", reader->m_string, false);
      qDebug("Showing tools");
      showEditTools();
      qDebug("Shown tools");
      readbkmks();
      qDebug("read markss");
      m_savedpos = 0xffffffff;
    }
  else
    {
      QString msg = f;
       msg += "\nFile does not exist";
      QMessageBox::information(this, PROGNAME, msg);
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
//    qDebug("Keypress event");
    timeb now;
    ftime(&now);
    unsigned long etime = (1000*(now.time - m_lastkeytime.time) + now.millitm)-m_lastkeytime.millitm;
    if (etime < m_debounce)
    {
	return;
    }
    switch(e->key())
    {
	case Key_Escape:
	    qDebug("escape event");
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
//		qDebug("escape action");
		    doAction(e);
		}
	    }
	    break;
	    /*
	case Key_Left:
	{
	    if (reader->m_autoScroll)
	    {
		reader->reduceScroll();
	    }
	    else
	    {
		doAction(e);
	    }
	}
	break;
	case Key_Right:
	{
	    if (reader->m_autoScroll)
	    {
		reader->increaseScroll();
	    }
	    else
	    {
		doAction(e);
	    }
	}
	break;
	case Key_Up:
	{
	    if (reader->m_autoScroll)
	    {
		reader->increaseScroll();
	    }
	    else
	    {
		doAction(e);
	    }
	}
	break;
	case Key_Down:
	{
	    if (reader->m_autoScroll)
	    {
		reader->reduceScroll();
	    }
	    else
	    {
		doAction(e);
	    }
	}
	break;
	    */
	default:
	{
	  doAction(e);
	}
	
/*
	QString msg("Key press was:");
	QString key;
	msg += key.setNum(e->key());
	QMessageBox::information(this, PROGNAME, msg);
*/
    }
    ftime(&m_lastkeytime);
}

#if defined(USEQPE) && defined(USENEWFULLSCREEN)
void QTReaderApp::focusInEvent(QFocusEvent *)
{
  if (m_usenewfullscreen && m_fullscreen && (editorStack->visibleWidget() == reader))
    {
      m_usenewfullscreen = false;
      reader->bDoUpdates = false;
      showEditTools();
      reader->bDoUpdates = true;
      reader->update();
      m_usenewfullscreen = true;
    }
}

void QTReaderApp::resizeEvent(QResizeEvent *)
{
  if (m_usenewfullscreen && m_fullscreen && (editorStack->visibleWidget() == reader))
    {
      m_usenewfullscreen = false;
      reader->bDoUpdates = false;
      showEditTools();
      reader->bDoUpdates = true;
      reader->update();
      m_usenewfullscreen = true;
    }
}
#endif

void QTReaderApp::showEditTools()
{
//    if ( !doc )
//	close();
    if (m_fullscreen)
    {
#if defined(USEQPE)
	if (menubar != NULL) menubar->hide();
#endif
	if (fileBar != NULL) fileBar->hide();
	if (viewBar != NULL) viewBar->hide();
	if (navBar != NULL) navBar->hide();
	if (markBar != NULL) markBar->hide();
	if (m_prog != NULL) 
	  {
	    //	    qDebug("Hiding status");
	    m_prog->hide();
	  }
	searchBar->hide();
	regBar->hide();
#ifdef USEQPE
	Global::hideInputMethod();
#endif
	if (m_scrollbar != NULL) m_scrollbar->hide();
	m_fontBar->hide();
//	showNormal();
	showFullScreen();
  }
    else
    {
//	qDebug("him");
#ifdef USEQPE
	Global::hideInputMethod();
#endif
//	qDebug("eb");
	if (m_scrollbar != NULL)
	  {
	    if (m_scrollishidden)
	      {
		m_scrollbar->hide();
	      }
	    else
	      {
		m_scrollbar->show();
	      }
	  }
	if (!m_hidebars)
	  {
#if defined(USEQPE)
	    menubar->show();
#endif
	    if (fileBar != NULL) fileBar->show();
	    if (viewBar != NULL) viewBar->show();
	    if (navBar != NULL) navBar->show();
	    if (markBar != NULL) markBar->show();
	    if (m_prog != NULL && !m_statusishidden) 
	      {
		//		qDebug("Showing status");
		m_prog->show();
		//		qDebug("Shown status");
	      }
	    //	    qDebug("Showing mb");
	    mb->show();
	  }
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
//	qDebug("sn");
	showNormal();
//	qDebug("sm");
#if defined(USEQPE) && !defined(SIMPAD) 
	showMaximized();
#endif
//	setCentralWidget(reader);
    }

    //    qDebug("uc");
    updateCaption();
    //    qDebug("rw");
    editorStack->raiseWidget( reader );
    //    qDebug("sf");
    reader->setFocus();
    //    qDebug("ref");
    //reader->refresh(true);
    //    qDebug("done");
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
//	delete doc;
//  QMessageBox::information(this, PROGNAME, "Deleted doc", 1);
//	doc = 0;
    //     }
    reader->clear();
}

void QTReaderApp::updateCaption()
{
//    if ( !doc )
//	setCaption( tr("QTReader") );
//    else {
//	QString s = doc->name();
//	if ( s.isEmpty() )
//	    s = tr( "Unnamed" );
	setCaption( reader->m_string + " - " + tr(SHORTPROGNAME) );
//    }
}

void QTReaderApp::setDocument(const QString& fileref)
{
//QMessageBox::information(0, "setDocument", fileref);
    openFile(fileref);
//    showEditTools();
}

void QTReaderApp::closeEvent( QCloseEvent *e )
{
//    qDebug("Close event");
    if (m_fullscreen)
    {
	m_fullscreen = false;
	showEditTools();
	e->ignore();
    }
    else if (editorStack->visibleWidget() == m_buttonprefs)
      {
	m_buttonprefs->mapkey(Qt::NoButton, Key_Escape);
	e->ignore();
      }
    else if (m_dontSave)
    {
	e->accept();
    }
    else
    {
	if (editorStack->visibleWidget() == reader)
	{
	    if ((kmap.find(orKey(Qt::NoButton,Key_Escape,false)) != kmap.end()) && m_bcloseDisabled)
	    {
//	qDebug("Close disabled");
		m_bcloseDisabled = false;
		e->ignore();
	    }
	    else
	    {
		if (m_fontVisible)
		{
		    m_fontBar->hide();
		    m_fontVisible = false;
		    e->ignore();
		    return;
		}
		if (regVisible)
		{
		    regBar->hide();
#ifdef USEQPE
		    Global::hideInputMethod();
#endif
		    regVisible = false;
		    e->ignore();
		    return;
		}
		if (searchVisible)
		{
		    searchBar->hide();
#ifdef USEQPE
		    Global::hideInputMethod();
#endif
		    searchVisible = false;
		    e->ignore();
		    return;
		}
		if (m_fBkmksChanged && pBkmklist != NULL)
		{
		    if (QMessageBox::warning(this, PROGNAME, "Save bookmarks?", "Save", "Don't bother") == 0)
			savebkmks();
		    delete pBkmklist;
		    reader->pBkmklist = pBkmklist = NULL;
		    m_fBkmksChanged = false;
		}
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

bool QTReaderApp::listbkmk(CList<Bkmk>* plist, const QString& _lab, bool presel)
{
    bkmkselector->clear();
    if (_lab.isEmpty())
	bkmkselector->setText("Cancel");
    else
	bkmkselector->setText(_lab);
    int cnt = 0;
    int slt = -1;
    if (plist != NULL)
      {
	for (CList<Bkmk>::iterator i = plist->begin(); i != plist->end(); i++)
	  {
	    if (presel)
	      {
		Bkmk* p = i.pContent();
		if (toQString(CFiledata(p->anno()).name()) == reader->m_lastfile)
		  {
		    slt = cnt;
		  }
	      }
#ifdef _UNICODE
//	      qDebug("Item:%s", (const char*)toQString(i->name()));
	    bkmkselector->insertItem(toQString(i->name()), cnt++);
#else
	    bkmkselector->insertItem(i->name(), cnt++);
#endif

	  }
      }
    if (cnt > 0)
      {
	  hidetoolbars();
        editorStack->raiseWidget( bkmkselector );
	if (slt != -1) bkmkselector->setCurrentItem(slt);
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
//    qDebug("Showing regbar");
  regBar->show();
//    qDebug("Showing kbd");
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
//	qDebug("fileinfo");
    if (!fn.isEmpty() && QFileInfo(fn).isFile())
    {
//	qDebug("Opening");
	struct stat fnstat;
	stat((const char *)fn, &fnstat);
	
	if (CFiledata(bk->anno()).date()
	    != fnstat.st_mtime)
	{
	    CFiledata fd(bk->anno());
	    fd.setdate(fnstat.st_mtime);
	    bk->value(0);
	    reader->m_lastposn = 0;
	    openFile(fn);
	}
	else
	{
	    unsigned short svlen = bk->filedatalen();
	    unsigned char* svdata = bk->filedata();
	    openFile(fn, bk->value());
	    reader->putSaveData(svdata, svlen);
//	    setstate(svdata, svlen);
	    if (svlen != 0)
	    {
		QMessageBox::warning(this, PROGNAME, "Not all file data used\nNew version?");
	    }
//	    qDebug("updating");
//	    showEditTools();
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
  qDebug("gbkmk");
    showEditTools();
    switch (m_nBkmkAction)
    {
	case cOpenFile:
	{
//	    qApp->processEvents();
	    if (!openfrombkmk((*pOpenlist)[ind]))
	    {
		pOpenlist->erase(ind);
		QMessageBox::information(this, PROGNAME, "Can't find file");
	    }
	}
	break;
	case cGotoBkmk:
	    reader->buffdoc.saveposn(reader->m_lastfile, reader->pagelocate());
	    reader->locate((*pBkmklist)[ind]->value());
	    break;
	case cDelBkmk:
////	    qDebug("Deleting:%s\n",(*pBkmklist)[ind]->name());
	    pBkmklist->erase(ind);
	    m_fBkmksChanged = true;
	    reader->refresh(true);
//	    pBkmklist->sort();
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
	    readconfig(APPDIR "/configs", bkmkselector->text(ind), false);
	    break;
	case cLdTheme:
	  m_themename = bkmkselector->text(ind);
	  readconfig(QString(APPDIR "/Themes/")+m_themename , "config", false);
	  setBackgroundBitmap();
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
#if defined(OPIE) && defined(OPIEFILEDIALOG)
		QString outfile = OFileDialog::getSaveFileName(OFileSelector::EXTENDED_ALL, QString::null, QString::null, MimeTypes(), 0, "OpieReader");
		if (!outfile.isEmpty())
		{
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
		fclose(fin);
		if (allok) unlink((const char *)Global::applicationFileName(APPDIR "/urls",bkmkselector->text(ind)));
#else
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
#endif
	    }
	    else
	    {
		QMessageBox::information(this, PROGNAME, "Couldn't open input");
	    }

/*
	    CFileSelector *f = new CFileSelector("text/html", this, NULL, !m_bFloatingDialog, TRUE, TRUE );
	    int ret = f->exec();
	    qDebug("Return:%d", ret);
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
		qDebug("YES");
	    }
	    else
	    {
		qDebug("NO");
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

void QTReaderApp::reparastring()
{
  m_nRegAction = cRepara;
  regEdit->setText(reader->m_reparastring);
  do_regedit();
}

void QTReaderApp::do_reparastring(const QString& _lcn)
{
  reader->m_reparastring = _lcn;
  if (reader->brepara)
    {
      reader->bDoUpdates = true;
      reader->setfilter(reader->getfilter());
    }
}

void QTReaderApp::jump()
{
  m_nRegAction = cJump;
  char lcn[20];
  sprintf(lcn, "%lu", reader->pagelocate());
  regEdit->setText(lcn);
  do_regedit();
}

void QTReaderApp::do_jump(const QString& _lcn)
{
    bool ok;
    QString lcn = _lcn.stripWhiteSpace();
    unsigned long ulcn = lcn.toULong(&ok);
    if (!ok)
      {
	double pc = 0.0;
#ifdef _WINDOWS
	if (lcn.at(lcn.length()-1) == '%')
#else
	if (lcn[lcn.length()-1] == '%')
#endif
	{
	    lcn = lcn.left(lcn.length()-1);
	    pc = lcn.toDouble(&ok);
	  }
	else
	  {
	    pc = lcn.toDouble(&ok);
	  }
	if (ok && 0 <= pc && pc <= 100)
	  {
	    ulcn = (pc*(reader->buffdoc.endSection()-reader->buffdoc.startSection()))/100 + reader->buffdoc.startSection();
	  }
	else
	  {
	    ok = false;
	  }
      }
    if (ok)
	reader->locate(ulcn);      
    else
	QMessageBox::information(this, PROGNAME, "Must be a number\nor a percentage");
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
      case cRepara:
	do_reparastring(regEdit->text());
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
//	  qDebug("Saving config");
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
	QFontDatabase f;
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
    qDebug("setencoding:%d", i);
    if (m_fontAction == cChooseEncoding)
    {
	reader->setencoding(i);
    }
    reader->refresh();
    m_fontBar->hide();
    m_fontVisible = false;
//    qDebug("showedit");
    if (reader->isVisible()) showEditTools();
//    qDebug("showeditdone");
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
    //    qDebug("setfont");
    //if (reader->isVisible())
    showEditTools();
//    qDebug("showeditdone");
}

void QTReaderApp::do_autogen(const QString& regText)
{
    unsigned long fs, ts;
    reader->sizes(fs,ts);
//  //  qDebug("Reg:%s\n", (const tchar*)(regEdit->text()));
  m_autogenstr = regText;
  QRegExp re(regText);
  CBuffer buff;
  if (pBkmklist != NULL) delete pBkmklist;
  reader->pBkmklist = pBkmklist = new CList<Bkmk>;
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
  unsigned int llcn = reader->locate();
  while (i >= 0)
    {
      unsigned int lcn = llcn;
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
      /*
      llcn = reader->locate();
      if ((i == 0) && (llcn+1 < ts))
	{
	  reader->jumpto(++llcn);
	  i = 1;
	}
      */
    }
  pBkmklist->sort();
  pbar->setProgress(100);
  qApp->processEvents();
  pbar->hide();
  reader->refresh();
}

void QTReaderApp::saveprefs()
{
  do_saveconfig( APPDIR, true );
  savefilelist();
}

/*
void QTReaderApp::oldFile()
{
//  qDebug("oldFile called");
  reader->setText(true);
//  qDebug("settext called");
  showEditTools();
//  qDebug("showedit called");
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
	BkmkFile bf((const char *)fi.absFilePath(), true, true);
#else /* USEQPE */
	BkmkFile bf((const char *)Global::applicationFileName(APPDIR,reader->m_string), true, true);
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
    BkmkFile bf((const char *)fi.absFilePath(), false, false);
#else /* USEQPE */
    BkmkFile bf((const char *)Global::applicationFileName(APPDIR,".openfiles"), false, false);
#endif /* USEQPE */
//    qDebug("Reading open files");
    pOpenlist = bf.readall();
//    if (pOpenlist != NULL) qDebug("...with success");
//    else qDebug("...without success!");
}

void QTReaderApp::savefilelist()
{
    if (pOpenlist != NULL)
    {
#ifndef USEQPE
	    QDir d = QDir::home();                      // "/"
		d.cd(APPDIR);
		QFileInfo fi(d, ".openfiles");
	BkmkFile bf((const char *)fi.absFilePath(), true, false);
#else /* USEQPE */
	BkmkFile bf((const char *)Global::applicationFileName(APPDIR,".openfiles"), true, false);
#endif /* USEQPE */
//	qDebug("Writing open files");
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
    BkmkFile bf((const char *)fi.absFilePath(), false, true);
#else /* USEQPE */
    BkmkFile bf((const char *)Global::applicationFileName(APPDIR,reader->m_string), false, true);
#endif /* USEQPE */

    reader->pBkmklist = pBkmklist = bf.readall();
    m_fBkmksChanged = bf.upgraded();
    if (pBkmklist == NULL)
    {
	reader->pBkmklist = pBkmklist = reader->getbkmklist();
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
		if (pBkmklist == NULL) reader->pBkmklist = pBkmklist = new CList<Bkmk>;
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
		reader->refresh(true);
	}
}

void QTReaderApp::UpdateStatus()
{
  QString status;
  for (int i = 0; i < m_statusstring.length(); i++)
    {
      if (m_statusstring[i].unicode() == '%')
	{
	  i++;
	  if (i < m_statusstring.length())
	    {
	      switch (m_statusstring[i].unicode())
		{
		case 'F':
		  {
		    unsigned long fs,ts;
		    reader->sizes(fs,ts);
		    status += filesize(ts);
		  }
		  break;
		case 'f':
		  {
		    status += filesize(reader->pagelocate());
		  }
		  break;
		case 'D':
		  {
		    status += filesize(reader->buffdoc.endSection()-reader->buffdoc.startSection());
		  }
		  break;
		case 'd':
		  {
		    status += filesize(reader->pagelocate()-reader->buffdoc.startSection());
		  }
		  break;
		case 'P':
		  {
		    unsigned long fs,ts;
		    reader->sizes(fs,ts);
		    status += percent(reader->pagelocate(),ts);
		  }
		  break;
		case 'p':
		  {
		    status += percent(reader->pagelocate()-reader->buffdoc.startSection(),reader->buffdoc.endSection()-reader->buffdoc.startSection());
		  }
		  break;
		case 'z':
		  {
		    //	    qDebug("case d");
		    status += QString().setNum(reader->m_fontControl.currentsize()*10);
		  }
		  break;
		default:
		  status += m_statusstring[i];
		}
	    }
	}
      else
	{
	  status += m_statusstring[i];
	}
    }
  m_prog->setText(status);
}

void QTReaderApp::OnRedraw()
{
  if ((pBkmklist != NULL) && (m_bkmkAvail != NULL))
    {
      bool found = findNextBookmark(reader->pagelocate());
      m_bkmkAvail->setEnabled(found);
    }
  if (m_scrollbar != NULL)
    {
      //qDebug("OnRedraw:[%u, %u]", reader->buffdoc.startSection(), reader->buffdoc.endSection());
      scrollbar->setRange(reader->buffdoc.startSection(), reader->buffdoc.endSection()-1);
      scrollbar->setPageStep(reader->locate()-reader->pagelocate());
      scrollbar->setValue((reader->m_rotated) ? 
			  (reader->buffdoc.endSection() - reader->locate()+reader->buffdoc.startSection()) :
			  reader->pagelocate());
    }
  if (m_prog->isVisible())
    {
      //      qDebug("updating status");
      UpdateStatus();
      //      qDebug("updated status");
    }
}

void QTReaderApp::showAnnotation()
{
    m_annoWin->setName(toQString(m_anno->name()));
    m_annoWin->setAnno(toQString(m_anno->anno()));
    m_annoWin->setColor(QColor(m_anno->red(), m_anno->green(), m_anno->blue()));
    m_annoIsEditing = false;
#ifdef USEQPE
    Global::showInputMethod();
#endif
    editorStack->raiseWidget( m_annoWin );
    hidetoolbars();
    m_annoWin->setFocus();
}

void QTReaderApp::OnWordSelected(const QString& wrd, size_t posn, size_t posn2, const QString& line)
{
////    qDebug("OnWordSelected(%u):%s", posn, (const char*)wrd);

    if (m_doOutput && reader->checkoutput())
      {
	reader->doOutput(wrd);
      }
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
//	addAnno(wrd, "Need to be able to edit this", posn);
	m_annoWin->setName(line);
	m_annoWin->setAnno("");
	m_annoWin->setPosn(posn);
	m_annoWin->setPosn2(posn2);
	m_annoIsEditing = true;
#ifdef USEQPE
	Global::showInputMethod();
#endif
	editorStack->raiseWidget( m_annoWin );
	hidetoolbars();
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

void QTReaderApp::doAction(QKeyEvent* e)
{
  QMap<orKey,int>::Iterator f = kmap.end();

  if (reader->m_autoScroll)
    {
      f = kmap.find(orKey(e->state(), e->key(), true));
    }
  if (f == kmap.end())
    {
      f = kmap.find(orKey(e->state(), e->key(), false));
    }

    if (f == kmap.end())
    {
      qDebug("doaction (no action) : %d %d %d", e->key(), e->state(), f.data());
	e->ignore();
    }
    else
    {
      qDebug("doaction (some action) : %d %d %d", e->key(), e->state(), f.data());
      ActionTypes a = (ActionTypes)f.data();
	e->accept();
//	qDebug("Accepted");
	switch (a)
	{
	case cesGotoLink:
	  {
	    reader->gotoLink();
	  }
	  break;
	case cesNextLink:
	  {
	    reader->getNextLink();
	  }
	  break;
	case cesInvertColours:
	  m_inverse_action->setOn(!reader->bInverse);
	  break;
	case cesToggleBars:
	  m_hidebars = !m_hidebars;
	  if (m_hidebars)
	    {
#if defined(USEQPE)
	      menubar->hide();
#endif
	      if (fileBar != NULL) fileBar->hide();
	      if (viewBar != NULL) viewBar->hide();
	      if (navBar != NULL) navBar->hide();
	      if (markBar != NULL) markBar->hide();
	      mb->hide();
	    }
	  else
	    {
#if defined(USEQPE)
	      menubar->show();
#endif
	      if (fileBar != NULL) fileBar->show();
	      if (viewBar != NULL) viewBar->show();
	      if (navBar != NULL) navBar->show();
	      if (markBar != NULL) markBar->show();
	      mb->show();
	    }
	  break;
	case cesToggleScrollBar:
	  if (m_scrollbar != NULL)
	    {
	      if (m_scrollbar->isHidden())
		{
		  m_scrollishidden = false;
		  m_scrollbar->show();
		  reader->m_scrollpos = (m_qtscroll == 0) ? m_localscroll : 0;
		}
	      else
		{
		  m_scrollishidden = true;
		  m_scrollbar->hide();
		  reader->m_scrollpos = m_localscroll;
		}
	    }
	  break;
	case cesToggleStatusBar:
	  if (m_prog != NULL)
	    {
	      if (m_prog->isHidden())
		{
		  m_statusishidden = false;
		  m_prog->show();
		}
	      else
		{
		  m_statusishidden = true;
		  m_prog->hide();
		}
	    }
	  break;
	    case cesOpenFile:
	    {
	      qDebug("Open file");
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
	    case cesRotate:
	    {
		m_rotate_action->setOn(!reader->m_rotated);
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
		/*
		ftime(&m_lastkeytime);
		m_fndelay = m_debounce;
		//
		{
		  timeb now;
		  ftime(&now);
		  m_fndelay = (1000*(now.time - m_lastkeytime.time) + now.millitm)-m_lastkeytime.millitm+100;
		}
		*/
		break;
	    case cesPageDown:
		reader->dopagedn();
		/*
		ftime(&m_lastkeytime);
		m_fndelay = m_debounce;
		//		
			if (m_debounce != 0)
			{
			timeb now;
			ftime(&now);
			m_fndelay = (1000*(now.time - m_lastkeytime.time) + now.millitm)-m_lastkeytime.millitm+10;
			}
		*/
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
	    case cesScrollMore:
		reader->increaseScroll();
		break;
	    case cesScrollLess:
		reader->reduceScroll();
		break;
	    default:
		qDebug("Unknown ActionType:%u", a);
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
//    qDebug("do_saveconfig:%s", (const char*)_txt);
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
//	    qDebug("full:%s", (const char*)_txt);
	    QDir d = QDir::home();                      // "/"
		if ( !d.cd(_txt) )
		{                       // "/tmp"
			qWarning( "Cannot find the \"~/%s\" directory", (const char*)_txt );
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
			qWarning( "Cannot find the \"~/" APPDIR "\" directory" );
			d = QDir::home();
			d.mkdir(APPDIR);
			d.cd(APPDIR);
		}
		if ( !d.cd("configs") )
		{                       // "/tmp"
			qWarning( "Cannot find the \"~/" APPDIR "/configs\" directory" );
			d = QDir::home();
			d.cd(APPDIR);
			d.mkdir("configs");
			d.cd("configs");
		}
		fi.setFile(d, _txt);
	}
//	qDebug("Path:%s", (const char*)fi.absFilePath());
	Config config(fi.absFilePath());
#endif


    config.writeEntry( "StripCr", reader->bstripcr );
    config.writeEntry( "NoInlineTables", reader->bNoInlineTables );
    config.writeEntry( "AutoFmt", reader->bautofmt );
    config.writeEntry( "TextFmt", reader->btextfmt );
    config.writeEntry( "StripHtml", reader->bstriphtml );
    config.writeEntry( "Dehyphen", reader->bdehyphen );
    config.writeEntry( "Depluck", reader->bdepluck );
    config.writeEntry( "Dejpluck", reader->bdejpluck );
    config.writeEntry( "OneSpace", reader->bonespace );
    config.writeEntry( "Unindent", reader->bunindent );
    config.writeEntry( "Repara", reader->brepara );
    config.writeEntry( "ReparaString", reader->m_reparastring);
    config.writeEntry( "BackgroundType" , (int)m_bgtype );
    config.writeEntry( "Theme", m_themename );
    config.writeEntry( "DoubleSpace", reader->bdblspce );
    config.writeEntry( "Indent", reader->bindenter );
    config.writeEntry( "FontSize", (int)(reader->m_fontControl.currentsize()) );
    config.writeEntry( "ScrollDelay", reader->m_delay);
    config.writeEntry( "ScrollStep", reader->m_scrollstep);
    config.writeEntry( "ScrollType", reader->m_scrolltype );
    if (full)
    {
	config.writeEntry("Debounce", m_debounce);
	config.writeEntry("FloatDialogs", m_bFloatingDialog);
	reader->m_lastposn = reader->pagelocate();
	config.writeEntry( "LastFile", reader->m_lastfile );
	config.writeEntry( "LastPosn", (int)(reader->pagelocate()) );
	config.writeEntry( "OutputCodec", reader->m_outputName);
    }
    config.writeEntry( "PageMode", reader->m_bpagemode );
    config.writeEntry( "MonoSpaced", reader->m_bMonoSpaced );
    config.writeEntry( "SwapMouse", reader->m_swapmouse);
    config.writeEntry( "IsRotated", reader->m_rotated );
    config.writeEntry("StatusContent", m_statusstring);
    config.writeEntry("StatusHidden", m_statusishidden);
    config.writeEntry( "Background", m_background );
    config.writeEntry( "Foreground", m_foreground );
    config.writeEntry( "ScrollColour", m_scrollcolor );
    config.writeEntry( "ScrollBarColour", m_scrollbarcolor );
    config.writeEntry( "Hyphenate", reader->hyphenate );
    //    config.writeEntry( "CustomHyphen", reader->buffdoc.getCustomHyphen() );
    config.writeEntry( "Fontname", reader->m_fontname );
    config.writeEntry( "Encoding", reader->m_encd );
    config.writeEntry( "CharSpacing", reader->m_charpc );
    config.writeEntry( "Overlap", (int)(reader->m_overlap) );
    config.writeEntry( "Top Margin", (int)reader->m_abstopmargin );
    config.writeEntry( "Bottom Margin", (int)reader->m_absbottommargin );
    config.writeEntry( "Left Margin", (int)reader->m_absleft_border );
    config.writeEntry( "Right Margin", (int)reader->m_absright_border );
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
    config.writeEntry( "OutputTgt", m_doOutput);
    /*
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
    */
    config.writeEntry( "Repalm", reader->brepalm );
    config.writeEntry( "UnderlineLink", reader->bunderlineLink );
    config.writeEntry( "HideScrollBar", m_scrollishidden );
    config.writeEntry( "HideToolBar", m_hidebars );
    config.writeEntry( "Kern", reader->bkern );
    config.writeEntry( "Remap", reader->bremap );
    config.writeEntry( "Peanut", reader->bpeanut );
    config.writeEntry( "MakeBold", reader->bmakebold );
    config.writeEntry( "Continuous", reader->m_continuousDocument );
    config.writeEntry( "DoubleBuffer", reader->m_doubleBuffered);
    config.writeEntry( "FullJust", reader->bfulljust );
    //    config.writeEntry( "Negative", reader->bNegative );
    config.writeEntry( "Inverse", reader->bInverse );
    config.writeEntry( "FixGraphics", reader->m_fontControl.FixGraphics());
    config.writeEntry( "ExtraSpace", reader->getextraspace() );
    config.writeEntry( "ExtraLead", reader->getlead() );
    config.writeEntry( "Basesize",  (int)reader->getBaseSize());
    config.writeEntry( "RequestorFontChange", m_propogatefontchange);
#ifdef USEQPE
    config.writeEntry( "GrabKeyboard", m_grabkeyboard );
#endif
    if (full)
    {
	config.setGroup( "Toolbar" );
	config.writeEntry("QTScrollBar", m_qtscroll);
	config.writeEntry("LocalScrollBar", m_localscroll);
	config.writeEntry("Movable", m_tbmovesave);
	config.writeEntry("Policy", m_tbpolsave);
	config.writeEntry("Position", m_tbposition);
#ifndef USEQPE
	if (!isMaximized() && !isMinimized())
	  {
	    config.setGroup( "Geometry" );
	    config.writeEntry( "x", x() );
	    config.writeEntry( "y", y() );
	    config.writeEntry( "width", width() );
	    config.writeEntry( "height", height() );
	  }
#endif
    }
}

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

bool QTReaderApp::readconfig(const QString& dirname, const QString& _txt, bool full=false)
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
	configname = Global::applicationFileName(dirname, _txt);
	QFileInfo fm(configname);
	if ( !fm.exists() ) return false;
	dom = Config::File;
    }

    Config config(configname, dom);
    config.setGroup( "View" );

#else
	QString fullname;
	if (full)
	{
	  fullname = QDir::homeDirPath() + "/" + _txt + "/" + INIFILE;
	}
	else
	{
	  fullname = QDir::homeDirPath() + "/" + dirname + "/" + _txt;
	}
    if (!QFile::exists(fullname)) return false;

    Config config(fullname);
#endif
    if (full)
    {
	config.setGroup("Toolbar");
	m_tbmovesave = m_tbmove = config.readBoolEntry("Movable", m_tbmovesave);
	m_tbpolsave = m_tbpol = (ToolbarPolicy)config.readNumEntry("Policy", m_tbpolsave);
	m_tbposition = (ToolBarDock)config.readNumEntry("Position", m_tbposition);
    }
    config.setGroup( "View" );
    m_bFloatingDialog = config.readBoolEntry("FloatDialogs", m_bFloatingDialog);
    reader->bstripcr = config.readBoolEntry( "StripCr", reader->bstripcr );
    reader->bNoInlineTables = config.readBoolEntry( "NoInlineTables", reader->bNoInlineTables );
    reader->bfulljust = config.readBoolEntry( "FullJust", reader->bfulljust );
    reader->bInverse = config.readBoolEntry( "Inverse", reader->bInverse );
    //    reader->bNegative = config.readBoolEntry( "Negative", false );
    reader->m_fontControl.FixGraphics(config.readBoolEntry( "FixGraphics", reader->m_fontControl.FixGraphics() ));
    reader->setextraspace(config.readNumEntry( "ExtraSpace", reader->getextraspace() ));
    reader->setlead(config.readNumEntry( "ExtraLead", reader->getlead() ));
    reader->btextfmt = config.readBoolEntry( "TextFmt", reader->btextfmt );
    reader->bautofmt = config.readBoolEntry( "AutoFmt", reader->bautofmt );
    reader->bstriphtml = config.readBoolEntry( "StripHtml", reader->bstriphtml );
    reader->bpeanut = config.readBoolEntry( "Peanut", reader->bpeanut );
    reader->bdehyphen = config.readBoolEntry( "Dehyphen", reader->bdehyphen );
    reader->bdepluck = config.readBoolEntry( "Depluck", reader->bdepluck );
    reader->bdejpluck = config.readBoolEntry( "Dejpluck", reader->bdejpluck );
    reader->bonespace = config.readBoolEntry( "OneSpace", reader->bonespace );
    reader->bunindent = config.readBoolEntry( "Unindent", reader->bunindent );
    reader->brepara = config.readBoolEntry( "Repara", reader->brepara );
    reader->m_reparastring = config.readEntry( "ReparaString", reader->m_reparastring);
    m_bgtype = (bground)config.readNumEntry( "BackgroundType" , m_bgtype );
    m_themename = config.readEntry("Theme", m_themename );
    reader->bdblspce = config.readBoolEntry( "DoubleSpace", reader->bdblspce );
    reader->bindenter = config.readNumEntry( "Indent", reader->bindenter );
    reader->m_textsize = config.readNumEntry( "FontSize", reader->m_textsize );
    reader->m_delay = config.readNumEntry( "ScrollDelay", reader->m_delay);
    reader->m_scrollstep = config.readNumEntry( "ScrollStep", reader->m_scrollstep);
    reader->m_scrolltype = config.readNumEntry( "ScrollType", reader->m_scrolltype);
    if (full)
    {
      reader->m_lastfile = config.readEntry( "LastFile", reader->m_lastfile );
      reader->m_lastposn = config.readNumEntry( "LastPosn", reader->m_lastposn );
    }
    reader->m_bpagemode = config.readBoolEntry( "PageMode", reader->m_bpagemode );
    reader->m_bMonoSpaced = config.readBoolEntry( "MonoSpaced", reader->m_bMonoSpaced);
    reader->m_rotated = config.readBoolEntry( "IsRotated", reader->m_rotated );
    m_statusstring = config.readEntry("StatusContent", m_statusstring);
    m_statusishidden = config.readBoolEntry("StatusHidden", m_statusishidden);
    m_background = config.readNumEntry( "Background", m_background );
    reader->setBackground(getcolour(m_background));
    m_foreground = config.readNumEntry( "Foreground", m_foreground );
    reader->setForeground(getcolour(m_foreground));
    m_scrollcolor = config.readNumEntry( "ScrollColour", m_scrollcolor);
    setscrollcolour();
    m_scrollbarcolor = config.readNumEntry( "ScrollBarColour", m_scrollbarcolor);
    setscrollbarcolour();
    reader->hyphenate = config.readBoolEntry( "Hyphenate", reader->hyphenate );
    //  reader->buffdoc.setCustomHyphen(config.readBoolEntry( "CustomHyphen", false ));
    reader->m_swapmouse = config.readBoolEntry( "SwapMouse", reader->m_swapmouse);
    reader->m_fontname = config.readEntry( "Fontname", reader->m_fontname );
    reader->m_encd = config.readNumEntry( "Encoding", reader->m_encd );
    reader->m_charpc = config.readNumEntry( "CharSpacing", reader->m_charpc );
    reader->m_overlap = config.readNumEntry( "Overlap", reader->m_overlap );
    reader->m_abstopmargin = config.readNumEntry( "Top Margin", reader->m_abstopmargin );
    reader->m_absbottommargin = config.readNumEntry( "Bottom Margin", reader->m_absbottommargin );
    reader->m_absleft_border = config.readNumEntry( "Left Margin", reader->m_absleft_border );
    reader->m_absright_border = config.readNumEntry( "Right Margin", reader->m_absright_border );
    m_scrollishidden = config.readBoolEntry( "HideScrollBar", m_scrollishidden );
    m_hidebars = config.readBoolEntry( "HideToolBar", m_hidebars );
    reader->brepalm = config.readBoolEntry( "Repalm", reader->brepalm );
    reader->bunderlineLink = config.readBoolEntry( "UnderlineLink", reader->bunderlineLink );
    reader->bkern = config.readBoolEntry( "Kern", reader->bkern );
    reader->bremap = config.readBoolEntry( "Remap", reader->bremap );
    reader->bmakebold = config.readBoolEntry( "MakeBold", reader->bmakebold );
    reader->setContinuous(config.readBoolEntry( "Continuous", reader->m_continuousDocument ));
    reader->setDoubleBuffer(config.readBoolEntry("DoubleBuffer", reader->m_doubleBuffered));
    m_targetapp = config.readEntry( "TargetApp", m_targetapp );
    m_targetmsg = config.readEntry( "TargetMsg", m_targetmsg );
#ifdef _SCROLLPIPE
    reader->m_pipetarget = config.readEntry( "PipeTarget", reader->m_pipetarget );
    reader->m_pauseAfterEachPara = config.readBoolEntry( "PauseAfterPara", reader->m_pauseAfterEachPara );
#endif
    m_twoTouch = config.readBoolEntry( "TwoTouch", m_twoTouch);
    m_doAnnotation = config.readBoolEntry( "Annotation", m_doAnnotation);
    m_doDictionary = config.readBoolEntry( "Dictionary", m_doDictionary);
    m_doClipboard = config.readBoolEntry( "Clipboard", m_doClipboard);
    m_doOutput = config.readBoolEntry( "OutputTgt", m_doOutput);
#ifdef USEQPE
    m_grabkeyboard = config.readBoolEntry( "GrabKeyboard", m_grabkeyboard);
#endif
    m_propogatefontchange = config.readBoolEntry( "RequestorFontChange", m_propogatefontchange);
    reader->setBaseSize(config.readNumEntry( "Basesize", reader->getBaseSize() ));
    reader->setTwoTouch(m_twoTouch);
    
    reader->m_outputName = config.readEntry( "OutputCodec", reader->m_outputName);

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

bool QTReaderApp::PopulateConfig(const char* tgtdir, bool usedirs)
{
    bkmkselector->clear();
    bkmkselector->setText("Cancel");
#ifndef USEQPE
    int cnt = 0;

    QDir d = QDir::home();                      // "/"
    if ( !d.cd(APPDIR) ) {                       // "/tmp"
        qWarning( "Cannot find the \"~/" APPDIR "\" directory" );
	d = QDir::home();
	d.mkdir(APPDIR);
	d.cd(APPDIR);
    }
    if ( !d.cd(tgtdir) ) {                       // "/tmp"
        qWarning( "Cannot find the \"~/" APPDIR "/%s\" directory", tgtdir );
	d = QDir::home();
	d.mkdir(tgtdir);
	d.cd(tgtdir);
    }
    d.setFilter( ((usedirs) ? QDir::Dirs : QDir::Files) | QDir::NoSymLinks );
//        d.setSorting( QDir::Size | QDir::Reversed );

    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator it( *list );      // create list iterator
    QFileInfo *fi;                          // pointer for traversing

    while ( (fi=it.current()) ) {           // for each file...
  
	bkmkselector->insertItem(fi->fileName(), cnt++);
			
	//qDebug( "%10li %s", fi->size(), fi->fileName().data() );
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

	if (lstat((const char *)Global::applicationFileName(finaldir,de->d_name),&buf) == 0 && ((usedirs && S_ISDIR(buf.st_mode)) || (!usedirs && S_ISREG(buf.st_mode))))
	{
	    bkmkselector->insertItem(de->d_name, cnt++);
	}
    }
    delete [] finaldir;
    closedir(d);
#endif
    return (cnt > 0);
}

void QTReaderApp::LoadTheme()
{
    if (PopulateConfig("Themes", true))
      {
        editorStack->raiseWidget( bkmkselector );
        hidetoolbars();
	m_nBkmkAction = cLdTheme;
      }
    else
      QMessageBox::information(this, PROGNAME, "No config files");
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

void QTReaderApp::OnURLSelected(const QString& href, const size_t tgt)
{
#ifndef USEQPE
  qDebug("URL:%s", (const char*)href);
  int col = href.find(':');
  if (col > 0)
    {
      QString type = href.left(col);
      qDebug("Type:%s", (const char*)type);
    }
  else
    {
      qDebug("No type");
    }
  QString msg = "External URL\n" + href;
  QMessageBox::information(this, PROGNAME, msg);
#else
  if (href.isEmpty())
    {
      QMessageBox::information(this, PROGNAME, "No URL information supplied");
    }
  else
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
	      qDebug("<a href=\"%s\">%s</a>", (const char*)href, (const char*)href);
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
#endif
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
	qWarning( "Cannot find the \"~/" APPDIR "\" directory" );
	d = QDir::home();
	d.mkdir(APPDIR);
	d.cd(APPDIR);
    }
    if ( !d.cd("urls") )
    {                       // "/tmp"
	qWarning( "Cannot find the \"~/" APPDIR "/urls\" directory" );
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

QColor QTReaderApp::getcolour(int _c)
{
  QColor c = white;
  switch (_c)
    {
    case 0:
      c = white;
      break;
    case 1:
      c = black;
      break;
    case 2:
      c = darkGray;
      break;
    case 3:
      c = gray;
      break;
    case 4:
      c = lightGray;
      break;
    case 5:
      c = red;
      break;
    case 6:
      c = green;
      break;
    case 7:
      c = blue;
      break;
    case 8:
      c = cyan;
      break;
    case 9:
      c = magenta;
      break;
    case 10:
      c = yellow;
      break;
    case 11:
      c = darkRed;
      break;
    case 12:
      c = darkGreen;
      break;
    case 13:
      c = darkBlue;
      break;
    case 14:
      c = darkCyan;
      break;
    case 15:
      c = darkMagenta;
      break;
    case 16:
      c = darkYellow;
      break;
    default:
      c = lightGray;
      break;
    }
  return c;
}

void QTReaderApp::setscrollcolour()
{
  /*
  QColor xc = getcolour(m_scrollcolor);
  int r,g,b;
  xc.rgb(&r,&g,&b);
  reader->m_scrollcolor.setRgb(255^r, 255^g, 255^b);
  */
  reader->m_scrollcolor = getcolour(m_scrollcolor);
}

void QTReaderApp::setscrollbarcolour()
{
  /*
  QColor xc = getcolour(m_scrollcolor);
  int r,g,b;
  xc.rgb(&r,&g,&b);
  reader->m_scrollcolor.setRgb(255^r, 255^g, 255^b);
  */
  reader->m_scrollbarcolor = getcolour(m_scrollbarcolor);
}

void QTReaderApp::forceopen(const QString& filename)
{
  /*
    QFileInfo fi(reader->m_lastfile);
    fi = QFileInfo(filename);
    QString flnm = fi.absFilePath();
  */
  if (!filename.isEmpty() && QFileInfo(filename).exists())
    {
      updatefileinfo();
      if (pBkmklist != NULL)
	{
	  if (m_fBkmksChanged)
	    {
	      savebkmks();
	    }     
	  delete pBkmklist;
	  pBkmklist = NULL;
	  m_fBkmksChanged = false;
	}
      reader->disableAutoscroll();
      openFile(filename);
      reader->setFocus();
    }
  else
    {
      OnURLSelected(filename, 0);
    }
}

void QTReaderApp::actionscroll(int v)
{
  if (reader->m_rotated)
    {
      reader->dopageup(reader->buffdoc.startSection()+reader->buffdoc.endSection()-v);
    }
  else
    {
      /*
      if (reader->pagelocate() < v)
	{
	  while (reader->pagelocate() < v) reader->lineDown();
	}
      else
      */
	reader->locate(v);
    }
}

void QTReaderApp::setBackgroundBitmap()
{
#ifdef USEQPE
  QString file = APPDIR "/Themes/";
  file += m_themename;
    QString tgt = Global::applicationFileName(file,"background");
#else
    QString tgt(QDir::homeDirPath());
    tgt += QString("/" APPDIR "/Themes/") + m_themename + "/background";
#endif
    qDebug("Trying to load %s", (const char *)tgt);
    QPixmap pm(tgt);
    reader->setBackgroundBitmap(pm, m_bgtype);
}

/*

     myChannel = new QCopChannel( "QPE/FooBar", this );
      connect( myChannel, SIGNAL(received(const QCString &, const QByteArray &)),
               this, SLOT(fooBarMessage( const QCString &, const QByteArray &)) );
  
*/
