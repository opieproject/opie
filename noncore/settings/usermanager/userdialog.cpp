/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "userdialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qmessagebox.h>

#include <stdlib.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include "passwd.h"

/**
 * UserDialog constructor. Setup the dialog, fill the groupComboBox & groupsListView with all groups.
 *
 */
UserDialog::UserDialog(QWidget* parent, const char* name, bool modal, WFlags fl) : QDialog(parent, name, modal, fl) {
	QVBoxLayout *layout = new QVBoxLayout(this);
	myTabWidget=new QTabWidget(this,"User Tab Widget");
	layout->addWidget(myTabWidget);
	setupTab1();
	setupTab2();

	// And also fill the listview & the combobox with all available groups.
	for( QStringList::Iterator it = accounts->groupStringList.begin(); it!=accounts->groupStringList.end(); ++it) {
		accounts->splitGroupEntry(*it);
		//new QListViewItem(groupsListView,accounts->gr_name);
		new QCheckListItem(groupsListView,accounts->gr_name,QCheckListItem::CheckBox);
		groupComboBox->insertItem(accounts->gr_name);
	}
	
	showMaximized();
}

/**
 * Empty destructor.
 *
 */
UserDialog::~UserDialog() {
}

/**
 *  Creates the first tab, all userinfo is here.
 *
 */
void UserDialog::setupTab1() {
	QPixmap mypixmap;
	QWidget *tabpage = new QWidget(myTabWidget,"page1");
	QVBoxLayout *layout = new QVBoxLayout(tabpage);
	layout->setMargin(5);

	// Picture
	picturePushButton = new QPushButton(tabpage,"Label");
	picturePushButton->setMinimumSize(48,48);
	picturePushButton->setMaximumSize(48,48);
	picturePushButton->setPixmap(Resource::loadPixmap("usermanager/usericon"));	// Load default usericon.
	connect(picturePushButton,SIGNAL(clicked()),this,SLOT(clickedPicture()));	// Clicking the picture should invoke pictureselector.
	
	// Login
	QLabel *loginLabel=new QLabel(tabpage,"Login: ");
	loginLabel->setText("Login: ");
	loginLineEdit=new QLineEdit(tabpage,"Login: ");

	// UID
	QLabel *uidLabel=new QLabel(tabpage,"uid: ");
	uidLabel->setText("UserID: ");
	uidLineEdit=new QLineEdit(tabpage,"uid: ");
	uidLineEdit->setEnabled(false);

	// Username (gecos)
	QLabel *gecosLabel=new QLabel(tabpage,"gecos");
	gecosLabel->setText("Username: ");
	gecosLineEdit=new QLineEdit(tabpage,"gecos");

	// Password
	QLabel *passwordLabel=new QLabel(tabpage,"password");
	passwordLabel->setText("Password: ");
	passwordLineEdit=new QLineEdit(tabpage,"password");
	passwordLineEdit->setEchoMode(QLineEdit::Password);

	// Shell
	QLabel *shellLabel=new QLabel(tabpage,"shell");
	shellLabel->setText("Shell: ");
	shellComboBox=new QComboBox(tabpage,"shell");
	shellComboBox->setEditable(true);
	shellComboBox->insertItem("/bin/sh");
	shellComboBox->insertItem("/bin/ash");
	shellComboBox->insertItem("/bin/false");
	
	// Primary Group
	QLabel *groupLabel=new QLabel(tabpage,"group");
	groupLabel->setText("Primary group: ");
	groupComboBox=new QComboBox(tabpage,"PrimaryGroup");

		// Widget layout
	QHBoxLayout *hlayout=new QHBoxLayout(-1,"hlayout");
	layout->addWidget(picturePushButton);
	layout->addSpacing(5);
	layout->addLayout(hlayout);
	QVBoxLayout *vlayout1=new QVBoxLayout(-1,"vlayout1");
	QVBoxLayout *vlayout2=new QVBoxLayout(-1,"vlayout2");
		// First column, labels
		vlayout1->addWidget(loginLabel);
		vlayout1->addSpacing(5);
		vlayout1->addWidget(uidLabel);
		vlayout1->addSpacing(5);
		vlayout1->addWidget(gecosLabel);
		vlayout1->addSpacing(5);
		vlayout1->addWidget(passwordLabel);
		vlayout1->addSpacing(5);
		vlayout1->addWidget(shellLabel);
		vlayout1->addSpacing(5);
		vlayout1->addWidget(groupLabel);
		// Second column, data
		vlayout2->addWidget(loginLineEdit);
		vlayout2->addSpacing(5);
		vlayout2->addWidget(uidLineEdit);
		vlayout2->addSpacing(5);
		vlayout2->addWidget(gecosLineEdit);
		vlayout2->addSpacing(5);
		vlayout2->addWidget(passwordLineEdit);
		vlayout2->addSpacing(5);
		vlayout2->addWidget(shellComboBox);
		vlayout2->addSpacing(5);
		vlayout2->addWidget(groupComboBox);
	hlayout->addLayout(vlayout1);
	hlayout->addLayout(vlayout2);
	
	myTabWidget->addTab(tabpage,"User Info");
}

