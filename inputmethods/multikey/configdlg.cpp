/*
 * TODO
 * make a font selection thing (size too)
 * make a keymap editor
 * make keys translucent
 * make vertical keys possible
 *
 *
 */

#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/resource.h>

#include <qlayout.h>
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
#include <qtoolbutton.h>
#include <opie2/ofiledialog.h>
#include <opie2/qcolordialog.h>
#include <qdir.h>
#include <qfileinfo.h>
#include "configdlg.h"
#include "keyboard.h"

using namespace Opie;
using namespace Opie::Ui;
// ConfigDlg::ConfigDlg() {{{1
ConfigDlg::ConfigDlg () : QDialog ()
{
    setCaption( tr("Multikey Configuration") );
    Config config ("multikey");
    config.setGroup("keymaps");
    QString current_map = config.readEntry("current", 0);

    /*
     * 'general config' tab
     */

    QVBoxLayout *base_lay = new QVBoxLayout(this);
 
    QTabWidget *tabs = new QTabWidget(this, "tabs");

    QWidget *gen_box = new QWidget(tabs, "gen_tab");
    QVBoxLayout *gen_lay = new QVBoxLayout(gen_box);
    gen_lay->setMargin(3);
    QGroupBox *map_group = new QGroupBox (2, Qt::Vertical, tr("Keymap File"), gen_box);

    QHBox *hbox1 = new QHBox(map_group);
    keymaps = new QListBox(hbox1);
    keymaps->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    QVBox *vbox1 = new QVBox(hbox1);
    
    QToolButton *tb1 = new QToolButton(vbox1, tr("Move Up"));
    tb1->setPixmap(Resource::loadPixmap("up"));
    tb1->setAutoRaise(TRUE);
    tb1->setFocusPolicy(QWidget::NoFocus);
    tb1->setToggleButton(FALSE);
    connect(tb1, SIGNAL(clicked()), this, SLOT(moveSelectedUp()));

    QToolButton *tb2 = new QToolButton(vbox1, tr("Move Down"));
    tb2->setPixmap(Resource::loadPixmap("down"));
    tb2->setAutoRaise(TRUE);
    tb2->setFocusPolicy(QWidget::NoFocus);
    tb2->setToggleButton(FALSE);
    connect(tb2, SIGNAL(clicked()), this, SLOT(moveSelectedDown()));

    QString cur(tr("Current Language"));
    keymaps->insertItem(cur);
    keymaps->setSelected(0, true);
    
    QDir map_dir(QPEApplication::qpeDir() + "/share/multikey", "*.keymap");
    default_maps = map_dir.entryList(); // so i can access it in other places
    custom_maps = config.readListEntry("maps", QChar('|'));
    sw_maps = ConfigDlg::loadSw();

    QStringList sw_copy(sw_maps);
    for (uint i = 0; i < sw_copy.count(); i++) {

	QString keymap_map;
	if (sw_copy[i][0] != '/') { /* share/multikey */

	    keymap_map =  map_dir.absPath() + "/" + sw_copy[i];
	} else {

	    if (map_dir.exists(QFileInfo(sw_copy[i]).fileName(), false) 
		|| !QFile::exists(sw_copy[i])) {

		custom_maps.remove(sw_copy[i]);
		sw_maps.remove(sw_copy[i]);

		// remove it from the list too
		config.writeEntry("maps", custom_maps.join("|"));

		continue;
	    }
	    keymap_map = sw_copy[i];
	}

	QFile map(keymap_map);
	if (map.open(IO_ReadOnly)) {

	    QString line; bool found = 0;

	    map.readLine(line, 1024);
	    while (!map.atEnd()) {

		if (line.find(QRegExp("^title\\s*=\\s*")) != -1) {
                
		    keymaps->insertItem(line.right(line.length() - line.find(QChar('=')) - 1).stripWhiteSpace());
		    found = 1;
		    break;
		}
		map.readLine(line, 1024);
	    }
	    if (!found)
		keymaps->insertItem(keymap_map);

	    map.close();
	}

	if (keymap_map == current_map) {
	    keymaps->setSelected(i + 1, true);
	}
    }

    // have to "+1" because the "current language" listItem... remember?
    connect(keymaps, SIGNAL(highlighted(int)), SLOT(setMap(int)));

    QGrid *add_remove_grid = new QGrid(2, map_group);
    add_remove_grid->setMargin(3);
    add_remove_grid->setSpacing(3);

    add_button = new QPushButton(tr("Add"), add_remove_grid);
    add_button->setFlat(TRUE);
    connect(add_button, SIGNAL(clicked()), SLOT(addMap()));

    remove_button = new QPushButton(tr("Remove"), add_remove_grid);
    remove_button->setFlat(TRUE);
    if (keymaps->currentItem() == 0 || default_maps.find(QFileInfo(current_map).fileName()) != default_maps.end())
	remove_button->setDisabled(true);
    connect(remove_button, SIGNAL(clicked()), SLOT(removeMap()));

    gen_lay->addWidget(map_group);

    // make a box that will contain the buttons on the bottom
    QGrid *other_grid = new QGrid(2, gen_box);
    pick_button = new QCheckBox(tr("Pickboard"), other_grid);

    config.setGroup ("general");
    bool pick_open = config.readBoolEntry ("usePickboard", FALSE); // default closed
    if (pick_open) {

        pick_button->setChecked(true);
    }

    repeat_button = new QCheckBox(tr("Key Repeat"), other_grid);
    bool repeat_on = config.readBoolEntry ("useRepeat", TRUE);

    if (repeat_on) {

        repeat_button->setChecked(true);
    }

    gen_lay->addWidget(other_grid);
    tabs->addTab(gen_box, tr("General Settings"));

    /*
     * 'color' tab
     */

    QWidget *color_box = new QWidget(tabs, "color_tab");
    
    QGridLayout *color_lay = new QGridLayout(color_box);
    QGrid *color_grid = new QGrid(2, color_box);
    color_lay->setAlignment(Qt::AlignTop);
    color_grid->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    color_grid->layout()->setAlignment(Qt::AlignTop);
    color_grid->setMargin(3);
    color_grid->setSpacing(3);

    QLabel *label;
    QStringList color;
    config.setGroup("colors");

    label = new QLabel(tr("Key Color"), color_grid);
    keycolor_button = new QPushButton(color_grid);
    connect(keycolor_button, SIGNAL(clicked()), SLOT(keyColorClicked()));
    keycolor_button->setFlat(TRUE);
    color = config.readListEntry("keycolor", QChar(','));
    /*
     * hopefully not required

    if (color.isEmpty()) {
        color = QStringList::split(",", "240,240,240");
        config.writeEntry("keycolor", color.join(","));

    }
    */
    keycolor_button->setPalette(QPalette(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt())));


    label = new QLabel(tr("Key Pressed Color"), color_grid);
    keycolor_pressed_button = new QPushButton(color_grid);
    connect(keycolor_pressed_button, SIGNAL(clicked()), SLOT(keyColorPressedClicked()));
    keycolor_pressed_button->setFlat(TRUE);
    color = config.readListEntry("keycolor_pressed", QChar(','));
    keycolor_pressed_button->setPalette(QPalette((QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()))));

    label = new QLabel(tr("Line Color"), color_grid);
    keycolor_lines_button = new QPushButton(color_grid);
    connect(keycolor_lines_button, SIGNAL(clicked()), SLOT(keyColorLinesClicked()));
    keycolor_lines_button->setFlat(TRUE);
    color = config.readListEntry("keycolor_lines", QChar(','));
    keycolor_lines_button->setPalette(QPalette((QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()))));


    label = new QLabel(tr("Text Color"), color_grid);
    textcolor_button = new QPushButton(color_grid);
    connect(textcolor_button, SIGNAL(clicked()), SLOT(textColorClicked()));
    textcolor_button->setFlat(TRUE);
    color = config.readListEntry("textcolor", QChar(','));
    textcolor_button->setPalette(QPalette((QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()))));

    label = new QLabel("", color_grid); // a spacer so the above buttons dont expand
    label->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    /* FIXME: hacked spacer height */
    QSpacerItem *spacer = new QSpacerItem(0, 300, QSizePolicy::Expanding, QSizePolicy::Maximum);

    color_lay->addWidget(color_grid, 0, 0);
    color_lay->addItem(spacer);

    tabs->addTab(color_box, tr("Colors"));
    base_lay->addWidget(tabs);
}

