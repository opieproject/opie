/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef USERDIALOG_H
#define USERDIALOG_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qcheckbox.h>

#include <qpe/resource.h>

#include <opie/ofiledialog.h>

class UserDialog : public QDialog 
{
	Q_OBJECT
private:
	QTabWidget *myTabWidget;
	QPushButton *picturePushButton;
	QLineEdit *loginLineEdit;
	QLineEdit *uidLineEdit;
	QLineEdit *gecosLineEdit;
	QLineEdit *passwordLineEdit;
	QComboBox *shellComboBox;
	QComboBox *groupComboBox;
	QLabel *skelLabel;
	QCheckBox *skelCheckBox;
	QListView *groupsListView;
	
	QStringList groupMembers;
	QString pictureLocation;
	QImage userImage;
	int groupID;
	int userID;
	int vm;
	enum VIEWMODE {
		VIEWMODE_NEW,
		VIEWMODE_EDIT
	};
		
	void setupTab1(void);
	void setupTab2(void);
	void accept(void);

private slots:
	void clickedPicture(void);

public:
	UserDialog( int viewmode=VIEWMODE_NEW, QWidget* parent = 0, const char* name = 0, bool modal=true, WFlags fl = 0 );
	~UserDialog();
	static bool addUser(int uid, int gid);
	static bool editUser(const char *username);
	static bool delUser(const char *username);
};

#endif
