// opie-console includes
#include "emulation_widget.h"
#include "common.h"
#include "widget_layer.h"
#include "profile.h"

// qt includes
#include <qwidget.h>
#include <qarray.h>
#include <qstring.h>
#include <qpainter.h>
#include <qrect.h>
#include <qscrollbar.h>

#define rimX 0 // left/right rim width
#define rimY 0 // top/bottom rim high

#define SCRWIDTH 16 // width of scrollbar

EmulationWidget::EmulationWidget( const Profile& config, QWidget *parent, const char* name ) : WidgetLayer( config, parent, name )
{

	// initialize font attributes 
	QFontMetrics fm( font() );
	f_height = fm.height();
	f_width = fm.maxWidth();
	f_ascent = fm.ascent();

	// initialize scrollbar related vars
	m_scrollbar = new QScrollBar( this );
	m_scrollbar->setCursor( arrowCursor );

	// give reasonable defaults to m_columns, m_lines
	calcGeometry();
	
	// load config
	reloadConfig( config );
	
	m_resizing = false;
}

void EmulationWidget::reloadConfig( const Profile& config )
{
  
  // nothing yet
}

EmulationWidget::~EmulationWidget()
{
  //clean up
}

static QChar vt100extended(QChar c)
{
  switch (c.unicode())
  {
    case 0x25c6 : return  1;
    case 0x2592 : return  2;
    case 0x2409 : return  3;
    case 0x240c : return  4;
    case 0x240d : return  5;
    case 0x240a : return  6;
    case 0x00b0 : return  7;
    case 0x00b1 : return  8;
    case 0x2424 : return  9;
    case 0x240b : return 10;
    case 0x2518 : return 11;
    case 0x2510 : return 12;
    case 0x250c : return 13;
    case 0x2514 : return 14;
    case 0x253c : return 15;
    case 0xf800 : return 16;
    case 0xf801 : return 17;
    case 0x2500 : return 18;
    case 0xf803 : return 19;
    case 0xf804 : return 20;
    case 0x251c : return 21;
    case 0x2524 : return 22;
    case 0x2534 : return 23;
    case 0x252c : return 24;
    case 0x2502 : return 25;
    case 0x2264 : return 26;
    case 0x2265 : return 27;
    case 0x03c0 : return 28;
    case 0x2260 : return 29;
    case 0x00a3 : return 30;
    case 0x00b7 : return 31;
  }
  return c;
}


QSize EmulationWidget::calcSize( int cols, int lins ) const
{
    int frw = width() - contentsRect().width();
    int frh = height() - contentsRect().height();
    int scw = (scrollLoc == SCRNONE? 0 : m_scrollbar->width() );
    return QSize( f_width * cols + 2 * rimX + frw + scw, f_height * lins + 2 * rimY + frh );
}

void EmulationWidget::setImage( QArray<Character> const newimg, int lines, int columns )
{
	const QPixmap* pm = backgroundPixmap();
	QPainter paint;

	// dont allow updates, while drawing
	setUpdatesEnabled( false );

	paint.begin( this );
	
	QPoint tL = contentsRect().topLeft();
	int tLx = tL.x();
	int tLy = tL.y();
//	hasBlinker = false;

	int cf = -1;
	int cb = -1;
	int cr = -1;

	int lins = QMIN( m_lines, QMAX( 0, lines ) );
	int cols = QMIN( m_columns, QMAX( 0, columns ) );
	QArray<QChar> disstrU = QArray<QChar>( cols );	

	for ( int y = 0; y < lins; ++y )
	{	int len;
		const Character* lcl = &m_image[y * m_columns];
		const Character* ext = &newimg[y * m_columns];
		if ( ! m_resizing )
		for ( int x = 0; x < cols; ++x )
		{
			// disable, till widget works, WITHOUT blinking
			//hasBlinker |= ( ext[x].r & RE_BLINK );
			
			if ( ext[x] != lcl[x] )
			{
				cr = ext[x].r;
				cb = ext[x].b;
				if ( ext[x].f != cf ) cf = ext[x].f;
				int lln = cols - x;
				disstrU[0] = vt100extended( ext[x+0].c );
				for ( len = 1; len < lln; ++len )
				{
					if ( ext[x+len].f != cf || ext[x+len].b != cb || ext[x+len].r != cr || ext[x+len] == lcl[x+len] )
						break;
					disstrU[len] = vt100extended( ext[x+len].c );
				}
				QString unistr( disstrU, len );
				drawAttrString( unistr, paint, QRect( m_blX+tLx+f_width*x, m_bY+tLy+f_height*y, f_width*len, f_height ), ext[x], pm != NULL, true );
				x += len -1;
			}
		}
		// make image become newimg
		memcpy( (void*) lcl, (const void*) ext, cols*sizeof( Character ) );
	}
	drawFrame( &paint );
	paint.end();
	setUpdatesEnabled( true );

	/*if ( hasBlinker && !blinkT->isActive() )
		blinkT->start(1000); //ms
	if ( ! hasBlinker && blinkT->isActive() )
	{
		blinkT->stop();
		blinking = false;
	}*/

	delete [] disstrU;
}

