
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
	qWarning( "painting" );
	if ( m_buff == 0 ) {
		QPainter p ( this );
		p. fillRect ( rect ( ), black );
		p. drawImage ( 0, 0, *m_image );
		qWarning ( "logo\n" );
	}
	else {
		qWarning ( "paintevent\n" );
		
		QArray <QRect> qt_bug_workaround_clip_rects;
		
		{
			QDirectPainter dp ( this );

			uchar *fb = dp. frameBuffer ( );
			uchar *frame = m_buff;  // rot == 0 ? m_buff : m_buff + ( m_thisframe. height ( ) - 1 ) * m_bytes_per_line_frame;
			
			QRect framerect = QRect ( mapToGlobal ( m_thisframe. topLeft ( )), m_thisframe. size ( ));

			qt_bug_workaround_clip_rects. resize ( dp. numRects ( ));

			for ( int i = dp. numRects ( ) - 1; i >= 0; i-- ) {
				const QRect &clip = dp. rect ( i );
								
				qt_bug_workaround_clip_rects [i] = clip;
				
				int rot = dp. transformOrientation ( );

				if ( rot == 0 || rot == 180 ) {			
					uchar *dst = fb + ( clip. x ( ) * m_bytes_per_pixel ) + ( clip. y ( ) * m_bytes_per_line_fb ); 
					uchar *src = frame + (( clip. x ( ) - framerect. x ( )) * m_bytes_per_pixel ) + (( clip. y ( ) - framerect. y ( )) * m_bytes_per_line_frame );
					
					if ( rot == 180 )
						src += (( framerect. height ( ) - 1 ) * m_bytes_per_line_frame );
										
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
								if ( rot == 0 )
									memcpy ( dst + leftfill, src, framefill ); 	
								else
									memcpy_rev ( dst + leftfill, src, framefill );
							}	
							if ( rightfill )
								memset ( dst + leftfill + framefill, 0, rightfill );
						}
						
						dst += m_bytes_per_line_fb;
						src += ( rot == 0 ? m_bytes_per_line_frame : -m_bytes_per_line_frame );
					}
				}
				else { // rot == 90 || rot == 270
					uchar *dst = fb + ( clip. y ( ) * m_bytes_per_pixel ) + ( clip. x ( ) * m_bytes_per_line_fb ); 
					uchar *src = frame + (( clip. x ( ) - framerect. x ( )) * m_bytes_per_pixel ) + (( clip. y ( ) - framerect. y ( )) * m_bytes_per_line_frame );
					
					if ( rot == 270 )
						src += (( framerect. height ( ) - 1 ) * m_bytes_per_line_frame );
										
					uint leftfill = 0;
					uint framefill = 0;
					uint rightfill = 0;
					uint clipwidth = clip. height ( ) * m_bytes_per_pixel;
					
					if ( clip. bottom ( ) > framerect. bottom ( ))
						leftfill = (( clip. bottom ( ) - framerect. bottom ( )) * m_bytes_per_pixel ) <? clipwidth;
					if ( clip. top ( ) < framerect. top ( ))
						rightfill = (( framerect. top ( ) - framerect. top ( )) * m_bytes_per_pixel ) <? clipwidth;
					
					framefill = clipwidth - ( leftfill + rightfill );
					
					for ( int y = clip. left ( ); y <= clip. right ( ); y++ ) {
						if (( y < framerect. left ( )) || ( y > framerect. right ( ))) {
							memset ( dst, 0, clipwidth );
						}
						else {
							if ( leftfill )
								memset ( dst, 0, leftfill );
								
							if ( framefill ) {
								if ( rot == 90 )
									memcpy_step ( dst + leftfill, src, framefill, m_bytes_per_line_frame );
								else
									memcpy_step_rev ( dst + leftfill, src, framefill, m_bytes_per_line_frame );
							}	
							if ( rightfill )
								memset ( dst + leftfill + framefill, 0, rightfill );
						}
						
						dst += m_bytes_per_line_fb;
						src += ( rot == 90 ? +1 : -1 ); // m_bytes_per_line_frame : -m_bytes_per_line_frame );
					}					
				}
			}
		}		
 		{
			// QVFB hack by MArtin Jones
			QPainter p ( this );
			
			for ( int i = qt_bug_workaround_clip_rects. size ( ) - 1; i >= 0; i-- ) {  
				p. fillRect ( QRect ( mapFromGlobal ( qt_bug_workaround_clip_rects [i]. topLeft ( )), qt_bug_workaround_clip_rects [i]. size ( )), QBrush ( NoBrush ) );
			}
		}
	}
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
