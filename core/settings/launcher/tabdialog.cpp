/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:       
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/applnk.h>

#include <qlayout.h>
#include <qvbox.h>
#include <qtabbar.h>
#include <qiconview.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>

#include <opie/ofontselector.h>
#include <opie/otabwidget.h>
#include <opie/ocolorbutton.h>
#include <opie/ofiledialog.h>

#include "tabdialog.h"


class SampleItem : public QIconViewItem {
public:
	SampleItem ( QIconView *v, const QString &text, const QPixmap &pix ) : QIconViewItem ( v, text )
	{	
		m_large = pix;
		m_small. convertFromImage ( pix. convertToImage ( ). smoothScale ( pix. width ( ) / 2, pix. height ( ) / 2 ));
	}
	
	void sizeChange ( )
	{
		calcRect ( );
		repaint ( );
	}
	
	QPixmap *pixmap ( ) const 
	{
		if ( iconView ( )-> itemTextPos ( ) == QIconView::Right ) 
			return (QPixmap *) &m_small;
		else
			return (QPixmap *) &m_large;
	}
	
private:
	QPixmap m_large, m_small;
};

class SampleView : public QIconView {
public:
	SampleView ( QWidget *parent = 0, const char *name = 0 ) : QIconView ( parent, name )
	{
		setItemsMovable ( false );
		setAutoArrange ( true );
		setSorting ( true );
		setFrameStyle ( QFrame::NoFrame );
		setSpacing ( 4 );
		setMargin ( 0 );
		setSelectionMode ( QIconView::NoSelection );
		setBackgroundMode ( PaletteBase );
		setViewMode ( TabConfig::Icon );
		calculateGrid ( Bottom );


		new SampleItem ( this, tr( "Sample 1" ), Resource::loadPixmap ( "DateBook" ));
		new SampleItem ( this, tr( "Sample 2" ), Resource::loadPixmap ( "Calibrate" ));
		new SampleItem ( this, tr( "Sample 3" ), Resource::loadPixmap ( "UnknownDocument" ));
		
		setBackgroundType ( TabConfig::Ruled, QString::null );
		
		setMaximumHeight ( firstItem ( )-> height ( ) + 16 );
	}           

	void setViewMode ( TabConfig::ViewMode m )
	{
		viewport ( )-> setUpdatesEnabled ( false );
		
		switch ( m ) {
			case TabConfig::List:				
				setItemTextPos( QIconView::Right );
				break;
			case TabConfig::Icon:
				setItemTextPos( QIconView::Bottom );
				break;
		}
//		hideOrShowItems ( false );

		for ( QIconViewItem *it = firstItem ( ); it; it = it-> nextItem ( ))
			((SampleItem *) it )-> sizeChange ( );
		arrangeItemsInGrid ( true );
		viewport ( )-> setUpdatesEnabled ( true );
		update ( );
	} 


	void setBackgroundType( TabConfig::BackgroundType t, const QString &val )
	{
		switch ( t ) {
			case TabConfig::Ruled: {
				QPixmap bg ( width ( ), 9 );
				QPainter painter ( &bg );
				for ( int i = 0; i < 3; i++ ) {
					painter. setPen ( white );
					painter. drawLine ( 0, i*3, width()-1, i*3 );
					painter. drawLine ( 0, i*3+1, width()-1, i*3+1 );
					painter. setPen ( colorGroup().background().light(105) );
					painter. drawLine ( 0, i*3+2, width()-1, i*3+2 );
				}
				painter.end ( );
				setBackgroundPixmap ( bg );
				break;
			}

			case TabConfig::SolidColor: {
				setBackgroundPixmap ( QPixmap ( ));
				if ( val. isEmpty ( ))
					setBackgroundColor ( colorGroup ( ). base ( ));
				else
					setBackgroundColor ( val );
				break;
			}

			case TabConfig::Image: {
				qDebug( "Loading image: %s", val.latin1() );
				QPixmap bg ( Resource::loadPixmap ( "wallpaper/" + val ));
				if ( bg. isNull ( )) {
					QImageIO imgio;
					imgio. setFileName ( val );
					QSize ds = qApp-> desktop ( )-> size ( );
					QString param ( "Scale( %1, %2, ScaleMin )" ); // No tr
					imgio. setParameters ( param. arg ( ds. width ( )). arg ( ds. height ( )). latin1 ( ));
					imgio. read ( );
					bg = imgio. image ( );
				}
				setBackgroundPixmap ( bg );
				break;
			}
		}
		m_bgtype = t;
		viewport ( )-> update ( );
	}

