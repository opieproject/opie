/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "usermanager.h"

#include <qlayout.h>
#include <stdio.h>

#include <qmessagebox.h>
#include <qfile.h>
#include <qpe/resource.h>

#include <qregexp.h>

/**
 * The mainwindow constructor.
 *
 * @param QWidget *parent
 * @param const char *name
 * @ param WFlags fl
 *
 */
UserConfig::UserConfig(QWidget* parent, const char* name, WFlags fl) : QMainWindow(parent, name, fl) {
	setCaption(tr("OPIE User Manager"));
	
	// Create an instance of the global object 'accounts'. This holds all user/group info, and functions to modify them.
	accounts=new Passwd();
	accounts->open();	// This actually loads the files /etc/passwd & /etc/group into memory.

	// Create the toolbar.
	QPEToolBar *toolbar = new QPEToolBar(this,"Toolbar");
	toolbar->setHorizontalStretchable(1); // Is there any other way to get the toolbar to stretch of the full screen!?
	adduserToolButton = new QToolButton(Resource::loadPixmap("usermanager/adduser"),"Add User",0,this,SLOT(addUser()),toolbar,"Add User");
	edituserToolButton = new QToolButton(Resource::loadPixmap("usermanager/edituser"),"Edit User",0,this,SLOT(editUser()),toolbar,"Edit User");
	deleteuserToolButton = new QToolButton(Resource::loadPixmap("usermanager/deleteuser"),"Delete User",0,this,SLOT(delUser()),toolbar,"Delete User");
	QToolButton *userstext = new QToolButton(0,"User",0,0,0,toolbar,"User");
	userstext->setUsesTextLabel(true);
	toolbar->addSeparator();
	addgroupToolButton = new QToolButton(Resource::loadPixmap("usermanager/addgroup"),"Add Group",0,this,SLOT(addGroup()),toolbar,"Add Group");
	editgroupToolButton = new QToolButton(Resource::loadPixmap("usermanager/editgroup"),"Edit Group",0,this,SLOT(editGroup()),toolbar,"Edit Group");
	deletegroupToolButton = new QToolButton(Resource::loadPixmap("usermanager/deletegroup"),"Delete Group",0,this,SLOT(delGroup()),toolbar,"Delete Group");
	QToolButton *groupstext = new QToolButton(0,"Group",0,0,0,toolbar,"Group");
	groupstext->setUsesTextLabel(true);
	addToolBar(toolbar,"myToolBar");

	// Add a tabwidget and all the tabs.
	myTabWidget = new QTabWidget(this,"My Tab Widget");
	setupTabAccounts();
	setupTabAllUsers();
	setupTabAllGroups();
	
	getUsers(); // Fill out the iconview & listview with all users.
	getGroups(); // Fill out the group listview with all groups.
	
	setCentralWidget(myTabWidget);
}

UserConfig::~UserConfig() {
	accounts->close();
	delete accounts;
}

void UserConfig::setupTabAccounts() {
	QWidget *tabpage = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(tabpage);
	layout->setMargin(5);
	
	usersIconView=new QIconView(tabpage,"users");
	usersIconView->setItemTextPos(QIconView::Right);
	usersIconView->setArrangement(QIconView::LeftToRight);
	layout->addWidget(usersIconView);
	
	myTabWidget->addTab(tabpage,"Users");
}

void UserConfig::setupTabAllUsers() {
	QWidget *tabpage = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(tabpage);
	layout->setMargin(5);
	
	usersListView=new QListView(tabpage,"allusers");
	usersListView->addColumn("UID");
	usersListView->addColumn("Login");
	usersListView->addColumn("Username");
	layout->addWidget(usersListView);
	usersListView->setSorting(1,1);
	usersListView->setAllColumnsShowFocus(true);

	myTabWidget->addTab(tabpage,"All Users");
}

void UserConfig::setupTabAllGroups() {
	QWidget *tabpage = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(tabpage);
	layout->setMargin(5);

	groupsListView=new QListView(tabpage,"groups");
	groupsListView->addColumn("GID");
	groupsListView->addColumn("Groupname");
	layout->addWidget(groupsListView);
	groupsListView->setSorting(1,1);
	groupsListView->setAllColumnsShowFocus(true);
	
	myTabWidget->addTab(tabpage,"All Groups");
}
void UserConfig::getUsers() {
	QString mytext;
	QPixmap mypixmap;
		
	// Empty the iconview & the listview.
	usersIconView->clear();
	usersListView->clear();
	
	// availableUID is used as a deposite for the next available UID on the system, this should start at an ID over 500.
	availableUID=500;
	for(QStringList::Iterator it=accounts->passwdStringList.begin(); it!=accounts->passwdStringList.end(); ++it) {
		accounts->splitPasswdEntry(*it); // Split the string into it's components and store in variables in the accounts object. ("pr_name" and so on.)
		new QListViewItem(usersListView,QString::number(accounts->pw_uid),accounts->pw_name,accounts->pw_gecos);
		if((accounts->pw_uid>=500) && (accounts->pw_uid<65000)) {	// Is this user a "normal" user ?
			mytext=QString(accounts->pw_name)+" - ("+QString(accounts->pw_gecos)+")"; // The string displayed next to the icon.
//			mypixmap=Resource::loadPixmap(QString("users/"+accounts->pw_name));	// Is there an icon for this user? Resource::loadPixmap is caching, doesn't work.
			if(!(mypixmap.load("/opt/QtPalmtop/pics/users/"+accounts->pw_name+".png"))) {
//			if(mypixmap.isNull()) {
				mypixmap=Resource::loadPixmap(QString("usermanager/usericon"));	// If this user has no icon, load the default icon.
			}
			new QIconViewItem(usersIconView,mytext,mypixmap);	// Add the icon+text to the qiconview.
		}
		if((accounts->pw_uid>=availableUID) && (accounts->pw_uid<65000)) availableUID=accounts->pw_uid+1; // Increase 1 to the latest know UID to get a free uid. 
	}
	usersIconView->sort();
}

