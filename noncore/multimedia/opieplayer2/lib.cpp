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

#include <stdio.h>
#include <stdlib.h>
#include <qimage.h>
#include <qtextstream.h>
#include <qpe/resource.h>

#include <qfile.h>

#include <qgfx_qws.h>
#include <qdirectpainter_qws.h>

#include "xinevideowidget.h"
#include "frame.h"
#include "lib.h"

typedef void (*display_xine_frame_t) (void *user_data, uint8_t* frame,
				      int  width, int height,int bytes );

extern "C" {
    xine_vo_driver_t* init_video_out_plugin( xine_cfg_entry_t* conf, void* video);
    int null_is_showing_video( const xine_vo_driver_t* self );
    void null_set_show_video( const xine_vo_driver_t* self, int show );
    int null_is_fullscreen( const xine_vo_driver_t* self );
    void null_set_fullscreen( const xine_vo_driver_t* self, int screen );
    int null_is_scaling( const xine_vo_driver_t* self );
    void null_set_scaling( const xine_vo_driver_t* self, int scale );
    void null_set_gui_width( const xine_vo_driver_t* self, int width );
    void null_set_gui_height( const xine_vo_driver_t* self, int height );
    void null_set_mode( const xine_vo_driver_t* self, int depth,  int rgb  );
    void null_set_videoGamma( const  xine_vo_driver_t* self , int value );
    void null_display_handler( const xine_vo_driver_t* self, display_xine_frame_t t, void* user_data );
}

using namespace XINE;

Lib::Lib( XineVideoWidget* widget ) {
    m_video = false;
    m_wid = widget;
    printf("Lib");
    QCString str( getenv("HOME") );
    str += "/Settings/opiexine.cf";
    // get the configuration

    // not really OO, should be an extra class, later
    if ( !QFile(str).exists() ) {
        QFile f(str);
        f.open(IO_WriteOnly);
        QTextStream ts( &f );
        ts << "misc.memcpy_method:glibc\n";
        f.close();
    }

    m_xine =  xine_new( );

    xine_config_load( m_xine, str.data() );


    // allocate oss for sound
    // and fb for framebuffer
    m_audioOutput = xine_open_audio_driver( m_xine,  "oss", NULL );
    m_videoOutput = ::init_video_out_plugin( m_config, NULL );


//xine_open_video_driver( m_xine,  NULL,  XINE_VISUAL_TYPE_FB,  NULL);


    null_display_handler( m_videoOutput, xine_display_frame, this );
    xine_init( m_xine,  m_audioOutput,  m_videoOutput );

    if (m_wid != 0 ) {
        printf( "!0\n" );
        resize ( m_wid-> size ( ) );
        ::null_set_mode( m_videoOutput, qt_screen->depth(), qt_screen->pixelType() );

        m_wid->repaint();
    }

    xine_register_event_listener( m_xine, xine_event_handler, this );
}

Lib::~Lib() {
//    free( m_config );
    xine_remove_event_listener( m_xine, xine_event_handler );
    xine_exit( m_xine );
    /* FIXME either free or delete but valgrind bitches against both */
    //free( m_videoOutput );
    //delete m_audioOutput;
}

void Lib::resize ( const QSize &s ) {
    if ( s. width ( ) && s. height ( ) ) {
        ::null_set_gui_width( m_videoOutput,  s. width() );
        ::null_set_gui_height( m_videoOutput,  s. height() );
    }
}

QCString Lib::version() {
    //   QCString str( xine_get_str_version() );
    // return str;
    return "test";
}

int Lib::majorVersion() {
     xine_get_version ( &m_major_version, &m_minor_version, &m_sub_version );
     return m_major_version;
}

int Lib::minorVersion() {
    xine_get_version ( &m_major_version, &m_minor_version, &m_sub_version );
     return m_minor_version;
}

int Lib::subVersion() {
    xine_get_version ( &m_major_version, &m_minor_version, &m_sub_version );
     return m_sub_version;
}

int Lib::play( const QString& fileName, int startPos, int start_time ) {
    QString str = fileName.stripWhiteSpace();
    xine_open( m_xine,  QFile::encodeName(str.utf8() ).data() );
    return xine_play( m_xine, startPos, start_time);
}

void Lib::stop() {
    qDebug("<<<<<<<< STOP IN LIB TRIGGERED >>>>>>>");
    xine_stop( m_xine );
}

void Lib::pause() {
    xine_set_param( m_xine, XINE_PARAM_SPEED,  XINE_SPEED_PAUSE );
}

int Lib::speed() {
    return  xine_get_param ( m_xine, XINE_PARAM_SPEED );
}

void Lib::setSpeed( int speed ) {
    xine_set_param ( m_xine, XINE_PARAM_SPEED, speed );
}

int Lib::status() {
    return xine_get_status( m_xine );
}

int Lib::currentPosition() {
    xine_get_pos_length( m_xine, &m_pos, &m_time, &m_length );
    return m_pos;
}

int Lib::currentTime() {
     xine_get_pos_length( m_xine, &m_pos, &m_time, &m_length );
    return m_time/1000;
}

int Lib::length() {
      xine_get_pos_length( m_xine, &m_pos, &m_time, &m_length );
      return m_length/1000;
}

bool Lib::isSeekable() {
    return xine_get_stream_info ( m_xine, XINE_STREAM_INFO_SEEKABLE );
}

Frame Lib::currentFrame() {
    Frame frame;
    return frame;
};

QString Lib::metaInfo() {
    xine_get_meta_info( m_xine, 0 );
}

int Lib::error() {
    return xine_get_error( m_xine );
};

void Lib::handleXineEvent( xine_event_t* t ) {
    if ( t->type == XINE_EVENT_PLAYBACK_FINISHED ) {
        emit stopped();
    }
}


void Lib::setShowVideo( bool video ) {
    m_video = video;
    ::null_set_show_video( m_videoOutput,  video );
}

bool Lib::isShowingVideo() {
    return ::null_is_showing_video( m_videoOutput );
}

void Lib::showVideoFullScreen( bool fullScreen ) {
    ::null_set_fullscreen( m_videoOutput, fullScreen );
}

bool Lib::isVideoFullScreen() {
    return ::null_is_fullscreen( m_videoOutput );
}

void Lib::setScaling( bool scale ) {
    ::null_set_scaling( m_videoOutput, scale  );
}

void Lib::setGamma( int value ) {
    //qDebug( QString( "%1").arg(value)  );
    ::null_set_videoGamma( m_videoOutput, value );
}

bool Lib::isScaling() {
    return ::null_is_scaling( m_videoOutput );
}

void Lib::xine_event_handler( void* user_data, xine_event_t* t ) {
    ( (Lib*)user_data)->handleXineEvent( t );
}

void Lib::xine_display_frame( void* user_data, uint8_t *frame,
                              int width,  int height,  int bytes ) {
    ( (Lib*)user_data)->drawFrame( frame,  width, height, bytes );
}

void Lib::drawFrame( uint8_t* frame,  int width,  int height,  int bytes ) {
    if ( !m_video ) {
        qWarning("not showing video now");
        return;
    }

//    qWarning( "called draw frame %d %d", width, height );

    m_wid-> setVideoFrame ( frame, width, height, bytes );
}