	void setTextColor ( const QColor &tc )
	{
		m_textcolor = tc;
		QColorGroup cg = colorGroup ( );
		cg. setColor ( QColorGroup::Text, tc );
		setPalette ( QPalette ( cg, cg, cg ));
		viewport ( )-> update ( );
	}

	void setViewFont ( const QFont &f )
	{
		setFont ( f );
	}
	
	void setItemTextPos ( ItemTextPos pos )
	{
		calculateGrid ( pos );
		QIconView::setItemTextPos( pos );
	}
	                    
	void calculateGrid ( ItemTextPos pos )
	{
		int dw = QApplication::desktop ( )-> width ( );
		int viewerWidth = dw - style ( ).scrollBarExtent ( ). width ( );
		if ( pos == Bottom ) {
			int cols = 3;
			if ( viewerWidth <= 200 )
				cols = 2;
			else if ( viewerWidth >= 400 )
				cols = viewerWidth/96;
			setSpacing ( 4 );
			setGridX (( viewerWidth - ( cols + 1 ) * spacing ( )) / cols );
			setGridY ( fontMetrics ( ). height ( ) * 2 + 24 );
		} 
		else {
			int cols = 2;
			if ( viewerWidth < 150 )
				cols = 1;
			else if ( viewerWidth >= 400 )
				cols = viewerWidth / 150;
			setSpacing ( 2 );
			setGridX (( viewerWidth - ( cols + 1 ) * spacing ( )) / cols );
			setGridY ( fontMetrics ( ). height ( ) + 2 );
		}
	}
                                                                                                                                                                  
	void paletteChange( const QPalette &p )
	{
		static bool excllock = false;
		
		if ( excllock )
			return;
		excllock = true;
	
		unsetPalette ( );
		QIconView::paletteChange ( p );
		if ( m_bgtype == TabConfig::Ruled )
			setBackgroundType ( TabConfig::Ruled, QString::null );
		QColorGroup cg = colorGroup ( );
		cg.setColor ( QColorGroup::Text, m_textcolor );
		setPalette ( QPalette ( cg, cg, cg ));
		
		excllock = false;
	}
                            
	void setBackgroundPixmap ( const QPixmap &pm ) 
	{
		m_bgpix = pm;
	}
	
	void setBackgroundColor ( const QColor &c ) 
	{
		m_bgcolor = c;
	}
	
	void drawBackground ( QPainter *p, const QRect &r )
	{
		if ( !m_bgpix. isNull ( )) {
			p-> drawTiledPixmap ( r, m_bgpix, QPoint (( r. x ( ) + contentsX ( )) % m_bgpix. width ( ), 
 			                                          ( r. y ( ) + contentsY ( )) % m_bgpix. height ( )));
		} 
		else
			p-> fillRect ( r, m_bgcolor );
	}
	
private:
	QColor m_textcolor;
	QColor m_bgcolor;
	QPixmap m_bgpix;
	TabConfig::BackgroundType m_bgtype;
};



TabDialog::TabDialog ( const QPixmap *tabicon, const QString &tabname, TabConfig &tc, QWidget *parent, const char *dname, bool modal, WFlags fl ) 
	: QDialog ( parent, dname, modal, fl | WStyle_ContextHelp ), m_tc ( tc )
{
	setCaption ( tr( "Edit Tab" ));
	
	QVBoxLayout *lay = new QVBoxLayout ( this, 3, 3 );
	
	OTabWidget *tw = new OTabWidget ( this, "tabwidget", OTabWidget::Global, OTabWidget::Bottom );
	QWidget *bgtab;
	       
	tw-> addTab ( bgtab = createBgTab ( tw ), "appearance/backgroundtabicon.png", tr( "Background" ));
	tw-> addTab ( createFontTab ( tw ), "appearance/fonttabicon.png", tr( "Font" ));
	tw-> addTab ( createIconTab ( tw ), "appearance/colorstabicon.png", tr( "Icons" ) );	                           
	
	tw-> setCurrentTab ( bgtab );                                       
	
	QWidget *sample = new QVBox ( this );	
	QTabBar *tb = new QTabBar ( sample );
	QString name ( tr( "Previewing %1" ). arg ( tabname ));
	
	tb-> addTab ( tabicon ? new QTab ( *tabicon, name ) : new QTab ( name ));
	
	m_sample = new SampleView ( sample );
			
	lay-> addWidget ( tw, 10 );
	lay-> addWidget ( sample, 1 );
	
	m_iconsize-> setButton ( tc. m_view );
	iconSizeClicked ( tc. m_view );
	m_iconcolor-> setColor ( QColor ( m_tc. m_text_color ));
	iconColorClicked ( m_iconcolor-> color ( ));
	m_bgtype-> setButton ( tc. m_bg_type );
	m_solidcolor-> setColor ( QColor ( tc. m_bg_color ));
	m_bgimage = tc. m_bg_image;
	bgTypeClicked ( tc. m_bg_type );
	m_fontuse-> setChecked ( tc. m_font_use );
	m_fontselect-> setSelectedFont ( QFont ( tc. m_font_family, tc. m_font_size, tc. m_font_weight, tc. m_font_italic ));
	m_fontselect-> setEnabled ( m_fontuse-> isChecked ( ));
	fontClicked ( m_fontselect-> selectedFont ( ));

	QWhatsThis::add ( sample, tr( "This is a rough preview of what the currently selected Tab will look like." ));
}