void EmulationWidget::calcGeometry()
{
	m_scrollbar->resize(QApplication::style().scrollBarExtent().width(), contentsRect().height() );

	switch( scrollLoc )
	{
	case SCRNONE :
		m_columns = ( contentsRect().width() -2 * rimX ) / f_width;
		m_blX = ( contentsRect().width() - ( m_columns*f_width ) ) / 2;
		m_brX = m_blX;
		m_scrollbar->hide();
		break;
	case SCRLEFT :
		m_columns = ( contentsRect().width() - 2 * rimX - m_scrollbar->width() ) / f_width;
		m_brX = ( contentsRect().width() - ( m_columns*f_width ) - m_scrollbar->width() ) / 2;
		m_blX = m_brX + m_scrollbar->width();
		m_scrollbar->move( contentsRect().topLeft() );
		m_scrollbar->show();
		break;
	case SCRIGHT:
		m_columns = ( contentsRect().width()  - 2 * rimX - m_scrollbar->width() ) / f_width;
		m_blX = ( contentsRect().width() - ( m_columns*f_width ) - m_scrollbar->width() ) / 2;
		m_brX = m_blX;
		m_scrollbar->move( contentsRect().topRight() - QPoint (m_scrollbar->width()-1,0 ) );
		m_scrollbar->show();
		break;
	}
	
	m_lines = ( contentsRect().height() - 2 * rimY ) / f_height;
	m_bY = ( contentsRect().height() - (m_lines * f_height ) ) / 2;
}

void EmulationWidget::drawAttrString( QString& string, QPainter &painter, QRect rect, Character attr, bool usePixmap, bool clear )    
{
    if ( usePixmap && color_table[attr.b].transparent )
    {
	painter.setBackgroundMode( TransparentMode );
	if ( clear )
	    erase( rect );
    }
    else
    {
	if ( blinking )
	    painter.fillRect( rect, color_table[attr.b].color );
	else
	{
	    painter.setBackgroundMode( OpaqueMode );
	    painter.setBackgroundColor( color_table[attr.b].color );
	}
    }
    if ( color_table[attr.f].bold )
	painter.setPen( QColor( 0x8F, 0x00, 0x00 ) );
    else
	painter.setPen( color_table[attr.f].color );
    painter.drawText( rect.x(), rect.y() + f_ascent, string );

}


///////////////////////
// scrollbar
// ////////////////////

void EmulationWidget::scroll( int value )
{
}

void EmulationWidget::setScroll( int cursor, int slines )
{
}


static const ColorEntry color_table[TABLE_COLORS] =
{
    ColorEntry(QColor(0x00,0x00,0x00), 0, 0 ), ColorEntry( QColor(0xB2,0xB2,0xB2), 1, 0 ), // Dfore, Dback
  ColorEntry(QColor(0x00,0x00,0x00), 0, 0 ), ColorEntry( QColor(0xB2,0x18,0x18), 0, 0 ), // Black, Red
  ColorEntry(QColor(0x18,0xB2,0x18), 0, 0 ), ColorEntry( QColor(0xB2,0x68,0x18), 0, 0 ), // Green, Yellow
  ColorEntry(QColor(0x18,0x18,0xB2), 0, 0 ), ColorEntry( QColor(0xB2,0x18,0xB2), 0, 0 ), // Blue,  Magenta
  ColorEntry(QColor(0x18,0xB2,0xB2), 0, 0 ), ColorEntry( QColor(0xB2,0xB2,0xB2), 0, 0 ), // Cyan,  White
  // intensiv
  ColorEntry(QColor(0x00,0x00,0x00), 0, 1 ), ColorEntry( QColor(0xFF,0xFF,0xFF), 1, 0 ),
  ColorEntry(QColor(0x68,0x68,0x68), 0, 0 ), ColorEntry( QColor(0xFF,0x54,0x54), 0, 0 ),
  ColorEntry(QColor(0x54,0xFF,0x54), 0, 0 ), ColorEntry( QColor(0xFF,0xFF,0x54), 0, 0 ),
  ColorEntry(QColor(0x54,0x54,0xFF), 0, 0 ), ColorEntry( QColor(0xB2,0x18,0xB2), 0, 0 ),
  ColorEntry(QColor(0x54,0xFF,0xFF), 0, 0 ), ColorEntry( QColor(0xFF,0xFF,0xFF), 0, 0 )
};
