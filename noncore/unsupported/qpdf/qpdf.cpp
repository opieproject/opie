//========================================================================
//
// qpdf.cc
//
// Copyright 2001 Robert Griebl
//
//========================================================================

#include "aconf.h"
#include "GString.h"
#include "PDFDoc.h"
#include "TextOutputDev.h"

#include "QPEOutputDev.h"

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/applnk.h>
#include <qpe/qcopenvelope_qws.h>


#include <qclipboard.h>
#include <qpe/qpetoolbar.h>
#include <qtoolbutton.h>
#include <qpe/qpemenubar.h>
#include <qpopupmenu.h>
#include <qwidgetstack.h>
#include <qtimer.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qmessagebox.h>

#include "qpdf.h"

#ifdef QPDF_QPE_ONLY
#include <qpe/fileselector.h>
#else
#include <opie/ofileselector.h>
#endif


int main ( int argc, char **argv ) 
{
	QPEApplication app ( argc, argv );

	// read config file
	globalParams = new GlobalParams ( "" );
	globalParams-> setErrQuiet ( true );

	QPdfDlg *dlg = new QPdfDlg ( );				
	app. showMainDocumentWidget ( dlg );

	if (( app. argc ( ) == 3 ) && ( app. argv ( ) [1] == QCString ( "-f" )))
		dlg-> openFile ( app. argv ( ) [2] );

	return app. exec ( );
}


QPdfDlg::QPdfDlg ( ) : QMainWindow ( )
{
	setCaption ( tr( "QPdf" ));
	setIcon ( Resource::loadPixmap ( "qpdf_icon" ));
	
	m_busy = false;
	
	m_doc = 0;
	m_pages = 0;
	m_zoom = 72;
	m_currentpage = 0;	

	m_fullscreen = false;
	m_renderok = false;
		
	
	setToolBarsMovable ( false );

	m_stack = new QWidgetStack ( this );
	m_stack-> setSizePolicy ( QSizePolicy ( QSizePolicy::Expanding, QSizePolicy::Expanding ));
	setCentralWidget ( m_stack );

	m_outdev = new QPEOutputDev ( m_stack );
	connect ( m_outdev, SIGNAL( selectionChanged ( const QRect & )), this, SLOT( copyToClipboard ( const QRect & )));

#ifdef QPDF_QPE_ONLY
	m_filesel = new FileSelector ( "application/pdf", m_stack, "fs", false, true );
#else
	m_filesel = new OFileSelector ( "application/pdf", m_stack, "fs", false, true );
#endif

	connect ( m_filesel, SIGNAL( closeMe ( )), this, SLOT( closeFileSelector ( )));
	connect ( m_filesel, SIGNAL( fileSelected ( const DocLnk & )), this, SLOT( openFile ( const DocLnk & )));

	m_tb_menu = new QToolBar ( this );
	m_tb_menu-> setHorizontalStretchable ( true );

	QMenuBar *mb = new QPEMenuBar ( m_tb_menu );

	m_pm_zoom = new QPopupMenu ( mb );
	m_pm_zoom-> setCheckable ( true );

	mb-> insertItem ( tr( "Zoom" ), m_pm_zoom );

	m_pm_zoom-> insertItem ( tr( "Fit to width" ), 1 );
	m_pm_zoom-> insertItem ( tr( "Fit to page" ),  2 );
	m_pm_zoom-> insertSeparator ( );
	m_pm_zoom-> insertItem ( tr( "50%" ),   50 );
	m_pm_zoom-> insertItem ( tr( "75%" ),   75 );
	m_pm_zoom-> insertItem ( tr( "100%" ), 100 );
	m_pm_zoom-> insertItem ( tr( "125%" ), 125 );
	m_pm_zoom-> insertItem ( tr( "150%" ), 150 );
	m_pm_zoom-> insertItem ( tr( "200%" ), 200 );
	
	connect ( m_pm_zoom, SIGNAL( activated ( int )), this, SLOT( setZoom ( int )));

	m_tb_tool = new QPEToolBar ( this );
		
	new QToolButton ( Resource::loadIconSet ( "fileopen" ),    tr( "Open..." ),       QString::null, this, SLOT( openFile ( )),  m_tb_tool, "open" );
	m_tb_tool-> addSeparator ( );
	m_to_find = new QToolButton ( Resource::loadIconSet ( "find" ),        tr( "Find..." ),       QString::null, this, SLOT( toggleFindBar ( )),     m_tb_tool, "find" );
	m_to_find-> setToggleButton ( true );
	m_tb_tool-> addSeparator ( );
	m_to_full = new QToolButton ( Resource::loadIconSet ( "fullscreen" ),  tr( "Fullscreen" ),    QString::null, this, SLOT( toggleFullscreen ( )),  m_tb_tool, "fullscreen" );
	m_to_full-> setToggleButton ( true );
	m_tb_tool-> addSeparator ( );
	new QToolButton ( Resource::loadIconSet ( "fastback" ),    tr( "First page" ),    QString::null, this, SLOT( firstPage ( )), m_tb_tool, "first" );
	new QToolButton ( Resource::loadIconSet ( "back" ),        tr( "Previous page" ), QString::null, this, SLOT( prevPage ( )),  m_tb_tool, "prev" );
	new QToolButton ( Resource::loadIconSet ( "down" ),        tr( "Goto page..." ),  QString::null, this, SLOT( gotoPageDialog ( )),  m_tb_tool, "goto" );
	new QToolButton ( Resource::loadIconSet ( "forward" ),     tr( "Next page" ),     QString::null, this, SLOT( nextPage ( )),  m_tb_tool, "next" );
	new QToolButton ( Resource::loadIconSet ( "fastforward" ), tr( "Last page" ),     QString::null, this, SLOT( lastPage ( )),  m_tb_tool, "last" );

	m_tb_find = new QPEToolBar ( this );
	addToolBar ( m_tb_find, "Search", QMainWindow::Top, true );
	m_tb_find-> setHorizontalStretchable ( true );
	m_tb_find-> hide ( );

	m_findedit = new QLineEdit ( m_tb_find, "findedit" );
	m_tb_find-> setStretchableWidget ( m_findedit );
	connect ( m_findedit, SIGNAL( textChanged ( const QString & )), this, SLOT( findText ( const QString & )));

	new QToolButton ( Resource::loadIconSet ( "next" ),        tr( "Next" ),          QString::null, this, SLOT( findText ( )),  m_tb_find, "findnext" );

	openFile ( );
}

