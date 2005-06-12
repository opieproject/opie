#ifndef STUMBLERSETTINGS_H
#define STUMBLERSETTINGS_H

#include <qdialog.h>

class Config;
class QLineEdit;

class StumblerSettings: public QDialog {

public:
    StumblerSettings(QWidget* parent = 0, const char* name = 0, bool modal = false, WFlags f = 0);
    ~StumblerSettings();

protected slots:
    void accept();
protected:
    Config *m_config;
    QLineEdit *m_interface;
};

#endif
