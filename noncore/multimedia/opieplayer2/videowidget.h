
#ifndef VIDEO_WIDGET_H
#define VIDEO_WIDGET_H


#include <qwidget.h>

class QPixmap;
class QSlider;

enum VideoButtons {
    VideoPrevious,
    VideoStop,
    VideoPlay,
    VideoPause,
    VideoNext,
    VideoPlayList,
    VideoFullscreen
};

class VideoWidget : public QWidget {
    Q_OBJECT
public:
    VideoWidget( QWidget* parent=0, const char* name=0, WFlags f=0 );
    ~VideoWidget();

    bool playVideo();

public slots:
    void updateSlider( long, long );
    void sliderPressed( );
    void sliderReleased( );
    void setPaused( bool b)      { setToggleButton( VideoPause, b ); }
    void setPlaying( bool b)     { setToggleButton( VideoPlay, b ); }
    void setFullscreen( bool b ) { setToggleButton( VideoFullscreen, b ); }
    void makeVisible();
    void setPosition( long );
    void setLength( long );
    void setView( char );

signals:
    void sliderMoved( long );

protected:
    void paintEvent( QPaintEvent *pe );
    void mouseMoveEvent( QMouseEvent *event );
    void mousePressEvent( QMouseEvent *event );
    void mouseReleaseEvent( QMouseEvent *event );
    void closeEvent( QCloseEvent *event );
    void keyReleaseEvent( QKeyEvent *e);

private:
    void paintButton( QPainter *p, int i );
    void toggleButton( int );
    void setToggleButton( int, bool );

    QSlider *slider;
    QPixmap *pixmaps[3];
    QImage *currentFrame;
    int scaledWidth;
    int scaledHeight;
};

#endif // VIDEO_WIDGET_H