QStringList ConfigDlg::loadSw()
{
    Config *config = new Config("multikey");
    config->setGroup("keymaps");
    QDir map_dir(QPEApplication::qpeDir() + "/share/multikey", "*.keymap");
    QStringList d_maps = map_dir.entryList(); // so i can access it in other places
    QStringList c_maps = config->readListEntry("maps", QChar('|'));
    QStringList s_maps = config->readListEntry("sw", QChar('|'));
    delete config;

    if (!s_maps.count())
    {
	s_maps = d_maps+c_maps;
    }
    else
    {
	/* Clear non existents entries */
	QStringList s_copy(s_maps);
	for (uint i = 0; i < s_copy.count(); ++i) {
	    if (d_maps.find(s_copy[i]) == d_maps.end()
		&& c_maps.find(s_copy[i]) == c_maps.end()) {
		s_maps.remove(s_copy[i]);
	    }
	}
	/* Update sw_maps from default_maps */
	for (uint i = 0; i < d_maps.count(); ++i) {
	    if (s_maps.find(d_maps[i]) == s_maps.end()) {
		s_maps.append(d_maps[i]);
	    }
	}
	/* Update sw_maps from custom_maps */
	for (uint i = 0; i < c_maps.count(); ++i) {
	    if (s_maps.find(c_maps[i]) == s_maps.end()) {
		s_maps.append(c_maps[i]);
	    }
	}
    }

    return s_maps;
}

