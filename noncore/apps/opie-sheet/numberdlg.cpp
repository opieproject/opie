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

#include "numberdlg.h"

NumberDialog::NumberDialog(QWidget *parent)
             :QDialog(parent, 0, TRUE)
{
  edit=new QSpinBox(this);
  edit->setGeometry(120, 10, 70, 25);

  label=new QLabel(this);
  label->setGeometry(10, 10, 100, 25);
  label->setBuddy(edit);

  resize(200, 45);
}

NumberDialog::~NumberDialog()
{
}

int NumberDialog::exec(const QString &caption, const QString &text,
                       int value, int min, int max, int step)
{
  setCaption(caption);
  label->setText(text);
  edit->setValue(value);
  edit->setMinValue(min);
  edit->setMaxValue(max);
  edit->setLineStep(step);

  return QDialog::exec();
}

int NumberDialog::getValue()
{
  return edit->value();
}
