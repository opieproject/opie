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
#ifndef __VOLUME_APPLET_H__
#define __VOLUME_APPLET_H__


#include <qwidget.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qguardedptr.h>
#include <qtimer.h>

class QSlider;
class QCheckBox;

class VolumeControl : public QFrame
{
    Q_OBJECT
public:
    VolumeControl( bool showMic=FALSE, QWidget *parent=0, const char *name=0 );

public:
    QSlider *slider;
	QSlider *mic;
    QCheckBox *muteBox;

private:
    void keyPressEvent( QKeyEvent * );
	void createView(bool showMic = FALSE);
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
    void micChanged( bool muted );

    void setVolume( int percent );
    void setMic( int percent );

    void sliderMoved( int percent );
    void micMoved( int percent );
    void mute( bool );

	void showVolControl(bool showMic = FALSE);
	void advVolControl();

private:
    void readSystemVolume();
	void readSystemMic();

    void writeSystemVolume();
    void writeSystemMic();

	void keyPressEvent ( QKeyEvent * e );
    void mousePressEvent( QMouseEvent * );
	void mouseReleaseEvent( QMouseEvent *);
    void paintEvent( QPaintEvent* );

private:
    int volumePercent, micPercent;
    bool muted, micMuted;
    QPixmap volumePixmap;
    QTimer *advancedTimer;
};


#endif // __VOLUME_APPLET_H__