void ConfigDlg::accept()
{
    /* Writing all stuffs to config */
    Config *config = new Config("multikey");
    config->setGroup("general");
    config->writeEntry("usePickboard", pick_button->isChecked()); // default closed
    config->writeEntry("useRepeat", repeat_button->isChecked()); // default closed

    config->setGroup("keymaps");
    config->writeEntry("sw", sw_maps, QChar('|'));
    config->writeEntry("maps", custom_maps, QChar('|'));
    delete config;

    int index = keymaps->currentItem();
    if (index == 0) {

        remove_button->setDisabled(true);
        emit setMapToDefault();
    }
    else if (default_maps.find(sw_maps[index-1]) != default_maps.end()) {

        remove_button->setDisabled(true);
        emit setMapToFile(QPEApplication::qpeDir() + "share/multikey/" + sw_maps[index - 1]);

    } else {

        remove_button->setEnabled(true);
        emit setMapToFile(sw_maps[index - 1]);
    }

    emit pickboardToggled(pick_button->isChecked());
    emit repeatToggled(repeat_button->isChecked());
    emit reloadSw();

    QDialog::accept();
    emit configDlgClosed();
}

void ConfigDlg::moveSelectedUp()
{
    int i = keymaps->currentItem();
    /* Ignore Current Language */
    if (i > 1) {
	QString t = sw_maps[i-1];
	sw_maps[i-1] = sw_maps[i-2];
	sw_maps[i-2] = t;

    	QString item = keymaps->currentText();
	keymaps->removeItem(i);
	keymaps->insertItem(item, i-1);
	keymaps->setCurrentItem(i-1);
    }
}

void ConfigDlg::moveSelectedDown()
{
    int i = keymaps->currentItem();
    /* Ignore Current Language */
    if (i > 0 && i < (int)keymaps->count() - 1) {
	QString t = sw_maps[i-1];
	sw_maps[i-1] = sw_maps[i];
	sw_maps[i] = t;

    	QString item = keymaps->currentText();
	keymaps->removeItem(i);
	keymaps->insertItem(item, i+1);
	keymaps->setCurrentItem(i+1);
    }
}

