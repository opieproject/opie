/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PASSWD_H
#define PASSWD_H

#include <qobject.h>
#include <qfile.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qdir.h>

#include <crypt.h>
#include <pwd.h>
#include <grp.h>

class Passwd
{
public:
	QStringList passwdStringList;	// List of all users (name,passwd,uid,gid,gecos,dir,shell)
	QStringList groupStringList;	// List of all groups (name,gid,mem)
	
	// pwentry
	QString pw_name;
	QString pw_passwd;
	int pw_uid;
	int pw_gid;
	QString pw_gecos;
	QString pw_dir;
	QString pw_shell;

	// grentry
	QString gr_name;
	int gr_gid;
	QStringList gr_mem;
	
	Passwd(void);
	~Passwd(void);
	bool open(void);
	bool close(void);
	char *crypt_make_salt(void);
	void splitPasswdEntry(QString &userString);
	void splitGroupEntry(QString &groupString);
	bool findUser(const char *username);
	bool findUser(int uid);
	bool addUser(QString pw_name, QString pw_passwd, int pw_uid, int pw_gid, QString pw_gecos, QString pw_dir, QString pw_shell, bool createGroup=true);
	bool updateUser(QString login);
	bool delUser(const char *username, bool delGroup=true);
	bool delUser(int uid, bool delGroup=true);
	bool findGroup(const char *groupname);
	bool findGroup(int gid);
	bool addGroup(QString gr_name, int gr_gid);
	bool updateGroup(int gid);
	bool delGroup(const char *groupname);
	bool delGroup(int gid);
	bool addGroupMember(QString groupname,QString member);
	bool delGroupMember(QString groupname,QString member);

private:
	QString userString;
	QString groupString;
	
	int i64c(int i);
	bool searchUser(QRegExp &userRegExp);
	bool deleteUser(QRegExp &userRegExp, bool delGroup);
	bool searchGroup(QRegExp &groupRegExp);
	bool deleteGroup(QRegExp &groupRegExp);
	QStringList userdataStringList;
	QStringList groupdataStringList;
	
	QFile *passwd_file;
	QFile *group_file;
};

extern Passwd *accounts;	// Create a global object.

#endif