void UserConfig::addUser() {
	if(UserDialog::addUser(availableUID,availableGID)) {	// Add the user to the system, also send next available UID and GID.
		getUsers(); // Update users views.
		getGroups(); // Update groups view.
	}
}

void UserConfig::editUser() {
	QString username;
	if(myTabWidget->currentPageIndex()==0) {	// Users
		if(usersIconView->currentItem()) {	// Any icon selected?
			username=usersIconView->currentItem()->text();	// Get the text associated with the icon.
			username=username.left(username.find(" - (",0,true));	// Strip out the username.
			if(UserDialog::editUser(username)) {	// Bring up the userinfo dialog.
				// If there were any changed also update the views.
				getUsers();
				getGroups();
			}
		} else {
			QMessageBox::information(this,"No selection.","No user has been selected.");
		}
	}
	if(myTabWidget->currentPageIndex()==1) {	// All users
		if(usersListView->currentItem()) {	// Anything changed!?
			username=usersListView->currentItem()->text(1);	// Get the username.
			if(UserDialog::editUser(username)) {	// Bring up the userinfo dialog.
				// And again update the views if there were any changes.
				getUsers();
				getGroups();
			}
		} else  {
			QMessageBox::information(this,"No selection.","No user has been selected.");
		}
	}
}

void UserConfig::delUser() {
	QString username;
	
	if(myTabWidget->currentPageIndex()==0) {	// Users, Iconview.
		if(usersIconView->currentItem()) {	// Anything selected?
			username=usersIconView->currentItem()->text();	// Get string associated with icon.
			username=username.left(username.find(" - (",0,true));	// Strip out the username.
			if(QMessageBox::warning(this,"Delete user","Are you sure you want to\ndelete this user? \""+QString(username)+"\" ?","&No","&Yes",0,0,1)) {
				if(UserDialog::delUser(username)) {	// Delete the user if possible.
					// Update views.
					getUsers();
					getGroups();
				}
			}
		} else  {
			QMessageBox::information(this,"No selection","No user has been selected.");
		}
	}
	if(myTabWidget->currentPageIndex()==1) {	// All users
		if(usersListView->currentItem()) {	// Anything changed!?
			username=usersListView->currentItem()->text(1);	// Get the username.
			if(QMessageBox::warning(this,"Delete user","Are you sure you want to\ndelete this user? \""+QString(username)+"\" ?","&No","&Yes",0,0,1)) {
				if(UserDialog::delUser(username)) {	// Try to delete the user.
					// Update views.
					getUsers();
					getGroups();
				}
			}
		} else  {
			QMessageBox::information(this,"No selection","No user has been selected.");
		}
	}
	
}

void UserConfig::getGroups() {
	groupsListView->clear();	// Empty the listview.
	availableGID=500;	// We need to find the next free GID, and are only interested in values between 500 & 65000.
	for(QStringList::Iterator it=accounts->groupStringList.begin(); it!=accounts->groupStringList.end(); ++it) {	// Split the list into lines.
		accounts->splitGroupEntry(*it);	// Split the line into its components and fill the variables of 'accounts'. (gr_name, gr_uid & gr_mem).
		new QListViewItem(groupsListView,QString::number(accounts->gr_gid),accounts->gr_name);
		if((accounts->gr_gid>=availableGID) && (accounts->gr_gid<65000)) availableGID=accounts->gr_gid+1;	// Maybe a new free GID.
	}
}        

void UserConfig::addGroup() {
	if(GroupDialog::addGroup(availableGID)) getGroups();	// Bring up the add group dialog.
}

void UserConfig::editGroup() {
	int gid;
	if(groupsListView->currentItem()) {	// Any group selected?
		gid=groupsListView->currentItem()->text(0).toInt();	// Get the GID from the listview.
		if(GroupDialog::editGroup(gid)) getGroups();	// Bring up the edit group dialog.
	} else  {
		QMessageBox::information(this,"No selection","No group has been selected.");
	}
}

void UserConfig::delGroup() {
	const char *groupname;
	if(groupsListView->currentItem()) {	// Any group selected?
		groupname=groupsListView->currentItem()->text(1);	// Get the groupname from the listview.
		if(QMessageBox::warning(this,"Delete group","Are you sure you want to\ndelete the group \""+QString(groupname)+"\" ?","&No","&Yes",0,0,1)) {
			// If confirmed, try to delete the group.
			if(GroupDialog::delGroup(groupname)) getGroups(); // And also update the view afterwards if the user was deleted.
		}
	} else  {
		QMessageBox::information(this,"No selection","No group has been selected.");
	}
}