void ConfigDlg::closeEvent(QCloseEvent *) {

    // tell the parent it was closed, so delete me
    emit configDlgClosed();
}

void ConfigDlg::setMap(int index) {

    if (index == 0 || default_maps.find(sw_maps[index-1]) != default_maps.end()) {
        remove_button->setDisabled(true);
    } else {
        remove_button->setEnabled(true);
    }
}

// ConfigDlg::addMap() {{{1
void ConfigDlg::addMap() {

    QString map = OFileDialog::getOpenFileName(1, QDir::home().absPath());

    if (map.isNull()) return;

    Config config ("multikey");
    config.setGroup("keymaps");
    QStringList maps = config.readListEntry("maps", QChar('|'));
    maps.append(map);
    custom_maps.append(map);
    if (sw_maps.find(map) == sw_maps.end())
	sw_maps.append(map);

    QFile map_file (map);
    if (map_file.open(IO_ReadOnly)) {

        QString line; bool found = 0;

        map_file.readLine(line, 1024);
        while (!map_file.atEnd()) {

            if (line.find(QRegExp("^title\\s*=\\s*")) != -1) {
                
                keymaps->insertItem(line.right(line.length() - line.find(QChar('=')) - 1).stripWhiteSpace());
                found = 1;
                break;
            }
            map_file.readLine(line, 1024);
        }
        if (!found) keymaps->insertItem(map);

        map_file.close();
    }

    keymaps->setSelected(keymaps->count() - 1, true);
}

// ConfigDlg::removeMap() {{{1
void ConfigDlg::removeMap() {

    // move selection up one
    keymaps->setSelected(keymaps->currentItem() - 1, true);
    // delete the next selected item cus you just moved it up
    keymaps->removeItem(keymaps->currentItem() + 1);

    custom_maps.remove(sw_maps[keymaps->currentItem()]);
    sw_maps.remove(sw_maps.at(keymaps->currentItem()));
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

    QColor newcolor = QColorDialog::getColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()));

    color[0].setNum(newcolor.red());
    color[1].setNum(newcolor.green());
    color[2].setNum(newcolor.blue());

    config.writeEntry("keycolor", color, QChar(','));
    config.write();
    
    keycolor_button->setPalette(QPalette(newcolor));
    emit reloadKeyboard();
}
void ConfigDlg::keyColorPressedClicked() {

    Config config ("multikey");
    config.setGroup ("colors");

    QStringList color = config.readListEntry("keycolor_pressed", QChar(','));

    QColor newcolor = QColorDialog::getColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()));

    color[0].setNum(newcolor.red());
    color[1].setNum(newcolor.green());
    color[2].setNum(newcolor.blue());

    config.writeEntry("keycolor_pressed", color, QChar(','));
    config.write();
    
    keycolor_pressed_button->setPalette(QPalette(newcolor));
    emit reloadKeyboard();
}
void ConfigDlg::keyColorLinesClicked() {

    Config config ("multikey");
    config.setGroup ("colors");

    QStringList color = config.readListEntry("keycolor_lines", QChar(','));

    QColor newcolor = QColorDialog::getColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()));

    color[0].setNum(newcolor.red());
    color[1].setNum(newcolor.green());
    color[2].setNum(newcolor.blue());

    config.writeEntry("keycolor_lines", color, QChar(','));
    config.write();
    
    keycolor_lines_button->setPalette(QPalette(newcolor));
    emit reloadKeyboard();
}
void ConfigDlg::textColorClicked() {

    Config config ("multikey");
    config.setGroup ("colors");

    QStringList color = config.readListEntry("textcolor", QChar(','));

    QColor newcolor = QColorDialog::getColor(QColor(color[0].toInt(), color[1].toInt(), color[2].toInt()));

    color[0].setNum(newcolor.red());
    color[1].setNum(newcolor.green());
    color[2].setNum(newcolor.blue());

    config.writeEntry("textcolor", color, QChar(','));
    config.write();
    
    textcolor_button->setPalette(QPalette(newcolor));
    emit reloadKeyboard();
}
