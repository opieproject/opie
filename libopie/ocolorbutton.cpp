/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <opie/colorpopupmenu.h>
#include <opie/ocolorbutton.h>
#include <qcolor.h>
#include <qpixmap.h>
#include <qimage.h>
	
#include <qpe/resource.h>	
	
class OColorButtonPrivate {
public:
	QPopupMenu *m_menu;
	QColor m_color;
};

OColorButton::OColorButton ( QWidget *parent, const char *name ) 
	: QPushButton ( parent, name )
{
	d = new OColorButtonPrivate;
	
	d-> m_menu = new ColorPopupMenu ( black, 0, 0 );
	setPopup ( d-> m_menu );
//	setPopupDelay ( 0 );
	connect ( d-> m_menu, SIGNAL( colorSelected ( const QColor & )), this, SLOT( updateColor ( const QColor & )));
	
	updateColor ( black );
	setMinimumSize ( sizeHint ( ) + QSize ( 8, 0 ));
}

OColorButton::~OColorButton ( )
{
	delete d;
}

QColor OColorButton::color ( ) const
{
	return d-> m_color;
}

void OColorButton::setColor ( const QColor &c )
{
	updateColor ( c );
}

void OColorButton::updateColor ( const QColor &c )
{
	d-> m_color = c;
	
	QImage img ( 16, 16, 32 );
	img. fill ( 0 );
	
	int r, g, b;
	c. rgb ( &r, &g, &b );
	
	int w = img. width ( );
	int h = img. height ( );
	
	int dx = w * 20 / 100; // 15%
	int dy = h * 20 / 100;
	
	for ( int y = 0; y < h; y++ ) {
		for ( int x = 0; x < w; x++ ) {
			double alpha = 1.0;

			if ( x < dx )
				alpha *= ( double ( x + 1 ) / dx );
			else if ( x >= w - dx )
				alpha *= ( double ( w - x ) / dx );
			if ( y < dy )
				alpha *= ( double ( y + 1 ) / dy );
			else if ( y >= h - dy )
				alpha *= ( double ( h - y ) / dy );
				
			int a = int ( alpha * 255.0 );
			if ( a < 0 )
				a = 0;
			if ( a > 255 )
				a = 255;

			img. setPixel ( x, y, qRgba ( r, g, b, a ));
		}
	}
	img. setAlphaBuffer ( true );

	QPixmap pix;
	pix. convertFromImage ( img );
	setPixmap ( pix );	

	emit colorSelected ( c );
}

