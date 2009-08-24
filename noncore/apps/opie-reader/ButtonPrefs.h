/****************************************************************************
** Form interface generated from reading ui file 'Prefs.ui'
**
** Created: Tue Feb 11 23:53:32 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef __BUTTON_PREFS_H
#define __BUTTON_PREFS_H

#include <qvariant.h>
#include <qwidget.h>
#include <qtabdialog.h>
#include <qtabwidget.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>

#define USECOMBO

#ifdef USECOMBO
#include <qcombobox.h>
#else
#include <qpe/menubutton.h>
#endif

#include "orkey.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
//class QCheckBox;
class QLabel;
//class QSpinBox;
class QListViewItem;

class QListView;
class QListViewItem;

class CButtonPrefs : public QWidget
{
  Q_OBJECT

  QMap<orKey, int> *kmap;
  QMap<orKey, QListViewItem*> listmap;
  QListView* lb;
  void keyPressEvent(QKeyEvent* e);
#ifdef USECOMBO
    void populate(QComboBox*);
#else
    void populate(MenuButton*);
#endif

#ifdef USECOMBO
    QComboBox *action;
#else
    MenuButton *action;
#endif
    QSpinBox* debounce;
 signals:
    void Closed();
 private slots:
     void slotClosed() { emit Closed(); }

public:

    CButtonPrefs( QMap<orKey, int>*, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CButtonPrefs();
    int Debounce() { return debounce->value(); }
    void Debounce(int v) { debounce->setValue(v); }
    void mapkey(Qt::ButtonState st, int key);
    void mapkey(Qt::ButtonState st, int key, int _fn);
 private slots:
    void erasemapping();
};
#endif // CPREFS_H