/**
 *  Creates the second tab containing additional groups for the user.
 *
 */
void UserDialog::setupTab2() {
	QWidget *tabpage = new QWidget(myTabWidget,"page2");
	QVBoxLayout *layout = new QVBoxLayout(tabpage);
	layout->setMargin(5);

	// Additional groups
	groupsListView=new QListView(tabpage,"groups");
	groupsListView->addColumn("Additional groups");
	groupsListView->setColumnWidthMode(0,QListView::Maximum);
	groupsListView->setMultiSelection(true);
	groupsListView->setAllColumnsShowFocus(false);
	
	layout->addSpacing(5);
		// Grouplist
		layout->addWidget(groupsListView);
	
	myTabWidget->addTab(tabpage,"User Groups");
}

/**
 * Static function that creates the userinfo dialog.
 * The user will be prompted to add a user. 
 *
 * @param uid This is a suggested available UID.
 * @param gid This is a suggested available GID.
 *
 * @return <code>true</code> if the user was successfully added, otherwise <code>false</code>.
 *
 */
bool UserDialog::addUser(int uid, int gid) {
	UserDialog *adduserDialog=new UserDialog();
	adduserDialog->setCaption(tr("Add User"));
	adduserDialog->userID=uid;	// Set next available UID as default uid.
	adduserDialog->groupID=gid;	// Set next available GID as default gid.
	// Insert default group into groupComboBox
	adduserDialog->groupComboBox->insertItem("<create new group>",0);
	adduserDialog->uidLineEdit->setText(QString::number(uid));
	// Show the dialog! 
	if(!(adduserDialog->exec())) return false;
	if((adduserDialog->groupComboBox->currentItem()!=0)) {
		accounts->findGroup(adduserDialog->groupComboBox->currentText());
		adduserDialog->groupID=accounts->gr_gid;
		qWarning(QString::number(accounts->gr_gid));
	}
	if(!(accounts->addUser(adduserDialog->loginLineEdit->text(), adduserDialog->passwordLineEdit->text(), 
							adduserDialog->uidLineEdit->text().toInt(), adduserDialog->groupID, adduserDialog->gecosLineEdit->text(), 
							QString("/home/")+adduserDialog->loginLineEdit->text() , adduserDialog->shellComboBox->currentText()))) {
		QMessageBox::information(0,"Ooops!","Something went wrong!\nUnable to add user.");
		return false;
	}
    
	// Add User to additional groups.
	QListViewItemIterator it( adduserDialog->groupsListView );
	for ( ; it.current(); ++it ) {
	if ( it.current()->isSelected() )
		accounts->addGroupMember(it.current()->text(0),adduserDialog->loginLineEdit->text());
	}
	// Copy image to pics/users/
	if(!(adduserDialog->userImage.isNull())) {
		QDir d;
		if(!(d.exists("/opt/QtPalmtop/pics/users"))) {
			d.mkdir("/opt/QtPalmtop/pics/users");
		}
		QString filename="/opt/QtPalmtop/pics/users/"+accounts->pw_name+".png";
		adduserDialog->userImage=adduserDialog->userImage.smoothScale(48,48);
		adduserDialog->userImage.save(filename,"PNG");
	}
	return true;
}

/**
 * Deletes the user account.
 *
 * @param username User to be deleted.
 *
 * @return <code>true</code> if the user was successfully deleted, otherwise <code>false</code>.
 *
 */
bool UserDialog::delUser(const char *username) {
	if((accounts->findUser(username))) {	// Does that user exist?
		if(!(accounts->delUser(username))) {	// Delete the user.
			QMessageBox::information(0,"Ooops!","Something went wrong\nUnable to delete user: "+QString(username)+".");
		}
	} else {
		QMessageBox::information(0,"Invalid Username","That username ("+QString(username)+")does not exist.");
		return false;
	}
	return true;
}

/**
 * This displays a confirmation dialog wether a user should be deleted or not.
 *  (And also deletes the account)
 *
 * @param username User to be deleted.
 *
 * @return <code>true</code> if the user was successfully deleted, otherwise <code>false</code>.
 *
 */
