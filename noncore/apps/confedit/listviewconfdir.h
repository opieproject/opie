/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

 // (c) 2002 Patrick S. Vogtp <tille@handhelds.org>

#ifndef LISTVIEWCONFDIR_H
#define LISTVIEWCONFDIR_H

#include <qwidget.h>
#include <qlistview.h>
#include <qdir.h>

class QDir;

class ListViewConfDir : public QListView  {
   Q_OBJECT
public: 
	ListViewConfDir(QString settingsPath, QWidget *parent=0, const char *name=0);
	~ListViewConfDir();
protected slots:
	void expand(QListViewItem*);
private:
	QDir confDir;

 	void readConfFiles();
};

#endif
