#ifndef FINDDLG_H
#define FINDDLG_H

#include <qdialog.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qvbuttongroup.h>
#include "sheet.h"

class FindDialog: public QDialog
{
  Q_OBJECT

  // QT objects
  QBoxLayout *box;
  QTabWidget *tabs;
  QWidget *widgetFind, *widgetOptions;
  QCheckBox *checkCase, *checkSelection, *checkEntire;
  QLineEdit *editFind, *editReplace;
  QVButtonGroup *groupType;

  private slots:
    void typeChanged(int id);

  public:
    FindDialog(QWidget *parent=0);
    ~FindDialog();

    int exec(Sheet *s);
};

#endif
