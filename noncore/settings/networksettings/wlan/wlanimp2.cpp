#include "wlanimp2.h"
#include "keyedit.h"
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

#ifdef QWS
 #include <opie/oprocess.h>
#else
 #define OProcess KProcess
 #include <kprocess.h>
#endif

#define WIRELESS_OPTS "/etc/pcmcia/wireless.opts"
#define PREUP "/etc/netwrok/if-pre-up.d/wireless-tools"

/**
 * Constructor, read in the wireless.opts file for parsing later.
 */
WLANImp::WLANImp( QWidget* parent, const char* name, Interface *i, bool modal, WFlags fl) : WLAN(parent, name, modal, fl), interface(i), currentProfile("*") {
  interfaces = new Interfaces();
  interfaceSetup = new InterfaceSetupImp(tabWidget, "InterfaceSetupImp", i, interfaces);
  tabWidget->insertTab(interfaceSetup, "TCP/IP");

  // Check sanity - the existance of the wireless-tools if-pre-up script
  QFile file(QString(PREUP));
  if (file.exists()) {
    qWarning(QString("WLANImp: Unable to open /etc/network/if-pre-up.d/wireless-tools"));
  }
}

WLANImp::~WLANImp() {
//FIXME:  delete interfaces;
}

/**
 * Change the profile for both wireless settings and network settings.
 */
void WLANImp::setProfile(const QString &profile){
  interfaceSetup->setProfile(profile);
  parseOpts();
}

void WLANImp::parseOpts() {
  bool error;
  QString opt;

  if (! interfaces->isInterfaceSet())
    return;


  opt = interfaces->getInterfaceOption("wireless_essid", error);
  if(opt == "any" || opt == "off" || opt.isNull()){
    essid->setEditText("any");
  } else {
    essid->setEditText(opt);
  }

  opt = interfaces->getInterfaceOption("wireless_mode", error).simplifyWhiteSpace();
  if (opt == "Auto") {
    mode->setCurrentItem(0);
  } else if (opt == "Ad-Hoc") {
    mode->setCurrentItem(2);
  } else {
    // Managed/Infrastructure mode
    mode->setCurrentItem(1);
  }

  opt = interfaces->getInterfaceOption("wireless_ap", error).simplifyWhiteSpace();
  if (! opt.isNull()) {
    specifyAp->setChecked(true);
    macEdit->setText(opt);
  }

  opt = interfaces->getInterfaceOption("wireless_channel", error).simplifyWhiteSpace();
  if (! opt.isNull()) {
    specifyChan->setChecked(true);
    networkChannel->setValue(opt.toInt());
  }

  opt = interfaces->getInterfaceOption("wireless_key", error).simplifyWhiteSpace();
  if (opt.isNull())
    opt = interfaces->getInterfaceOption("wireless_enc", error).simplifyWhiteSpace();
  parseKeyStr(opt);
}

void WLANImp::parseKeyStr(QString keystr) {
  int loc = 0;
  int index = 1;
  QString key;
  QStringList keys = QStringList::split(QRegExp("\\s+"), keystr);
  int enc = -1; // encryption state

  for (QStringList::Iterator it = keys.begin(); it != keys.end(); ++it) {
    if ((*it).left(3) == "off") {
      // encryption disabled
      enc = 0;
    } else if ((*it).left(2) == "on") {
      // encryption enabled
      enc = 1;
    } else if ((*it).left(4) == "open") {
      // open mode, accept non encrypted packets
      acceptNonEnc->setChecked(true);
    } else if ((*it).left(10) == "restricted") {
      // restricted mode, only accept encrypted packets
      rejectNonEnc->setChecked(true);
    } else if ((*it).left(3) == "key") {
      // new set of options
    } else if ((*it).left(1) == "[") {
      index = (*it).mid(1, 1).toInt();
      // switch current key to index
      switch (index) {
        case 1:
          keyRadio0->setChecked(true);
	  break;
        case 2:
          keyRadio1->setChecked(true);
	  break;
        case 3:
          keyRadio2->setChecked(true);
	  break;
        case 4:
          keyRadio3->setChecked(true);
	  break;
      }
    } else {
      // key
      key = (*it);
    }
    if (! key.isNull()) {
      if (enc == -1)
        enc = 1;
      QStringList::Iterator next = ++it;
      if (it == keys.end()) {
        break;
      }
      if ((*(next)).left(1) == "[") {
        // set key at index
        index = (*(next)).mid(1, 1).toInt();
      } else {
        index = 1;
      }
      switch (index) {
          case 1:
            keyLineEdit0->setText(key);
	    break;
          case 2:
            keyLineEdit1->setText(key);
	    break;
          case 3:
            keyLineEdit2->setText(key);
	    break;
          case 4:
            keyLineEdit3->setText(key);
	    break;
      }
      key = QString::null;
    }
  }
  if (enc == 1) {
    wepEnabled->setChecked(true);
  } else {
    wepEnabled->setChecked(false);
  }
}

