/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "groupdialog.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>

#include <stdlib.h>

#include "passwd.h"

GroupDialog::GroupDialog(QWidget* parent, const char* name, bool modal, WFlags fl) : QDialog(parent, name, modal, fl) {
	// GID
	QLabel *gidLabel=new QLabel(this,"gid: ");
	gidLabel->setText("GroupID: ");
	gidLineEdit=new QLineEdit(this,"gid: ");
	gidLineEdit->setEnabled(false);

	// Groupname
	QLabel *groupnameLabel=new QLabel(this,"groupname");
	groupnameLabel->setText("Groupname: ");
	groupnameLineEdit=new QLineEdit(this,"groupname");

		// Widget layout
	QVBoxLayout *layout=new QVBoxLayout(this);
	layout->setMargin(5);
	QHBoxLayout *hlayout=new QHBoxLayout(-1,"hlayout");
	layout->addLayout(hlayout);
	QVBoxLayout *vlayout1=new QVBoxLayout(-1,"vlayout1");
	QVBoxLayout *vlayout2=new QVBoxLayout(-1,"vlayout2");
		// First column, labels
		vlayout1->addWidget(gidLabel);
		vlayout1->addWidget(groupnameLabel);
		// Second column, data
		vlayout2->addWidget(gidLineEdit);
		vlayout2->addWidget(groupnameLineEdit);
	hlayout->addLayout(vlayout1);
	hlayout->addLayout(vlayout2);
	layout->addSpacing(5);
	
//	showMaximized();
}

GroupDialog::~GroupDialog() {
}

bool GroupDialog::addGroup(int gid) {
	GroupDialog *addgroupDialog=new GroupDialog();	// Make a groupinfo dialog.
	addgroupDialog->setCaption(tr("Add Group"));	// Set the caption.
	addgroupDialog->gidLineEdit->setText(QString::number(gid));	// Set the next available gid.
	if(!(addgroupDialog->exec())) return false;	// View the dialog, and only continue if 'ok' was pressed.
	if(!(accounts->addGroup(addgroupDialog->groupnameLineEdit->text(),addgroupDialog->gidLineEdit->text().toInt()))) {	// Try to add the group.
		QMessageBox::information(0,"Ooops!","Something went wrong.\nUnable to add group.");
		return false;
	}
	return true;
}

bool GroupDialog::editGroup(int gid) {
	GroupDialog *addgroupDialog=new GroupDialog();	// Create the groupinfo dialog.
	accounts->findGroup(gid);	// Locate the group in the database, and fill out the variables in 'accounts'.
	// Fill out the widgets with previous data for this group.
	addgroupDialog->setCaption(tr("Edit Group"));
	addgroupDialog->gidLineEdit->setText(QString::number(accounts->gr_gid));
	addgroupDialog->groupnameLineEdit->setText(accounts->gr_name);
	if(!(addgroupDialog->exec())) return false;	// View the dialog, and only continue if 'ok' was pressed.
	accounts->findGroup(gid);	// Locate the group, and fill out the variables in 'accounts' with all info about the group.
	accounts->gr_name=addgroupDialog->groupnameLineEdit->text();	// Change the name
	accounts->gr_gid=addgroupDialog->gidLineEdit->text().toInt();	// Change the GID. (Unneeded as its disabled right now.)
	accounts->updateGroup(gid);	// Update the database with the variables (gr_name,gr_gid,gr_mem) in 'accounts'.
	return true;
}

bool GroupDialog::delGroup(const char *groupname) {
	if((accounts->findGroup(groupname))) {	// Does this group exist?
		if(!(accounts->delGroup(groupname))) {	// Try to delete it.
			QMessageBox::information(0,"Ooops!","Something went wrong\nUnable to delete group: "+QString(groupname)+".");
		}
	} else {
		QMessageBox::information(0,"Invalid Groupname","That groupname ("+QString(groupname)+")does not exist.");
		return false;
	}
	return true;
}

void GroupDialog::accept() {
	// Check if gid is already taken.
//	if((accounts->findGroup(gidLineEdit->text().toInt()))) {
//		QMessageBox::information(this,"GroupID taken","That GroupID is already taken.");
//		return;
//	}
	// Check if groupname is already taken.
	if((accounts->findGroup(groupnameLineEdit->text()))) {
		QMessageBox::information(0,"Groupname taken","That groupname is already taken.");
		return;	// Don't close the dialog.
	}
	QDialog::accept();
}
