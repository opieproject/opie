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

#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/storage.h>

#include <qapplication.h>
#include <qslider.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>


SoundSettings::SoundSettings( QWidget* parent,  const char* name, WFlags fl )
    : SoundSettingsBase( parent, name, TRUE, fl )
{
    keyReset=FALSE;

    Config config( "qpe");
    config.setGroup( "Volume" );
    volume->setValue(100-config.readNumEntry("VolumePercent"));
    mic->setValue(100-config.readNumEntry("Mic"));
//     touchsound->setChecked(config.readBoolEntry("TouchSound"));
//     keysound->setChecked(config.readBoolEntry("KeySound"));
    Config cfg("Vmemo");
    connect( qApp,SIGNAL( aboutToQuit()),SLOT( cleanUp()) );
    AlertCheckBox->setChecked(cfg.readBoolEntry("Alert"));
    
    cfg.setGroup("Record");
    int rate=config.readNumEntry("SampleRate", 22050);
    if(rate == 8000)
        sampleRate->setCurrentItem(0);
    else if(rate == 11025)
        sampleRate->setCurrentItem(1);
    else if(rate == 22050)
        sampleRate->setCurrentItem(2);
    else if(rate == 33075)
        sampleRate->setCurrentItem(3);
    else if(rate==44100)
        sampleRate->setCurrentItem(4);

    stereoCheckBox->setChecked(cfg.readNumEntry("Stereo", 0));
    sixteenBitCheckBox->setChecked(cfg.readNumEntry("SixteenBit", 1));

    cfg.setGroup("Defaults");
    keyComboBox->setCurrentItem(cfg.readNumEntry("toggleKey") );

    updateStorageCombo();

    connect(volume, SIGNAL(valueChanged(int)), this, SLOT(setVolume(int)));
    connect(mic, SIGNAL(valueChanged(int)), this, SLOT(setMic(int)));
    connect(qApp, SIGNAL( volumeChanged(bool) ), this, SLOT( volumeChanged(bool) ) );
    connect(qApp, SIGNAL( micChanged(bool) ), this, SLOT ( micChanged(bool) ) );
    connect( LocationComboBox,SIGNAL(activated(const QString &)),this,SLOT( setLocation(const QString &)));
    connect( keyComboBox,SIGNAL(activated(const QString &)),this,SLOT(setKeyButton(const QString &)));
//     connect( qApp,SIGNAL( aboutToQuit()),SLOT( cleanUp()) );
}

// void SoundSettings::reject()
// {
//   qDebug("reject");
//     Config config( "qpe");
//     config.setGroup( "Volume");

//     setVolume(100-config.readNumEntry("VolumePercent"));
//     setMic(100-config.readNumEntry("Mic"));

// //   config.setGroup("Record");
// //     int rate=config.readNumEntry("SampleRate", 11025);
// //     if(rate == 11025)
// //         sampleRate->setCurrentItem(0);
// //     else if(rate == 22050)
// //         sampleRate->setCurrentItem(1);
// //     else if(rate == 32000)
// //         sampleRate->setCurrentItem(2);
// //     else if(rate==44100)
// //         sampleRate->setCurrentItem(3);
// //   stereoCheckBox->setChecked(config.readNumEntry("Stereo", 0));
// //   sixteenBitCheckBox->setChecked(config.readNumEntry("SixteenBit", 0));
//     qDebug("QDialog::reject();");
//     ::exit(-1);
// }

// void SoundSettings::accept()
// {
//   qDebug("accept");
//   Config config( "qpe" );
//     config.setGroup( "Volume" );
//     config.writeEntry("VolumePercent",100-volume->value());
//     config.writeEntry("Mic",100-mic->value());
// //     config.writeEntry("TouchSound",touchsound->isChecked());
// //     config.writeEntry("KeySound",keysound->isChecked());

//     Config cfg("Vmemo");
//     cfg.writeEntry("Alert",AlertCheckBox->isChecked());
//     setVolume(volume->value());
//     setMic(mic->value());

