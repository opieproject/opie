/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
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

#include <qclipboard.h>
#include <qwidgetstack.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qpe/fontdatabase.h>
#include <qcombobox.h>
#include <qpopupmenu.h>
#include <qaction.h>
#include <qapplication.h>
#include <qlineedit.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qobjectlist.h>
#include <qpe/global.h>
#include <qpe/applnk.h>
#include <qfileinfo.h>
#include <stdlib.h> //getenv
#include <qprogressbar.h>
#include <qpe/config.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qpe/qcopenvelope_qws.h>

#include "QTReader.h"
#include "Bkmks.h"
#include "cbkmkselector.h"
#include "infowin.h"
#include "CAnnoEdit.h"
#include "QFloatBar.h"
//#include <qpe/fontdatabase.h>

#include <qpe/resource.h>
#include <qpe/qpeapplication.h>

#include "QTReaderApp.h"
#include "fileBrowser.h"
#include "CDrawBuffer.h"


unsigned long QTReaderApp::m_uid = 0;

void QTReaderApp::setScrollState(bool _b) { m_scrollButton->setOn(_b); }

#include <unistd.h>
#include <stddef.h>
#include <dirent.h>

void QTReaderApp::listBkmkFiles()
{
    bkmkselector->clear();
    int cnt = 0;
    DIR *d;
    d = opendir((const char *)Global::applicationFileName("uqtreader",""));

    while(1)
    {
	struct dirent* de;
	struct stat buf;
	de = readdir(d);
	if (de == NULL) break;

	if (lstat((const char *)Global::applicationFileName("uqtreader",de->d_name),&buf) == 0 && S_ISREG(buf.st_mode))
	{
	    bkmkselector->insertItem(de->d_name);
	    cnt++;
	}
    }

    closedir(d);

    if (cnt > 0)
      {
//tjw        menu->hide();
        editBar->hide();
        if (m_fontVisible) m_fontBar->hide();
        if (regVisible) regBar->hide();
        if (searchVisible) searchBar->hide();
	m_nRegAction = cRmBkmkFile;
        editorStack->raiseWidget( bkmkselector );
      }
    else
      QMessageBox::information(this, "QTReader", "No bookmark files");
}

