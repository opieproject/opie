#ifndef LINPHONECONFIG_H
#define LINPHONECONFIG_H

#include "settingsdialog.h"

class LinPhoneConfig : public SettingsDialog {

    Q_OBJECT

public:
    LinPhoneConfig( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    virtual ~LinPhoneConfig();

private slots:
    void accept();

private:
    void writeConfig();
    void loadConfig();
};
#endif
