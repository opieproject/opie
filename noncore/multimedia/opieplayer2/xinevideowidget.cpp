/*
                            This file is part of the Opie Project

                             Copyright (c)  2002 Max Reiss <harlekin@handhelds.org>
                             Copyright (c)  2002 L. Potter <ljp@llornkcor.com>
                             Copyright (c)  2002 Holger Freyther <zecke@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "xinevideowidget.h"
#include <opie2/odebug.h>

#include <qimage.h>
#include <qdirectpainter_qws.h>
#include <qgfx_qws.h>
#include <qsize.h>
#include <qapplication.h>

#include <qpe/resource.h>

#include <pthread.h>



// 0 deg rot: copy a line from src to dst (use libc memcpy)

// 180 deg rot: copy a line from src to dst reversed

/*
 * This code relies the len be a multiply of 16bit
 */
static inline void memcpy_rev ( void *_dst, void *_src, size_t len )
{

    /*
     * move the source to the end
     */
    char *src_c = static_cast<char*>(_src) + len;

    /*
     * as we copy by 16bit and not 8bit
     * devide the length by two
     */
    len >>= 1;

    short int* dst = static_cast<short int*>( _dst );
    short int* src = reinterpret_cast<short int*>( src_c );

    /*
     * Increment dst after assigning
     * Decrement src before  assigning becase we move backwards
     */
    while ( len-- )
        *dst++ = *--src;

}

// 90 deg rot: copy a column from src to dst

static inline void memcpy_step ( void *_dst, void *_src, size_t len, size_t step )
{
    short int *dst = static_cast<short int*>( _dst );
    short int *src = static_cast<short int*>( _src );

    len >>= 1;

    /*
     * Copy 16bit from src to dst and move to the next address
     */
    while ( len-- ) {
        *dst++ = *src;
        src = reinterpret_cast<short int*>(reinterpret_cast<char*>(src)+step);
    }
}

// 270 deg rot: copy a column from src to dst reversed

static inline void memcpy_step_rev ( void *_dst, void *_src, size_t len, size_t step )
{
    len >>= 1;

    char *src_c = static_cast<char*>( _src ) + (len*step);
    short int* dst = static_cast<short int*>( _dst );
    short int* src = reinterpret_cast<short int*>( src_c );

    while ( len-- ) {
        src_c -= step;
        src = reinterpret_cast<short int*>( src_c );
        *dst++ = *src;
    }
}


XineVideoWidget::XineVideoWidget ( QWidget* parent, const char* name )
    : QWidget ( parent, name, WRepaintNoErase | WResizeNoErase ),old_framerect(0,0,0,0),old_size(0,0)
{
    setBackgroundMode ( NoBackground );

    m_logo              = 0;
    m_buff              = 0;
    m_bytes_per_line_fb = qt_screen-> linestep ( );
    m_bytes_per_pixel   = ( qt_screen->depth() + 7 ) / 8;
    m_rotation          = 0;
    m_lastsize = 0;
}


XineVideoWidget::~XineVideoWidget ( )
{
#if 0
    if (m_buff) {
        delete[]m_buff;
        m_lastsize=0;
        m_buff = 0;
    }
#endif
    if (m_logo) {
        delete m_logo;
    }
}

void XineVideoWidget::clear ( )
{
#if 0
    ThreadUtil::AutoLock a(m_bufmutex);
    if (m_buff) {
        delete[]m_buff;
        m_lastsize=0;
        m_buff = 0;
    }
#endif
    repaint ( false );
}

QSize XineVideoWidget::videoSize() const
{
    QSize s = size();
    bool fs = ( s == qApp->desktop()->size() );

    // if we are in fullscreen mode, do not rotate the video
    // (!! the paint routine uses m_rotation + qt_screen-> transformOrientation() !!)
    m_rotation = fs ? - qt_screen->transformOrientation() : 0;

    if ( fs && qt_screen->isTransformed() )
        s = qt_screen->mapToDevice( s );

    return s;
}

void XineVideoWidget::paintEvent ( QPaintEvent * )
{
    QPainter p ( this );
    p. fillRect ( rect (), black );
    if (m_logo)
        p. drawImage ( 0, 0, *m_logo );
}

