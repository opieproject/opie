/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Opie Sheet (formerly Sheet/Qt)
 * by Serdar Ozler <sozler@sitebest.com>
 */

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
