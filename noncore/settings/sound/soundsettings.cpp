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
#include "soundsettings.h"

#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
#include <qapplication.h>
#include <qslider.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>


SoundSettings::SoundSettings( QWidget* parent,  const char* name, WFlags fl )
    : SoundSettingsBase( parent, name, TRUE, fl )
{
    Config config( "Sound" );
    config.setGroup( "System" );
    volume->setValue(100-config.readNumEntry("Volume"));
    mic->setValue(100-config.readNumEntry("Mic"));
    touchsound->setChecked(config.readBoolEntry("Touch"));
    keysound->setChecked(config.readBoolEntry("Key"));

    config.setGroup("Record");
    int rate=config.readNumEntry("SampleRate", 11025);
    if(rate == 11025)
        sampleRate->setCurrentItem(0);
    else if(rate == 22050)
        sampleRate->setCurrentItem(1);
    else if(rate==44100)
        sampleRate->setCurrentItem(2);

    stereoCheckBox->setChecked(config.readNumEntry("Stereo", 0));
  sixteenBitCheckBox->setChecked(config.readNumEntry("SixteenBit", 0));

    connect(volume, SIGNAL(valueChanged(int)), this, SLOT(setVolume(int)));
    connect(mic, SIGNAL(valueChanged(int)), this, SLOT(setMic(int)));
    connect(qApp, SIGNAL( volumeChanged(bool) ), this, SLOT( volumeChanged(bool) ) );
  connect(qApp, SIGNAL( micChanged(bool) ), this, SLOT ( micChanged(bool) ) );
}

void SoundSettings::reject()
{
    Config config( "Sound" );
    config.setGroup( "System" );
    setVolume(100-config.readNumEntry("Volume"));
  setMic(100-config.readNumEntry("Mic"));

  config.setGroup("Record");
    int rate=config.readNumEntry("SampleRate", 11025);
    if(rate == 11025)
        sampleRate->setCurrentItem(0);
    else if(rate == 22050)
        sampleRate->setCurrentItem(1);
    else if(rate==44100)
        sampleRate->setCurrentItem(2);
  stereoCheckBox->setChecked(config.readNumEntry("Stereo", 0));
  sixteenBitCheckBox->setChecked(config.readNumEntry("SixteenBit", 0));

    QDialog::reject();
}

void SoundSettings::accept()
{
    Config config( "Sound" );
    config.setGroup( "System" );
    config.writeEntry("Volume",100-volume->value());
    config.writeEntry("Mic",100-mic->value());
    config.writeEntry("Touch",touchsound->isChecked());
    config.writeEntry("Key",keysound->isChecked());

    setVolume(volume->value());
  setMic(mic->value());

  config.setGroup("Record");
  config.writeEntry("SampleRate",sampleRate->currentText());
  config.writeEntry("Stereo",stereoCheckBox->isChecked());
  config.writeEntry("SixteenBit",sixteenBitCheckBox->isChecked());
  
    QDialog::accept();
}

void SoundSettings::setVolume(int v)
{
    Config config( "Sound" );
    config.setGroup( "System" );
    config.writeEntry("Volume",100-v);
#if ( defined Q_WS_QWS || defined(_WS_QWS_) ) && !defined(QT_NO_COP)
  QCopEnvelope( "QPE/System", "volumeChange(bool)" ) << FALSE;
#endif
}

void SoundSettings::setMic(int m)
{
    Config config( "Sound" );
    config.setGroup( "System" );
    config.writeEntry("Mic",100-m);
#if ( defined Q_WS_QWS || defined(_WS_QWS_) ) && !defined(QT_NO_COP)
    QCopEnvelope( "QPE/System", "micChange(bool)" ) << FALSE;
#endif
}

void SoundSettings::volumeChanged( bool )
{
  Config config( "Sound" );
  config.setGroup( "System" );
  volume->setValue(100-config.readNumEntry("Volume"));
}

void SoundSettings::micChanged( bool )
{
  Config config( "Sound" );
  config.setGroup( "System" );
  mic->setValue(100-config.readNumEntry("Mic"));
}