QTReaderApp::QTReaderApp( QWidget *parent, const char *name, WFlags f )
    : QMainWindow( parent, name, f ), bFromDocView( FALSE ), m_dontSave(false)
{
//  qDebug("Application directory = %s", (const tchar *)QPEApplication::documentDir());
//  qDebug("Application directory = %s", (const tchar *)Global::applicationFileName("uqtreader","bkmks.xml"));

    pBkmklist = NULL;
    doc = 0;

	m_fBkmksChanged = false;
    
  QString lang = getenv( "LANG" );

	m_autogenstr = "^ *[A-Z].*[a-z] *$";
    setToolBarsMovable( FALSE );

    setIcon( Resource::loadPixmap( "uqtreader" ) );

    QPEToolBar *bar = new QPEToolBar( this );
    bar->setHorizontalStretchable( TRUE );
    addToolBar(bar, "tool",QMainWindow::Top, true);
//tjw    menu = bar;

    QPEMenuBar *mb = new QPEMenuBar( bar );
    QPopupMenu *file = new QPopupMenu( this );
    QPopupMenu *format = new QPopupMenu( this );
    //    QPopupMenu *edit = new QPopupMenu( this );

//    bar = new QToolBar( this );
    editBar = bar;

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

    pbar = new QProgressBar(this);
    pbar->hide();

    m_annoWin = new CAnnoEdit(editorStack);
    editorStack->addWidget(m_annoWin, get_unique_id());
    connect( m_annoWin, SIGNAL( finished(const QString&, const QString&) ), this, SLOT( addAnno(const QString&, const QString&) ) );
    connect( m_annoWin, SIGNAL( cancelled() ), this, SLOT( infoClose() ) );

    m_infoWin = new infowin(editorStack);
    editorStack->addWidget(m_infoWin, get_unique_id());
    connect( m_infoWin, SIGNAL( Close() ), this, SLOT( infoClose() ) );

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

    reader = new QTReader( editorStack );

    ((QPEApplication*)qApp)->setStylusOperation(reader, QPEApplication::RightOnHold);

  Config config( "uqtreader" );
  config.setGroup( "View" );

  reader->bstripcr = config.readBoolEntry( "StripCr", true );
  reader->btextfmt = config.readBoolEntry( "TextFmt", false );
  reader->bautofmt = config.readBoolEntry( "AutoFmt", true );
  reader->bstriphtml = config.readBoolEntry( "StripHtml", false );
  reader->bpeanut = config.readBoolEntry( "Peanut", false );
  reader->bdehyphen = config.readBoolEntry( "Dehyphen", false );
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
  reader->m_fontname = config.readEntry( "Fontname", "helvetica" );
  reader->m_encd = config.readNumEntry( "Encoding", 0 );
  reader->m_charpc = config.readNumEntry( "CharSpacing", 100 );
  reader->m_overlap = config.readNumEntry( "Overlap", 0 );
  reader->bremap = config.readBoolEntry( "Remap", true );
  reader->bmakebold = config.readBoolEntry( "MakeBold", false );
  m_targetapp = config.readEntry( "TargetApp", QString::null );
  m_targetmsg = config.readEntry( "TargetMsg", QString::null );
  m_twoTouch = config.readBoolEntry( "TwoTouch", false);
  m_doAnnotation = config.readBoolEntry( "Annotation", false);
  m_doDictionary = config.readBoolEntry( "Dictionary", false);
  m_doClipboard = config.readBoolEntry( "Clipboard", false);
  m_spaceTarget = (ActionTypes)config.readNumEntry("SpaceTarget", cesAutoScroll);
  setTwoTouch(m_twoTouch);


    connect( reader, SIGNAL( OnRedraw() ), this, SLOT( OnRedraw() ) );
    connect( reader, SIGNAL( OnActionPressed() ), this, SLOT( OnActionPressed() ) );
    connect( reader, SIGNAL( OnWordSelected(const QString&, size_t, const QString&) ), this, SLOT( OnWordSelected(const QString&, size_t, const QString&) ) );
    editorStack->addWidget( reader, get_unique_id() );

    QAction *a = new QAction( tr( "Open" ), Resource::loadPixmap( "fileopen" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    a->addTo( bar );
    a->addTo( file );

    /*
    a = new QAction( tr( "Revert" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( fileRevert() ) );
    a->addTo( file );

    a = new QAction( tr( "Cut" ), Resource::loadPixmap( "cut" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editCut() ) );
    a->addTo( editBar );
    a->addTo( edit );
    */

    a = new QAction( tr( "Info" ), QString::null, 0, this, NULL);
    connect( a, SIGNAL( activated() ), this, SLOT( showinfo() ) );
    a->addTo( file );

    QActionGroup* ag = new QActionGroup(this);
    QPopupMenu *spacemenu = new QPopupMenu(this);
    file->insertItem( tr( "On Action..." ), spacemenu );

    m_buttonAction[0] = new QAction( tr( "Open File" ), QString::null, 0, ag, NULL, true );

    m_buttonAction[1] = new QAction( tr( "Autoscroll" ), QString::null, 0, ag, NULL, true );

    m_buttonAction[2] = new QAction( tr( "Mark" ), QString::null, 0, ag, NULL, true );

    ag->addTo(spacemenu);

    connect(ag, SIGNAL( selected(QAction*) ), this, SLOT( buttonActionSelected(QAction*) ) );


    a = m_scrollButton = new QAction( tr( "Scroll" ), Resource::loadPixmap( "panel-arrow-down" ), QString::null, 0, this, 0, true );
//    connect( a, SIGNAL( activated() ), this, SLOT( autoScroll() ) );
    a->setOn(false);
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( autoScroll(bool) ) );
     file->insertSeparator();
    a->addTo( bar );
    a->addTo( file );

    /*
    a = new QAction( tr( "Find" ), QString::null, 0, this, NULL, true );
    //    connect( a, SIGNAL( activated() ), this, SLOT( pagedn() ) );
    a->addTo( file );

    a = new QAction( tr( "Find Again" ), QString::null, 0, this, NULL, true );
    //    connect( a, SIGNAL( activated() ), this, SLOT( pagedn() ) );
    a->addTo( file );
    */
    a = new QAction( tr( "Jump" ), QString::null, 0, this, NULL);
    connect( a, SIGNAL( activated() ), this, SLOT( jump() ) );
    a->addTo( file );

    a = new QAction( tr( "Page/Line Scroll" ), QString::null, 0, this, NULL, true );
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( pagemode(bool) ) );
    a->setOn(reader->m_bpagemode);
    a->addTo( file );

    a = new QAction( tr( "Set Overlap" ), QString::null, 0, this, NULL);
    connect( a, SIGNAL( activated() ), this, SLOT( setoverlap() ) );
    a->addTo( file );

    file->insertSeparator();

    a = new QAction( tr( "Set Dictionary" ), QString::null, 0, this, NULL);
    connect( a, SIGNAL( activated() ), this, SLOT( settarget() ) );
    a->addTo( file );

    a = new QAction( tr( "Two/One Touch" ), QString::null, 0, this, NULL, true );
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( setTwoTouch(bool) ) );
    a->setOn(m_twoTouch);
    a->addTo( file );

    ag = new QActionGroup(this);
    ag->setExclusive(false);
    QPopupMenu *encoding = new QPopupMenu(this);
    file->insertItem( tr( "Target" ), encoding );

    a = new QAction( tr( "Annotation" ), QString::null, 0, ag, NULL, true );
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( OnAnnotation(bool) ) );
    a->setOn(m_doAnnotation);

    a = new QAction( tr( "Dictionary" ), QString::null, 0, ag, NULL, true );
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( OnDictionary(bool) ) );
    a->setOn(m_doDictionary);

    a = new QAction( tr( "Clipboard" ), QString::null, 0, ag, NULL, true );
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( OnClipboard(bool) ) );
    a->setOn(m_doClipboard);

    ag->addTo(encoding);


