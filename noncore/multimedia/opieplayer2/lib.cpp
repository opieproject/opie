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
#include <qdir.h>

#include <qgfx_qws.h>
#include <qdirectpainter_qws.h>

#include <assert.h>

#include "xinevideowidget.h"
#include "frame.h"
#include "lib.h"


typedef void (*display_xine_frame_t) (void *user_data, uint8_t* frame,
				      int  width, int height,int bytes );

extern "C" {
    xine_vo_driver_t* init_video_out_plugin( xine_t *xine, void* video, display_xine_frame_t, void * );
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

Lib::Lib( InitializationMode initMode, XineVideoWidget* widget ) 
{
    m_initialized = false;
    m_video = false;
    m_wid = widget;
    printf("Lib");
    QString configPath = QDir::homeDirPath() + "/Settings/opiexine.cf";
    // get the configuration

    // not really OO, should be an extra class, later
    if ( !QFile::exists(configPath) ) {
        QFile f(configPath);
        f.open(IO_WriteOnly);
        QTextStream ts( &f );
        ts << "misc.memcpy_method:glibc\n";
        f.close();
    }

    if ( initMode == InitializeImmediately ) {
        initialize();
        m_initialized = true;
    }
    else
        assert( false );
}

void Lib::run()
{
    assert( false );
}

void Lib::initialize()
{
    m_xine =  xine_new( );

    QString configPath = QDir::homeDirPath() + "/Settings/opiexine.cf";
    xine_config_load( m_xine, QFile::encodeName( configPath ) );

    xine_init( m_xine );

    // allocate oss for sound
    // and fb for framebuffer
    m_audioOutput = xine_open_audio_driver( m_xine,  "oss", NULL );
    m_videoOutput = ::init_video_out_plugin( m_xine, NULL, xine_display_frame, this );


//xine_open_video_driver( m_xine,  NULL,  XINE_VISUAL_TYPE_FB,  NULL);


//    null_display_handler( m_videoOutput, xine_display_frame, this );

    m_stream = xine_stream_new (m_xine,  m_audioOutput,  m_videoOutput );

    if (m_wid != 0 ) {
        printf( "!0\n" );
        resize ( m_wid-> size ( ) );
        ::null_set_mode( m_videoOutput, qt_screen->depth(), qt_screen->pixelType() );

//        m_wid->repaint();
    }

    m_queue = xine_event_new_queue (m_stream);

    xine_event_create_listener_thread (m_queue, xine_event_handler, this);
}

Lib::~Lib() {
    assert( isRunning() == false );
    assert( m_initialized );

//    free( m_config );

    xine_close( m_stream );

    xine_event_dispose_queue( m_queue );

    xine_dispose( m_stream );

    xine_exit( m_xine );
    /* FIXME either free or delete but valgrind bitches against both */
    //free( m_videoOutput );
    //delete m_audioOutput;
}

void Lib::resize ( const QSize &s ) {
    assert( m_initialized );

    if ( s. width ( ) && s. height ( ) ) {
        ::null_set_gui_width( m_videoOutput,  s. width() );
        ::null_set_gui_height( m_videoOutput,  s. height() );
    }
}

int Lib::majorVersion() {
     int major, minor, sub;
     xine_get_version ( &major, &minor, &sub );
     return major;
}

int Lib::minorVersion() {
     int major, minor, sub;
     xine_get_version ( &major, &minor, &sub );
     return minor;
}

int Lib::subVersion() {
     int major, minor, sub;
     xine_get_version ( &major, &minor, &sub );
     return sub;
}

int Lib::play( const QString& fileName, int startPos, int start_time ) {
    assert( m_initialized );

    QString str = fileName.stripWhiteSpace();
    if ( !xine_open( m_stream, QFile::encodeName(str.utf8() ).data() ) ) {
        return 0;
    }
    return xine_play( m_stream, startPos, start_time);
}

void Lib::stop() {
    assert( m_initialized );

    qDebug("<<<<<<<< STOP IN LIB TRIGGERED >>>>>>>");
    xine_stop( m_stream );
}

void Lib::pause( bool toggle ) {
    assert( m_initialized );

    xine_set_param( m_stream, XINE_PARAM_SPEED, toggle ? XINE_SPEED_PAUSE : XINE_SPEED_NORMAL );
}

int Lib::speed() const {
    assert( m_initialized );

    return  xine_get_param ( m_stream, XINE_PARAM_SPEED );
}

void Lib::setSpeed( int speed ) {
    assert( m_initialized );

    xine_set_param ( m_stream, XINE_PARAM_SPEED, speed );
}

int Lib::status() const {
    assert( m_initialized );

    return xine_get_status( m_stream );
}

int Lib::currentPosition() const {
    assert( m_initialized );

    int pos, time, length;
    xine_get_pos_length( m_stream, &pos, &time, &length );
    return pos;
}

int Lib::currentTime() const {
    assert( m_initialized );

    int pos, time, length;
    xine_get_pos_length( m_stream, &pos, &time, &length );
    return time/1000;
}

int Lib::length() const {
      assert( m_initialized );

      int pos, time, length;
      xine_get_pos_length( m_stream, &pos, &time, &length );
      return length/1000;
}

bool Lib::isSeekable() const {
    assert( m_initialized );

    return xine_get_stream_info( m_stream, XINE_STREAM_INFO_SEEKABLE );
}

void Lib::seekTo( int time ) {
    assert( m_initialized );

    //xine_trick_mode ( m_stream, XINE_TRICK_MODE_SEEK_TO_TIME, time ); NOT IMPLEMENTED YET IN XINE :_(
    // since its now milliseconds we need *1000
    xine_play( m_stream, 0, time*1000 );
}


Frame Lib::currentFrame() const {
    assert( m_initialized );

    Frame frame;
    return frame;
};

QString Lib::metaInfo( int number) const {
    assert( m_initialized );

    return xine_get_meta_info( m_stream, number );
}

int Lib::error() const {
    assert( m_initialized );

    return xine_get_error( m_stream );
};

void Lib::receiveMessage( ThreadUtil::ChannelMessage *msg, SendType sendType )
{
    assert( sendType == ThreadUtil::Channel::OneWay );
    handleXineEvent( msg->type() );
    delete msg;
}

void Lib::handleXineEvent( const xine_event_t* t ) {
    send( new ThreadUtil::ChannelMessage( t->type ), OneWay );
}

void Lib::handleXineEvent( int type ) {
    assert( m_initialized );

    if ( type == XINE_EVENT_UI_PLAYBACK_FINISHED ) {
        emit stopped();
    }
}


void Lib::setShowVideo( bool video ) {
    assert( m_initialized );

    m_video = video;
    ::null_set_show_video( m_videoOutput, video );
}

bool Lib::isShowingVideo() const {
    assert( m_initialized );

    return ::null_is_showing_video( m_videoOutput );
}

bool Lib::hasVideo() const {
    assert( m_initialized );

    return  xine_get_stream_info( m_stream, 18 );
}

void Lib::showVideoFullScreen( bool fullScreen ) {
    assert( m_initialized );

    ::null_set_fullscreen( m_videoOutput, fullScreen );
}

bool Lib::isVideoFullScreen() const {
    assert( m_initialized );

    return ::null_is_fullscreen( m_videoOutput );
}

void Lib::setScaling( bool scale ) {
    assert( m_initialized );

    ::null_set_scaling( m_videoOutput, scale );
}

void Lib::setGamma( int value ) {
    assert( m_initialized );

  //qDebug( QString( "%1").arg(value)  );
  /* int gammaValue = ( 100 + value ); */
  ::null_set_videoGamma( m_videoOutput, value );
}

bool Lib::isScaling() const {
    assert( m_initialized );

    return ::null_is_scaling( m_videoOutput );
}

void Lib::xine_event_handler( void* user_data, const xine_event_t* t ) {
    ( (Lib*)user_data)->handleXineEvent( t );
}

void Lib::xine_display_frame( void* user_data, uint8_t *frame,
                              int width,  int height,  int bytes ) {
    ( (Lib*)user_data)->drawFrame( frame, width, height, bytes );
}

void Lib::drawFrame( uint8_t* frame,  int width,  int height,  int bytes ) {
    assert( m_initialized );

    if ( !m_video ) {
        qWarning("not showing video now");
        return;
    }
    m_wid-> setVideoFrame ( frame, width, height, bytes );
}
