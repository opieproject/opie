/***************************************************************************
                          inputdlg.h  -  description
                             -------------------
    begin                : Mon Aug 26 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <qdialog.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qpushbutton.h>

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    static QString getText( const QString &caption, const QString &label, const QString &text = QString::null,
			    bool *ok = 0, QWidget *parent = 0, const char *name = 0 );

    InputDialog( const QString &label, QWidget* parent = 0, const char* name = 0,
		 bool modal = TRUE );
    ~InputDialog();

    void setText( const QString &text );
    QString getText();

private slots:
    void textChanged( const QString &s );
    void tryAccept();

private:
    QLineEdit *lineEdit;
    QPushButton *ok;
};

#endif // INPUTDIALOG_H
