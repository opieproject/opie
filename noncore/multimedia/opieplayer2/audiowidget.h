#ifndef AUDIO_WIDGET_H
#define AUDIO_WIDGET_H

#include <qwidget.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qslider.h>
#include <qframe.h>


class QPixmap;

enum AudioButtons {
    AudioPlay=0,
    AudioStop,
    AudioPause,
    AudioNext,
    AudioPrevious,
    AudioVolumeUp,
    AudioVolumeDown,
    AudioLoop,
    AudioPlayList
};


#define USE_DBLBUF


class Ticker : public QFrame {
    Q_OBJECT

public:
    Ticker( QWidget* parent=0 );
    ~Ticker();
    void setText( const QString& text ) ;

protected:
    void timerEvent( QTimerEvent * );
    void drawContents( QPainter *p );
private:
    QString scrollText;
    int pos, pixelLen;
};


class AudioWidget : public QWidget {
    Q_OBJECT
public:
    AudioWidget( QWidget* parent=0, const char* name=0, WFlags f=0 );
    ~AudioWidget();
    void setTickerText( const QString &text ) { songInfo->setText( text ); }
    bool isStreaming;
public slots:
    void updateSlider( long, long );
    void sliderPressed( );
    void sliderReleased( );
    void setPaused( bool b)  { setToggleButton( AudioPause, b ); }
    void setLooping( bool b) { setToggleButton( AudioLoop, b ); }
    void setPlaying( bool b) { setToggleButton( AudioPlay, b ); }
    void setPosition( long );
    void setLength( long );
    void setView( char );

signals:
    void moreClicked();
    void lessClicked();
    void moreReleased();
    void lessReleased();
    void sliderMoved(long);

protected:
    void doBlank();
    void doUnblank();
    void paintEvent( QPaintEvent *pe );
    void showEvent( QShowEvent *se );
    void mouseMoveEvent( QMouseEvent *event );
    void mousePressEvent( QMouseEvent *event );
    void mouseReleaseEvent( QMouseEvent *event );
    void timerEvent( QTimerEvent *event );
    void closeEvent( QCloseEvent *event );
    void keyReleaseEvent( QKeyEvent *e);
private:
    void toggleButton( int );
    void setToggleButton( int, bool );
    void paintButton( QPainter *p, int i );
    QPixmap *pixmaps[4];
    Ticker *songInfo;
    QSlider *slider;
};


#endif // AUDIO_WIDGET_H

