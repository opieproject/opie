#include "wlanimp.h"
#include "interfacesetupimp.h"

#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qtabwidget.h>
#include <qcombobox.h>

/* system() */
#include <stdlib.h>

#define WIRELESS_OPTS "/etc/pcmcia/wireless.opts"

/**
 * Constructor, read in the wireless.opts file for parsing later.
 */ 
WLANImp::WLANImp( QWidget* parent, const char* name, Interface *i, bool modal, WFlags fl):WLAN(parent, name, modal, fl), currentProfile("*") {
  interfaceSetup = new InterfaceSetupImp(tabWidget, "InterfaceSetupImp", i);
  tabWidget->insertTab(interfaceSetup, "TCP/IP");

  // Read in the config file.
  QString wlanFile = WIRELESS_OPTS;
  QFile file(wlanFile);
  if (file.open(IO_ReadOnly)){
    QTextStream stream( &file );
    QString line = "";
    while ( !stream.eof() ) {
      line += stream.readLine();
      line += "\n";
    }
    file.close();
    settingsFileText = QStringList::split("\n", line, true);
    parseSettingFile();
  }
  else
    qDebug(QString("WLANImp: Can't open file: %1 for reading.").arg(wlanFile).latin1());
  connect(networkType, SIGNAL(activated(int)), this, SLOT(typeChanged(int)));
}

void WLANImp::typeChanged(int mod){
  networkChannel->setEnabled(mod);
  channelLabel->setEnabled(mod);
}

/**
 * Change the profile for both wireless settings and network settings.
 */ 
void WLANImp::setProfile(const QString &profile){
  interfaceSetup->setProfile(profile);
  parseSettingFile();
}

/**
 * Parses the settings file that was read in and gets any setting from it.
 */ 
void WLANImp::parseSettingFile(){
  bool foundCase = false;
  bool found = false;
  for ( QStringList::Iterator it = settingsFileText.begin(); it != settingsFileText.end(); ++it ) {
    QString line = (*it).simplifyWhiteSpace();
    if(line.contains("case"))
      foundCase = true;
    // See if we found our scheme to write or the sceme couldn't be found
    if((foundCase && line.contains("esac")) || 
   (foundCase && line.left(currentProfile.length()+7) == currentProfile + ",*,*,*)" && line.at(0) != '#'))
      found = true;

    if(line.contains(";;"))
      found = false;
    if(found){
      // write out scheme
      if(line.contains("ESSID=")){
        QString id = line.mid(line.find("ESSID=")+6, line.length());
        if(id == "any"){
          essAny->setChecked(false);
        }else{ 
          essAny->setChecked(true);
	  essSpecificLineEdit->setText(id);
	}
      }
      if(line.contains("MODE=")){
        QString mode = line.mid(line.find("MODE=")+5, line.length());
        if(mode == "Managed"){
          networkType->setCurrentItem(0);
	  channelLabel->setEnabled(false);
	  networkChannel->setEnabled(false);
	}
	else{
	  networkType->setCurrentItem(1);
          networkChannel->setEnabled(true);
	  channelLabel->setEnabled(true);
	}
      }
      if(line.contains("#KEY0="))
        keyLineEdit0->setText(line.mid(6, line.length()));
      if(line.contains("#KEY1="))
        keyLineEdit1->setText(line.mid(6, line.length()));
      if(line.contains("#KEY2="))
        keyLineEdit2->setText(line.mid(6, line.length()));
      if(line.contains("#KEY3="))
        keyLineEdit3->setText(line.mid(6, line.length()));
      
      if(line.contains("KEY=")){
        wepEnabled->setChecked(true);
        QString key;
	if(line.right(5) == (" open")){
	  key = line.mid(4, line.length()-5);
	  authOpen->setChecked(true);
	  authShared->setChecked(false);
	}
	else{
	  authOpen->setChecked(false);
	  authShared->setChecked(true);
	  key = line.mid(4, line.length());
	}
        if(key == keyLineEdit0->text()) keyRadio0->setChecked(true);
	if(key == keyLineEdit1->text()) keyRadio1->setChecked(true);
	if(key == keyLineEdit2->text()) keyRadio2->setChecked(true);
	if(key == keyLineEdit3->text()) keyRadio3->setChecked(true);
      }
      if(line.contains("CHANNEL=")){
        networkChannel->setValue(line.mid(line.find("CHANNEL=")+8, line.length()).toInt());
      }
    }
  }
}

