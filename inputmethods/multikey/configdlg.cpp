/*
 * TODO
 * make a font selection thing (size too)
 * make a cursor thing
 *
 *
 *
 */

#include <iostream.h>

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
#include <opie/colordialog.h>
#include <qdir.h>
#include <qfileinfo.h>
#include "configdlg.h"
#include "keyboard.h"

// ConfigDlg::ConfigDlg() {{{1
ConfigDlg::ConfigDlg () : QTabWidget ()
{
    setCaption( tr("Multikey Configuration") );
    Config config ("multikey");
    config.setGroup("keymaps");
    QString current_map = config.readEntry("current", 0);

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
    keymaps->setSelected(0, true);
    
    QDir map_dir(QPEApplication::qpeDir() + "/share/multikey", "*.keymap");
    default_maps = map_dir.entryList(); // so i can access it in other places

    for (uint i = 0; i <map_dir.count(); i++) {

        keymaps->insertItem(map_dir.absPath() + "/" + map_dir[i]);
        if (map_dir.absPath() + "/" + map_dir[i] == current_map) {

            keymaps->setSelected(i + 1, true);
        }

    } 

    custom_maps = config.readListEntry("maps", QChar('|'));

    for (uint i = 0; i < custom_maps.count(); i++) {

        if (map_dir.exists(QFileInfo(custom_maps[i]).fileName(), false) 
                        || !QFile::exists(custom_maps[i])) {

            custom_maps.remove(custom_maps.at(i));

            // remove it from the list too
            config.writeEntry("maps", custom_maps.join("|"));


        } else {

            keymaps->insertItem(custom_maps[i]);
            if (custom_maps[i] == current_map) {

                keymaps->setSelected(map_dir.count() + i + 1, true);
            }
        }
    }

    // have to "+1" because the "current language" listItem... remember?

    connect(keymaps, SIGNAL(highlighted(int)), SLOT(setMap(int)));


    QGrid *add_remove_grid = new QGrid(2, map_group);
    add_remove_grid->setMargin(3);
    add_remove_grid->setSpacing(3);

    add_button = new QPushButton(tr("Add"), add_remove_grid);
    add_button->setFlat((bool)1);
    connect(add_button, SIGNAL(clicked()), SLOT(addMap()));

    remove_button = new QPushButton(tr("Remove"), add_remove_grid);
    remove_button->setFlat((bool)1);
    if ((int)map_dir.count() >= keymaps->currentItem()) 
        remove_button->setDisabled(true);
    connect(remove_button, SIGNAL(clicked()), SLOT(removeMap()));

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
    QStringList color;

    label = new QLabel(tr("Key Color"), color_box);
    key_color_button = new QPushButton(color_box);
    connect(key_color_button, SIGNAL(clicked()), SLOT(keyColorButtonClicked()));
    key_color_button->setFlat((bool)1);

    config.setGroup("colors");
    color = config.readListEntry("keycolor", QChar(','));
    if (color.isEmpty()) {
        color = QStringList::split(",", "240,240,240");
        config.writeEntry("keycolor", color.join(","));

    }
    key_color_button->setBackgroundColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()));


    label = new QLabel(tr("Key Pressed Color"), color_box);
    QPushButton *button = new QPushButton(color_box);
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
 * listItem. therefor you have to minus one to the index before you access it.
 *
 */

// ConfigDlg::setMap {{{1
void ConfigDlg::setMap(int index) {

    if (index == 0) {

        remove_button->setDisabled(true);
        emit setMapToDefault();
    }
    else if ((uint)index <= default_maps.count()) {

        remove_button->setDisabled(true);
        emit setMapToFile(keymaps->text(index));

    } else {

        remove_button->setEnabled(true);
        emit setMapToFile(keymaps->text(index));
    }
}

// ConfigDlg::addMap() {{{1
void ConfigDlg::addMap() {

    QString map = OFileDialog::getOpenFileName(1, QDir::home().absPath());
    Config config ("multikey");
    config.setGroup("keymaps");
    QStringList maps = config.readListEntry("maps", QChar('|'));
    maps.append(map);
    keymaps->insertItem(map);
    keymaps->setSelected(keymaps->count() - 1, true);


    config.writeEntry("maps", maps, QChar('|'));
    config.writeEntry("current", map);

}

// ConfigDlg::removeMap() {{{1
void ConfigDlg::removeMap() {

    cout << "removing : " << custom_maps[keymaps->currentItem() - default_maps.count() - 1] << "\n";
    cout << "currentItem : " << keymaps->currentItem() << "\n";

    // move selection up one
    keymaps->setSelected(keymaps->currentItem() - 1, true);
    // delete the next selected item cus you just moved it up
    keymaps->removeItem(keymaps->currentItem() + 1);

    custom_maps.remove(custom_maps[keymaps->currentItem() - default_maps.count()]);

    // write the changes
    Config config ("multikey");
    config.setGroup("keymaps");
    config.writeEntry("maps", custom_maps, QChar('|'));
}

// ConfigDlg::color {{{1
void ConfigDlg::keyColorButtonClicked() {

    Config config ("multikey");
    config.setGroup ("colors");

    QStringList color = config.readListEntry("keycolor", QChar(','));

    QColor newcolor = OColorDialog::getColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()));

    color[0].setNum(newcolor.red());
    color[1].setNum(newcolor.green());
    color[2].setNum(newcolor.blue());

    config.writeEntry("keycolor", color, QChar(','));
    config.write();
    
    key_color_button->setBackgroundColor(newcolor);
    emit reloadKeyboard();
}
