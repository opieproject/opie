/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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
#ifndef __VOLUME_APPLET_H__
#define __VOLUME_APPLET_H__


#include <qwidget.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qguardedptr.h>

class QSlider;
class QCheckBox;
class QPushButton;
class QTimer;

class VolumeControl : public QFrame
{
    Q_OBJECT
public:
    VolumeControl( QWidget *parent=0, const char *name=0 );

public:
    QSlider *slider;
    QCheckBox *muteBox;

private:
    void keyPressEvent( QKeyEvent * );

private slots:
    void ButtonChanged();
    void rateTimerDone();

private:
    QPushButton *upButton;
    QPushButton *downButton;
    QTimer *rateTimer;
};

class VolumeApplet : public QWidget
{
    Q_OBJECT
public:
    VolumeApplet( QWidget *parent = 0, const char *name=0 );
    ~VolumeApplet();
    bool isMute( ) { return muted; }
    int percent( ) { return volumePercent; }

public slots:
    void volumeChanged( bool muted );
    void setVolume( int percent );
    void sliderMoved( int percent );
    void mute( bool );

private:
    void readSystemVolume();
    void writeSystemVolume();
    void mousePressEvent( QMouseEvent * );
    void paintEvent( QPaintEvent* );

private:
    int volumePercent;
    bool muted;
    QPixmap volumePixmap;
};


#endif // __VOLUME_APPLET_H__
