/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
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
#ifndef ADDATT_H
#define ADDATT_H

#include <qdialog.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>
#include <qstring.h>
#include <qfileinfo.h>
#include <qstringlist.h>

class FileItem : public QListViewItem
{
public:
	FileItem(QListView *parent, QFileInfo fileInfo, QString fileType);
	QFileInfo getFileInfo();
	QString getFileType();

private:
	QFileInfo file;
	QString type;
};

class AddAtt : public QDialog
{
	Q_OBJECT

public:
	AddAtt(QWidget *parent = 0, const char *name = 0, WFlags f = 0);
	QStringList returnAttatchedFiles();	
	QStringList returnFileTypes();
	void getFiles();
	void clear();
	
public slots:
	void fileCategorySelected(int);
	void addAttatchment();
	void removeAttatchment();
	void reject();
	void accept();
	
private:
	FileItem *item;
	QListView *listView, *attView;
	QPushButton *fileCategoryButton, *attatchButton, *removeButton;
	QPopupMenu *fileCategories;
	bool modified;
	QFileInfo *fi;
};

#endif
