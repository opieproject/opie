/***************************************************************************
                          bookmarksdlg.h  -  description
                             -------------------
    begin                : Wed Dec 1 1999
    copyright            : (C) 1999 - 2004 by llornkcor
    email                : ljp@llornkcor.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BOOKMARKSDLG_H
#define BOOKMARKSDLG_H

#include <qwidget.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qlabel.h>
//#include <qlistbox.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qdialog.h>


class bookMarksDlg : public QDialog  {
   Q_OBJECT
public: 
	bookMarksDlg(QWidget *parent, QString name);
	~bookMarksDlg();

protected: 
//	void initDialog();
	QLineEdit *QLineEdit_1;
	QMultiLineEdit *QMultiLineEdit_1;
	QLabel *QLabel_1;
	QLabel *QLabel_2;
	QLabel *QLabel_3;
	QComboBox *QComboBox_1;
	QPushButton *QPushButton_Ok;
	QPushButton *QPushButton_cancel;
	

private:

protected slots:
	virtual void doIt();
};

#endif
