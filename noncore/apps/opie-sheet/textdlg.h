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

#ifndef TEXTDLG_H
#define TEXTDLG_H

#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>

class TextDialog: public QDialog
{
  Q_OBJECT

  // QT objects
  QLabel *label;
  QLineEdit *edit;

  public:
    TextDialog(QWidget *parent=0);
    ~TextDialog();

    int exec(const QString &caption, const QString &text, const QString &value="");
    QString getValue();
};

#endif
