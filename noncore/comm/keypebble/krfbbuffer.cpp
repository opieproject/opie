#include <assert.h>
#include <qpixmap.h>
#include <qbrush.h>
#include <qimage.h>
#include <qpainter.h>
#include <qapplication.h>
#include "krfbdecoder.h"
#include "krfbbuffer.h"
#include "krfbserverinfo.h"

//
// Endian stuff
//
#ifndef KDE_USE_FINAL
const int endianTest = 1;
#endif

#define Swap16IfLE(s) \
    (*(char *)&endianTest ? ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff)) : (s))

#define Swap32IfLE(l) \
    (*(char *)&endianTest ? ((((l) & 0xff000000) >> 24) | \
			     (((l) & 0x00ff0000) >> 8)  | \
			     (((l) & 0x0000ff00) << 8)  | \
			     (((l) & 0x000000ff) << 24))  : (l))

KRFBBuffer::KRFBBuffer( KRFBDecoder *decoder,
			QObject *parent, const char *name )
  : QObject( parent, name )
{
  assert( decoder );
  this->decoder = decoder;
  pix = new QPixmap();
}

KRFBBuffer::~KRFBBuffer()
{
  delete pix;
}

void KRFBBuffer::resize( int w, int h )
{
  qWarning( "Resizing buffer" );

  pix->resize( w, h );
  
  QPalette pal = qApp->palette();
  pix->fill( pal.active().base() );

  emit sizeChanged( w, h );
}

void KRFBBuffer::soundBell()
{
  emit bell();
}

void KRFBBuffer::mouseEvent( QMouseEvent *e )
{
  decoder->sendMouseEvent( e );
}

void KRFBBuffer::keyPressEvent( QKeyEvent *e )
{
    qWarning( "Buffer got a key" );

  decoder->sendKeyPressEvent( e );
}

void KRFBBuffer::keyReleaseEvent( QKeyEvent *e )
{
  decoder->sendKeyReleaseEvent( e );
}

void KRFBBuffer::copyRect( int srcX, int srcY,
                           int destX, int destY, int w, int h )
{
//  qWarning( "Got copy rect" );
  bitBlt( pix, destX, destY, pix, srcX, srcY, w, h, CopyROP );

  emit updated( destX, destY, w, h );
}

void KRFBBuffer::drawRawRectChunk( void *data,
				   int x, int y, int w, int h )
{
  QImage img( w, h, 32 );

  int redMax = Swap16IfLE( decoder->format->redMax );
  int greenMax = Swap16IfLE( decoder->format->greenMax );
  int blueMax = Swap16IfLE( decoder->format->blueMax );

  QPainter p( pix );

  if ( decoder->format->bpp == 8 ) {
    uchar *d = (unsigned char *) data;

    uint r,g,b;

    for ( int j = 0; j < h; j++ ) {
      for ( int i = 0; i < w ; i++ ) {
	r = d[ j * w + i ];
	r = r >> decoder->format->redShift;
	r = r & redMax;

	g = d[ j * w + i ];
	g = g >> decoder->format->greenShift;
	g = g & greenMax;

	b = d[ j * w + i ];
	b = b >> decoder->format->blueShift;
	b = b & blueMax;

	r = ( r * 255 ) / redMax;
	g = ( g * 255 ) / greenMax;
	b = ( b * 255 ) / blueMax;

        uint *p = ( uint * ) img.scanLine( j ) + i;
	*p = qRgb( r,g,b );
      }
    }
  }
  else if ( decoder->format->bpp == 32 ) {
    ulong *d = (ulong *) data;

    ulong r,g,b;

    for ( int j = 0; j < h; j++ ) {
      for ( int i = 0; i < w ; i++ ) {
	ulong pixel = d[ j * w + i ];
	pixel = Swap32IfLE( pixel );

	r = pixel;
	r = r >> decoder->format->redShift;
	r = r & redMax;

	g = pixel;
	g = g >> decoder->format->greenShift;
	g = g & greenMax;

	b = pixel;
	b = b >> decoder->format->blueShift;
	b = b & blueMax;

	r = ( r * 255 ) / redMax;
	g = ( g * 255 ) / greenMax;
	b = ( b * 255 ) / blueMax;

        uint *p = ( uint * ) img.scanLine( j ) + i;
	*p = qRgb( r,g,b );
      }
    }
  } else if (decoder->format->bpp == 16 ) {

				CARD16 *d = (CARD16 *) data;

				uint r,g,b;

				for ( int j = 0; j < h; j++ ) {
						for ( int i = 0; i < w ; i++ ) {
								CARD16 pixel = d[ j * w + i ];
								pixel = Swap16IfLE( pixel );

								r = pixel;
								r = r >> decoder->format->redShift;
								r = r & redMax;

								g = pixel;
								g = g >> decoder->format->greenShift;
								g = g & greenMax;

								b = pixel;
								b = b >> decoder->format->blueShift;
								b = b & blueMax;

								r = ( r * 255 ) / redMax;
								g = ( g * 255 ) / greenMax;
								b = ( b * 255 ) / blueMax;

								ulong *p = ( ulong * ) img.scanLine( j ) + i;
								*p = qRgb( r,g,b );
						}
				}
		}
  else {
    p.setBrush( QBrush( Qt::black ) );
    p.drawRect( x, y, w, h );
  }

  p.drawImage( x, y, img );

  emit updated( x, y, w, h );
}

