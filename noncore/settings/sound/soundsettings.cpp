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
#include "soundsettings.h"

#include <qpe/config.h>
#ifdef QWS
#include <qpe/qcopenvelope_qws.h>
#endif

#include <qslider.h>
#include <qcheckbox.h>

SoundSettings::SoundSettings( QWidget* parent,  const char* name, WFlags fl )
    : SoundSettingsBase( parent, name, TRUE, fl )
{
    Config config( "Sound" );

    config.setGroup( "System" );
    volume->setValue(100-config.readNumEntry("Volume"));
    touchsound->setChecked(config.readBoolEntry("Touch"));
    keysound->setChecked(config.readBoolEntry("Key"));

    connect(volume, SIGNAL(valueChanged(int)), this, SLOT(setVolume(int)));
}

void SoundSettings::reject()
{
    Config config( "Sound" );
    config.setGroup( "System" );
    setVolume(100-config.readNumEntry("Volume"));

    QDialog::reject();
}

void SoundSettings::accept()
{
    Config config( "Sound" );
    config.setGroup( "System" );
    config.writeEntry("Volume",100-volume->value());
    config.writeEntry("Touch",touchsound->isChecked());
    config.writeEntry("Key",keysound->isChecked());
    setVolume(volume->value());
    QDialog::accept();
}

void SoundSettings::done(int r) { 
  QDialog::done(r);
  close();
}

void SoundSettings::setVolume(int v)
{
#ifndef QT_NO_COP
    QCopEnvelope( "QPE/System", "setVolume(int,int)" ) << 0 << 100-v;
#endif
}