/*
    a = new QAction( tr( "Import" ), QString::null, 0, this, NULL );
    connect( a, SIGNAL( activated() ), this, SLOT( importFiles() ) );
    a->addTo( file );
*/
    
    a = new QAction( tr( "Up" ), Resource::loadPixmap( "up" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( pageup() ) );
    a->addTo( editBar );

    a = new QAction( tr( "Down" ), Resource::loadPixmap( "down" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( pagedn() ) );
    a->addTo( editBar );

    /*
    a = new QAction( tr( "Paste" ), Resource::loadPixmap( "paste" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editPaste() ) );
    a->addTo( editBar );
    a->addTo( edit );
    */

//     a = new QAction( tr( "Find..." ), Resource::loadPixmap( "find" ), QString::null, 0, this, 0 );
     a = new QAction( tr( "Find..." ), QString::null, 0, this, NULL);
     connect( a, SIGNAL( activated() ), this, SLOT( editFind() ) );
     file->insertSeparator();
//     a->addTo( bar );
     a->addTo( file );


     a = m_bkmkAvail = new QAction( tr( "Annotation" ), Resource::loadPixmap( "find" ), QString::null, 0, this, 0 );
     connect( a, SIGNAL( activated() ), this, SLOT( showAnnotation() ) );
     a->addTo( bar );

     m_bkmkAvail->setEnabled(false);


    ag = new QActionGroup(this);
//    ag->setExclusive(false);
    encoding = new QPopupMenu(this);
    format->insertItem( tr( "Markup" ), encoding );

    a = new QAction( tr( "Auto" ), QString::null, 0, ag, NULL, true );
    a->setOn(reader->bautofmt);
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( autofmt(bool) ) );

    a = new QAction( tr( "None" ), QString::null, 0, ag, NULL, true );
    a->setOn(!reader->bautofmt && !(reader->btextfmt || reader->bstriphtml || reader->bpeanut));
//    connect( a, SIGNAL( toggled(bool) ), this, SLOT( textfmt(bool) ) );

    a = new QAction( tr( "Text" ), QString::null, 0, ag, NULL, true );
    a->setOn(reader->btextfmt);
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( textfmt(bool) ) );

    a = new QAction( tr( "HTML" ), QString::null, 0, ag, NULL, true );
    a->setOn(reader->bstriphtml);
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( striphtml(bool) ) );

    a = new QAction( tr( "Peanut/PML" ), QString::null, 0, ag, NULL, true );
    a->setOn(reader->bpeanut);
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( peanut(bool) ) );

    ag->addTo(encoding);



    ag = new QActionGroup(this);
    ag->setExclusive(false);
    encoding = new QPopupMenu(this);
    format->insertItem( tr( "Layout" ), encoding );

    a = new QAction( tr( "Strip CR" ), QString::null, 0, ag, NULL, true );
    a->setOn(reader->bstripcr);
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( stripcr(bool) ) );

    a = new QAction( tr( "Dehyphen" ), QString::null, 0, ag, NULL, true );
    a->setOn(reader->bdehyphen);
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( dehyphen(bool) ) );
//    a->addTo( format );

    a = new QAction( tr( "Unindent" ), QString::null, 0, ag, NULL, true );
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( unindent(bool) ) );
    a->setOn(reader->bunindent);
//    a->addTo( format );

    a = new QAction( tr( "Re-paragraph" ), QString::null, 0, ag, NULL, true );
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( repara(bool) ) );
    a->setOn(reader->brepara);
//    a->addTo( format );

    a = new QAction( tr( "Double Space" ), QString::null, 0, ag, NULL, true );
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( dblspce(bool) ) );
    a->setOn(reader->bdblspce);
//    a->addTo( format );

    a = new QAction( tr( "Indent+" ), QString::null, 0, ag, NULL );
    connect( a, SIGNAL( activated() ), this, SLOT( indentplus() ) );
//    a->addTo( format );

    a = new QAction( tr( "Indent-" ), QString::null, 0, ag, NULL );
    connect( a, SIGNAL( activated() ), this, SLOT( indentminus() ) );

    a = new QAction( tr( "Remap" ), QString::null, 0, ag, NULL, true );
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( remap(bool) ) );
    a->setOn(reader->bremap);

    a = new QAction( tr( "Embolden" ), QString::null, 0, ag, NULL, true );
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( embolden(bool) ) );
    a->setOn(reader->bmakebold);

    ag->addTo(encoding);

    //    a = new QAction( tr( "Zoom" ), QString::null, 0, this, NULL, true );
    //    a = new QAction( tr( "Zoom" ), Resource::loadPixmap( "mag" ), QString::null, 0, this, 0 );
    a = new QAction( tr( "Zoom" ), QString::null, 0, this);
    connect( a, SIGNAL( activated() ), this, SLOT( TBDzoom() ) );
     format->insertSeparator();
    a->addTo( format );
    //    a->addTo( editBar );


    a = new QAction( tr( "Ideogram/Word" ), QString::null, 0, this, NULL, true );
    connect( a, SIGNAL( toggled(bool) ), this, SLOT( monospace(bool) ) );
    a->setOn(reader->m_bMonoSpaced);
     format->insertSeparator();
    a->addTo( format );

    a = new QAction( tr( "Set width" ), QString::null, 0, this, NULL);
    connect( a, SIGNAL( activated() ), this, SLOT( setspacing() ) );
    a->addTo( format );

    encoding = new QPopupMenu(this);
