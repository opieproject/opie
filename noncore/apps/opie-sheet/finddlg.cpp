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

#include <qlabel.h>
#include <qradiobutton.h>
#include "finddlg.h"

FindDialog::FindDialog(QWidget *parent=0)
           :QDialog(parent, 0, TRUE)
{
  // Main widget
  tabs=new QTabWidget(this);
  widgetFind=new QWidget(tabs);
  widgetOptions=new QWidget(tabs);
  tabs->addTab(widgetFind, tr("&Find && Replace"));
  tabs->addTab(widgetOptions, tr("&Options"));

  // Find tab
  QLabel *label=new QLabel(tr("&Search for:"), widgetFind);
  label->setGeometry(10, 10, 215, 20);
  editFind=new QLineEdit(widgetFind);
  editFind->setGeometry(10, 40, 215, 20);
  label->setBuddy(editFind);

  label=new QLabel(tr("&Replace with:"), widgetFind);
  label->setGeometry(10, 80, 215, 20);
  editReplace=new QLineEdit(widgetFind);
  editReplace->setGeometry(10, 110, 215, 20);
  editReplace->setEnabled(FALSE);
  label->setBuddy(editReplace);

  groupType=new QVButtonGroup(tr("&Type"), widgetFind);
  groupType->setGeometry(10, 150, 215, 90);
  QRadioButton *radio=new QRadioButton(tr("&Find"), groupType);
  radio=new QRadioButton(tr("&Replace"), groupType);
  radio=new QRadioButton(tr("Replace &all"), groupType);
  groupType->setButton(0);
  connect(groupType, SIGNAL(clicked(int)), this, SLOT(typeChanged(int)));

  // Options tab
  checkCase=new QCheckBox(tr("Match &case"), widgetOptions);
  checkCase->setGeometry(10, 10, 215, 20);
  checkSelection=new QCheckBox(tr("Current &selection only"), widgetOptions);
  checkSelection->setGeometry(10, 40, 215, 20);
  checkEntire=new QCheckBox(tr("&Entire cell"), widgetOptions);
  checkEntire->setGeometry(10, 70, 215, 20);

  // Main widget
  box=new QVBoxLayout(this);
  box->addWidget(tabs);

  setCaption(tr("Find & Replace"));
}

FindDialog::~FindDialog()
{
}

void FindDialog::typeChanged(int id)
{
  editReplace->setEnabled(id>0);
}

int FindDialog::exec(Sheet *s)
{
  if (QDialog::exec()==QDialog::Accepted)
  {
    int id=groupType->id(groupType->selected());
    s->dataFindReplace(editFind->text(), editReplace->text(), checkCase->isChecked(), !checkSelection->isChecked(), checkEntire->isChecked(), id>0, id>1);
    return QDialog::Accepted;
  }
  return QDialog::Rejected;
}
