/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef AUDIO_WIDGET_H
#define AUDIO_WIDGET_H

#include <qwidget.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qstring.h>
#include <qslider.h>
#include <qlineedit.h>
#include <qframe.h>

#include <opie/oticker.h>

class QPixmap;

enum AudioButtons {
    AudioPlay,
    AudioStop,
    AudioPause,
    AudioNext,
    AudioPrevious,
    AudioVolumeUp,
    AudioVolumeDown,
    AudioLoop,
    AudioPlayList
};


//#define USE_DBLBUF

class AudioWidget : public QWidget {
    Q_OBJECT
public:
    AudioWidget( QWidget* parent=0, const char* name=0, WFlags f=0 );
    ~AudioWidget();
    void setTickerText( const QString &text ) { songInfo.setText( text ); }
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
    void resizeEvent( QResizeEvent *re );
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
    QString skin;
    QPixmap *pixBg;
    QImage  *imgUp;
    QImage  *imgDn;
    QImage  *imgButtonMask;
    QBitmap *masks[11];
    QPixmap *buttonPixUp[11];
    QPixmap *buttonPixDown[11];
    
    QPixmap *pixmaps[4];
    OTicker  songInfo;
    QSlider slider;
    QLineEdit time;
    int xoff, yoff;
    
};


#endif // AUDIO_WIDGET_H