//     format->insertSeparator();
    format->insertItem( tr( "Encoding" ), encoding );

    ag = new QActionGroup(this);

    m_EncodingAction[0] = new QAction( tr( "Ascii" ), QString::null, 0, ag, NULL, true );

    m_EncodingAction[1] = new QAction( tr( "UTF-8" ), QString::null, 0, ag, NULL, true );

    m_EncodingAction[2] = new QAction( tr( "UCS-2(BE)" ), QString::null, 0, ag, NULL, true );

    m_EncodingAction[3] = new QAction( tr( "USC-2(LE)" ), QString::null, 0, ag, NULL, true );

    m_EncodingAction[4] = new QAction( tr( "Palm" ), QString::null, 0, ag, NULL, true );

    m_EncodingAction[5] = new QAction( tr( "Windows(1252)" ), QString::null, 0, ag, NULL, true );

    ag->addTo(encoding);

    connect(ag, SIGNAL( selected(QAction*) ), this, SLOT( encodingSelected(QAction*) ) );

    a = new QAction( tr( "Set Font" ), QString::null, 0, this);
    connect( a, SIGNAL( activated() ), this, SLOT( setfont() ) );
     format->insertSeparator();
    a->addTo( format );

    QPopupMenu *marks = new QPopupMenu( this );

    a = new QAction( tr( "Mark" ), QString::null, 0, this, NULL);
    connect( a, SIGNAL( activated() ), this, SLOT( addbkmk() ) );
    a->addTo( marks );

    a = new QAction( tr( "Annotate" ), QString::null, 0, this, NULL);
    connect( a, SIGNAL( activated() ), this, SLOT( addanno() ) );
    a->addTo( marks );

    a = new QAction( tr( "Goto" ), QString::null, 0, this, NULL, false );
    connect( a, SIGNAL( activated() ), this, SLOT( do_gotomark() ) );
    a->addTo( marks );

    a = new QAction( tr( "Delete" ), QString::null, 0, this, NULL);
    connect( a, SIGNAL( activated() ), this, SLOT( do_delmark() ) );
    a->addTo( marks );

    a = new QAction( tr( "Autogen" ), QString::null, 0, this, NULL, false );
    connect( a, SIGNAL( activated() ), this, SLOT( do_autogen() ) );
     marks->insertSeparator();
    a->addTo( marks );

    a = new QAction( tr( "Clear" ), QString::null, 0, this, NULL);
    connect( a, SIGNAL( activated() ), this, SLOT( clearBkmkList() ) );
    a->addTo( marks );

    a = new QAction( tr( "Save" ), QString::null, 0, this, NULL );
    connect( a, SIGNAL( activated() ), this, SLOT( savebkmks() ) );
    a->addTo( marks );

    a = new QAction( tr( "Tidy" ), QString::null, 0, this, NULL);
    connect( a, SIGNAL( activated() ), this, SLOT( listBkmkFiles() ) );
     marks->insertSeparator();
    a->addTo( marks );

    a = new QAction( tr( "Start Block" ), QString::null, 0, this, NULL);
    connect( a, SIGNAL( activated() ), this, SLOT( editMark() ) );
    marks->insertSeparator();
    a->addTo( marks );

    a = new QAction( tr( "Copy Block" ), QString::null, 0, this, NULL);
    connect( a, SIGNAL( activated() ), this, SLOT( editCopy() ) );
    a->addTo( marks );


    mb->insertItem( tr( "File" ), file );
    //    mb->insertItem( tr( "Edit" ), edit );
    mb->insertItem( tr( "Format" ), format );
    mb->insertItem( tr( "Marks" ), marks );

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
     a = new QAction( tr( "Find Next" ), Resource::loadPixmap( "next" ), QString::null, 0, this, 0 );
     connect( a, SIGNAL( activated() ), this, SLOT( findNext() ) );
     a->addTo( searchBar );

    a = new QAction( tr( "Close Find" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
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

    a = new QAction( tr( "Do Reg" ), Resource::loadPixmap( "enter" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( do_regaction() ) );
    a->addTo( regBar );

    a = new QAction( tr( "Close Edit" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( regClose() ) );
    a->addTo( regBar );

    regBar->hide();

    m_fontBar = new QToolBar( "Autogen", this, QMainWindow::Top, TRUE );

    m_fontBar->setHorizontalStretchable( TRUE );

    m_fontSelector = new QComboBox(false, m_fontBar);
    m_fontBar->setStretchableWidget( m_fontSelector );
    {
	FontDatabase f;
	QStringList flist = f.families();
	m_fontSelector->insertStringList(flist);

	bool realfont = false;
	for (QStringList::Iterator nm = flist.begin(); nm != flist.end(); nm++)
	{
	    if (reader->m_fontname == *nm)
	    {
		realfont = true;
		break;
	    }
	}
	if (!realfont) reader->m_fontname = flist[0];
    } // delete the FontDatabase!!!

    connect( m_fontSelector, SIGNAL( activated(const QString& ) ),
	     this, SLOT( do_setfont(const QString&) ) );

    m_fontBar->hide();
    m_fontVisible = false;

    connect(qApp, SIGNAL( appMessage(const QCString&, const QByteArray& ) ),
	     this, SLOT( msgHandler(const QCString&, const QByteArray&) ) );


  reader->init();
    if (!reader->m_lastfile.isEmpty())
    {
	openFile( reader->m_lastfile );
	doc = new DocLnk(reader->m_lastfile);
    }
    m_EncodingAction[reader->m_encd]->setOn(true);
    m_buttonAction[m_spaceTarget]->setOn(true);
    do_setfont(reader->m_fontname);
}

void QTReaderApp::msgHandler(const QCString& _msg, const QByteArray& _data)
{
    QString msg = QString::fromUtf8(_msg);

//    qDebug("Received:%s", (const char*)msg);

    QDataStream stream( _data, IO_ReadOnly );
    if ( msg == "info(QString)" )
    {
	QString info;
	stream >> info;
	QMessageBox::information(this, "QTReader", info);
    }
    else if ( msg == "warn(QString)" )
    {
	QString info;
	stream >> info;
	QMessageBox::warning(this, "QTReader", info);
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
    else if ( msg == "File/Start Block()" )
    {
	editMark();
    }
    else if ( msg == "File/Copy Block()" )
    {
	editCopy();
    }
    else if ( msg == "File/Scroll(int)" )
    {
	int info;
	stream >> info;
	autoScroll(info);
    }
    else if ( msg == "File/Jump(int)" )
    {
	int info;
	stream >> info;
	reader->locate(info);
    }
    else if ( msg == "File/Page/Line Scroll(int)" )
    {
	int info;
	stream >> info;
	pagemode(info);
    }
    else if ( msg == "File/Set Overlap(int)" )
    {
	int info;
	stream >> info;
	reader->m_overlap = info;
    }
    else if ( msg == "File/Set Dictionary(QString)" )
    {
	QString info;
	stream >> info;
	do_settarget(info);
    }
    else if ( msg == "File/Two/One Touch(int)" )
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
	reader->buffdoc.getline(&test,reader->width());
	while (arg.match(toQString(test.data())) == -1)
	{
	    pos = reader->locate();
	    if (!reader->buffdoc.getline(&test,reader->width()))
	    {
		QMessageBox::information(this, "QTReader", QString("Can't find\n")+info);
		pos = start;
		break;
	    }
	}
	reader->locate(pos);
    }
    else if ( msg == "Layout/Strip CR(int)" )
    {
	int info;
	stream >> info;
	stripcr(info);
    }
    else if ( msg == "Markup/Auto(int)" )
    {
	int info;
	stream >> info;
	autofmt(info);
    }
    else if ( msg == "Markup/Text(int)" )
    {
	int info;
	stream >> info;
	textfmt(info);
    }
    else if ( msg == "Markup/HTML(int)" )
    {
	int info;
	stream >> info;
	striphtml(info);
    }
    else if ( msg == "Markup/Peanut(int)" )
    {
	int info;
	stream >> info;
	peanut(info);
    }
    else if ( msg == "Layout/Dehyphen(int)" )
    {
	int info;
	stream >> info;
	dehyphen(info);
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
    else if ( msg == "Layout/Double Space(int)" )
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
    else if ( msg == "Format/Set width(int)" )
    {
	int info;
	stream >> info;
	reader->m_charpc = info;
	reader->setfont();
	reader->refresh();
    }
    else if ( msg == "Format/Encoding(QString)" )
    {
	QString info;
	stream >> info;
	reader->setencoding(EncNameToInt(info));
    }
    else if ( msg == "Format/Set Font(QString,int)" )
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
}

ActionTypes QTReaderApp::ActNameToInt(const QString& _enc)
{
    for (int i = 0; i < MAX_ACTIONS; i++)
    {
	if (m_buttonAction[i]->text() == _enc) return (ActionTypes)i;
    }
    return cesAutoScroll;
}

int QTReaderApp::EncNameToInt(const QString& _enc)
{
    for (int i = 0; i < MAX_ENCODING; i++)
    {
	if (m_EncodingAction[i]->text() == _enc) return i;
    }
    return 0;
/*
    if (_enc == "Ascii") return 0;
    if (_enc == "UTF-8") return 1;
    if (_enc == "UCS-2(BE)") return 2;
    if (_enc == "USC-2(LE)") return 3;
*/
}

void QTReaderApp::encodingSelected(QAction* _a)
{
//    qDebug("es:%x : %s", _a, (const char *)(_a->text()));
    reader->setencoding(EncNameToInt(_a->text()));
}

void QTReaderApp::buttonActionSelected(QAction* _a)
{
//    qDebug("es:%x : %s (%u)", _a, (const char *)(_a->text()), ActNameToInt(_a->text()));
    m_spaceTarget = ActNameToInt(_a->text());
}

QTReaderApp::~QTReaderApp()
{
}

void QTReaderApp::autoScroll(bool _b)
{
    reader->setautoscroll(_b);
}

void QTReaderApp::TBD()
{
  QMessageBox::information(this, "QTReader", "Not yet implemented", 1);
}

void QTReaderApp::TBDzoom()
{
  QMessageBox::information(this, "QTReader", "Zooming is done interactively\nTry left/right cursor keys", 1);
}

void QTReaderApp::clearBkmkList()
{
	delete pBkmklist;
	pBkmklist = NULL;
	m_fBkmksChanged = false;
}

void QTReaderApp::fileOpen()
{
/*
    menu->hide();
    editBar->hide();
    if (regVisible) regBar->hide();
    if (searchVisible) searchBar->hide();
*/
    if (pBkmklist != NULL)
    {
	if (m_fBkmksChanged)
	{
	    if (QMessageBox::warning(this, "QTReader", "Save bookmarks?", "Save", "Don't bother") == 0)
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
    fileBrowser* fb = new fileBrowser(this,"QTReader",TRUE,
				      0,
//				      WStyle_Customize | WStyle_NoBorderEx,
				      "*", QFileInfo(reader->m_lastfile).dirPath(true));

    if (fb->exec())
    {
	QString fn(fb->fileList[0]);
//    fb->populateList();
	if (!fn.isEmpty() && QFileInfo(fn).isFile()) openFile(fn);
    }
    delete fb;
    reader->setFocus();
}

void QTReaderApp::showinfo()
{
    unsigned long fs, ts, pl;
    if (reader->empty())
    {
	QMessageBox::information(this, "QTReader", "No file loaded", 1);
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
	    QMessageBox::information(this, "QTReader", "Need a name for the bookmark\nPlease try again", 1);
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
	QMessageBox::information(this, "QTReader", "No file loaded", 1);
    }
    else
    {
	m_annoWin->setName("");
	m_annoWin->setAnno("");
	m_annoWin->setPosn(reader->pagelocate());
	m_annoIsEditing = true;
	editorStack->raiseWidget( m_annoWin );
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
	reader->jumpto(m_savedpos);
	while (reader->explocate() < endpos && (ch = reader->getch()) != UEOF)
	{
	    text += ch;
	}
	cb->setText(text);
	reader->locate(currentpos);
}

void QTReaderApp::pageup()
{
    reader->goUp();
}

void QTReaderApp::pagedn()
{
    reader->goDown();
}

void QTReaderApp::stripcr(bool _b)
{
    reader->setstripcr(_b);
}
void QTReaderApp::remap(bool _b)
{
    reader->setremap(_b);
}
void QTReaderApp::peanut(bool _b)
{
    reader->setpeanut(_b);
}
void QTReaderApp::embolden(bool _b)
{
    reader->setmakebold(_b);
}
void QTReaderApp::autofmt(bool _b)
{
    reader->setautofmt(_b);
}
void QTReaderApp::textfmt(bool _b)
{
    reader->settextfmt(_b);
}
void QTReaderApp::striphtml(bool _b)
{
    reader->setstriphtml(_b);
}
void QTReaderApp::dehyphen(bool _b)
{
    reader->setdehyphen(_b);
}
void QTReaderApp::unindent(bool _b)
{
    reader->setunindent(_b);
}
void QTReaderApp::repara(bool _b)
{
    reader->setrepara(_b);
}
void QTReaderApp::dblspce(bool _b)
{
    reader->setdblspce(_b);
}
void QTReaderApp::pagemode(bool _b)
{
    reader->setpagemode(_b);
}

void QTReaderApp::monospace(bool _b)
{
    reader->setmono(_b);
}

void QTReaderApp::setspacing()
{
    m_nRegAction = cMonoSpace;
    char lcn[20];
    sprintf(lcn, "%lu", reader->m_charpc);
    regEdit->setText(lcn);
    do_regedit();
}

void QTReaderApp::setoverlap()
{
    m_nRegAction = cOverlap;
    char lcn[20];
    sprintf(lcn, "%lu", reader->m_overlap);
    regEdit->setText(lcn);
    do_regedit();
}

void QTReaderApp::settarget()
{
    m_nRegAction = cSetTarget;
    QString text = ((m_targetapp.isEmpty()) ? QString("") : m_targetapp)
	+ "/"
	+ ((m_targetmsg.isEmpty()) ? QString("") : m_targetmsg);
    regEdit->setText(text);
    do_regedit();
}

void QTReaderApp::do_overlap(const QString& lcn)
{
    bool ok;
    unsigned long ulcn = lcn.toULong(&ok);
    if (ok)
    {
	reader->m_overlap = ulcn;
    }
    else
	QMessageBox::information(this, "QTReader", "Must be a number");
}

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
	QMessageBox::information(this, "QTReader", "Must be a number");
}

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
  searchBar->show();
  searchVisible = TRUE;
  searchEdit->setFocus();
#ifdef __ISEARCH
  searchStack->push(new searchrecord("",reader->pagelocate()));
#endif
}

void QTReaderApp::findNext()
{
  //  qDebug("findNext called\n");
#ifdef __ISEARCH
  QString arg = searchEdit->text();
#else
  QRegExp arg = searchEdit->text();
#endif
  CDrawBuffer test(&(reader->m_fontControl));
  size_t start = reader->pagelocate();
  reader->jumpto(start);
  reader->buffdoc.getline(&test,reader->width());
  dosearch(start, test, arg);
}

void QTReaderApp::findClose()
{
  searchVisible = FALSE;
  searchEdit->setText("");
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
  reader->setFocus();
}

#ifdef __ISEARCH
bool QTReaderApp::dosearch(size_t start, CDrawBuffer& test, const QString& arg)
#else
bool QTReaderApp::dosearch(size_t start, CDrawBuffer& test, const QRegExp& arg)
#endif
{
  bool ret = true;
  size_t pos = reader->locate();
  reader->buffdoc.getline(&test,reader->width());
#ifdef __ISEARCH
  while (strstr(test.data(),(const tchar*)arg) == NULL)
#else
#ifdef _UNICODE
  while (arg.match(toQString(test.data())) == -1)
#else
  while (arg.match(test.data()) == -1)
#endif
#endif
    {
      pos = reader->locate();
      if (!reader->buffdoc.getline(&test,reader->width()))
	{
	  if (QMessageBox::warning(this, "Can't find", searchEdit->text(), 1, 2) == 2)
	    pos = searchStart;
	  else
	    pos = start;
	  ret = false;
	  findClose();
	  break;
	}
    }
  reader->locate(pos);
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
    openFile(DocLnk(f));
}

void QTReaderApp::openFile( const DocLnk &f )
{
  clear();
  FileManager fm;
  if ( fm.exists( f ) )
    {
//      QMessageBox::information(0, "Progress", "Calling fileNew()");

      clear();

      //      editorStack->raiseWidget( reader );

      //      reader->setFocus();

      //      QMessageBox::information(0, "DocLnk", "Begin");
      doc = new DocLnk(f);
      //      QMessageBox::information(0, "DocLnk done", doc->file());
      //      QMessageBox::information(0, "Progress", "Calling setText()");
      //      QMessageBox::information(0, "Progress", "Textset");

      //      updateCaption();
      showEditTools();
      reader->setText(doc->name(), doc->file());
      readbkmks();
    }
  else
    {
      QMessageBox::information(this, "QTReader", "File does not exist");
    }
	
}

void QTReaderApp::showEditTools()
{
    if ( !doc )
	close();
//    fileSelector->hide();
//tjw    menu->show();
    editBar->show();
    if ( searchVisible )
	searchBar->show();
    if ( regVisible )
	regBar->show();
    if (m_fontVisible) m_fontBar->show();

    updateCaption();
    editorStack->raiseWidget( reader );
    reader->setFocus();
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
    if (doc != 0)
      {
//  QMessageBox::information(this, "QTReader", "Deleting doc", 1);
	delete doc;
//  QMessageBox::information(this, "QTReader", "Deleted doc", 1);
	doc = 0;
      }
    reader->clear();
}

void QTReaderApp::updateCaption()
{
    if ( !doc )
	setCaption( tr("QTReader") );
    else {
	QString s = doc->name();
	if ( s.isEmpty() )
	    s = tr( "Unnamed" );
	setCaption( s + " - " + tr("QTReader") );
    }
}

void QTReaderApp::setDocument(const QString& fileref)
{
    bFromDocView = TRUE;
//QMessageBox::information(0, "setDocument", fileref);
    openFile(DocLnk(fileref));
//    showEditTools();
}

void QTReaderApp::closeEvent( QCloseEvent *e )
{
    if (m_dontSave)
    {
	e->accept();
    }
    else
    {
	if (editorStack->visibleWidget() == reader)
	{
	    if (m_fontVisible)
	    {
		m_fontBar->hide();
		m_fontVisible = false;
	    }
	    if (regVisible)
	    {
		regBar->hide();
		regVisible = false;
		return;
	    }
	    if (searchVisible)
	    {
		searchBar->hide();
		searchVisible = false;
		return;
	    }
	    if (m_fBkmksChanged && pBkmklist != NULL)
	    {
		if (QMessageBox::warning(this, "QTReader", "Save bookmarks?", "Save", "Don't bother") == 0)
		    savebkmks();
		delete pBkmklist;
		pBkmklist = NULL;
		m_fBkmksChanged = false;
	    }
	    bFromDocView = FALSE;
	    saveprefs();
	    e->accept();
	}
	else
	{
	    showEditTools();
	}
    }
}

void QTReaderApp::do_gotomark()
{
    m_nRegAction = cGotoBkmk;
    listbkmk();
}

void QTReaderApp::do_delmark()
{
    m_nRegAction = cDelBkmk;
    listbkmk();
}

void QTReaderApp::listbkmk()
{
    bkmkselector->clear();
    int cnt = 0;
    if (pBkmklist != NULL)
      {
	for (CList<Bkmk>::iterator i = pBkmklist->begin(); i != pBkmklist->end(); i++)
	  {
#ifdef _UNICODE
	    bkmkselector->insertItem(toQString(i->name()));
#else
	    bkmkselector->insertItem(i->name());
#endif
	    cnt++;
	  }
      }
    if (cnt > 0)
      {
//tjw        menu->hide();
        editBar->hide();
        if (m_fontVisible) m_fontBar->hide();
        if (regVisible) regBar->hide();
        if (searchVisible) searchBar->hide();
        editorStack->raiseWidget( bkmkselector );
      }
    else
      QMessageBox::information(this, "QTReader", "No bookmarks in memory");
}

void QTReaderApp::do_autogen()
{
  m_nRegAction = cAutoGen;
  regEdit->setText(m_autogenstr);
  do_regedit();
}

void QTReaderApp::do_regedit()
{
//    editBar->hide();
  regBar->show();
  regVisible = true;
  regEdit->setFocus();
}

void QTReaderApp::gotobkmk(int ind)
{
    switch (m_nRegAction)
    {
	case cGotoBkmk:
	    reader->locate((*pBkmklist)[ind]->value());
	    break;
	case cDelBkmk:
//	    qDebug("Deleting:%s\n",(*pBkmklist)[ind]->name());
	    pBkmklist->erase(ind);
	    m_fBkmksChanged = true;
//	    pBkmklist->sort();
	    break;
	case cRmBkmkFile:
	    unlink((const char *)Global::applicationFileName("uqtreader",bkmkselector->text(ind)));
	    break;
    }
  showEditTools();
}

void QTReaderApp::cancelbkmk()
{
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
	QMessageBox::information(this, "QTReader", "Must be a number");
}

void QTReaderApp::do_regaction()
{
  regBar->hide();
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
      case cMonoSpace:
	  do_mono(regEdit->text());
	  break;
      case cOverlap:
	  do_overlap(regEdit->text());
	  break;
      case cSetTarget:
	  do_settarget(regEdit->text());
	  break;
  }
  reader->restore();
//    editBar->show();
  reader->setFocus();
}

void QTReaderApp::do_settarget(const QString& _txt)
{
    int ind = _txt.find('/');
    if (ind == -1)
    {
	m_targetapp = "";
	m_targetmsg = "";
	QMessageBox::information(this, "QTReader", "Format is\nappname/messagename");
    }
    else
    {
	m_targetapp = _txt.left(ind);
	m_targetmsg = _txt.right(_txt.length()-ind-1);
    }
}

void QTReaderApp::setfont()
{
    for (int i = 1; i <= m_fontSelector->count(); i++)
    {
	if (m_fontSelector->text(i) == reader->m_fontname)
	{
	    m_fontSelector->setCurrentItem(i);
	    break;
	}
    }
    m_fontBar->show();
    m_fontVisible = true;
}

void QTReaderApp::setfontHelper(const QString& lcn, int size = 0)
{
    if (size == 0) size = reader->m_fontControl.currentsize();
    QFont f(lcn, 10 /*, QFont::Bold*/);
    bkmkselector->setFont( f );
    regEdit->setFont( f );
    searchEdit->setFont( f );
    m_annoWin->setFont( f );
    reader->m_fontname = lcn;
    if (!reader->ChangeFont(size))
    {
	reader->ChangeFont(size);
    }
    reader->refresh();
    m_fontBar->hide();
    m_fontVisible = false;
    showEditTools();
}

void QTReaderApp::do_setfont(const QString& lcn)
{
    setfontHelper(lcn);
}

void QTReaderApp::do_autogen(const QString& regText)
{
    unsigned long fs, ts;
    reader->sizes(fs,ts);
  //  qDebug("Reg:%s\n", (const tchar*)(regEdit->text()));
  m_autogenstr = regText;
  QRegExp re(regText);
  CBuffer buff;
  if (pBkmklist != NULL) delete pBkmklist;
  pBkmklist = new CList<Bkmk>;
  m_fBkmksChanged = true;
  pbar->show();
pbar->resize(width(), editBar->height());
  pbar->reset();
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
}

void QTReaderApp::saveprefs()
{
//  reader->saveprefs("uqtreader");
    Config config( "uqtreader" );
    config.setGroup( "View" );
    
    reader->m_lastposn = reader->pagelocate();
    
    config.writeEntry( "StripCr", reader->bstripcr );
    config.writeEntry( "AutoFmt", reader->bautofmt );
    config.writeEntry( "TextFmt", reader->btextfmt );
    config.writeEntry( "StripHtml", reader->bstriphtml );
    config.writeEntry( "Dehyphen", reader->bdehyphen );
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
    config.writeEntry( "Fontname", reader->m_fontname );
    config.writeEntry( "Encoding", reader->m_encd );
    config.writeEntry( "CharSpacing", reader->m_charpc );
    config.writeEntry( "Overlap", (int)(reader->m_overlap) );
    config.writeEntry( "TargetApp", m_targetapp );
    config.writeEntry( "TargetMsg", m_targetmsg );
    config.writeEntry( "TwoTouch", m_twoTouch );
    config.writeEntry( "Annotation", m_doAnnotation);
    config.writeEntry( "Dictionary", m_doDictionary);
    config.writeEntry( "Clipboard", m_doClipboard);
    config.writeEntry( "SpaceTarget", m_spaceTarget);
    config.writeEntry( "Remap", reader->bremap );
    config.writeEntry( "Peanut", reader->bpeanut );
    config.writeEntry( "MakeBold", reader->bmakebold );
}

void QTReaderApp::indentplus()
{
  reader->indentplus();
}

void QTReaderApp::indentminus()
{
  reader->indentminus();
}

/*
void QTReaderApp::oldFile()
{
  qDebug("oldFile called");
  reader->setText(true);
  qDebug("settext called");
  showEditTools();
  qDebug("showedit called");
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
	BkmkFile bf((const char *)Global::applicationFileName("uqtreader",reader->m_string), true);
	bf.write(*pBkmklist);
    }
	m_fBkmksChanged = false;
}

void QTReaderApp::readbkmks()
{
    if (pBkmklist != NULL)
    {
    	delete pBkmklist;
    }
    BkmkFile bf((const char *)Global::applicationFileName("uqtreader",reader->m_string));
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
	  QMessageBox::information(this, "QTReader", "Need a name for the bookmark\nSelect add again", 1);
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
		pBkmklist->push_front(Bkmk((const tchar*)text,reader->pagelocate()));
#endif
		m_fBkmksChanged = true;
		pBkmklist->sort();
	}
}

void QTReaderApp::OnRedraw()
{
    if (pBkmklist != NULL)
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
    editorStack->raiseWidget( m_annoWin );
    m_annoWin->setFocus();
}

void QTReaderApp::OnWordSelected(const QString& wrd, size_t posn, const QString& line)
{
//    qDebug("OnWordSelected(%u):%s", posn, (const char*)wrd);

    if (m_doClipboard)
    {
	QClipboard* cb = QApplication::clipboard();
	cb->setText(wrd);
	if (wrd.length() > 10)
	{
	    Global::statusMessage(wrd.left(8) + "..");
	}
	else
	{
	    Global::statusMessage(wrd);
	}
    }
    if (m_doAnnotation)
    {
//	addAnno(wrd, "Need to be able to edit this", posn);
	m_annoWin->setName(line);
	m_annoWin->setAnno("");
	m_annoWin->setPosn(posn);
	m_annoIsEditing = true;
	editorStack->raiseWidget( m_annoWin );
    }
    if (m_doDictionary)
    {
	if (!m_targetapp.isEmpty() && !m_targetmsg.isEmpty())
	{
	    QCopEnvelope e(("QPE/Application/"+m_targetapp).utf8(), (m_targetmsg+"(QString)").utf8());
	    e << wrd;
	}
    }
}

void QTReaderApp::OnActionPressed()
{
    switch (m_spaceTarget)
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
	default:
	{
	    qDebug("Unknown ActionType:%u", m_spaceTarget);
	}
	break;
    }
}

void QTReaderApp::setTwoTouch(bool _b) { reader->setTwoTouch(_b); }
void QTReaderApp::restoreFocus() { reader->setFocus(); } 