QPdfDlg::~QPdfDlg ( )
{
	delete m_doc;
}

// vv Fullscreen handling (for broken QT-lib) [David Hedbor, www.eongames.com]

void QPdfDlg::resizeEvent ( QResizeEvent * )
{
	if ( m_fullscreen && ( size ( ) != qApp-> desktop ( )-> size ( ))) 
		setFullscreen ( true );
}

void QPdfDlg::focusInEvent ( QFocusEvent * )
{
	if ( m_fullscreen )
		setFullscreen ( true );
}

void QPdfDlg::toggleFullscreen ( )
{
	if ( m_to_full-> isOn ( ) == m_fullscreen )
		m_to_full-> setOn ( !m_fullscreen );

	m_fullscreen = !m_fullscreen;
	setFullscreen ( m_fullscreen );
}

void QPdfDlg::setFullscreen ( bool b )
{
	static QSize normalsize;

	if ( b ) {
		if ( !normalsize. isValid ( )) 
			normalsize = size ( );
		
		setFixedSize ( qApp-> desktop ( )-> size ( ));
		showNormal ( );
		reparent ( 0, WStyle_Customize | WStyle_NoBorder, QPoint ( 0, 0 ));
		showFullScreen ( );
	}
	else {
		showNormal ( );
		reparent ( 0, 0, QPoint ( 0, 0 ));
		resize ( normalsize );
		showMaximized ( );
		normalsize = QSize ( );
	}	
}

// ^^ Fullscreen handling (for broken QT-lib)

void QPdfDlg::setBusy ( bool b )
{
	if ( b != m_busy ) {
		m_busy = b;
		
		m_outdev-> setBusy ( m_busy );
		setEnabled ( !m_busy );
	}
}

bool QPdfDlg::busy ( ) const
{
	return m_busy;
}


void QPdfDlg::updateCaption ( )
{
	QString cap = "";
	
	if ( !m_currentdoc. isEmpty ( )) 
		cap = QString ( "%1 - " ). arg ( m_currentdoc );
	cap += "QPdf";
	
	setCaption ( cap );
}


