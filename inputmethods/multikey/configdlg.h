#include <qpe/qpeapplication.h>
#include <qtabwidget.h>
#include <qcheckbox.h>
#include <qlistbox.h>
#include <qpushbutton.h>

#ifndef CONFIGDLG_H
#define CONFIGDLG_H

class ConfigDlg : public QTabWidget
{
    Q_OBJECT

public:
    ConfigDlg ();

signals:
    void pickboardToggled(bool on_off);
    void repeatToggled(bool on_off);
    void setMapToDefault();
    void setMapToFile(QString map);
    void reloadKeyboard();

private slots:
    void pickTog();
    void repeatTog();
    void setMap(int index);
    void addMap();
    void removeMap();

    // all those required slots for the color push buttons
    void keyColorClicked();
    void keyColorPressedClicked();
    void keyColorLinesClicked();
    void textColorClicked();

private:
    QCheckBox *pick_button;
    QCheckBox *repeat_button;
    QListBox *keymaps;
    QPushButton *add_button;
    QPushButton *remove_button;


    QStringList default_maps; // the maps in your share/multikey/ dir
    QStringList custom_maps; // maps you added with the 'add' button

    /* color buttons */
    QPushButton *keycolor_button;
    QPushButton *keycolor_pressed_button;
    QPushButton *keycolor_lines_button;
    QPushButton *textcolor_button;

};

#endif
