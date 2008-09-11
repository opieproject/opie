#include "wlanimp2.h"
#include "keyedit.h"
#include "interfacesetupimp.h"
#include "../interfaces/interface.h"

#include <assert.h>
#include <errno.h>
#include <string.h>

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oprocess.h>
#include <opie2/onetwork.h>
#include <opie2/opcap.h>
#include <opie2/oresource.h>
using namespace Opie::Core;
using namespace Opie::Net;

/* QT */
#include <qapplication.h>
#include <qfile.h>
#include <qdir.h>
#include <qdialog.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qtabwidget.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <qvbox.h>
#include <qprogressbar.h>
#include <qbuttongroup.h>

/* STD */
#include <assert.h>
#include <errno.h>
#include <string.h>

#define WIRELESS_OPTS "/etc/pcmcia/wireless.opts"
#define PREUP "/etc/network/if-pre-up.d/wireless-tools"

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
    owarn << QString("WLANImp: Unable to open /etc/network/if-pre-up.d/wireless-tools") << oendl;
  }

  connect( rescanButton, SIGNAL( clicked() ), this, SLOT( rescanNeighbourhood() ) );
  connect( netView, SIGNAL( clicked(QListViewItem*) ), this, SLOT( selectNetwork(QListViewItem*) ) );
  connect( encryptCombo, SIGNAL( activated(int) ), this, SLOT( encryptionChanged(int) ) );
  netView->setColumnAlignment( col_chn, AlignCenter );
  netView->setItemMargin( 3 );
  netView->setAllColumnsShowFocus( true );
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
  QString opt,key;

  if (! interfaces->isInterfaceSet())
    return;

  opt = interfaces->getInterfaceOption("wireless_essid", error);
  if(opt.isNull())
    opt = interfaces->getInterfaceOption("wireless-essid", error);
  if(opt.isNull())
    opt = interfaces->getInterfaceOption("wpa-essid", error);
  if(opt == "any" || opt == "off" || opt.isNull()){
    essid->setEditText("any");
  } else {
    essid->setEditText(opt);
  }

  opt = interfaces->getInterfaceOption("wireless_mode", error).simplifyWhiteSpace();

  for ( int i = 0; i < mode->count(); i++)
      if ( mode->text( i ) == opt ) mode->setCurrentItem( i );

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

  opt = interfaces->getInterfaceOption("wireless_type", error).simplifyWhiteSpace();

  if ( opt == "wlan-ng") {

     //FIXME:Handle wlan_ng_priv_genstr

    // get default key_id
    opt = interfaces->getInterfaceOption("wlan_ng_default_key_id", error).simplifyWhiteSpace();

    if (opt == "0")
       keyRadio0->setChecked(true);
    if (opt == "1")
       keyRadio1->setChecked(true);
    if (opt == "2")
       keyRadio2->setChecked(true);
    if (opt == "3")
       keyRadio3->setChecked(true);

    // get key0
    key = interfaces->getInterfaceOption("wlan_ng_key0", error).simplifyWhiteSpace();
    key.replace(QString(":"),QString(""));
    keyLineEdit0->setText(key);

    // get key1
    key = interfaces->getInterfaceOption("wlan_ng_key1", error).simplifyWhiteSpace();
    key.replace(QString(":"),QString(""));
    keyLineEdit1->setText(key);

    // get key2
    key = interfaces->getInterfaceOption("wlan_ng_key2", error).simplifyWhiteSpace();
    key.replace(QString(":"),QString(""));
    keyLineEdit2->setText(key);

    // get key3
    key = interfaces->getInterfaceOption("wlan_ng_key3", error).simplifyWhiteSpace();
    key.replace(QString(":"),QString(""));
    keyLineEdit3->setText(key);

    opt = interfaces->getInterfaceOption("wireless_enc", error).simplifyWhiteSpace();

    // encryption on?
    if(opt == "on"){
      encryptCombo->setCurrentItem(encWEP);
    } else {
      encryptCombo->setCurrentItem(encNone);
    }

    opt = interfaces->getInterfaceOption("wireless_keymode", error).simplifyWhiteSpace();

    if(opt == "restricted"){
      // restricted mode, only accept encrypted packets
      rejectNonEnc->setChecked(true);
    }
  }
  else {
    opt = interfaces->getInterfaceOption("wpa-psk", error).simplifyWhiteSpace();
    if(!opt.isNull()) {
      encryptCombo->setCurrentItem(encWPA);
      keyRadio0->setChecked(true);
      keyLineEdit0->setText(opt);
    }
    else {
      opt = interfaces->getInterfaceOption("wireless_key", error).simplifyWhiteSpace();
      parseKeyStr(opt);
    }
  }

  encryptionChanged( encryptCombo->currentItem() );
}