void QPdfDlg::setZoom ( int id )
{
	int dpi = 0;

	switch ( id ) {
	case 1:
		if ( m_doc && m_doc-> isOk ( )) 
			dpi = m_outdev-> visibleWidth ( ) * 72 / m_doc-> getPageWidth ( m_currentpage );
		break;
	
	case 2:
		if ( m_doc && m_doc-> isOk ( )) 
			dpi = QMIN( m_outdev-> visibleWidth ( ) * 72 / m_doc-> getPageWidth ( m_currentpage ), \
			            m_outdev-> visibleHeight ( ) * 72 / m_doc-> getPageHeight ( m_currentpage ));
		break;
		
	default:
		dpi = id * 72 / 100;
		break;
	}
	
	if ( dpi < 18 )
		dpi = 18;
	if ( dpi > 216 )
		dpi = 216;

	for ( uint i = 0; i < m_pm_zoom-> count ( ); i++ ) {
		int xid = m_pm_zoom-> idAt ( i );
		m_pm_zoom-> setItemChecked ( xid, xid == id );
	}

	if ( dpi != m_zoom ) { 
		m_zoom = dpi;

		renderPage ( );
	}
}


void QPdfDlg::gotoPageDialog ( )
{
	QDialog *d = new QDialog ( this, "gotodlg", true );
	d-> setCaption ( tr( "Goto page" ));
	
	QBoxLayout *lay = new QVBoxLayout ( d, 4, 4  );	
	
	QLabel *l = new QLabel ( tr( "Select from 1 .. %1:" ). arg ( m_pages ), d );
	lay-> addWidget ( l );
	
	QSpinBox *spin = new QSpinBox ( 1, m_pages, 1, d );
	spin-> setValue ( m_currentpage );
	spin-> setWrapping ( true );
	spin-> setButtonSymbols ( QSpinBox::PlusMinus );
	lay-> addWidget ( spin );
	
	if ( d-> exec ( ) == QDialog::Accepted ) {
		gotoPage ( spin-> value ( ));
	}
	
	delete d;
}

void QPdfDlg::toggleFindBar ( )
{
	if ( m_to_find-> isOn ( ) == m_tb_find-> isVisible ( ))
		m_to_find-> setOn ( !m_tb_find-> isVisible ( ));

	if ( m_tb_find-> isVisible ( )) {
		m_tb_find-> hide ( );
		m_outdev-> setFocus ( );
	}
	else {
		m_tb_find-> show ( );
		m_findedit-> setFocus ( );
	}
}

void QPdfDlg::findText ( const QString &str )
{
	if ( !m_doc || !m_doc-> isOk ( ) || str. isEmpty ( )) 
		return;

	TextOutputDev *textOut = 0;
	int pg = 0;

	setBusy ( true );
		
	int len = str. length ( );
	Unicode *u = new Unicode [len];
	for ( int i = 0; i < len; i++ )
		u [i] = str [i]. unicode ( );
		
	int xMin = 0, yMin = 0, xMax = 0, yMax = 0;
	QRect selr = m_outdev-> selection ( );
	bool fromtop = true; 
	
	if ( selr. isValid ( )) {
		xMin = selr. right ( );
		yMin = selr. top ( ) + selr. height ( ) / 2;
		fromtop = false;
	}

	if ( m_outdev-> findText ( u, len, fromtop, true, &xMin, &yMin, &xMax, &yMax )) 
		goto found;

	qApp-> processEvents ( );
		
	// search following pages
	textOut = new TextOutputDev ( 0, gFalse, gFalse );
	if ( !textOut-> isOk ( )) 
		goto done;
	
	qApp-> processEvents ( );
		
	for ( pg = ( m_currentpage % m_pages ) + 1; pg != m_currentpage; pg = ( pg % m_pages ) + 1 ) {
		m_doc-> displayPage ( textOut, pg, 72, 0, gFalse );
		
		fp_t xMin1, yMin1, xMax1, yMax1;
		
		qApp-> processEvents ( );
		
		if ( textOut-> findText ( u, len, gTrue, gTrue, &xMin1, &yMin1, &xMax1, &yMax1 ))
			goto foundPage;
			
		qApp-> processEvents ( );	
	}

	// search current page ending at current selection
	if ( selr. isValid ( )) {
		xMax = selr. left ( );
		yMax = selr. top ( ) + selr. height ( ) / 2;

		if ( m_outdev-> findText ( u, len, gTrue, gFalse, &xMin, &yMin, &xMax, &yMax )) 
			goto found;
	}
	
	// not found
	QMessageBox::information ( this, tr( "Find..." ), tr( "'%1' could not be found." ). arg ( str ));
	goto done;

foundPage:
	qApp-> processEvents ( );

	gotoPage ( pg );
	
	if ( !m_outdev-> findText ( u, len, gTrue, gTrue, &xMin, &yMin, &xMax, &yMax )) {
		// this can happen if coalescing is bad
		goto done;
	}

found:
	selr. setCoords ( xMin, yMin, xMax, yMax );
	m_outdev-> setSelection ( selr, true ); // this will emit QPEOutputDev::selectionChanged ( ) -> copyToClipboard ( )

done:			

	delete [] u; 
	delete textOut;

	setBusy ( false );
}
		

