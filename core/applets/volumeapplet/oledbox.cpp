
#include <qpixmap.h>
#include <qbitmap.h>
#include <qpainter.h>

#include "oledbox.h"


#ifdef _QTE_IS_TOO_DUMB_TO_DRAW_AN_ARC

/* XPM */
static const char * ledborder_xpm[] = {
"16 16 11 1",
" 	c None",
".	c #626562",
"+	c #7B7D7B",
"@	c #949594",
"#	c #ACAEAC",
"$	c #CDCACD",
"%	c #CDCECD",
";	c #E6E6E6",
">	c #FFFFFF",
",	c #E6E2E6",
"'	c #FFFAFF",
"     .++@@#     ",
"   ...++@@##$   ",
"  .....+@##$$%  ",
" .....    #$%%% ",
" ...        %%; ",
"....        ;;;;",
"++.          ;>>",
"+++          >>>",
"@@@          >>>",
"@@#          >>>",
"####        >>>>",
" #$$        >>> ",
" $$,,'    >>>>> ",
"  ,,,''>>>>>>>  ",
"   ,''>>>>>>>   ",
"     '>>>>>     "};


QPixmap *OLedBox::s_border_pix = 0;

#endif


OLedBox::OLedBox ( const QColor &col, QWidget *parent, const char *name ) : QWidget ( parent, name )
{
	m_color = col;
	m_on = false;

	m_pix [ 0 ] = m_pix [ 1 ] = false;
	
	setBackgroundMode ( PaletteBackground );
	
#ifdef _QTE_IS_TOO_DUMB_TO_DRAW_AN_ARC
	if ( !s_border_pix )
		s_border_pix = new QPixmap ( ledborder_xpm );
#endif
}

OLedBox::~OLedBox ( )
{
	delete m_pix [ 0 ];
	delete m_pix [ 1 ];
}

QSize OLedBox::sizeHint ( ) const
{
	return QSize ( 16, 16 );
}

bool OLedBox::isOn ( ) const
{
	return m_on;
}

QColor OLedBox::color ( ) const
{
	return m_color;
}

void OLedBox::setOn ( bool b )
{
	if ( m_on != b ) {
		m_on = b;
		update ( );
	}
}

void OLedBox::toggle ( )
{
	setOn ( !isOn ( ) );
}

void OLedBox::setColor ( const QColor &col )
{
	if ( m_color != col ) {
		m_color = col;

		delete m_pix [ 0 ];
		delete m_pix [ 1 ];

		update ( );
	}
}

void OLedBox::mousePressEvent ( QMouseEvent *e )
{
	if ( e-> button ( ) == LeftButton ) {
		m_on = !m_on;
		update ( );
		emit toggled ( m_on );
	}
}


void OLedBox::resizeEvent ( QResizeEvent * )
{
	delete m_pix [ 0 ];
	delete m_pix [ 1 ];

	update ( );
}

void OLedBox::paintEvent ( QPaintEvent *e )
{
	int ind = m_on ? 1 : 0;

	if ( !m_pix [ ind ] ) {
		m_pix [ ind ] = new QPixmap ( size ( ));

		drawLed ( m_pix [ ind ], m_on ? m_color : m_color. dark ( 300 ) );
	}
	if ( !e-> erased ( ))
		erase ( );

	QPainter p ( this );
	p. drawPixmap ( 0, 0, *m_pix [ ind ] );
}

// From KDE libkdeui / led.cpp

