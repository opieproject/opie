
/*
                            This file is part of the Opie Project
 
                             Copyright (c)  2002 Max Reiss <harlekin@handhelds.org>
                             Copyright (c)  2002 LJP <>
                             Copyright (c)  2002 Holger Freyther <zecke@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
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

#include <qimage.h>
#include <qpainter.h>
#include <qgfx_qws.h>
#include <qdirectpainter_qws.h>
#include <qgfx_qws.h>
#include <qsize.h>

#include <qpe/resource.h>

#include "xinevideowidget.h"

static inline void memcpy_rev ( void *dst, void *src, size_t len )
{
	((char *) src ) += len;

	len >>= 1; 
	while ( len-- )
		*((short int *) dst )++ = *--((short int *) src );
}

static inline void memcpy_step ( void *dst, void *src, size_t len, size_t step )
{
	len >>= 1;
	while ( len-- ) {
		*((short int *) dst )++ = *((short int *) src );
		((char *) src ) += step;
	}
}

static inline void memcpy_step_rev ( void *dst, void *src, size_t len, size_t step )
{
	len >>= 1;
	
	((char *) src ) += ( len * step );
	
	while ( len-- ) {
		((char *) src ) -= step;
		*((short int *) dst )++ = *((short int *) src );
	}
}


XineVideoWidget::XineVideoWidget( int width,
                                  int height,
                                  QWidget* parent,
                                  const char* name )
		: QWidget ( parent, name, WRepaintNoErase | WResizeNoErase )
{
	m_image = new QImage ( width, height, qt_screen-> depth ( ));
	m_buff = 0;
	setBackgroundMode ( NoBackground );
	/*    QImage image = Resource::loadImage("SoundPlayer");
	    image = image.smoothScale( width, height );
	 
	    m_image = new QImage( image );*/
}

XineVideoWidget::~XineVideoWidget ( )
{
	delete m_image;
}

void XineVideoWidget::clear ( )
{
	m_buff = 0;
	repaint ( false );
}