TabDialog::~TabDialog ( )
{
}

QWidget *TabDialog::createFontTab ( QWidget *parent )
{
    QWidget *tab = new QWidget ( parent, "FontTab" );
    QVBoxLayout *vertLayout = new QVBoxLayout ( tab, 3, 3 );

	m_fontuse = new QCheckBox ( tr( "Use a custom font" ), tab );
	vertLayout-> addWidget ( m_fontuse );

    m_fontselect = new OFontSelector ( false, tab, "fontsel" );    
    vertLayout-> addWidget ( m_fontselect );
    
	connect ( m_fontuse, SIGNAL( toggled ( bool )), m_fontselect, SLOT( setEnabled ( bool )));
    connect( m_fontselect, SIGNAL( fontSelected ( const QFont & )),
             this, SLOT( fontClicked ( const QFont & )));

    return tab; 
}

QWidget *TabDialog::createBgTab ( QWidget *parent )
{
    QWidget *tab = new QWidget( parent, "BgTab" );
    QVBoxLayout *vertLayout = new QVBoxLayout( tab, 3, 3 );
        
    QGridLayout* gridLayout = new QGridLayout ( vertLayout );
    gridLayout-> setColStretch ( 1, 10 );

    QLabel* label = new QLabel( tr( "Type:" ), tab );
    gridLayout-> addWidget ( label, 0, 0 );
    m_bgtype = new QButtonGroup( tab, "buttongroup" );
    m_bgtype-> hide ( );
    m_bgtype-> setExclusive ( true );

	QRadioButton *rb;
    rb = new QRadioButton( tr( "Ruled" ), tab, "ruled" );
    m_bgtype-> insert ( rb, TabConfig::Ruled );
    gridLayout-> addWidget( rb, 0, 1 );

	QHBoxLayout *hb = new QHBoxLayout ( );
	hb-> setSpacing ( 3 );

    rb = new QRadioButton( tr( "Solid color" ), tab, "solid" );
    m_bgtype-> insert ( rb, TabConfig::SolidColor ); 
    hb-> addWidget ( rb );
	hb-> addSpacing ( 10 );
    
	m_solidcolor = new OColorButton ( tab );
	connect ( m_solidcolor, SIGNAL( colorSelected ( const QColor & )), this, SLOT( bgColorClicked ( const QColor & )));
	hb-> addWidget ( m_solidcolor );
	hb-> addStretch ( 10 );

    gridLayout-> addLayout ( hb, 1, 1 );

	hb = new QHBoxLayout ( );
	hb-> setSpacing ( 3 );
	
	rb = new QRadioButton( tr( "Image" ), tab, "image" );
    m_bgtype-> insert ( rb, TabConfig::Image );
    hb-> addWidget( rb );
    hb-> addSpacing ( 10 );
    
	m_imagebrowse = new QPushButton ( tr( "Select..." ), tab );
	connect ( m_imagebrowse, SIGNAL( clicked ( )), this, SLOT( bgImageClicked ( )));
	hb-> addWidget ( m_imagebrowse );
	hb-> addStretch ( 10 );
	
    gridLayout-> addLayout ( hb, 2, 1 );

	QPushButton *p = new QPushButton ( tr( "Default" ), tab );	
	connect ( p, SIGNAL( clicked ( )), this, SLOT( bgDefaultClicked ( )));
	gridLayout-> addWidget ( p, 3, 1 );

	connect ( m_bgtype, SIGNAL( clicked ( int )), this, SLOT( bgTypeClicked ( int )));

	vertLayout-> addStretch ( 10 );

	return tab;
}