void OLedBox::drawLed ( QPixmap *pix, const QColor &col )  // paint a ROUND SUNKEN led lamp
{
	QPainter paint;
	QColor color;
	QBrush brush;
	QPen pen;

	pix-> fill ( black );

	// First of all we want to know what area should be updated
	// Initialize coordinates, width, and height of the LED
	int width = pix-> width ( );

	// Make sure the LED is round!
	if ( width > pix-> height ( ))
		width = pix-> height ( );
	width -= 2; // leave one pixel border
	if ( width < 0 )
		width = 0;

	// maybe we could stop HERE, if width <=0 ?

	// start painting widget
	//
	paint.begin( pix );

	// Set the color of the LED according to given parameters
	color = col;

	// Set the brush to SolidPattern, this fills the entire area
	// of the ellipse which is drawn first
	brush.setStyle( QBrush::SolidPattern );
	brush.setColor( color );
	paint.setBrush( brush );                // Assign the brush to the painter

	// Draws a "flat" LED with the given color:
	paint.drawEllipse( 1, 1, width, width );

	// Draw the bright light spot of the LED now, using modified "old"
	// painter routine taken from KDEUIs KLed widget:

	// Setting the new width of the pen is essential to avoid "pixelized"
	// shadow like it can be observed with the old LED code
	pen.setWidth( 2 );

	// shrink the light on the LED to a size about 2/3 of the complete LED
	int pos = width / 5 + 1;
	int light_width = width;
	light_width *= 2;
	light_width /= 3;

	// Calculate the LEDs "light factor":
	int light_quote = ( 130 * 2 / ( light_width ? light_width : 1 ) ) + 100;

	// Now draw the bright spot on the LED:
	while ( light_width )
	{
		color = color.light( light_quote );                      // make color lighter
		pen.setColor( color );                                   // set color as pen color
		paint.setPen( pen );                                     // select the pen for drawing
		paint.drawEllipse( pos, pos, light_width, light_width ); // draw the ellipse (circle)
		light_width--;
		if ( !light_width )
			break;
		paint.drawEllipse( pos, pos, light_width, light_width );
		light_width--;
		if ( !light_width )
			break;
		paint.drawEllipse( pos, pos, light_width, light_width );
		pos++;
		light_width--;
	}

	// Drawing of bright spot finished, now draw a thin border
	// around the LED which resembles a shadow with light coming
	// from the upper left.

#ifdef _QTE_IS_TOO_DUMB_TO_DRAW_AN_ARC
	paint. drawPixmap ( 0, 0, *s_border_pix );
	paint. end ( );
	
	pix-> setMask ( pix-> createHeuristicMask ( ));
	
#else
	pen.setWidth( 3 );
	brush.setStyle( QBrush::NoBrush );              // Switch off the brush
	paint.setBrush( brush );                        // This avoids filling of the ellipse

	// Set the initial color value to 200 (bright) and start
	// drawing the shadow border at 45 (45*16 = 720).
	int shadow_color = 200, angle;

	for ( angle = 720; angle < 6480; angle += 240 )
	{
		color.setRgb( shadow_color, shadow_color, shadow_color );
		pen.setColor( color );
		paint.setPen( pen );
		paint.drawArc( 0, 0, width+2, width+2, angle, 240 );
		paint.drawArc( 1, 1, width,   width,   angle, 240 );
		paint.drawArc( 2, 2, width-2, width-2, angle, 240 );
		if ( angle < 2320 ) {
			shadow_color -= 25;                     // set color to a darker value
			if ( shadow_color < 100 )
				shadow_color = 100;
		}
		else if ( ( angle > 2320 ) && ( angle < 5760 ) ) {
			shadow_color += 25;                     // set color to a brighter value
			if ( shadow_color > 255 )
				shadow_color = 255;
		}
		else {
			shadow_color -= 25;                     // set color to a darker value again
			if ( shadow_color < 100 )
				shadow_color = 100;
		} // end if ( angle < 2320 )
	}   // end for ( angle = 720; angle < 6480; angle += 160 )
	paint.end();
	//
	// painting done

	QBitmap mask ( pix-> width ( ), pix-> height ( ), true );
	QPainter mp ( &mask );
	mp. setPen ( Qt::NoPen );
	mp. setBrush ( Qt::color1 );
	mp. drawEllipse ( 0, 0, width + 2, width + 2 );
	mp. end ( );
	
	pix-> setMask ( mask );
#endif
}

