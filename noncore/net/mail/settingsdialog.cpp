#include <qcheckbox.h>
#include <qspinbox.h>

#include <qpe/config.h>

#include "settingsdialog.h"


SettingsDialog::SettingsDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : SettingsDialogUI( parent, name, modal, fl ) {

        readConfig();
}

SettingsDialog::~SettingsDialog() {

}

void SettingsDialog::readConfig() {
    Config cfg("mail");
    cfg.setGroup( "Settings" );
    showHtmlButton->setChecked( cfg.readBoolEntry( "showHtml", false ) );
    cfg.setGroup( "Compose" );
    checkBoxLater->setChecked( cfg.readBoolEntry( "sendLater", false ) );
    cfg.setGroup( "Applet" );
    cbEnableTaskbarApplet->setChecked( cfg.readBoolEntry( "Disabled", false ) );
    spCheckOften->setValue( cfg.readNumEntry( "CheckEvery", 5 ) );
    cbBlinkLed->setChecked( cfg.readBoolEntry( "BlinkLed", true ) );
    cbPlaySound->setChecked( cfg.readBoolEntry( "PlaySound", false ) );

}

void SettingsDialog::writeConfig() {
    Config cfg( "mail" );
    cfg.setGroup( "Settings" );
    cfg.writeEntry( "showHtml", showHtmlButton->isChecked() );
    cfg.setGroup( "Compose" );
    cfg.writeEntry( "sendLater", checkBoxLater->isChecked() );
    cfg.setGroup( "Applet" );
    cfg.writeEntry( "Disabled", cbEnableTaskbarApplet->isChecked() );
    cfg.writeEntry( "CheckEvery", spCheckOften->value() );
    cfg.writeEntry( "BlinkLed", cbBlinkLed->isChecked() );
    cfg.writeEntry( "PlaySound", cbPlaySound->isChecked() );
}

void SettingsDialog::accept() {
    writeConfig();
    QDialog::accept();
}