void QPdfDlg::findText ( )
{
	findText ( m_findedit-> text ( ));
}

void QPdfDlg::copyToClipboard ( const QRect &r )
{
	if ( m_doc && m_doc-> isOk ( ) && m_doc-> okToCopy ( ))
		qApp-> clipboard ( )-> setText ( m_outdev-> getText ( r ));
}

void QPdfDlg::firstPage ( )
{
	gotoPage ( 1 );
}

void QPdfDlg::prevPage ( )
{
	gotoPage ( m_currentpage - 1 );
}

void QPdfDlg::nextPage ( )
{
	gotoPage ( m_currentpage + 1 );
}

void QPdfDlg::lastPage ( )
{
	gotoPage ( m_pages );
}

void QPdfDlg::gotoPage ( int n )
{
	if ( n < 1 )
		n = 1;
	if ( n > m_pages )
		n = m_pages;
		
	if ( n != m_currentpage ) {
		m_currentpage = n;	
		
		renderPage ( );
	}
}

void QPdfDlg::renderPage ( )
{
	if ( m_renderok && m_doc && m_doc-> isOk ( )) {
		m_renderok = false;
	
		setBusy ( true );
		m_doc-> displayPage ( m_outdev, m_currentpage, m_zoom, 0, true ); 
		setBusy ( false );
		
		m_outdev-> setPageCount ( m_currentpage, m_pages );
		
		m_renderok = true;
	}
}

void QPdfDlg::closeFileSelector ( )
{
	m_tb_menu-> show ( );
	m_tb_tool-> show ( );
	m_stack-> raiseWidget ( m_outdev );
}

void QPdfDlg::openFile ( )
{
	m_tb_menu-> hide ( );
	m_tb_tool-> hide ( );
	m_tb_find-> hide ( );
	m_stack-> raiseWidget ( m_filesel );
}

void QPdfDlg::openFile ( const QString &f )
{
	DocLnk nf;
	nf. setType ( "application/pdf" );
	nf. setFile ( f );
	QFileInfo fi ( f );
	nf. setName ( fi. baseName ( ));
	openFile ( nf );
}

void QPdfDlg::openFile ( const DocLnk &f )
{
	QString fn = f. file ( );
	QFileInfo fi ( fn );
	
	if ( fi. exists ( )) {
		delete m_doc;

		m_doc = new PDFDoc ( new GString ( fn. local8Bit ( )), 0, 0 );

		if ( m_doc-> isOk ( )) {
			m_currentdoc = f. name ( );
			int sep = m_currentdoc. findRev ( '/' );
			if ( sep > 0 )
				m_currentdoc = m_currentdoc. mid ( sep + 1 );

			m_pages = m_doc-> getNumPages ( );
			m_currentpage = 0;
		
			QTimer::singleShot ( 0, this, SLOT( delayedInit ( )));
		}
		else {
			delete m_doc;
			m_doc = 0;
			
			m_currentdoc = QString::null;
		}
	
		updateCaption ( );
	}
	else
		QMessageBox::warning ( this, tr( "Error" ), tr( "File does not exist !" ));
}

void QPdfDlg::setDocument ( const QString &f )
{
	if ( f. find ( ".desktop", 0, true ) == -1 ) 
		openFile ( f );
	else 
		openFile ( DocLnk ( f ));
		
	closeFileSelector ( );
}

void QPdfDlg::delayedInit ( )
{	
	closeFileSelector ( );
	
	m_currentpage = 0;
	m_zoom = 0;
	m_renderok = false;
	
	setZoom ( 100 );
	gotoPage ( 1 );
	
	m_renderok = true;
	
	renderPage ( );
	
	m_outdev-> setFocus ( );
}