bool UserDialog::editUser(const char *username) {
	UserDialog *edituserDialog=new UserDialog();	// Create Dialog
	edituserDialog->setCaption(tr("Edit User"));
	accounts->findUser(username);	// Locate user in database and fill variables in 'accounts' object.
	accounts->findGroup(accounts->pw_gid);	// Locate the user's primary group, and fill group variables in 'accounts' object.
	// Fill widgets with userinfo.
	edituserDialog->loginLineEdit->setText(accounts->pw_name);
	edituserDialog->uidLineEdit->setText(QString::number(accounts->pw_uid));
	edituserDialog->gecosLineEdit->setText(accounts->pw_gecos);
	// Set password to '........', we will later check if this still is the contents, if not, the password has been changed.
	edituserDialog->passwordLineEdit->setText("........");
	// If this user is not using /bin/sh,/bin/ash or /bin/false as shell, add that entry to the shell-combobox.
	if(accounts->pw_shell!="/bin/sh" && accounts->pw_shell!="/bin/ash" && accounts->pw_shell!="/bin/false") {
		edituserDialog->shellComboBox->insertItem(accounts->pw_shell,0);
		edituserDialog->shellComboBox->setCurrentItem(0);
	}
	// Select the primary group for this user.
	for(int i=0;i<edituserDialog->groupComboBox->count();++i) {
		if(accounts->gr_name==edituserDialog->groupComboBox->text(i)) {
			edituserDialog->groupComboBox->setCurrentItem(i);
		}
	}
	// Select the groups in the listview, to which the user belongs.
	QRegExp userRegExp(QString("[:\\s]%1\\s").arg(username));
	QStringList tempList=accounts->groupStringList.grep(userRegExp);	// Find all entries in the group database, that the user is a member of.
	for(QStringList::Iterator it=tempList.begin(); it!=tempList.end(); ++it) {	// Iterate over all of them.
		QListViewItemIterator lvit( edituserDialog->groupsListView );	// Compare to all groups.
		for ( ; lvit.current(); ++lvit ) {
			if(lvit.current()->text(0)==(*it).left((*it).find(":"))) {
				lvit.current()->setSelected(true);	// If we find a line with that groupname, select it.;
			}
		}
	}
	
	if(!(edituserDialog->exec())) return false;	// SHOW THE DIALOG!
	
	accounts->findUser(username);	// Fill user variables in 'acccounts' object.
	accounts->pw_name=edituserDialog->loginLineEdit->text();
	// Has the password been changed ? Make a new "crypt":ed password.
	if(edituserDialog->passwordLineEdit->text()!="........") accounts->pw_passwd=crypt(edituserDialog->passwordLineEdit->text(), accounts->crypt_make_salt());
	
	// Set all variables in accounts object, that will be used when calling 'updateUser()'
	accounts->pw_uid=edituserDialog->uidLineEdit->text().toInt();
	accounts->findGroup(edituserDialog->groupComboBox->currentText());	// Fill all group variables in 'accounts' object.
	accounts->pw_gid=accounts->gr_gid;
	accounts->pw_gecos=edituserDialog->gecosLineEdit->text();
	accounts->pw_shell=edituserDialog->shellComboBox->currentText();
	// Update userinfo, using the information stored in the user variables stored in the accounts object.
	accounts->updateUser(username);
	
	// Remove user from all groups he/she is a member of. (could be done in a better way I guess, this was simple though.)
	for(QStringList::Iterator it=tempList.begin(); it!=tempList.end(); ++it) {
		accounts->delGroupMember((*it).left((*it).find(":")),username);
	}
	
	// Add User to additional groups that he/she is a member of.
	QListViewItemIterator it( edituserDialog->groupsListView );
	for ( ; it.current(); ++it ) {
	if ( it.current()->isSelected() )
		accounts->addGroupMember(it.current()->text(0),edituserDialog->loginLineEdit->text());
	}
	
	// Copy image to pics/users/
	if(!(edituserDialog->userImage.isNull())) {
		QDir d;
		if(!(d.exists("/opt/QtPalmtop/pics/users"))) {
			d.mkdir("/opt/QtPalmtop/pics/users");
		}
		QString filename="/opt/QtPalmtop/pics/users/"+accounts->pw_name+".png";
		edituserDialog->userImage=edituserDialog->userImage.smoothScale(48,48);
		edituserDialog->userImage.save(filename,"PNG");
	}
	return true;
}

/**
 * "OK" has been clicked. Verify some information before closing the dialog.
 *
 */
void UserDialog::accept() {
	// Add checking... valid username? username taken? 
	if(loginLineEdit->text().isEmpty()) {
		QMessageBox::information(0,"Empty Login","Please enter a login.");
		return;
	}
	QDialog::accept();
}

/**
 * This slot is called when the usericon is clicked, this loads (should) the iconselector.
 *
 */
void UserDialog::clickedPicture() {
	QString filename=OFileDialog::getOpenFileName(OFileSelector::EXTENDED,"/opt/QtPalmtop/pics");
	if(!(filename.isEmpty())) {
		userImage.reset();
		if(!(userImage.load(filename))) {
			QMessageBox::information(0,"Sorry!","Icon selection not yet implemented.\nComming real soon now! (tm)\n"+filename);
		} else {
			userImage=userImage.smoothScale(48,48);
			QPixmap *picture;
			picture=(QPixmap *)picturePushButton->pixmap();
			picture->convertFromImage(userImage,0);
			picturePushButton->update();
		}
	}
}
