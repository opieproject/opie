/*
 * TODO
 * make a font selection thing (size too)
 *
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
#include <qlistbox.h>
#include <qstringlist.h>
#include <opie/ofiledialog.h>
#include <iostream.h>
#include "configdlg.h"
#include "keyboard.h"

ConfigDlg::ConfigDlg () : QTabWidget ()
{
    setCaption( tr("Multikey Configuration") );
    Config config ("multikey");

    /*
     * 'general config' tab
     */

    QVBox *gen_box = new QVBox (this);
    gen_box->setMargin(3);
    addTab(gen_box, tr("General Settings"));

    QGroupBox *map_group = new QGroupBox (2, Qt::Vertical, tr("Keymap File"), gen_box);

    keymaps = new QListBox (map_group);
    keymaps->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    QString cur(tr("Current Language"));
    keymaps->insertItem(cur);

    config.setGroup ("keymaps");
    QStringList maps = config.readListEntry("maps", QChar('|'));

    for (unsigned int i = 0; i < maps.count(); i++) {

        keymaps->insertItem(maps[i]);
    }

    // have to "+1" because the "current language" listItem... remember?
    keymaps->setSelected(config.readNumEntry("current", 0) + 1, true);

    connect(keymaps, SIGNAL(highlighted(int)), SLOT(setMap(int)));


    QGrid *add_remove_grid = new QGrid(2, map_group);
    add_remove_grid->setMargin(3);
    add_remove_grid->setSpacing(3);

    add_button = new QPushButton(tr("Add"), add_remove_grid);
    add_button->setFlat((bool)1);
    connect(add_button, SIGNAL(clicked()), SLOT(addMap()));

    remove_button = new QPushButton(tr("Remove"), add_remove_grid);
    remove_button->setFlat((bool)1);

    pick_button = new QCheckBox(tr("Pickboard"), gen_box);

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
    color_box->setSpacing(3);
    addTab(color_box, tr("Colors"));

    QLabel *label;

    label = new QLabel(tr("Key Color"), color_box);
    QPushButton *button = new QPushButton(color_box);
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

/* 
 * the index is kinda screwy, because in the config file, index 0 is just the
 * first element in the QStringList, but here it's the "Current Language"
 * listItem. therefor you have to minus one to the index before you access
 * it from the config file. 
 *
 * and later on, the "current language" setting should be -1 in the config file
 */

void ConfigDlg::setMap(int index) {

    if (index == 0) {

        if (remove_button->isEnabled())
            remove_button->setDisabled(true);

        emit setMapToDefault();
    }
    else {

        if (!remove_button->isEnabled()) 
            remove_button->setEnabled(true);

        emit setMapToFile(index - 1);
    }
}

void ConfigDlg::addMap() {

    QString map = OFileDialog::getOpenFileName(2, QPEApplication::qpeDir() + "/share/multikey");
    cout << "added file " << map << "!\n";

}

void ConfigDlg::removeMap(int index) {

}
