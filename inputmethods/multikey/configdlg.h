#include <qpe/qpeapplication.h>
#include <qtabwidget.h>
#include <qcheckbox.h>
#include <qcombobox.h>

#ifndef CONFIGDLG_H
#define CONFIGDLG_H

class ConfigDlg : public QTabWidget
{
    Q_OBJECT

public:
    ConfigDlg ();

signals:
    void pickboardToggled(bool on_off);
    void setMapToDefault();
    void setMapToFile(QString file);

private slots:
    void pickTog();
    void setMap(int index);

private:
    QCheckBox *pick_button;
    QComboBox *map_combo;

};

#endif
