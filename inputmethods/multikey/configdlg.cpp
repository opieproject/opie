/*
 * TODO
 * make a font selection thing
 *
 * FIXME
 *  if you open the config dialog and close it from the little (x) button on the title bar, 
 *  you have to hit the button on the keyboard twice for it to open the next time
 *
 *
 */

#include <qpe/qpeapplication.h>
#include <qpe/config.h>

#include <qwidget.h>
#include <qdialog.h>
#include <qtabwidget.h>
#include <qvbox.h>
#include <qgrid.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qsizepolicy.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include "configdlg.h"
#include "keyboard.h"

ConfigDlg::ConfigDlg () : QTabWidget ()
{
    setCaption( tr("Multikey Configuration") );

    /*
     * 'general config' tab
     */

    QVBox *gen_box = new QVBox (this);
    gen_box->setMargin(3);
    addTab(gen_box, tr("General Settings"));

    QGroupBox *map_group = new QGroupBox (2, Qt::Horizontal, tr("Keymap File"), gen_box);

    map_combo = new QComboBox ((bool)0, map_group);
    map_combo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

    QString cur(tr("Current Language"));
    map_combo->insertItem(cur);
    connect(map_combo, SIGNAL(activated(int)), SLOT(setMap(int)));

    QString ko(tr(QPEApplication::qpeDir() + "/share/multikey/ko.keymap"));
    map_combo->insertItem(ko);

    QString en(tr(QPEApplication::qpeDir() + "/share/multikey/en.keymap"));
    map_combo->insertItem(en);

    QString de(tr(QPEApplication::qpeDir() + "/share/multikey/de.keymap"));
    map_combo->insertItem(de);

    QPushButton *button = new QPushButton(tr("Browse..."), map_group);
    button->setFlat((bool)1);

    pick_button = new QCheckBox(tr("Pickboard"), gen_box);

    Config config ("multikey");
    config.setGroup ("pickboard");
    bool pick_open = config.readBoolEntry ("open", "0"); // default closed
    if (pick_open) {

        pick_button->setChecked(true);
    }

    // by connecting it after checking it, the signal isn't emmited
    connect (pick_button, SIGNAL(clicked()), this, SLOT(pickTog()));

    /*
     * 'color' tab
     */

    QGrid *color_box = new QGrid(2, this);
    color_box->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    color_box->setMargin(3);
    addTab(color_box, tr("Colors"));

    QLabel *label;

    label = new QLabel(tr("Key Color"), color_box);
    button = new QPushButton(color_box);
    button->setFlat((bool)1);
    label = new QLabel(tr("Key Pressed Color"), color_box);
    button = new QPushButton(color_box);
    button->setFlat((bool)1);
    label = new QLabel(tr("Line Color"), color_box);
    button = new QPushButton(color_box);
    button->setFlat((bool)1);
    label = new QLabel(tr("Text Color"), color_box);
    button = new QPushButton(color_box);
    button->setFlat((bool)1);

    label = new QLabel("", color_box); // a spacer so the above buttons dont expand
    label->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

}

void ConfigDlg::pickTog() {

    Config config ("multikey");
    config.setGroup ("pickboard");
    config.writeEntry ("open", pick_button->isChecked()); // default closed

    emit pickboardToggled(pick_button->isChecked());
}

void ConfigDlg::setMap(int index) {

    if (index == 0) {

        emit setMapToDefault();
    }
    else {

        emit setMapToFile(map_combo->text(index));
    }
}
