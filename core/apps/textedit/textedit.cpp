/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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

#include "textedit.h"

#include <qpe/global.h>
#include <qpe/fileselector.h>
#include <qpe/applnk.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
//#include <qpe/finddialog.h>

#include <qaction.h>
#include <qcolordialog.h>
#include <qfileinfo.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qobjectlist.h>
#include <qpopupmenu.h>
#include <qspinbox.h>
#include <qtoolbutton.h>
#include <qwidgetstack.h>

#include <stdlib.h> //getenv


#if QT_VERSION < 300

class QpeEditor : public QMultiLineEdit
{
    //    Q_OBJECT
public:
    QpeEditor( QWidget *parent, const char * name = 0 )
	: QMultiLineEdit( parent, name )
        {
            clearTableFlags();
            setTableFlags( Tbl_vScrollBar | Tbl_autoHScrollBar );
        }

    //public slots:
    void find( const QString &txt, bool caseSensitive,
                            bool backwards );
    /*
signals:
    void notFound();
    void searchWrapped();
    */

private:
    
};


void QpeEditor::find ( const QString &txt, bool caseSensitive,
		       bool backwards )
{
    static bool wrap = FALSE;
    int line, col;
    if ( wrap ) {
	if ( !backwards )
	    line = col = 0;
	wrap = FALSE;
	//	emit searchWrapped();
    } else {
	getCursorPosition( &line, &col );
    }
    //ignore backwards for now....
    if ( !backwards ) {
	for ( ; ; ) {
	    if ( line >= numLines() ) {
		wrap = TRUE;
		//emit notFound();
		break;
	    }
	    int findCol = getString( line )->find( txt, col, caseSensitive );
	    if ( findCol >= 0 ) {
		setCursorPosition( line, findCol, FALSE );
		col = findCol + txt.length();
		setCursorPosition( line, col, TRUE );
		
		//found = TRUE;
		break;
	    }
	    line++;
	    col = 0;
	}

    }

}


#else

#error "Must make a QpeEditor that inherits QTextEdit"

#endif




static int u_id = 1;
static int get_unique_id()
{
    return u_id++;
}

static const int nfontsizes = 6;
static const int fontsize[nfontsizes] = {8,10,12,14,18,24};