/**
 * Check to see if the current config is valid
 * Save interfaces
 */
void WLANImp::accept() {
  if (wepEnabled->isChecked()) {
    if ((keyRadio0->isChecked() && keyLineEdit0->text().isEmpty()) ||
        (keyRadio1->isChecked() && keyLineEdit1->text().isEmpty()) ||
        (keyRadio2->isChecked() && keyLineEdit2->text().isEmpty()) ||
        (keyRadio3->isChecked() && keyLineEdit3->text().isEmpty())) {
      QMessageBox::information(this, "Error", "Please enter a WEP key.", QMessageBox::Ok);
      return;
    }
  }

  if (essid->currentText().isEmpty()) {
    QMessageBox::information(this, "Error", "Please select/enter an ESSID.", QMessageBox::Ok);
    return;
  }

  if (specifyAp->isChecked() && macEdit->text().isEmpty()) {
    QMessageBox::information(this, "Error", "Please enter the MAC address of the Access Point.", QMessageBox::Ok);
    return;
  }

  // Try to save the interfaces settings.
  writeOpts();

  // Close out the dialog
// FIXME:  QDialog::accept();
}

void WLANImp::writeOpts() {
    // eh can't really do anything about it other then return. :-D
    if(!interfaces->isInterfaceSet()){
        QMessageBox::warning(0,"Inface not set","should not happen!!!");
        return;
    }
    bool error = false;

    qDebug("setting wlan interface %s", interfaces->getInterfaceName( error ).latin1() );

    if (error)  QMessageBox::warning(0,"Inface not set","should not happen!!!");

  interfaces->setInterfaceOption(QString("wireless_mode"), mode->currentText());
  interfaces->setInterfaceOption(QString("wireless_essid"), essid->currentText());

  if (specifyAp->isChecked()) {
    interfaces->setInterfaceOption(QString("wireless_ap"), macEdit->text());
  } else {
    interfaces->removeInterfaceOption(QString("wireless_ap"));
  }

  if (specifyChan->isChecked()) {
    interfaces->setInterfaceOption(QString("wireless_channel"), networkChannel->text());
  } else {
    interfaces->removeInterfaceOption(QString("wireless_channel"));
  }

  if (wepEnabled->isChecked()) {
    QStringList keyList;

    if (! keyLineEdit0->text().isNull()) {
      keyList += keyLineEdit0->text();
      keyList += "[1]";
    } //else
    if (! keyLineEdit1->text().isNull()) {
      keyList += keyLineEdit1->text();
      keyList += "[2]";
    } //else
    if (! keyLineEdit2->text().isNull()) {
      keyList += keyLineEdit2->text();
      keyList += "[3]";
    } //else
    if (! keyLineEdit3->text().isNull()) {
      keyList += keyLineEdit3->text();
      keyList += "[4]";
    }
    if (acceptNonEnc->isChecked()) {
      keyList += "open";
    } else {
      keyList += "restricted";
    }

    keyList += "key";
    if (keyRadio0->isChecked()) {
      keyList += "[1]";
    } else if (keyRadio1->isChecked()) {
      keyList += "[2]";
    } else if (keyRadio2->isChecked()) {
      keyList += "[3]";
    } else if (keyRadio3->isChecked()) {
      keyList += "[4]";
    }
    interfaces->setInterfaceOption(QString("wireless_key"), keyList.join(QString(" ")));
  } else {
    interfaces->removeInterfaceOption(QString("wireless_key"));
  }
  interfaces->removeInterfaceOption(QString("wireless_enc"));

  if(!interfaceSetup->saveChanges())
    return;

  QDialog::accept();
}
