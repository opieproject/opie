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

    QGroupBox *map_group = new QGroupBox (2, Qt::Vertical, tr("Keymap File"), gen_box);

    keymaps = new QListBox (map_group);
    keymaps->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    //keymaps->setMaximumHeight(150);

    QString cur(tr("Current Language"));
    keymaps->insertItem(cur);
    connect(keymaps, SIGNAL(highlighted(int)), SLOT(setMap(int)));

    QString ko(QPEApplication::qpeDir() + "/share/multikey/ko.keymap");
    keymaps->insertItem(ko);

    QString en(QPEApplication::qpeDir() + "/share/multikey/en.keymap");
    keymaps->insertItem(en);

    QString de(QPEApplication::qpeDir() + "/share/multikey/de.keymap");
    keymaps->insertItem(de);

    QGrid *add_remove_grid = new QGrid(2, map_group);
    add_remove_grid->setMargin(3);
    add_remove_grid->setSpacing(3);

    add_button = new QPushButton(tr("Add"), add_remove_grid);
    add_button->setFlat((bool)1);

    remove_button = new QPushButton(tr("Remove"), add_remove_grid);
    remove_button->setFlat((bool)1);

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

void ConfigDlg::setMap(int index) {

    if (index == 0) {

        if (remove_button->isEnabled())
            remove_button->setDisabled(true);

        emit setMapToDefault();
    }
    else {

        if (!remove_button->isEnabled()) 
            remove_button->setEnabled(true);

        emit setMapToFile(keymaps->text(index));
    }
}

void ConfigDlg::addMap() {

}

void ConfigDlg::removeMap(int index) {

}
