#include <qspinbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qslider.h>

#include <qpe/config.h>

#include "linphoneconfig.h"


LinPhoneConfig::LinPhoneConfig( QWidget* parent , const char* name , bool modal, WFlags fl ) : SettingsDialog( parent, name, modal,  fl ) {
    loadConfig();
}

LinPhoneConfig::~LinPhoneConfig() {
}

void LinPhoneConfig::loadConfig() {
    Config cfg("opie-phone");
    cfg.setGroup( "net" );
    cfg.setGroup( "sip" );
    PortSpin->setValue( cfg.readNumEntry( "sip_port", 5060 ) );
    UserPartLine->setText( cfg.readEntry( "username", "" ) );
    HostPartLine->setText( cfg.readEntry( "hostname", "" ) );
    CheckBoxReg->setChecked( cfg.readBoolEntry( "use_registrar", 0 ) );
    ServerAddressLine->setText(  cfg.readEntry( "registrar", "" ) );
      // cannot use crypt due to gnome stuff in linephone
    PasswordLine->setText(  cfg.readEntry( "reg_password",  "" ) );
    RecordLine->setText(  cfg.readEntry( "addr_of_rec", "" ) );
    CheckBoxProxy->setChecked( cfg.readBoolEntry( "as_proxy",  0 ) );
    cfg.setGroup( "rtp" );
    RTPPort->setText( cfg.readEntry( "audio_rtp_port", "" ) );
    SliderJitter->setValue( cfg.readNumEntry( "jitt_comp",  60 ) );
    cfg.setGroup( "audio" );
    // source
    cfg.setGroup( "video" );
    cfg.setGroup( "codecs" );
    cfg.setGroup( "address_book" );
}

void LinPhoneConfig::writeConfig() {
    Config cfg("opie-phone");
    cfg.setGroup( "net" );
    cfg.setGroup( "sip" );
    cfg.writeEntry( "sip_port", PortSpin->value() );
    cfg.writeEntry( "username", UserPartLine->text() );
    cfg.writeEntry( "hostname", HostPartLine->text() );
    cfg.writeEntry( "use_registrar", CheckBoxReg->isChecked() );
    cfg.writeEntry( "registrar", ServerAddressLine->text() );
    cfg.writeEntry( "reg_password",  PasswordLine->text() );
    cfg.writeEntry( "addr_of_rec", RecordLine->text() );
    cfg.writeEntry(  "as_proxy",  CheckBoxProxy->isChecked() );
    cfg.setGroup( "rtp" );
    cfg.writeEntry(  "audio_rtp_port",  RTPPort->text() );
    cfg.writeEntry(  "jitt_comp", SliderJitter->value() );
    cfg.setGroup( "audio" );
    cfg.setGroup( "video" );
    cfg.setGroup( "codecs" );
    cfg.setGroup( "address_book" );
}

void LinPhoneConfig::accept() {
    writeConfig();
    QDialog::accept();
}