/**
 * Saves settings to the wireless.opts file using the current profile
 */ 
void WLANImp::changeAndSaveSettingFile(){
  QString wlanFile = WIRELESS_OPTS;
  QFile::remove(wlanFile);
  QFile file(wlanFile);

  if (!file.open(IO_ReadWrite)){
    qDebug(QString("WLANImp::changeAndSaveSettingFile(): Can't open file: %1 for writing.").arg(wlanFile).latin1());
    return;
  }
  
  QTextStream stream( &file );
  bool foundCase = false;
  bool found = false;
  bool output = true;
  for ( QStringList::Iterator it = settingsFileText.begin(); it != settingsFileText.end(); ++it ) {
    QString line = (*it).simplifyWhiteSpace();
    if(line.contains("case"))
      foundCase = true;
    // See if we found our scheme to write or the sceme couldn't be found
    if((foundCase && line.contains("esac") && !found) || 
   (foundCase && line.left(currentProfile.length()+7) == currentProfile + ",*,*,*)" && line.at(0) != '#')){
      // write out scheme
      found = true;
      output = false;
      
      if(!line.contains("esac"))
        stream << line << "\n"; 
      if(!essAny->isChecked() == true){
        stream << "\tESSID=any\n";
	stream << "\tMODE=Managed\n";
      }
      else{
        stream << "\tESSID=" << essSpecificLineEdit->text() << '\n';
        stream << "\tMODE=" << ( networkType->currentItem() == 0  ? "Managed" : "ad-hoc") << '\n';
        stream << "\tCHANNEL=" << networkChannel->value() << "\n";
      } 

      stream << "\t#KEY0=" << keyLineEdit0->text() << "\n";
      stream << "\t#KEY1=" << keyLineEdit1->text() << "\n";
      stream << "\t#KEY2=" << keyLineEdit2->text() << "\n";
      stream << "\t#KEY3=" << keyLineEdit3->text() << "\n";
      
      if(wepEnabled->isChecked()){
        stream << "\tKEY=\"";
        if(keyRadio0->isChecked()) stream << keyLineEdit0->text();
        if(keyRadio1->isChecked()) stream << keyLineEdit1->text();
        if(keyRadio2->isChecked()) stream << keyLineEdit2->text();
        if(keyRadio3->isChecked()) stream << keyLineEdit3->text();
	if(authOpen->isChecked())
	  stream << " open";
	else
	  stream << " restricted";
	stream << "\"\n";
      }
      stream << "\tRATE=auto\n";
      if(line.contains("esac"))
        stream << line << "\n"; 
    }
    if(line.contains(";;"))
      output = true;
    if(output && (*it).length() )
      stream << (*it) << '\n';
  }
  file.close();
}

/**
 * Check to see if the current config is valid
 * Save wireless.opts, save interfaces
 */
void WLANImp::accept(){
  if(wepEnabled->isChecked()){
    if(keyLineEdit0->text().isEmpty() && keyLineEdit1->text().isEmpty() && keyLineEdit2->text().isEmpty() && keyLineEdit3->text().isEmpty() ){
      QMessageBox::information(this, "Error", "Please enter a key for WEP.", QMessageBox::Ok);
      return;
    }
  }	
  
  if(essAny->isChecked() && essSpecificLineEdit->text().isEmpty()){
    QMessageBox::information(this, "Error", "Please enter a ESS-ID.", QMessageBox::Ok);
    return;
  }
  
  // Ok settings are good here, save
  changeAndSaveSettingFile();
  
  // Try to save the interfaces settings.
  if(!interfaceSetup->saveChanges())
    return;

  // Restart the device now that the settings have changed
  QString initpath;
  if( QDir("/etc/rc.d/init.d").exists() )
    initpath = "/etc/rc.d/init.d";
  else if( QDir("/etc/init.d").exists() )
    initpath = "/etc/init.d";
  if( initpath )
    system(QString("%1/pcmcia stop").arg(initpath));
  if( initpath )
    system(QString("%1/pcmcia start").arg(initpath));

  // Close out the dialog
  QDialog::accept();
}

// wlanimp.cpp