void XineVideoWidget::paintEvent2 ( QPaintEvent * )
{
    if ( m_buff == 0 ) {
        return;
    } else if (m_lastsize){
        {
            QDirectPainter dp ( this );
            int rot = dp. transformOrientation ( ) + m_rotation; // device rotation + custom rotation
            bool rot90 = (( -m_rotation ) & 1 );
            int _vw,_vh;
            switch (rot90) {
                case true:
                    _vh = m_framesize.width();
                    _vw = m_framesize.height();
                    break;
                default:
                    _vw = m_framesize.width();
                    _vh = m_framesize.height();
                    break;
                 break;
            }
            int middle_w = _vw/2;
            int middle_h = _vh/2;
            m_thisframe.setRect(width()/2-middle_w,height()/2-middle_h,_vw,_vh);
            uchar *fb = dp. frameBuffer ( );
            uchar *frame = m_buff;

            // where is the video frame in fb coordinates
            QRect framerect = qt_screen-> mapToDevice ( QRect ( mapToGlobal ( m_thisframe. topLeft ( )), m_thisframe. size ( )), QSize ( qt_screen-> width ( ),
                             qt_screen-> height ( )));

            uchar * src = frame;
            uchar * dst = fb+framerect.y()*m_bytes_per_line_fb+framerect.x()*m_bytes_per_pixel;

            /* clean up the fb screen when shrinking the image only! */
            if (old_framerect.isValid() && old_size.width()==width()&&old_size.height()==height() &&
                (old_framerect.width()>framerect.width() || old_framerect.height()>framerect.height())) {
                uchar*_dst = fb+old_framerect.y()*m_bytes_per_line_fb+old_framerect.x()*m_bytes_per_pixel;
                for (int z=0;z<old_framerect.height();++z) {
                    memset(_dst,0,m_bytes_per_line_fb);
                    _dst+=m_bytes_per_line_fb;
                }
            }
            old_framerect=framerect;
            old_size = size();

            if (framerect.height()!=m_framesize.height()) {
                odebug << "Hoehm: " << framerect.height() << " <-> " << m_framesize.height() << oendl;
            }
            for (int y = 0;y<framerect.height();++y) {
                switch (rot) {
                    case 0:
                        memcpy(dst,src,m_bytes_per_line_frame);
                        src+=m_bytes_per_line_frame;
                    break;
                    case 1:
                        memcpy_step ( dst, src, m_thisframe.size().width(), m_bytes_per_line_frame );
                        src -= m_bytes_per_pixel;
                        break;
                    case 2:
                        memcpy_rev ( dst, src, m_thisframe.size().width());
                        src-=m_bytes_per_line_frame;
                    break;
                    case 3:
                        memcpy_step_rev ( dst, src, m_thisframe.size().width(), m_bytes_per_line_frame );
                        src += m_bytes_per_pixel;
                    break;
                    default:
                        break;
                }
                dst += m_bytes_per_line_fb;
            }
        }
    }
}

QImage *XineVideoWidget::logo ( ) const
{
    return m_logo;
}


void XineVideoWidget::setLogo ( QImage* logo )
{
    delete m_logo;
    m_logo = logo;
}

void XineVideoWidget::setVideoFrame ( uchar* img, int w, int h, int bpl )
{
    // mutex area for AutoLock
    {
#if 0
        ThreadUtil::AutoLock a(m_bufmutex);
#endif
        if (!isVisible()||w>width()||h>height()/*||m_bufmutex.isLocked()*/) {
            //drop frame
            return;
        }
        bool rot90 = (( -m_rotation ) & 1 );
        int l = h*bpl;
#if 0
        if (l!=m_lastsize) {
            if (m_buff) {
                delete[]m_buff;
            }
            if (l>0) {
                m_buff = new uchar[l];
                odebug << "Point to: " << (unsigned long)m_buff << oendl;
                m_lastsize=l;
            } else {
                m_buff = 0;
            }
        }
#endif
        m_lastsize = l;
        m_framesize = QSize(w,h);
#if 0
        if (m_buff && m_lastsize) {
            memcpy(m_buff,img,m_lastsize);
        }
#endif
        m_buff = img;
        m_bytes_per_line_frame = bpl;
        if (m_buff) paintEvent2(0);
    } // Release Mutex
}

void XineVideoWidget::resizeEvent ( QResizeEvent * )
{
#if 0
    ThreadUtil::AutoLock a(m_bufmutex);
#endif
    emit videoResized( videoSize() );
    odebug << "All resize done" << oendl;
}


void XineVideoWidget::mouseReleaseEvent ( QMouseEvent * /*me*/ )
{
    emit clicked();
}