void WLANImp::parseKeyStr(QString keystr) {
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
    encryptCombo->setCurrentItem(encWEP);
  } else {
    encryptCombo->setCurrentItem(encNone);
  }
}

void WLANImp::encryptionChanged(int index)
{
  if(index == encWPA) {
    KeyButtonGroup->setEnabled(true);
    keyRadio1->setEnabled(false);
    keyLineEdit1->setEnabled(false);
    keyRadio2->setEnabled(false);
    keyLineEdit2->setEnabled(false);
    keyRadio3->setEnabled(false);
    keyLineEdit3->setEnabled(false);
    NonEncButtonGroup->setEnabled(false);
  }
  else if(index == encWEP) {
    KeyButtonGroup->setEnabled(true);
    keyRadio1->setEnabled(true);
    keyLineEdit1->setEnabled(true);
    keyRadio2->setEnabled(true);
    keyLineEdit2->setEnabled(true);
    keyRadio3->setEnabled(true);
    keyLineEdit3->setEnabled(true);
    NonEncButtonGroup->setEnabled(true);
  }
  else {
    KeyButtonGroup->setEnabled(false);
    NonEncButtonGroup->setEnabled(false);
  }
}

/**
 * Check to see if the current config is valid
 * Save interfaces
 */
void WLANImp::accept() {
  if (encryptCombo->currentItem() > 0) {
    if ((keyRadio0->isChecked() && keyLineEdit0->text().isEmpty()) ||
        (keyRadio1->isChecked() && keyLineEdit1->text().isEmpty()) ||
        (keyRadio2->isChecked() && keyLineEdit2->text().isEmpty()) ||
        (keyRadio3->isChecked() && keyLineEdit3->text().isEmpty())) {
      QMessageBox::information(this, "Error", "Please enter an encryption key.", QMessageBox::Ok);
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

QString WLANImp::formatKey(QString input)
{
  int len,r=0;

  len = input.length();

  if (!len)
    return input;

  for(int i=1;i<len/2;i++)
  {
    input.insert(r+i*2,QString(":"));
    r++;
  }

  return input;
}

void WLANImp::writeOpts() {
  QString para, devicetype;

  // eh can't really do anything about it other then return. :-D
  if(!interfaces->isInterfaceSet()){
    QMessageBox::warning(0,"Inface not set","should not happen!!!");
    return;
  }
  bool error = false;

  odebug << "setting wlan interface " << interfaces->getInterfaceName( error ).latin1() << "" << oendl;

  if (error)  QMessageBox::warning(0,"Inface not set","should not happen!!!");

  interfaces->removeInterfaceOption(QString("wireless-mode"));
  interfaces->removeInterfaceOption(QString("wireless-essid"));
  interfaces->removeInterfaceOption(QString("wireless-key"));

  interfaces->setInterfaceOption(QString("wireless_mode"), mode->currentText());
  if( encryptCombo->currentItem() == encWPA ) {
    // WPA/WPA2
    interfaces->removeInterfaceOption(QString("wireless_essid"));
    interfaces->setInterfaceOption(QString("wpa-essid"), essid->currentText());
  }
  else {
    // WEP or unencrypted
    interfaces->setInterfaceOption(QString("wireless_essid"), essid->currentText());
  }

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

  devicetype = interfaces->getInterfaceOption("wireless_type", error).simplifyWhiteSpace();

  if ( devicetype == "wlan-ng") {

    // wlan-ng style
    interfaces->removeInterfaceOption(QString("wireless_key"));

    if (encryptCombo->currentItem() > 0) {

      interfaces->setInterfaceOption(QString("wireless_enc"),"on");

      if (! keyLineEdit0->text().isNull()) {
        interfaces->setInterfaceOption(QString("wlan_ng_key0"), formatKey(keyLineEdit0->text()));
      } else
        interfaces->removeInterfaceOption(QString("wlan_ng_key0"));
      if (! keyLineEdit1->text().isNull()) {
        interfaces->setInterfaceOption(QString("wlan_ng_key1"), formatKey(keyLineEdit1->text()));
      } else
        interfaces->removeInterfaceOption(QString("wlan_ng_key1"));
      if (! keyLineEdit2->text().isNull()) {
        interfaces->setInterfaceOption(QString("wlan_ng_key2"), formatKey(keyLineEdit2->text()));
      } else
        interfaces->removeInterfaceOption(QString("wlan_ng_key2"));
      if (! keyLineEdit3->text().isNull()) {
        interfaces->setInterfaceOption(QString("wlan_ng_key3"), formatKey(keyLineEdit3->text()));
      } else
        interfaces->removeInterfaceOption(QString("wlan_ng_key3"));

      if (acceptNonEnc->isChecked())
        interfaces->removeInterfaceOption(QString("wireless_keymode"));
      else
        interfaces->setInterfaceOption(QString("wireless_keymode"),"restricted");

      para = "";
      if (keyRadio0->isChecked()) {
        para = "0";
      } else if (keyRadio1->isChecked()) {
        para = "1";
      } else if (keyRadio2->isChecked()) {
        para = "2";
      } else if (keyRadio3->isChecked()) {
        para = "3";
      }

      interfaces->setInterfaceOption(QString("wlan_ng_default_key_id"), para);

    } else {
      // No wep, remove all previous keys
      interfaces->removeInterfaceOption(QString("wireless_enc"));
      interfaces->removeInterfaceOption(QString("wlan_ng_default_key_id"));
      interfaces->removeInterfaceOption(QString("wlan_ng_key0"));
      interfaces->removeInterfaceOption(QString("wlan_ng_key1"));
      interfaces->removeInterfaceOption(QString("wlan_ng_key2"));
      interfaces->removeInterfaceOption(QString("wlan_ng_key3"));
    }

  } else {
    if( encryptCombo->currentItem() == encWPA ) {
      // WPA/WPA2
      interfaces->setInterfaceOption(QString("wpa-psk"), keyLineEdit0->text());
    }
    else if( encryptCombo->currentItem() == encWEP ) {
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
      interfaces->removeInterfaceOption(QString("wpa-psk"));
    }
    interfaces->removeInterfaceOption(QString("wireless_enc"));
  }

  if(!interfaceSetup->saveChanges())
    return;

  QDialog::accept();
}

/*
 * Scan for possible wireless networks around...
 * ... powered by Wellenreiter II technology (C) Michael 'Mickey' Lauer <mickeyl@handhelds.org>
 */

void WLANImp::rescanNeighbourhood()
{
    QString name = interface->getInterfaceName();
    odebug << "rescanNeighbourhood via '" << name << "'" << oendl;

    OWirelessNetworkInterface* wiface = static_cast<OWirelessNetworkInterface*>( ONetwork::instance()->interface( name ) );
    assert( wiface );

    // try to guess device type
    QString devicetype;
    QFile m( "/proc/modules" );
    if ( m.open( IO_ReadOnly ) )
    {
        QString line;
        QTextStream modules( &m );
        while( !modules.atEnd() && !devicetype )
        {
            modules >> line;
            if ( line.contains( "cisco" ) ) devicetype = "cisco";
            else if ( line.contains( "hostap" ) ) devicetype = "hostap";
            else if ( line.contains( "prism" ) ) devicetype = "wlan-ng"; /* puke */
            else if ( line.contains( "orinoco" ) ) devicetype = "orinoco";
        }
    }
    if ( devicetype.isEmpty() )
    {
        owarn << "rescanNeighbourhood(): couldn't guess device type :(" << oendl;
        return;
    }
    else
    {
        odebug << "rescanNeighbourhood(): device type seems to be '" << devicetype << "'" << oendl;
    }

    // configure interface to receive 802.11 management frames

    wiface->setUp( true );
    wiface->setPromiscuousMode( true );

    if ( devicetype == "cisco" ) wiface->setMonitoring( new OCiscoMonitoringInterface( wiface, false ) );
    else if ( devicetype == "hostap" ) wiface->setMonitoring( new OHostAPMonitoringInterface( wiface, false ) );
    else if ( devicetype == "wlan-ng" ) wiface->setMonitoring( new OWlanNGMonitoringInterface( wiface, false ) );
    else if ( devicetype == "orinoco" ) wiface->setMonitoring( new OOrinocoMonitoringInterface( wiface, false ) );
    else
    {
        odebug << "rescanNeighbourhood(): unsupported device type for monitoring :(" << oendl;
        return;
    }

    wiface->setMode( "monitor" );
    if ( wiface->mode() != "monitor" )
    {
        owarn << "rescanNeighbourhood(): Unable to bring device into monitor mode (" << strerror( errno ) << ")." << oendl;
        return;
    }

    // open a packet capturer
    OPacketCapturer* cap = new OPacketCapturer();
    cap->open( name );
    if ( !cap->isOpen() )
    {
        owarn << "rescanNeighbourhood(): Unable to open libpcap (" << strerror( errno ) << ")." << oendl;
        return;
    }

    // disable button and display splash screen
    rescanButton->setEnabled( false );
    QFrame* splash = new QFrame( this, "splash", false, WStyle_StaysOnTop | WStyle_DialogBorder | WStyle_Customize );
    splash->setLineWidth( 2 );
    splash->setFrameStyle( QFrame::Panel | QFrame::Raised );
    QVBoxLayout* vbox = new QVBoxLayout( splash, 4, 4 );
    QLabel* lab = new QLabel( "<center><b>Scanning...</b><br>Please Wait...</center>", splash );
    QProgressBar* pb = new QProgressBar( wiface->channels(), splash );
    vbox->addWidget( lab );
    vbox->addWidget( pb );
    pb->setCenterIndicator( true );
    pb->setFixedHeight( pb->sizeHint().height() );
    QWidget* widgetDesktop = qApp->desktop();
    int dw = widgetDesktop->width();
    int dh = widgetDesktop->height();
    int pw = vbox->sizeHint().width();
    int ph = vbox->sizeHint().height();
    splash->setGeometry((dw-pw)/2,(dh-ph)/2,pw,ph);
    splash->show();
    splash->raise();
    qApp->processEvents();

    // set capturer to non-blocking mode
    cap->setBlocking( false );

    for ( int i = 1; i <= wiface->channels(); ++i )
    {
        wiface->setChannel( i );
        pb->setProgress( i );
        qApp->processEvents();
        odebug << "rescanNeighbourhood(): listening on channel " << i << "..." << oendl;
        OPacket* p = cap->next( 1000 );
        if ( !p )
        {
            odebug << "rescanNeighbourhood(): nothing received on channel " << i << "" << oendl;
        }
        else
        {
            odebug << "rescanNeighbourhood(): TADAA - something came in on channel " << i << "" << oendl;
            handlePacket( p );
        }
    }

    cap->close();
    wiface->setMode( "managed" ); // TODO: use previous mode
    wiface->setPromiscuousMode( false );

    // hide splash screen and reenable button
    splash->hide();
    delete splash;
    rescanButton->setEnabled( true );
}

void WLANImp::handlePacket( OPacket* p )
{

    // check if we received a beacon frame
    OWaveLanManagementPacket* beacon = static_cast<OWaveLanManagementPacket*>( p->child( "802.11 Management" ) );
    if ( beacon && beacon->managementType() == "Beacon" )
    {

        QString type;
        if ( beacon->canIBSS() )
        {
            type = "adhoc";
        }
        else if ( beacon->canESS() )
        {
            type = "managed";
        }
        else
        {
            owarn << "handlePacket(): invalid frame [possibly noise] detected!" << oendl;
            return;
        }

        OWaveLanManagementSSID* ssid = static_cast<OWaveLanManagementSSID*>( p->child( "802.11 SSID" ) );
        QString essid = ssid ? ssid->ID() : QString("<unknown>");
        OWaveLanManagementDS* ds = static_cast<OWaveLanManagementDS*>( p->child( "802.11 DS" ) );
        int channel = ds ? ds->channel() : -1;
        OWaveLanPacket* header = static_cast<OWaveLanPacket*>( p->child( "802.11" ) );
        displayFoundNetwork( type, channel, essid, header->macAddress2() );
    }
}


void WLANImp::displayFoundNetwork( const QString& mode, int channel, const QString& ssid, const OMacAddress& mac )
{

    odebug << "found network: <" << (const char*) mode << ">, chn " << channel
           << ", ssid '" << (const char*) ssid << "', mac '" << (const char*) mac.toString() << "'" << oendl;

    QListViewItemIterator it( netView );
    while ( it.current() && it.current()->text( col_ssid ) != ssid ) ++it;
    if ( !it.current() ) // ssid didn't show up yet
    {
        QListViewItem* item = new QListViewItem( netView, mode.left( 1 ).upper(), ssid, QString::number( channel ), mac.toString() );
        QString name;
        name.sprintf( "networksettings/%s", (const char*) mode );
        item->setPixmap( col_mode, Opie::Core::OResource::loadPixmap( name, Opie::Core::OResource::SmallIcon ) );
        qApp->processEvents();
    }

}


void WLANImp::selectNetwork( QListViewItem* item )
{
    bool ok;
    if ( item )
    {
        specifyAp->setChecked(true);
        macEdit->setText( item->text( col_mac ) );
        specifyChan->setChecked( item->text( col_mode ) == "A" );
        networkChannel->setValue( item->text( col_chn ).toInt( &ok ) );
        essid->setEditText( item->text( col_ssid ) );
        if ( item->text( col_mode ) == "A" )
            mode->setCurrentItem( 3 );
        else
            mode->setCurrentItem( 2 );
    }
}
