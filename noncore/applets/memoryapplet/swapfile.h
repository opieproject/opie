/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef SWAPFILE_H
#define SWAPFILE_H

#include <qwidget.h>

class QLabel;
class QRadioButton;
class QLineEdit;
class QComboBox;
class QProgressBar;

class Swapfile : public QWidget
{
    Q_OBJECT
public:
    Swapfile( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    ~Swapfile();

private slots:
	void swapon();
	void swapoff();
	void cfsdchecked();
	void makeswapfile();
	void removeswapfile();
	void cardnotify( const QCString &msg, const QByteArray & );
	void getStatusPcmcia();
	void getStatusSd();
	void status();

private:
	bool cardInPcmcia0;
    bool cardInPcmcia1;
	bool cardInSd;

	QRadioButton* ramRB;
	QRadioButton* cfRB;
	QRadioButton* sdRB;
	QLineEdit* swapPath1;
	QLabel* swapStatus;
	QLabel* swapStatusIcon;
	QComboBox* swapSize;
	QProgressBar* mkswapProgress;

	bool isRoot;

	int exec(const QString& arg);
	void setStatusMessage(const QString& text, bool error = false);
};

#endif
