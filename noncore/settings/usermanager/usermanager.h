/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef USERCONFIG_H
#define USERCONFIG_H

#include <qmainwindow.h>
#include <qtabwidget.h>
#include <qlistview.h>
#include <qiconview.h>

#include <qpe/qpemenubar.h>
#include <qpopupmenu.h>                       
#include <qpe/qpetoolbar.h>
#include <qtoolbutton.h>

#include "userdialog.h"
#include "groupdialog.h"
#include "passwd.h"

class UserConfig : public QMainWindow
{
	Q_OBJECT
	
public:
	UserConfig( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
	~UserConfig();

private:
	QToolButton *adduserToolButton;
	QToolButton *edituserToolButton;
	QToolButton *deleteuserToolButton;
	QToolButton *addgroupToolButton;
	QToolButton *editgroupToolButton;
	QToolButton *deletegroupToolButton;
	QTabWidget *myTabWidget;
	QIconView *usersIconView;
	QListView *usersListView;
	QListView *groupsListView;
	int availableUID;
	int availableGID;
	void setupTabAccounts();
	void setupTabAllUsers();
	void setupTabAllGroups();
	void setupTabPrefs();
	void setupTabAbout();
	void getUsers();
	void getGroups();
	
private slots:
	void addUser();
	void editUser();
	void delUser();
	void addGroup();
	void editGroup();
	void delGroup();
};

#endif // USERCONFIG_H
