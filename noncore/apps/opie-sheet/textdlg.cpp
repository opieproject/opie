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

#include "textdlg.h"

TextDialog::TextDialog(QWidget *parent)
           :QDialog(parent, 0, TRUE)
{
  edit=new QLineEdit(this);
  edit->setGeometry(90, 10, 100, 25);

  label=new QLabel(this);
  label->setGeometry(10, 10, 70, 25);
  label->setBuddy(edit);

  resize(200, 45);
}

TextDialog::~TextDialog()
{
}

int TextDialog::exec(const QString &caption, const QString &text,
                     const QString &value)
{
  setCaption(caption);
  label->setText(text);
  edit->setText(value);

  return QDialog::exec();
}

QString TextDialog::getValue()
{
  return edit->text();
}
