#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <qwidget.h>

#include "settingsdialogui.h"

class SettingsDialog : public SettingsDialogUI {

Q_OBJECT

public:
    SettingsDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SettingsDialog();

private:
    void readConfig();
    void writeConfig();


private slots:   
    void accept();


};



#endif
