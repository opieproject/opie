#include <qcheckbox.h>

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
}

void SettingsDialog::writeConfig() {
    Config cfg( "mail" );
    cfg.setGroup( "Settings" );
    cfg.writeEntry( "showHtml", showHtmlButton->isChecked() );
    cfg.setGroup( "Compose" );
    cfg.writeEntry( "sendLater", checkBoxLater->isChecked() );;

}

void SettingsDialog::accept() {
    writeConfig();
    QDialog::accept();
}
