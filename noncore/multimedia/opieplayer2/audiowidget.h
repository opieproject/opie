/*
                            This file is part of the Opie Project

                             Copyright (c)  2002 Max Reiss <harlekin@handhelds.org>
                             Copyright (c)  2002 L. Potter <ljp@llornkcor.com>
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

#ifndef AUDIO_WIDGET_H
#define AUDIO_WIDGET_H

#include <qwidget.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qslider.h>
#include <qframe.h>
#include <qlineedit.h>


class QPixmap;

enum AudioButtons {
    AudioPlay=0,
    AudioStop,
    AudioNext,
    AudioPrevious,
    AudioVolumeUp,
    AudioVolumeDown,
    AudioLoop,
    AudioPlayList,
    AudioForward,
    AudioBack
};


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
    void setTickerText( const QString &text ) { songInfo.setText( text ); }
public slots:
    void updateSlider( long, long );
    void sliderPressed( );
    void sliderReleased( );
//    void setPaused( bool b)  { setToggleButton( AudioPause, b ); }
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
    void forwardClicked();
    void backClicked();
    void forwardReleased();
    void backReleased();
    void sliderMoved(long);

protected:
    void doBlank();
    void doUnblank();
    void paintEvent( QPaintEvent *pe );
    void showEvent( QShowEvent *se );
    void resizeEvent( QResizeEvent *re );
    void mouseMoveEvent( QMouseEvent *event );
    void mousePressEvent( QMouseEvent *event );
    void mouseReleaseEvent( QMouseEvent *event );
    void timerEvent( QTimerEvent *event );
    void closeEvent( QCloseEvent *event );
    void keyReleaseEvent( QKeyEvent *e);
private slots:
    void skipFor();
    void skipBack();
    void stopSkip();
private:
    void toggleButton( int );
    void setToggleButton( int, bool );
    void paintButton( QPainter *p, int i );
    int skipDirection;
    QString skin;
    QPixmap *pixBg;
    QImage  *imgUp;
    QImage  *imgDn;
    QImage  *imgButtonMask;
    QBitmap *masks[10];
    QPixmap *buttonPixUp[10];
    QPixmap *buttonPixDown[10];

    QPixmap *pixmaps[4];
    Ticker  songInfo;
    QSlider slider;
    QLineEdit time;
    int xoff, yoff;
};


#endif // AUDIO_WIDGET_H