void XineVideoWidget::paintEvent ( QPaintEvent * )
{
	//qWarning( "painting <<<" );
	if ( m_buff == 0 ) {
		QPainter p ( this );
		p. fillRect ( rect ( ), black );
		p. drawImage ( 0, 0, *m_image );
		//qWarning ( "logo\n" );
	}
	else {
//		qWarning ( "paintevent\n" );
		
		QArray <QRect> qt_bug_workaround_clip_rects;
		
		{
			QDirectPainter dp ( this );

			int rot = dp. transformOrientation ( );

			uchar *fb = dp. frameBuffer ( );
			uchar *frame = m_buff;  // rot == 0 ? m_buff : m_buff + ( m_thisframe. height ( ) - 1 ) * m_bytes_per_line_frame;
			
			QRect framerect = qt_screen-> mapToDevice ( QRect ( mapToGlobal ( m_thisframe. topLeft ( )), m_thisframe. size ( )), QSize ( qt_screen-> width ( ), qt_screen-> height ( )));
			
			qt_bug_workaround_clip_rects. resize ( dp. numRects ( ));

			for ( int i = dp. numRects ( ) - 1; i >= 0; i-- ) {
				const QRect &clip = dp. rect ( i );
								
				qt_bug_workaround_clip_rects [i] = qt_screen-> mapFromDevice ( clip, QSize ( qt_screen-> width ( ), qt_screen-> height ( )));
				
				uchar *dst = fb + ( clip. x ( ) * m_bytes_per_pixel ) + ( clip. y ( ) * m_bytes_per_line_fb ); 
				uchar *src = frame;
				
				switch ( rot ) {
					case 0: src += ( (( clip. x ( ) - framerect. x ( )) * m_bytes_per_pixel ) + (( clip. y ( ) - framerect. y ( )) * m_bytes_per_line_frame ) ); break;
					case 1: src += ( (( clip. y ( ) - framerect. y ( )) * m_bytes_per_pixel ) + (( clip. x ( ) - framerect. x ( )) * m_bytes_per_line_frame ) + (( framerect. height ( ) - 1 ) * m_bytes_per_pixel ) ); break;
					case 2: src += ( (( clip. x ( ) - framerect. x ( )) * m_bytes_per_pixel ) + (( clip. y ( ) - framerect. y ( )) * m_bytes_per_line_frame ) + (( framerect. height ( ) - 1 ) * m_bytes_per_line_frame ) ); break;
					case 3: src += ( (( clip. y ( ) - framerect. y ( )) * m_bytes_per_pixel ) + (( clip. x ( ) - framerect. x ( )) * m_bytes_per_line_frame ) ); break;
				}

				uint leftfill = 0;
				uint framefill = 0;
				uint rightfill = 0;
				uint clipwidth = clip. width ( ) * m_bytes_per_pixel;
				
				if ( clip. left ( ) < framerect. left ( ))
					leftfill = (( framerect. left ( ) - clip. left ( )) * m_bytes_per_pixel ) <? clipwidth;
				if ( clip. right ( ) > framerect. right ( ))
					rightfill = (( clip. right ( ) - framerect. right ( )) * m_bytes_per_pixel ) <? clipwidth;
				
				framefill = clipwidth - ( leftfill + rightfill );

				for ( int y = clip. top ( ); y <= clip. bottom ( ); y++ ) {
					if (( y < framerect. top ( )) || ( y > framerect. bottom ( ))) {
						memset ( dst, 0, clipwidth );
					}
					else {
						if ( leftfill )
							memset ( dst, 0, leftfill );
							
						if ( framefill ) {
							switch ( rot ) {
								case 0: memcpy ( dst + leftfill, src, framefill );                                  break;
								case 1: memcpy_step ( dst + leftfill, src, framefill, m_bytes_per_line_frame );     break;
								case 2: memcpy_rev ( dst + leftfill, src, framefill );                              break;
								case 3: memcpy_step_rev ( dst + leftfill, src, framefill, m_bytes_per_line_frame ); break;
							}
						}	
						if ( rightfill )
							memset ( dst + leftfill + framefill, 0, rightfill );
					}
					
					dst += m_bytes_per_line_fb;
					
					switch ( rot ) {
						case 0: src += m_bytes_per_line_frame; break;
						case 1: src -= m_bytes_per_pixel;      break;
						case 2: src -= m_bytes_per_line_frame; break;
						case 3: src += m_bytes_per_pixel;      break;
					}
				}
			}
		}
		//qWarning ( " ||| painting |||" );		
 		{
			// QVFB hack by MArtin Jones
			QPainter p ( this );

			for ( int i = qt_bug_workaround_clip_rects. size ( ) - 1; i >= 0; i-- ) {  
				p. fillRect ( QRect ( mapFromGlobal ( qt_bug_workaround_clip_rects [i]. topLeft ( )), qt_bug_workaround_clip_rects [i]. size ( )), QBrush ( NoBrush ) );
			}
		}
	}
	//qWarning( "painting >>>" );
}

int XineVideoWidget::height ( ) const
{
	return m_image-> height ( );
}

int XineVideoWidget::width ( ) const
{
	return m_image-> width ( );
}

void XineVideoWidget::setImage ( QImage* image )
{
	delete m_image;
	m_image = image;
}

void XineVideoWidget::setImage( uchar* image, int yoffsetXLine,
                                int xoffsetXBytes, int width,
                                int height, int linestep, int bytes, int bpp )
{

	m_lastframe = m_thisframe;
	m_thisframe. setRect ( xoffsetXBytes, yoffsetXLine, width, height );

	m_buff                 = image;
	m_bytes_per_line_fb    = linestep;
	m_bytes_per_line_frame = bytes;
	m_bytes_per_pixel      = bpp;

	repaint ((( m_thisframe & m_lastframe ) != m_lastframe ) ? m_lastframe : m_thisframe, false );
}
