/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** Licensees holding valid Qtopia Developer license may use this
** file in accordance with the Qtopia Developer License Agreement
** provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
** THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE.
**
** email sales@trolltech.com for information about Qtopia License
** Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "mainwindow.h"
#include <qpe/fileselector.h>
#include <qpe/applnk.h>
#include <qpe/resource.h>
//#include "qspellchecker.h"
#include "qtextedit.h"
#include <qaction.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qtabwidget.h>
#include <qapplication.h>
#include <qfontdatabase.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qprinter.h>
#include <qpaintdevicemetrics.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qcolordialog.h>
#include <qpainter.h>
#include <qstyle.h>

class ButtonMenu : public QToolButton
{
    Q_OBJECT
public:
    ButtonMenu( QWidget *parent, const char *name=0 )
	: QToolButton( parent, name ), current(0)
    {
	setPopup( new QPopupMenu( this ) );
	setPopupDelay( 1 );
	connect( popup(), SIGNAL(activated(int)), this, SLOT(selected(int)) );
    }

    int insertItem(const QIconSet &icon, const QString &text, int id ) {
	if ( !popup()->count() ) {
	    setIconSet( icon );
	    current = id;
	}
	return popup()->insertItem( icon, text, id );
    }

    void setCurrentItem( int id ) {
	if ( id != current ) {
	    current = id;
	    setIconSet( *popup()->iconSet( id ) );
	}
    }

    virtual QSize sizeHint() const {
	return QToolButton::sizeHint() + QSize( 4, 0 );
    }

signals:
    void activated( int id );

protected slots:
    void selected( int id ) {
	current = id;
	setIconSet( *popup()->iconSet( id ) );
	emit activated( id );
    }

protected:
    virtual void drawButtonLabel( QPainter *p ) {
	p->translate( -4, 0 );
	QToolButton::drawButtonLabel( p );
	p->translate( 4, 0 );
    }

private:
    int current;
};

//===========================================================================

MainWindow::MainWindow( QWidget *parent, const char *name )
    : QMainWindow( parent, name ),
      doc( 0 )
{
    setRightJustification(TRUE);

    editorStack = new QWidgetStack( this );

    fileSelector = new FileSelector( "text/html",
				     editorStack, "fileselector" );


    fileSelector->setCloseVisible( FALSE );
    editorStack->addWidget( fileSelector, 0 );

    editor = new Qt3::QTextEdit( editorStack );
    editor->setTextFormat( Qt::RichText );
    editorStack->addWidget( editor, 1 );

    setupActions();

    QObject::connect( fileSelector, SIGNAL(closeMe()),
		      this, SLOT(showEditTools()) );
    QObject::connect( fileSelector, SIGNAL(fileSelected(const DocLnk &)),
		      this, SLOT(openFile(const DocLnk &)) );
    QObject::connect( fileSelector, SIGNAL(newSelected(const DocLnk&)),
		      this, SLOT(newFile(const DocLnk&)) );

    if ( fileSelector->fileCount() < 1 )
	fileNew();
    else {
	fileOpen();
    }
    doConnections( editor );

    setCentralWidget( editorStack );
}

MainWindow::~MainWindow()
{
    save();
}

