

#ifndef ZECKEXINELIB_H
#define ZECKEXINELIB_H

#include <qcstring.h>
#include <qstring.h>
#include <qobject.h>

#include <xine.h>

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
    class Lib : public QObject {
        Q_OBJECT
    public:
        Lib(XineVideoWidget* = 0);
        ~Lib();
        QCString version();
        int majorVersion()/*const*/;
        int minorVersion()/*const*/;
        int subVersion()/*const*/;


		void resize ( const QSize &s );

        int play( const QString& fileName,
                  int startPos = 0,
                  int start_time = 0 );
        void stop() /*const*/;
        void pause()/*const*/;

        int speed() /*const*/;
        void setSpeed( int speed = SPEED_PAUSE );

        int status() /*const*/;

        int currentPosition()/*const*/;
        //in seconds
        int currentTime()/*const*/;
        int length() /*const*/;

        bool isSeekable()/*const*/;

        /**
         * Whether or not to show video output
         */
        void setShowVideo(bool video);

        /**
         * is we show video
         */
        bool isShowingVideo() /*const*/;

        /**
         *
         */
        void showVideoFullScreen( bool fullScreen );

        /**
         *
         */
        bool isVideoFullScreen()/*const*/ ;

        /**
         *
         */
        bool isScaling();

        /**
         *
         */
        void setScaling( bool );
        /**
         * test
         */
        Frame currentFrame()/*const*/;

        /**
         * Returns the error code
         */
        int error() /*const*/;

    signals:
        void stopped();
    private:
        int m_bytes_per_pixel;
        bool m_video:1;
        XineVideoWidget *m_wid;
        xine_t *m_xine;
        config_values_t *m_config;
        vo_driver_t *m_videoOutput;
        ao_driver_t* m_audioOutput;

        void handleXineEvent( xine_event_t* t );
        void drawFrame( uint8_t* frame, int width, int height, int bytes );
        // C -> C++ bridge for the event system
        static void xine_event_handler( void* user_data, xine_event_t* t);
        static void xine_display_frame( void* user_data, uint8_t* frame ,
                                        int width, int height, int bytes );
    };
};


#endif