//     cfg.setGroup("Record");
//     cfg.writeEntry("SampleRate",sampleRate->currentText());
//     cfg.writeEntry("Stereo",stereoCheckBox->isChecked());
//     cfg.writeEntry("SixteenBit",sixteenBitCheckBox->isChecked());
// //     Config cfg( "VMemo" );
// //     cfg.setGroup( "Defaults" );
// //     cfg.writeEntry( "hideIcon", HideIcon_CheckBox->isChecked());  
//     qDebug("QDialog::accept();");
//     ::exit(0);
// }

void SoundSettings::setVolume(int v)
{
    Config config( "qpe" );
    config.setGroup( "Volume" );
    config.writeEntry("VolumePercent",100-v);
#if ( defined Q_WS_QWS || defined(_WS_QWS_) ) && !defined(QT_NO_COP)
  QCopEnvelope( "QPE/System", "volumeChange(bool)" ) << FALSE;
#endif
}

void SoundSettings::setMic(int m)
{
    Config config( "qpe" );
    config.setGroup( "Volume" );
    config.writeEntry("Mic",100-m);
#if ( defined Q_WS_QWS || defined(_WS_QWS_) ) && !defined(QT_NO_COP)
    QCopEnvelope( "QPE/System", "micChange(bool)" ) << FALSE;
#endif
}

void SoundSettings::volumeChanged( bool )
{
    Config config( "qpe" );
    config.setGroup( "Volume" );
  volume->setValue(100-config.readNumEntry("VolumePercent"));
}

void SoundSettings::micChanged( bool )
{
    Config config( "qpe" );
    config.setGroup( "Volume" );
  mic->setValue(100-config.readNumEntry("Mic"));
}

void SoundSettings::updateStorageCombo() {

   Config config( "Vmemo" );
   config.setGroup( "System" );
   QString loc = config.readEntry("RecLocation","/");
int i=0;
int set=0; 
   StorageInfo storageInfo;
   QString sName, sPath;
   QStringList list;
   const QList<FileSystem> &fs = storageInfo.fileSystems();
    QListIterator<FileSystem> it ( fs );
        for( ; it.current(); ++it ){
            const QString name = (*it)->name();
            const QString path = (*it)->path();
            qDebug("storage name "+name +" storage path is "+path);
            list << name + ": " +path;
            if( loc.find( path,0,TRUE) != -1)
                set = i;      
//             if(dit.current()->file().find(path) != -1 ) storage=name;
            i++;
        }
        LocationComboBox->insertStringList(list);
        qDebug("set item %d", set);
        LocationComboBox->setCurrentItem(set);
}

void SoundSettings::setLocation(const QString & string) {
   Config config( "Vmemo" );
   config.setGroup( "System" );
   config.writeEntry("RecLocation",string);
   qDebug("set location "+string);
   config.write();
}

void SoundSettings::cleanUp() {
    qDebug("cleanup");
    Config config( "qpe" );
    config.setGroup( "Volume" );
    config.writeEntry("VolumePercent",100-volume->value());
    config.writeEntry("Mic",100-mic->value());
//     config.writeEntry("TouchSound",touchsound->isChecked());
//     config.writeEntry("KeySound",keysound->isChecked());

    Config cfg("Vmemo");
    cfg.writeEntry("Alert",AlertCheckBox->isChecked());
    setVolume(volume->value());
    setMic(mic->value());

    cfg.setGroup("Record");
    cfg.writeEntry("SampleRate",sampleRate->currentText());
    cfg.writeEntry("Stereo",stereoCheckBox->isChecked());
    cfg.writeEntry("SixteenBit",sixteenBitCheckBox->isChecked());

    if(keyReset) QCopEnvelope ("QPE/System", "restart()");
        
}

void SoundSettings::setKeyButton(const QString &name) {
    Config cfg("Vmemo");
    cfg.setGroup("Defaults");
    cfg.writeEntry( "toggleKey", keyComboBox->currentItem()  );
    keyReset = TRUE;
    cfg.write();
}

void SoundSettings::updateLocationCombo() {

}