void MainWindow::setupActions()
{
    setToolBarsMovable(false);

    tbMenu = new QToolBar( this );
    tbMenu->setHorizontalStretchable( TRUE );

    QMenuBar *menu = new QMenuBar( tbMenu );

    tbEdit = new QToolBar( this );

    QPopupMenu *file = new QPopupMenu( this );
    menu->insertItem( tr("File"), file );

    QPopupMenu *edit = new QPopupMenu( this );
    menu->insertItem( tr("Edit"), edit );

    // ### perhaps these shortcut keys should have some
    //     IPaq keys defined???
    QAction *a;

    a = new QAction( tr( "New" ), Resource::loadPixmap("new"), QString::null, 0, this, 0 );
    connect( a, SIGNAL(activated()), this, SLOT(fileNew()) );
    a->addTo( file );

    a = new QAction( tr( "Open" ), Resource::loadPixmap( "fileopen" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL(activated()), this, SLOT(fileOpen()) );
    a->addTo( file );

    a = new QAction( tr( "Undo" ), Resource::loadIconSet("opie-write/undo"),
		     QString::null, 0, this, "editUndo" );
    connect( a, SIGNAL( activated() ), this, SLOT( editUndo() ) );
    connect( editor, SIGNAL(undoAvailable(bool)), a, SLOT(setEnabled(bool)) );
    a->addTo( tbEdit );
    a->addTo( edit );
    a = new QAction( tr( "Redo" ), Resource::loadIconSet("opie-write/redo"),
		     QString::null, 0, this, "editRedo" );
    connect( a, SIGNAL( activated() ), this, SLOT( editRedo() ) );
    connect( editor, SIGNAL(redoAvailable(bool)), a, SLOT(setEnabled(bool)) );
    a->addTo( tbEdit );
    a->addTo( edit );

    edit->insertSeparator();
    
    a = new QAction( tr( "Copy" ), Resource::loadIconSet("copy"),
		     QString::null, 0, this, "editCopy" );
    connect( a, SIGNAL( activated() ), this, SLOT( editCopy() ) );
    connect( editor, SIGNAL(copyAvailable(bool)), a, SLOT(setEnabled(bool)) );
    a->addTo( tbEdit );
    a->addTo( edit );
    a = new QAction( tr( "Cut" ), Resource::loadIconSet("cut"),
		     QString::null, 0, this, "editCut" );
    connect( a, SIGNAL( activated() ), this, SLOT( editCut() ) );
    connect( editor, SIGNAL(copyAvailable(bool)), a, SLOT(setEnabled(bool)) );
    a->addTo( tbEdit );
    a->addTo( edit );
    a = new QAction( tr( "Paste" ), Resource::loadPixmap("paste"),
		     QString::null, 0, this, "editPaste" );
    connect( a, SIGNAL( activated() ), this, SLOT( editPaste() ) );
    a->addTo( tbEdit );
    a->addTo( edit );

    tbFont = new QToolBar( this );
    tbFont->setLabel( "Font Actions" );
    tbFont->setHorizontalStretchable(TRUE);

    comboFont = new QComboBox( FALSE, tbFont );
    QFontDatabase db;
    comboFont->insertStringList( db.families() );
    connect( comboFont, SIGNAL( activated( const QString & ) ),
	     this, SLOT( textFamily( const QString & ) ) );
    comboFont->setCurrentItem( comboFont->listBox()->index( comboFont->listBox()->findItem( QApplication::font().family() ) ) );

    comboSize = new QComboBox( TRUE, tbFont );
    QValueList<int> sizes = db.standardSizes();
    QValueList<int>::Iterator it = sizes.begin();
    for ( ; it != sizes.end(); ++it )
	comboSize->insertItem( QString::number( *it ) );
    connect( comboSize, SIGNAL( activated( const QString & ) ),
	     this, SLOT( textSize( const QString & ) ) );
    comboSize->lineEdit()->setText( QString::number( QApplication::font().pointSize() ) );
    comboSize->setFixedWidth( 38 );

    tbStyle = new QToolBar( this );
    tbStyle->setLabel( "Style Actions" );

    actionTextBold = new QAction( tr( "Bold" ),
				  Resource::loadPixmap("bold"),
				  QString::null, CTRL + Key_B,
				  this, "textBold" );
    connect( actionTextBold, SIGNAL( activated() ), this, SLOT( textBold() ) );
    actionTextBold->addTo( tbStyle );
    actionTextBold->setToggleAction( TRUE );
    actionTextItalic = new QAction( tr( "Italic" ),
				    Resource::loadPixmap("italic"),
				    tr( "&Italic" ), CTRL + Key_I,
				    this, "textItalic" );
    connect( actionTextItalic, SIGNAL( activated() ), this,
	     SLOT( textItalic() ) );
    actionTextItalic->addTo( tbStyle );
    actionTextItalic->setToggleAction( TRUE );
    actionTextUnderline = new QAction( tr( "Underline" ),
				       Resource::loadPixmap("underline"),
				       tr( "&Underline" ), CTRL + Key_U,
				       this, "textUnderline" );
    connect( actionTextUnderline, SIGNAL( activated() ),
	     this, SLOT( textUnderline() ) );
    actionTextUnderline->addTo( tbStyle );
    actionTextUnderline->setToggleAction( TRUE );

    alignMenu = new ButtonMenu( tbStyle );
    alignMenu->insertItem( Resource::loadPixmap("left"), tr("Left"), AlignLeft );
    alignMenu->insertItem( Resource::loadPixmap("center"), tr("Center"), AlignCenter );
    alignMenu->insertItem( Resource::loadPixmap("right"), tr("Right"), AlignRight );
    alignMenu->insertItem( Resource::loadPixmap("opie-write/justify"), tr("Full"), Qt3::AlignJustify );
    connect( alignMenu, SIGNAL(activated(int)), this, SLOT(textAlign(int)) );
}

Qt3::QTextEdit *MainWindow::currentEditor() const
{
    return editor;	
}

void MainWindow::doConnections( Qt3::QTextEdit *e )
{
    connect( e, SIGNAL( currentFontChanged( const QFont & ) ),
	     this, SLOT( fontChanged( const QFont & ) ) );
    connect( e, SIGNAL( currentColorChanged( const QColor & ) ),
	     this, SLOT( colorChanged( const QColor & ) ) );
    connect( e, SIGNAL( currentAlignmentChanged( int ) ),
	     this, SLOT( alignmentChanged( int ) ) );
}

void MainWindow::updateFontSizeCombo( const QFont &f )
{
    comboSize->clear();
    QFontDatabase fdb;
    QValueList<int> sizes = fdb.pointSizes( f.family() );
    QValueList<int>::Iterator it = sizes.begin();
    for ( ; it != sizes.end(); ++it )
	comboSize->insertItem( QString::number( *it ) );
}

void MainWindow::editUndo()
{
    if ( !currentEditor() )
	return;
    currentEditor()->undo();
}

void MainWindow::editRedo()
{
    if ( !currentEditor() )
	return;
    currentEditor()->redo();
}

void MainWindow::editCut()
{
    if ( !currentEditor() )
	return;
    currentEditor()->cut();
}

void MainWindow::editCopy()
{
    if ( !currentEditor() )
	return;
    currentEditor()->copy();
}

void MainWindow::editPaste()
{
    if ( !currentEditor() )
	return;
    currentEditor()->paste();
}

void MainWindow::textBold()
{
    if ( !currentEditor() )
	return;
    currentEditor()->setBold( actionTextBold->isOn() );
}

void MainWindow::textUnderline()
{
    if ( !currentEditor() )
	return;
    currentEditor()->setUnderline( actionTextUnderline->isOn() );
}

void MainWindow::textItalic()
{
    if ( !currentEditor() )
	return;
    currentEditor()->setItalic( actionTextItalic->isOn() );
}

void MainWindow::textFamily( const QString &f )
{
    if ( !currentEditor() )
	return;
    currentEditor()->setFamily( f );
    currentEditor()->viewport()->setFocus();
}

void MainWindow::textSize( const QString &p )
{
    if ( !currentEditor() )
	return;
    currentEditor()->setPointSize( p.toInt() );
    currentEditor()->viewport()->setFocus();
}

void MainWindow::textStyle( int i )
{
    if ( !currentEditor() )
	return;
    if ( i == 0 )
	currentEditor()->setParagType( Qt3::QStyleSheetItem::DisplayBlock,
				       Qt3::QStyleSheetItem::ListDisc );
    else if ( i == 1 )
	currentEditor()->setParagType( Qt3::QStyleSheetItem::DisplayListItem,
				       Qt3::QStyleSheetItem::ListDisc );
    else if ( i == 2 )
	currentEditor()->setParagType( Qt3::QStyleSheetItem::DisplayListItem,
				       Qt3::QStyleSheetItem::ListCircle );
    else if ( i == 3 )
	currentEditor()->setParagType( Qt3::QStyleSheetItem::DisplayListItem,
				       Qt3::QStyleSheetItem::ListSquare );
    else if ( i == 4 )
	currentEditor()->setParagType( Qt3::QStyleSheetItem::DisplayListItem,
				       Qt3::QStyleSheetItem::ListDecimal );
    else if ( i == 5 )
	currentEditor()->setParagType( Qt3::QStyleSheetItem::DisplayListItem,
				       Qt3::QStyleSheetItem::ListLowerAlpha );
    else if ( i == 6 )
	currentEditor()->setParagType( Qt3::QStyleSheetItem::DisplayListItem,
				       Qt3::QStyleSheetItem::ListUpperAlpha );
    currentEditor()->viewport()->setFocus();
}

void MainWindow::textAlign( int a )
{
    if ( !currentEditor() )
	return;
    editor->setAlignment( a );
}

void MainWindow::fontChanged( const QFont &f )
{
    comboFont->setCurrentItem( comboFont->listBox()->index( comboFont->listBox()->findItem( f.family() ) ) );
    updateFontSizeCombo( f );
    comboSize->lineEdit()->setText( QString::number( f.pointSize() ) );
    actionTextBold->setOn( f.bold() );
    actionTextItalic->setOn( f.italic() );
    actionTextUnderline->setOn( f.underline() );
}

void MainWindow::colorChanged( const QColor & )
{
}

void MainWindow::alignmentChanged( int a )
{
    if ( ( a == Qt3::AlignAuto ) || ( a & AlignLeft )) {
	alignMenu->setCurrentItem(AlignLeft);
    } else if ( ( a & AlignCenter ) ) {
	alignMenu->setCurrentItem(AlignCenter);
    } else if ( ( a & AlignRight ) ) {
	alignMenu->setCurrentItem(AlignRight);
    } else if ( ( a & Qt3::AlignJustify ) ) {
	alignMenu->setCurrentItem(Qt3::AlignJustify);
    }
}

void MainWindow::editorChanged( QWidget * )
{
    if ( !currentEditor() )
	return;
    fontChanged( currentEditor()->font() );
    colorChanged( currentEditor()->color() );
    alignmentChanged( currentEditor()->alignment() );
}

void MainWindow::fileOpen()
{
    save();
    editorStack->raiseWidget( fileSelector );
    fileSelector->reread();
    hideEditTools();
    fileSelector->setNewVisible( TRUE );
    clear();
    updateCaption();
}

void MainWindow::fileRevert()
{
    qDebug( "QMainWindow::fileRevert needs to be done" );
}

void MainWindow::fileNew()
{
    editor->setTextFormat( Qt::RichText );
    save();
    newFile(DocLnk());
}

void MainWindow::insertTable()
{
    qDebug( "MainWindow::insertTable() needs to be done" );
}

void MainWindow::newFile( const DocLnk &dl )
{
    DocLnk nf = dl;
    nf.setType( "text/html" );
    clear();
    editorStack->raiseWidget( editor );
    editor->viewport()->setFocus();
    doc = new DocLnk( nf );
    updateCaption();
}

void MainWindow::openFile( const DocLnk &dl )
{
    FileManager fm;
    QString txt;
    if ( !fm.loadFile( dl, txt ) )
	qDebug( "couldn't open file" );
    clear();
    editorStack->raiseWidget( editor );
    editor->viewport()->setFocus();
    doc = new DocLnk( dl );
    editor->setText( txt );
    editor->setModified( FALSE );
    updateCaption();
}

void MainWindow::showEditTools( void )
{
    tbMenu->show();
    tbEdit->show();
    tbFont->show();
    tbStyle->show();
}

void MainWindow::hideEditTools( void )
{
    // let's reset the buttons...
    actionTextBold->setOn( FALSE );
    actionTextItalic->setOn( FALSE );
    actionTextUnderline->setOn( FALSE );
    //comboFont->setCurrentText( QApplication::font().family() );
    comboSize->lineEdit()->setText( QString::number(QApplication::font().pointSize() ) );
    tbMenu->hide();
    tbEdit->hide();
    tbFont->hide();
    tbStyle->hide();
}

	
void MainWindow::save()
{
    if ( !doc )
	return;
    if ( !editor->isModified() )
	return;

    QString rt = editor->text();

    // quick hack to get around formatting...
    editor->setTextFormat( Qt::PlainText );
    QString pt = editor->text();
    editor->setTextFormat( Qt::RichText );

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

void MainWindow::clear()
{
    delete doc;
    doc = 0;
    editor->clear();
}

void MainWindow::updateCaption()
{
    if ( !doc )
	setCaption( tr("Rich Text Editor") );
    else {
	QString s = doc->name();
	if ( s.isEmpty() )
	    s = tr( "Unnamed" );
	setCaption( s + " - " + tr("Rich Text Editor") );
    }
}

void MainWindow::closeEvent( QCloseEvent *e )
{
    if ( editorStack->visibleWidget() == editor ) {
	// call fileOpen instead, don't close it
	fileOpen();
	e->ignore();
    } else {
	e->accept();
    }
}

#include "mainwindow.moc"
