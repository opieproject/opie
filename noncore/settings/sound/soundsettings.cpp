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
// parts copyright 2002 L.J. Potter

#include "soundsettings.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/storage.h>
using namespace Opie::Core;

/* QT */
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qtoolbutton.h>
#include <qsound.h>
#include <qlistbox.h>

/* STD */
#include <sys/utsname.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>


SoundSettings::SoundSettings( QWidget* parent,  const char* objname, WFlags fl )
        : SoundSettingsBase( parent, objname, true, fl )
{
    Config cfg("Vmemo");
    cfg.setGroup("System");
    AlertCheckBox->setChecked( cfg.readBoolEntry("Alert", 1));

    cfg.setGroup("Record");
    int rate=cfg.readNumEntry("SampleRate", 22050);
    if(rate == 8000)
        sampleRate->setCurrentItem(0);
    else if(rate == 11025)
        sampleRate->setCurrentItem(1);
    else if(rate == 22050)
        sampleRate->setCurrentItem(2);
    else if(rate == 33075)
        sampleRate->setCurrentItem(3);
    else if(rate == 44100)
        sampleRate->setCurrentItem(4);

    stereoCheckBox->setChecked(cfg.readNumEntry("Stereo", 0));
//TODO hide if zaurus- mono only

#if defined(QT_QWS_IPAQ) || defined(QT_QWS_SL5XXX)
//since ipaq and zaurus have particular
//devices
    bool systemZaurus=false;
    struct utsname name; /* check for embedix kernel running on the zaurus*/
    if (uname(&name) != -1) {// TODO change this here,...
        QString release=name.release;
        if( release.find("embedix",0,true) != -1) {
            odebug << "IS System Zaurus" << oendl;
            systemZaurus=true;
        }
    }
    if(!systemZaurus) {
        stereoCheckBox->setChecked(true);
    }
    stereoCheckBox->setEnabled(false);
    sixteenBitCheckBox->setEnabled(false);
#endif
    sixteenBitCheckBox->setChecked(cfg.readNumEntry("SixteenBit", 1));

    int sLimit=cfg.readNumEntry("SizeLimit", 30);
    odebug << "" << sLimit << "" << oendl;
    timeLimitBox->setValue(sLimit);

    updateStorageCombo();

    cfg.setGroup("Defaults");
    adpcmCheckBox->setChecked( cfg.readBoolEntry("use_ADPCM", 0));
}

void SoundSettings::updateStorageCombo() {

    Config config( "Vmemo" );
    config.setGroup( "System" );
    QString loc = config.readEntry("RecLocation","/");
    int i = 0;
    int set = 0;
    StorageInfo storageInfo;
    QString sName, sPath;
    QStringList list;
    list << "Documents : "+QPEApplication::documentDir();
    list << "tmp : /tmp";

    const QList<FileSystem> &fs = storageInfo.fileSystems();
    QListIterator<FileSystem> it ( fs );
    for( ; it.current(); ++it ){
        const QString name = (*it)->name();
        const QString path = (*it)->path();
        odebug << "storage name "+name +" storage path is "+path << oendl;
        list << name + ": " +path;
        if( loc.find( path,0,true) != -1)
            set = i;
//   if(dit.current()->file().find(path) != -1 ) storage=name;
        i++;
    }

    LocationComboBox->insertStringList(list);
    odebug << "set item " << set << "" << oendl;
    LocationComboBox->setCurrentItem(set);
}

void SoundSettings::accept() {
    cleanUp();
    QCopEnvelope e("QPE/VMemo", "reloadSettings()");
    qApp->quit();
}

void SoundSettings::cleanUp() {
    Config cfg("Vmemo");
    cfg.setGroup("Record");
    cfg.writeEntry("SampleRate",sampleRate->currentText());
    cfg.writeEntry("Stereo",stereoCheckBox->isChecked());
    cfg.writeEntry("SixteenBit",sixteenBitCheckBox->isChecked());
    cfg.writeEntry("SizeLimit", timeLimitBox->value());
    cfg.setGroup("Defaults");
    cfg.writeEntry("use_ADPCM", adpcmCheckBox->isChecked());
    cfg.setGroup( "System" );
    cfg.writeEntry("RecLocation",LocationComboBox->currentText());
    cfg.writeEntry("Alert",AlertCheckBox->isChecked());
    cfg.write();
}
