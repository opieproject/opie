#include <qpe/qpeapplication.h>
#include <qtabwidget.h>
#include <qcheckbox.h>

#ifndef CONFIGDLG_H
#define CONFIGDLG_H

class ConfigDlg : public QTabWidget
{
    Q_OBJECT

public:
    ConfigDlg ();

signals:
    void pickboardToggled(bool on_off);

private slots:
    void pickTog();

private:
    QCheckBox *pick_button;

};

#endif
