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
#include <qstring.h>
#include <qslider.h>
#include <qframe.h>


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


#define USE_DBLBUF


class Ticker : public QFrame {
    Q_OBJECT
public:
    Ticker( QWidget* parent=0 ) : QFrame( parent ) {
  setFrameStyle( WinPanel | Sunken );
  setText( "No Song" );
    }
    ~Ticker() { }
    void setText( const QString& text ) {
  pos = 0; // reset it everytime the text is changed
  scrollText = text;
  pixelLen = fontMetrics().width( scrollText );
  killTimers();
  if ( pixelLen > width() )
      startTimer( 50 );
  update();
    }
protected:
    void timerEvent( QTimerEvent * ) {
  pos = ( pos + 1 > pixelLen ) ? 0 : pos + 1;
#ifndef USE_DBLBUF
  scroll( -1, 0, contentsRect() );
#else
  repaint( FALSE );
#endif
    }
    void drawContents( QPainter *p ) {
#ifndef USE_DBLBUF
  for ( int i = 0; i - pos < width() && (i < 1 || pixelLen > width()); i += pixelLen )
      p->drawText( i - pos, 0, INT_MAX, height(), AlignVCenter, scrollText );
#else
  // Double buffering code.
  // Looks like qvfb makes it look like it flickers but I don't think it really is
  QPixmap pm( width(), height() );
  pm.fill( colorGroup().base() );
  QPainter pmp( &pm );
  for ( int i = 0; i - pos < width() && (i < 1 || pixelLen > width()); i += pixelLen )
      pmp.drawText( i - pos, 0, INT_MAX, height(), AlignVCenter, scrollText );
  p->drawPixmap( 0, 0, pm );
#endif
    }
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

