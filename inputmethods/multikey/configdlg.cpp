/*
 * TODO
 * make a font selection thing (size too)
 * make a cursor thing
 * add meta key support for german, etc
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

    // make a box that will contain the buttons on the bottom
    QGrid *other_grid = new QGrid(2, gen_box);
    pick_button = new QCheckBox(tr("Pickboard"), other_grid);

    config.setGroup ("general");
    bool pick_open = config.readBoolEntry ("usePickboard", (bool)0); // default closed
    if (pick_open) {

        pick_button->setChecked(true);
    }

    // by connecting it after checking it, the signal isn't emmited
    connect (pick_button, SIGNAL(clicked()), this, SLOT(pickTog()));

    repeat_button = new QCheckBox(tr("Key Repeat"), other_grid);
    bool repeat_on = config.readBoolEntry ("useRepeat", (bool)1);

    if (repeat_on) {

        repeat_button->setChecked(true);
    }
    connect (repeat_button, SIGNAL(clicked()), this, SLOT(repeatTog()));


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
    config.setGroup("colors");

    label = new QLabel(tr("Key Color"), color_box);
    keycolor_button = new QPushButton(color_box);
    connect(keycolor_button, SIGNAL(clicked()), SLOT(keyColorClicked()));
    keycolor_button->setFlat((bool)1);
    color = config.readListEntry("keycolor", QChar(','));
    /*
     * hopefully not required

    if (color.isEmpty()) {
        color = QStringList::split(",", "240,240,240");
        config.writeEntry("keycolor", color.join(","));

    }
    */
    keycolor_button->setBackgroundColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()));


    label = new QLabel(tr("Key Pressed Color"), color_box);
    keycolor_pressed_button = new QPushButton(color_box);
    connect(keycolor_pressed_button, SIGNAL(clicked()), SLOT(keyColorPressedClicked()));
    keycolor_pressed_button->setFlat((bool)1);
    color = config.readListEntry("keycolor_pressed", QChar(','));
    keycolor_pressed_button->setBackgroundColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()));

    label = new QLabel(tr("Line Color"), color_box);
    keycolor_lines_button = new QPushButton(color_box);
    connect(keycolor_lines_button, SIGNAL(clicked()), SLOT(keyColorLinesClicked()));
    keycolor_lines_button->setFlat((bool)1);
    color = config.readListEntry("keycolor_lines", QChar(','));
    keycolor_lines_button->setBackgroundColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()));


    label = new QLabel(tr("Text Color"), color_box);
    textcolor_button = new QPushButton(color_box);
    connect(textcolor_button, SIGNAL(clicked()), SLOT(textColorClicked()));
    textcolor_button->setFlat((bool)1);
    color = config.readListEntry("textcolor", QChar(','));
    textcolor_button->setBackgroundColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()));


    label = new QLabel("", color_box); // a spacer so the above buttons dont expand
    label->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

}

void ConfigDlg::pickTog() {

    Config config ("multikey");
    config.setGroup ("general");
    config.writeEntry ("usePickboard", pick_button->isChecked()); // default closed

    emit pickboardToggled(pick_button->isChecked());
}

void ConfigDlg::repeatTog() {

    Config config ("multikey");
    config.setGroup ("general");
    config.writeEntry ("useRepeat", repeat_button->isChecked()); // default closed

    emit repeatToggled(repeat_button->isChecked());
}

// ConfigDlg::setMap {{{1

/* 
 * the index is kinda screwy, because in the config file, index 0 is just the
 * first element in the QStringList, but here it's the "Current Language"
 * listItem. therefor you have to minus one to the index before you access it.
 *
 */

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

/* ConfigDlg::slots for the color buttons {{{1
 *
 * these four slots are almost the same, except for the names. i was thinking
 * of making a map with pointers to the buttons and names of the configEntry
 * so it could be one slot, but then there would be no way of telling which 
 * of the buttons was clicked if they all connect to the same slot.
 *
 */

void ConfigDlg::keyColorClicked() {

    Config config ("multikey");
    config.setGroup ("colors");

    QStringList color = config.readListEntry("keycolor", QChar(','));

    QColor newcolor = OColorDialog::getColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()));

    color[0].setNum(newcolor.red());
    color[1].setNum(newcolor.green());
    color[2].setNum(newcolor.blue());

    config.writeEntry("keycolor", color, QChar(','));
    config.write();
    
    keycolor_button->setBackgroundColor(newcolor);
    emit reloadKeyboard();
}
void ConfigDlg::keyColorPressedClicked() {

    Config config ("multikey");
    config.setGroup ("colors");

    QStringList color = config.readListEntry("keycolor_pressed", QChar(','));

    QColor newcolor = OColorDialog::getColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()));

    color[0].setNum(newcolor.red());
    color[1].setNum(newcolor.green());
    color[2].setNum(newcolor.blue());

    config.writeEntry("keycolor_pressed", color, QChar(','));
    config.write();
    
    keycolor_pressed_button->setBackgroundColor(newcolor);
    emit reloadKeyboard();
}
void ConfigDlg::keyColorLinesClicked() {

    Config config ("multikey");
    config.setGroup ("colors");

    QStringList color = config.readListEntry("keycolor_lines", QChar(','));

    QColor newcolor = OColorDialog::getColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()));

    color[0].setNum(newcolor.red());
    color[1].setNum(newcolor.green());
    color[2].setNum(newcolor.blue());

    config.writeEntry("keycolor_lines", color, QChar(','));
    config.write();
    
    keycolor_lines_button->setBackgroundColor(newcolor);
    emit reloadKeyboard();
}
void ConfigDlg::textColorClicked() {

    Config config ("multikey");
    config.setGroup ("colors");

    QStringList color = config.readListEntry("textcolor", QChar(','));

    QColor newcolor = OColorDialog::getColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()));

    color[0].setNum(newcolor.red());
    color[1].setNum(newcolor.green());
    color[2].setNum(newcolor.blue());

    config.writeEntry("textcolor", color, QChar(','));
    config.write();
    
    textcolor_button->setBackgroundColor(newcolor);
    emit reloadKeyboard();
}
