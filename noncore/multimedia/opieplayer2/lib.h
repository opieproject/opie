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

#ifndef ZECKEXINELIB_H
#define ZECKEXINELIB_H

#include <qcstring.h>
#include <qstring.h>
#include <qobject.h>

#include <xine.h>
//#include "xine.h"

#include "threadutil.h"

class XineVideoWidget;

namespace XINE {

    /**
     * Lib wrapps the simple interface
     * of libxine for easy every day use
     * This will become a full C++ Wrapper
     * It supports playing, pausing, info,
     * stooping, seeking.
     */
    class Frame;
    class Lib : public ThreadUtil::Channel, private ThreadUtil::Thread
    {
        Q_OBJECT
    public:
        enum InitializationMode { InitializeImmediately, InitializeInThread };

        Lib( InitializationMode initMode, XineVideoWidget* = 0);

        ~Lib();
        static int majorVersion();
        static int minorVersion();
        static int subVersion();


        void resize ( const QSize &s );

        int play( const QString& fileName,
                  int startPos = 0,
                  int start_time = 0 );
        void stop();
        void pause( bool toggle );

        int speed() const;

        /**
         * Set the speed of the stream, if codec supports it
         * XINE_SPEED_PAUSE                   0
         * XINE_SPEED_SLOW_4                  1
         * XINE_SPEED_SLOW_2                  2
         * XINE_SPEED_NORMAL                  4
         * XINE_SPEED_FAST_2                  8
         *XINE_SPEED_FAST_4                  16
         */
        void setSpeed( int speed = XINE_SPEED_PAUSE );

        int status() const;

        int currentPosition()const;
        //in seconds
        int currentTime()const;

        int length() const;

        bool isSeekable()const;

        /**
         * Whether or not to show video output
         */
        void setShowVideo(bool video);

        /**
         * is we show video
         */
        bool isShowingVideo() const;

        /**
         *
         */
        void showVideoFullScreen( bool fullScreen );

        /**
         *
         */
        bool isVideoFullScreen() const;


        /**
         * Get the meta info (like author etc) from the stream
         * XINE_META_INFO_TITLE               0
         * XINE_META_INFO_COMMENT             1
         * XINE_META_INFO_ARTIST              2
         * XINE_META_INFO_GENRE               3
         * XINE_META_INFO_ALBUM               4
         * XINE_META_INFO_YEAR                5
         * XINE_META_INFO_VIDEOCODEC          6
         * XINE_META_INFO_AUDIOCODEC          7
         * XINE_META_INFO_SYSTEMLAYER         8
         * XINE_META_INFO_INPUT_PLUGIN        9
         */
        QString metaInfo( int number ) const;

        /**
         *
         */
        bool isScaling() const;

        /**
         * seek to a position
         */
        void seekTo( int time );

        /**
         *
         * @return is media stream has video
         */
        bool hasVideo() const;

        /**
         *
         */
        void setScaling( bool );

        /**
         * Set the Gamma value for video output
         * @param int the value between -100 and 100, 0 is original
         */
        void setGamma( int );

        /**
         * test
         */
        Frame currentFrame() const;

        /**
         * Returns the error code
         * XINE_ERROR_NONE                    0
         * XINE_ERROR_NO_INPUT_PLUGIN         1
         * XINE_ERROR_NO_DEMUXER_PLUGIN       2
         * XINE_ERROR_DEMUXER_FAILED          3
         */
        int error() const;

        void ensureInitialized();

    signals:

        void stopped();

        void initialized();

    protected:
        virtual void receiveMessage( ThreadUtil::ChannelMessage *msg, SendType sendType );

        virtual void run();

    private:
        void initialize();

        int m_bytes_per_pixel;
        bool m_initialized:1;
        bool m_duringInitialization:1;
        bool m_video:1;
        XineVideoWidget *m_wid;
        xine_t *m_xine;
	xine_stream_t *m_stream;
        xine_cfg_entry_t *m_config;
        xine_vo_driver_t *m_videoOutput;
        xine_ao_driver_t* m_audioOutput;
	xine_event_queue_t *m_queue;

        void handleXineEvent( const xine_event_t* t );
        void handleXineEvent( int type );
        void drawFrame( uint8_t* frame, int width, int height, int bytes );
        // C -> C++ bridge for the event system
        static void xine_event_handler( void* user_data, const xine_event_t* t);
        static void xine_display_frame( void* user_data, uint8_t* frame ,
                                        int width, int height, int bytes );
    };
};


#endif
