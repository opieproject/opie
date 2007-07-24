/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GROUPDIALOG_H
#define GROUPDIALOG_H

#include <qdialog.h>
#include <qlineedit.h>

class GroupDialog : public QDialog 
{
	Q_OBJECT
private:
	QString	gid;
	QString	groupname;
	QLineEdit *gidLineEdit;
	QLineEdit *groupnameLineEdit;
	int execStatus;
	
	void accept(void);	

public:
	GroupDialog( QWidget* parent = 0, const char* name = 0, bool modal=true, WFlags fl = 0 );
	~GroupDialog();
	static bool addGroup(int gid);
	static bool delGroup(const char *groupname);
	static bool editGroup(int gid);
};

#endif
