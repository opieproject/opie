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


#include <qframe.h>

class QPixmap;
class QTimer;
class QSlider;
class QCheckBox;
class QButton;
class OLedBox;

class VolumeApplet;

class VolumeControl : public QFrame {
	Q_OBJECT
	
public:
	VolumeControl ( VolumeApplet *icon, bool showMic = false, QWidget *parent=0, const char *name=0 );

	bool volMuted ( ) const;
	int volPercent ( ) const;

	virtual void show ( bool showmic );

protected:
	virtual void keyPressEvent ( QKeyEvent * e );
	
protected slots:
	void volumeChanged ( bool muted );
	void micChanged ( bool muted );
	
private slots:
	void volMoved ( int percent );
	void micMoved ( int percent );
	void alarmMoved ( int percent );
	void bassMoved( int percent );
	void trebleMoved( int percent );
	
	void volMuteToggled ( bool );
	void micMuteToggled ( bool );
	void alarmSoundToggled ( bool );
	void keyClickToggled ( bool );
	void screenTapToggled ( bool ); 

	void buttonChanged ( );
	void rateTimerDone ( );
	
private:
	void readConfig ( bool force = false );

	enum eUpdate {
		UPD_None,
		UPD_Vol,
		UPD_Mic,
		UPD_Bass,
		UPD_Treble
	};
	void writeConfigEntry ( const char *entry, int val, eUpdate upd );


private:
	QSlider *volSlider;
	QSlider *bassSlider;
	QSlider *trebleSlider;
	QSlider *micSlider;
	QSlider *alarmSlider;
	OLedBox *volLed;
	OLedBox *micLed;
	OLedBox *alarmLed;
	
	QCheckBox *alarmBox;
	QCheckBox *tapBox;
	QCheckBox *keyBox;
	QPushButton *upButton;
	QPushButton *downButton;
	QTimer *rateTimer;

	int  m_vol_percent;
	int  m_mic_percent;
	int  m_alarm_percent;
	int  m_bass_percent;
	int  m_treble_percent;
	bool m_vol_muted;
	bool m_mic_muted;	
	bool m_snd_alarm;
	bool m_snd_touch;
	bool m_snd_key;

	VolumeApplet *m_icon;	
};

class VolumeApplet : public QWidget {
	Q_OBJECT

public:
	VolumeApplet ( QWidget *parent = 0, const char *name=0 );
	~VolumeApplet ( );

	void redraw ( bool all = true );

protected:
	virtual void mousePressEvent ( QMouseEvent * );
	virtual void paintEvent ( QPaintEvent* );
	
private:
	QPixmap *      m_pixmap;
	VolumeControl *m_dialog;
};


#endif // __VOLUME_APPLET_H__