TextEdit::TextEdit( QWidget *parent, const char *name, WFlags f )
    : QMainWindow( parent, name, f ), bFromDocView( FALSE )
{
    doc = 0;

    setToolBarsMovable( FALSE );

    setIcon( Resource::loadPixmap( "TextEditor" ) );

    QPEToolBar *bar = new QPEToolBar( this );
    bar->setHorizontalStretchable( TRUE );
    menu = bar;

    QPEMenuBar *mb = new QPEMenuBar( bar );
    QPopupMenu *file = new QPopupMenu( this );
    QPopupMenu *edit = new QPopupMenu( this );
    QPopupMenu *font = new QPopupMenu( this );

    bar = new QPEToolBar( this );
    editBar = bar;

    QAction *a = new QAction( tr( "New" ), Resource::loadPixmap( "new" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( fileNew() ) );
    a->addTo( bar );
    a->addTo( file );

    a = new QAction( tr( "Open" ), Resource::loadPixmap( "fileopen" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    a->addTo( bar );
    a->addTo( file );

    a = new QAction( tr( "Cut" ), Resource::loadPixmap( "cut" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editCut() ) );
    a->addTo( editBar );
    a->addTo( edit );

    a = new QAction( tr( "Copy" ), Resource::loadPixmap( "copy" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editCopy() ) );
    a->addTo( editBar );
    a->addTo( edit );

    a = new QAction( tr( "Paste" ), Resource::loadPixmap( "paste" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( editPaste() ) );
    a->addTo( editBar );
    a->addTo( edit );

     a = new QAction( tr( "Find..." ), Resource::loadPixmap( "find" ), QString::null, 0, this, 0 );
     connect( a, SIGNAL( activated() ), this, SLOT( editFind() ) );
     edit->insertSeparator();
     a->addTo( bar );
     a->addTo( edit );

    int defsize;
    bool defb, defi, wrap;
    {
	Config cfg("TextEdit");
	cfg.setGroup("View");
	defsize = cfg.readNumEntry("FontSize",10);
	defb = cfg.readBoolEntry("Bold",FALSE);
	defi = cfg.readBoolEntry("Italic",FALSE);
	wrap = cfg.readBoolEntry("Wrap",TRUE);
    }

    zin = new QAction( tr("Zoom in"), QString::null, 0, this, 0 );
    connect( zin, SIGNAL( activated() ), this, SLOT( zoomIn() ) );
    zin->addTo( font );

    zout = new QAction( tr("Zoom out"), QString::null, 0, this, 0 );
    connect( zout, SIGNAL( activated() ), this, SLOT( zoomOut() ) );
    zout->addTo( font );

    font->insertSeparator();

#if 0    
    QAction *ba = new QAction( tr("Bold"), QString::null, 0, this, 0 );
    connect( ba, SIGNAL( toggled(bool) ), this, SLOT( setBold(bool) ) );
    ba->setToggleAction(TRUE);
    ba->addTo( font );

    QAction *ia = new QAction( tr("Italic"), QString::null, 0, this, 0 );
    connect( ia, SIGNAL( toggled(bool) ), this, SLOT( setItalic(bool) ) );
    ia->setToggleAction(TRUE);
    ia->addTo( font );

    ba->setOn(defb);
    ia->setOn(defi);
    
    font->insertSeparator();
#endif

    QAction *wa = new QAction( tr("Wrap lines"), QString::null, 0, this, 0 );
    connect( wa, SIGNAL( toggled(bool) ), this, SLOT( setWordWrap(bool) ) );
    wa->setToggleAction(TRUE);
    wa->addTo( font );

    mb->insertItem( tr( "File" ), file );
    mb->insertItem( tr( "Edit" ), edit );
    mb->insertItem( tr( "View" ), font );

    searchBar = new QPEToolBar(this);
    addToolBar( searchBar,  "Search", QMainWindow::Top, TRUE );

    searchBar->setHorizontalStretchable( TRUE );

    searchEdit = new QLineEdit( searchBar, "searchEdit" );
    searchBar->setStretchableWidget( searchEdit );
    connect( searchEdit, SIGNAL( textChanged( const QString & ) ),
			 this, SLOT( search() ) );

     a = new QAction( tr( "Find Next" ), Resource::loadPixmap( "next" ), QString::null, 0, this, 0 );
     connect( a, SIGNAL( activated() ), this, SLOT( findNext() ) );
     a->addTo( searchBar );
     a->addTo( edit );

    a = new QAction( tr( "Close Find" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( findClose() ) );
    a->addTo( searchBar );

    searchBar->hide();
				
    editorStack = new QWidgetStack( this );
    setCentralWidget( editorStack );

    searchVisible = FALSE;

    fileSelector = new FileSelector( "text/*", editorStack, "fileselector" ,
	    TRUE, FALSE );
    connect( fileSelector, SIGNAL( closeMe() ), this, SLOT( showEditTools() ) );
    connect( fileSelector, SIGNAL( newSelected( const DocLnk &) ), this, SLOT( newFile( const DocLnk & ) ) );
    connect( fileSelector, SIGNAL( fileSelected( const DocLnk &) ), this, SLOT( openFile( const DocLnk & ) ) );
    fileOpen();

    editor = new QpeEditor( editorStack );
    editor->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    editorStack->addWidget( editor, get_unique_id() );

    resize( 200, 300 );

    setFontSize(defsize,TRUE);
    wa->setOn(wrap);
}

TextEdit::~TextEdit()
{
    save();

    Config cfg("TextEdit");
    cfg.setGroup("View");
    QFont f = editor->font();
    cfg.writeEntry("FontSize",f.pointSize());
    cfg.writeEntry("Bold",f.bold());
    cfg.writeEntry("Italic",f.italic());
    cfg.writeEntry("Wrap",editor->wordWrap() == QMultiLineEdit::WidgetWidth);
}

void TextEdit::zoomIn()
{
    setFontSize(editor->font().pointSize()+1,FALSE);
}

void TextEdit::zoomOut()
{
    setFontSize(editor->font().pointSize()-1,TRUE);
}


void TextEdit::setFontSize(int sz, bool round_down_not_up)
{
    int s=10;
    for (int i=0; i<nfontsizes; i++) {
	if ( fontsize[i] == sz ) {
	    s = sz;
	    break;
	} else if ( round_down_not_up ) {
	    if ( fontsize[i] < sz )
		s = fontsize[i];
	} else {
	    if ( fontsize[i] > sz ) {
		s = fontsize[i];
		break;
	    }
	}
    }

    QFont f = editor->font();
    f.setPointSize(s);
    editor->setFont(f);

    zin->setEnabled(s != fontsize[nfontsizes-1]);
    zout->setEnabled(s != fontsize[0]);
}

void TextEdit::setBold(bool y)
{
    QFont f = editor->font();
    f.setBold(y);
    editor->setFont(f);
}

void TextEdit::setItalic(bool y)
{
    QFont f = editor->font();
    f.setItalic(y);
    editor->setFont(f);
}

void TextEdit::setWordWrap(bool y)
{
    bool state = editor->edited();
    editor->setWordWrap(y ? QMultiLineEdit::WidgetWidth : QMultiLineEdit::NoWrap );
    editor->setEdited( state );
}

void TextEdit::fileNew()
{
    save();
    newFile(DocLnk());
}

void TextEdit::fileOpen()
{
    if ( !save() ) {
	if ( QMessageBox::critical( this, tr( "Out of space" ),
				    tr( "Text Editor was unable to\n"
					"save your changes.\n"
					"Free some space and try again.\n"
					"\nContinue anyway?" ),
				    QMessageBox::Yes|QMessageBox::Escape,
				    QMessageBox::No|QMessageBox::Default )
	     != QMessageBox::Yes )
	    return;
	else {
	    delete doc;
	    doc = 0;
	}
    }
    menu->hide();
    editBar->hide();
    searchBar->hide();
    clearWState (WState_Reserved1 );
    editorStack->raiseWidget( fileSelector );
    fileSelector->reread();
    updateCaption();
}


#if 0
void TextEdit::slotFind()
{
    FindDialog frmFind( "Text Editor", this );
    connect( &frmFind, SIGNAL(signalFindClicked(const QString &, bool, bool, int)), 
	     editor, SLOT(slotDoFind( const QString&,bool,bool)));

    //case sensitive, backwards, [category] 


    connect( editor, SIGNAL(notFound()), 
	     &frmFind, SLOT(slotNotFound()) );
    connect( editor, SIGNAL(searchWrapped()), 
	     &frmFind, SLOT(slotWrapAround()) );

    frmFind.exec();


}
#endif

void TextEdit::fileRevert()
{
    clear();
    fileOpen();
}

void TextEdit::editCut()
{
#ifndef QT_NO_CLIPBOARD
    editor->cut();
#endif
}

void TextEdit::editCopy()
{
#ifndef QT_NO_CLIPBOARD
    editor->copy();
#endif
}

void TextEdit::editPaste()
{
#ifndef QT_NO_CLIPBOARD
    editor->paste();
#endif
}

void TextEdit::editFind()
{
    searchBar->show();
    searchVisible = TRUE;
    searchEdit->setFocus();
}

void TextEdit::findNext()
{
    editor->find( searchEdit->text(), FALSE, FALSE );

}

void TextEdit::findClose()
{
    searchVisible = FALSE;
    searchBar->hide();
}

void TextEdit::search()
{
    editor->find( searchEdit->text(), FALSE, FALSE );
}

void TextEdit::newFile( const DocLnk &f )
{
    DocLnk nf = f;
    nf.setType("text/plain");
    clear();
    editorStack->raiseWidget( editor );
    setWState (WState_Reserved1 );
    editor->setFocus();
    doc = new DocLnk(nf);
    updateCaption();
}

void TextEdit::openFile( const QString &f )
{
    bFromDocView = TRUE;
    DocLnk nf;
    nf.setType("text/plain");
    nf.setFile(f);
    openFile(nf);
    showEditTools();
    // Show filename in caption
    QString name = f;
    int sep = name.findRev( '/' );
    if ( sep > 0 )
	name = name.mid( sep+1 );
    updateCaption( name );
}

void TextEdit::openFile( const DocLnk &f )
{
    clear();
    FileManager fm;
    QString txt;
    if ( !fm.loadFile( f, txt ) ) {
	// ####### could be a new file
	//qDebug( "Cannot open file" );
	//return;
    }
    fileNew();
    if ( doc )
	delete doc;
    doc = new DocLnk(f);
    editor->setText(txt);
    editor->setEdited(FALSE);
    updateCaption();
}

void TextEdit::showEditTools()
{
    if ( !doc )
	close();
    fileSelector->hide();
    menu->show();
    editBar->show();
    if ( searchVisible )
	searchBar->show();
    updateCaption();
}

bool TextEdit::save()
{
    // case of nothing to save...
    if ( !doc )
	return true;
    if ( !editor->edited() ) {
	delete doc;
	doc = 0;
	return true;
    }

    QString rt = editor->text();

    if ( doc->name().isEmpty() ) {
	QString pt = rt.simplifyWhiteSpace();
	int i = pt.find( ' ' );
	QString docname = pt;
	if ( i > 0 )
	    docname = pt.left( i );
	// remove "." at the beginning
	while( docname.startsWith( "." ) )
	    docname = docname.mid( 1 );
	docname.replace( QRegExp("/"), "_" );
	// cut the length. filenames longer than that don't make sense and something goes wrong when they get too long.
	if ( docname.length() > 40 )
	    docname = docname.left(40);
	if ( docname.isEmpty() )
	    docname = "Empty Text";
	doc->setName(docname);
    }

	
    FileManager fm;
    if ( !fm.saveFile( *doc, rt ) ) {
	return false;
    }
    delete doc;
    doc = 0;
    editor->setEdited( false );
    return true;
}

void TextEdit::clear()
{
    delete doc;
    doc = 0;
    editor->clear();
}

void TextEdit::updateCaption( const QString &name )
{
    if ( !doc )
	setCaption( tr("Text Editor") );
    else {
	QString s = name;
	if ( s.isNull() )
	    s = doc->name();
	if ( s.isEmpty() )
	    s = tr( "Unnamed" );
	setCaption( s + " - " + tr("Text Editor") );
    }
}

void TextEdit::setDocument(const QString& fileref)
{
    bFromDocView = TRUE;
    openFile(DocLnk(fileref));
    showEditTools();
}

void TextEdit::closeEvent( QCloseEvent *e )
{
    if ( editorStack->visibleWidget() == editor && !bFromDocView ) {
	e->ignore();
	fileRevert();
    } else {
	bFromDocView = FALSE;
	e->accept();
    }
}
	
void TextEdit::accept()
{
    fileOpen();
}	
