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

#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/storage.h>

#include <qmessagebox.h>
#include <qapplication.h>
#include <qslider.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>

#include <sys/utsname.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

SoundSettings::SoundSettings( QWidget* parent,  const char* objname, WFlags fl )
        : SoundSettingsBase( parent, objname, TRUE, fl )
{
    keyReset=FALSE;

    Config config( "qpe");
    config.setGroup( "Volume" );
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

    stereoCheckBox->setChecked(cfg.readNumEntry("Stereo", 0));      //TODO hide if zaurus- mono only

#if defined(QT_QWS_IPAQ) || defined(QT_QWS_EBX) //since ipaq and zaurus have particular
                                                //devices
    bool systemZaurus=FALSE;
    struct utsname name; /* check for embedix kernel running on the zaurus*/
    if (uname(&name) != -1) {// TODO change this here,...
        QString release=name.release;
        if( release.find("embedix",0,TRUE) != -1) {
            qDebug("IS System Zaurus");
            systemZaurus=TRUE;
        }
    }
    if(!systemZaurus) {
        stereoCheckBox->setChecked(TRUE);
    }
    stereoCheckBox->setEnabled(FALSE);
    sixteenBitCheckBox->setEnabled(FALSE);
#else
#endif
    int sRate=cfg.readNumEntry("SizeLimit", 30);
    qDebug("%d",sRate);

    if(sRate ==30)
        timeLimitComboBox->setCurrentItem(0);
    else if(sRate==20)
        timeLimitComboBox->setCurrentItem(1);
    else if(sRate == 15)
        timeLimitComboBox->setCurrentItem(2);
    else if(sRate == 10)
        timeLimitComboBox->setCurrentItem(3);
    else if(sRate == 5)
        timeLimitComboBox->setCurrentItem(4);
    else 
        timeLimitComboBox->setCurrentItem(5);

    sixteenBitCheckBox->setChecked(cfg.readNumEntry("SixteenBit", 1));

    cfg.setGroup("Defaults");
    keyComboBox->setCurrentItem(cfg.readNumEntry("toggleKey") );

    updateStorageCombo();
    connect( LocationComboBox,SIGNAL(activated(const QString &)),this,SLOT( setLocation(const QString &)));
    connect( keyComboBox,SIGNAL(activated(const QString &)),this,SLOT( setKeyButton(const QString &)));
    connect( timeLimitComboBox,SIGNAL(activated( const QString &)),this,SLOT( setSizeLimitButton(const QString &)));
//     connect( qApp,SIGNAL( aboutToQuit()),SLOT( cleanUp()) );
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
    list << "Documents : "+QPEApplication::documentDir();
    list << "tmp : /tmp";

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
    Config cfg("Vmemo");
    cfg.writeEntry("Alert",AlertCheckBox->isChecked());

    cfg.setGroup("Record");
    cfg.writeEntry("SampleRate",sampleRate->currentText());
    cfg.writeEntry("Stereo",stereoCheckBox->isChecked());
    cfg.writeEntry("SixteenBit",sixteenBitCheckBox->isChecked());

    if(keyReset) {
        switch ( QMessageBox::warning(this,tr("Restart"),
                                      tr("To implement a new key switch\nOpie will have to be restarted./n<B>Restart</B> Opie now?"),
                                      tr("Yes"),tr("No"),0,1,1) ) {
          case 0: 
              QCopEnvelope ("QPE/System", "restart()");
              break;
        };
    }
}

void SoundSettings::setKeyButton(const QString &name) {
    Config cfg("Vmemo");
    cfg.setGroup("Defaults");
    cfg.writeEntry( "toggleKey", keyComboBox->currentItem()  );
    keyReset = TRUE;
    if(keyComboBox->currentItem() == 1)
        cfg.writeEntry( "hideIcon", 0 );
    else
        cfg.writeEntry( "hideIcon", 1);

        
    cfg.write();
}

void SoundSettings::updateLocationCombo() {

}

void SoundSettings::setSizeLimitButton(const QString &index) {

    Config cfg("Vmemo");
    cfg.setGroup("Record");
    if(index.find("Unlimited",0,TRUE) != -1)
        cfg.writeEntry("SizeLimit", -1);
    else
        cfg.writeEntry("SizeLimit", index);
    cfg.write();    
}