QWidget *TabDialog::createIconTab ( QWidget *parent )
{
    QWidget *tab = new QWidget( parent, "IconTab" );
    QVBoxLayout *vertLayout = new QVBoxLayout( tab, 3, 3 );
        
    QGridLayout* gridLayout = new QGridLayout ( vertLayout );
    gridLayout-> setColStretch ( 1, 10 );

    QLabel* label = new QLabel( tr( "Size:" ), tab );
    gridLayout-> addWidget ( label, 0, 0 );
    m_iconsize = new QButtonGroup( tab, "buttongroup" );
    m_iconsize-> hide ( );
    m_iconsize-> setExclusive ( true );

	QRadioButton *rb;
    rb = new QRadioButton( tr( "Small" ), tab, "iconsmall" );
    m_iconsize-> insert ( rb, TabConfig::List );
    gridLayout-> addWidget( rb, 0, 1 );
    
    rb = new QRadioButton( tr( "Large" ), tab, "iconlarge" );
    m_iconsize-> insert ( rb, TabConfig::Icon );
    gridLayout-> addWidget( rb, 1, 1 );

	connect ( m_iconsize, SIGNAL( clicked ( int )), this, SLOT( iconSizeClicked ( int )));

//	vertLayout-> addSpacing ( 8 );

//	gridLayout = new QGridLayout ( vertLayout );
	gridLayout-> addRowSpacing ( 2, 8 );
	
	label = new QLabel ( tr( "Color:" ), tab );
	gridLayout-> addWidget ( label, 3, 0 );
	
	m_iconcolor = new OColorButton ( tab );
	connect ( m_iconcolor, SIGNAL( colorSelected ( const QColor & )), this, SLOT( iconColorClicked ( const QColor & )));
	gridLayout-> addWidget ( m_iconcolor, 3, 1, AlignLeft );        	

	vertLayout-> addStretch ( 10 );

	return tab;
}


void TabDialog::iconSizeClicked ( int s )
{
	m_sample-> setViewMode ((TabConfig::ViewMode) s );
}

void TabDialog::fontClicked ( const QFont &f )
{
	m_sample-> setViewFont ( f );
}

void TabDialog::bgTypeClicked ( int t )
{
	QString s;

	if ( m_bgtype-> id ( m_bgtype-> selected ( )) != t )
		m_bgtype-> setButton ( t );
		
	m_solidcolor-> setEnabled ( t == TabConfig::SolidColor );
	m_imagebrowse-> setEnabled ( t == TabConfig::Image );

	if ( t == TabConfig::SolidColor )
		s = m_solidcolor-> color ( ). name ( );
	else if ( t == TabConfig::Image )
		s = Resource::findPixmap ( m_bgimage );
		
	m_sample-> setBackgroundType ((TabConfig::BackgroundType) t, s );
}

void TabDialog::bgColorClicked ( const QColor & )
{
	bgTypeClicked ( TabConfig::SolidColor );
}

void TabDialog::iconColorClicked ( const QColor &col )
{
	m_sample-> setTextColor ( col );
}

void TabDialog::bgImageClicked ( )
{
	// ### use OFileSelector here ###
	// this is just a quick c&p from the old appearance app

	MimeTypes types;
	QStringList list;
	list << "image/*";	
	types. insert ( "Images", list );
	
	QString file = OFileDialog::getOpenFileName ( 1, "/", QString::null, types );
	if ( !file. isEmpty ( )) {
		m_bgimage = DocLnk ( file ). file ( );
		bgTypeClicked ( TabConfig::Image );
	}                                                                    
}

void TabDialog::bgDefaultClicked ( )
{
	m_bgimage = "launcher/opie-background";
	bgTypeClicked ( TabConfig::Image );
}

void TabDialog::accept ( )
{
	m_tc. m_view = (TabConfig::ViewMode) m_iconsize-> id ( m_iconsize-> selected ( ));
	m_tc. m_bg_type = (TabConfig::BackgroundType) m_bgtype-> id ( m_bgtype-> selected ( ));
	m_tc. m_bg_color = m_solidcolor-> color ( ). name ( );
	m_tc. m_bg_image = m_bgimage;
	m_tc. m_text_color = m_iconcolor-> color ( ). name ( );
		
	m_tc. m_font_use = m_fontuse-> isChecked ( );

	if ( m_tc. m_font_use ) {	
		QFont f = m_fontselect-> selectedFont ( );
	
		m_tc. m_font_family = f. family ( );
		m_tc. m_font_size = f. pointSize ( );
		m_tc. m_font_weight = f. weight ( );
		m_tc. m_font_italic = f. italic ( );
	}
	
	QDialog::accept ( );
}
