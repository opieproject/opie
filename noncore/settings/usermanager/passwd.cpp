/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "passwd.h"

// Needed for crypt_make_salt();
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

Passwd::Passwd() {
}

Passwd::~Passwd() {
}

// This function is taken from 'busybox'.
int Passwd::i64c(int i)
{
	if (i <= 0)
		return ('.');
	if (i == 1)
		return ('/');
	if (i >= 2 && i < 12)
		return ('0' - 2 + i);
	if (i >= 12 && i < 38)
		return ('A' - 12 + i);
	if (i >= 38 && i < 63)
		return ('a' - 38 + i);
	return ('z');
}

// This function is taken from 'busybox'.
char *Passwd::crypt_make_salt() {
	time_t now;
	static unsigned long x;
	static char result[3];

	time(&now);
	x += now + getpid() + clock();
	result[0] = i64c(((x >> 18) ^ (x >> 6)) & 077);
	result[1] = i64c(((x >> 12) ^ x) & 077);
	result[2] = '\0';
	return result;
}

bool Passwd::open() {
	int returnvalue=false;
	
	QFile passwd_file("/etc/passwd");
	QFile group_file("/etc/group");
	passwdStringList.clear();
	groupStringList.clear();
	if((passwd_file.open(IO_ReadOnly))) {
		if((group_file.open(IO_ReadOnly))) {
			QTextStream ts_passwd(&passwd_file);
			while(!(ts_passwd.eof())) {
				passwdStringList << ts_passwd.readLine();
			}
			QTextStream ts_group(&group_file);
			while(!(ts_group.eof())) {
				groupStringList << ts_group.readLine();
			}
			returnvalue=true;
			group_file.close();
		}
		passwd_file.close();
	}
	return returnvalue;
}

bool Passwd::close() {
	int returnvalue=false;
	QFile passwd_file("/etc/passwd");
	QFile group_file("/etc/group");
	if((passwd_file.open(IO_WriteOnly))) {
		if((group_file.open(IO_WriteOnly))) {
			QTextStream ts_passwd(&passwd_file);
			for(QStringList::Iterator it=passwdStringList.begin(); it!=passwdStringList.end(); ++it) {
				ts_passwd << (*it) + "\n";
			}
			QTextStream ts_group(&group_file);
			for(QStringList::Iterator it=groupStringList.begin(); it!=groupStringList.end(); ++it) {
				ts_group << (*it) + "\n";
			}
			returnvalue=true;
			group_file.close();
		}
		passwd_file.close();
	}
	return returnvalue;
}

void Passwd::splitPasswdEntry(QString &userString) {
	userdataStringList=QStringList::split(":",userString,true);
	QStringList::Iterator it=userdataStringList.begin();
	pw_name=(*it++);
	pw_passwd=(*it++);
	pw_uid=(*it++).toInt();
	pw_gid=(*it++).toInt();
	pw_gecos=(*it++);
	pw_dir=(*it++);
	pw_shell=(*it++);
}

void Passwd::splitGroupEntry(QString &groupString) {
	groupdataStringList=QStringList::split(":",groupString,true);
	QStringList::Iterator it=groupdataStringList.begin();
	gr_name=(*it++);
	it++;
	gr_gid=(*it++).toInt();
	gr_mem=QStringList::split(" ",(*it++));
}

bool Passwd::searchUser(QRegExp &userRegExp) {
	QStringList tempStringList(passwdStringList.grep(userRegExp));
	if((tempStringList.isEmpty())) {
		return false;
	} else {
		userString=(*(tempStringList.begin()));
		splitPasswdEntry(userString);
	}
	return true;
}

bool Passwd::findUser(const char *username) {
	QRegExp userRegExp(QString("^%1\\:").arg(username));
	return searchUser(userRegExp);
}

bool Passwd::findUser(int uid) {
	QRegExp userRegExp(QString(":%1\\:").arg(uid));
	return searchUser(userRegExp);
}

bool Passwd::addUser(QString pw_name, QString pw_passwd, int pw_uid, int pw_gid, QString pw_gecos,QString pw_dir, QString pw_shell, bool createGroup) {
	QString tempString;
	if((createGroup) && (!(findGroup(pw_gid)))) addGroup(pw_name,pw_gid);
	pw_passwd = crypt(pw_passwd, crypt_make_salt());
	tempString=pw_name+":"+pw_passwd+":"+QString::number(pw_uid)+":"+QString::number(pw_gid)+":"+pw_gecos+":"+pw_dir+":"+pw_shell;
	passwdStringList.append(tempString);
	// Make home dir.
	QDir d;
	if(!(d.exists(pw_dir))) {
		d.mkdir(pw_dir);
		chown(pw_dir,pw_uid,pw_gid);
		chmod(pw_dir,S_IRUSR|S_IWUSR|S_IXUSR);
	}
	return 1;
}

bool Passwd::updateUser(QString login) {
	QRegExp userRegExp(QString("^%1\\:").arg(login));
	for(QStringList::Iterator it=passwdStringList.begin(); it!=passwdStringList.end(); ++it) {
		if(userRegExp.find((*it),0)!=-1) {
			*it=QString(pw_name+":"+pw_passwd+":"+QString::number(pw_uid)+":"+QString::number(pw_gid)+":"+pw_gecos+":"+pw_dir+":"+pw_shell);
			return true;
		}
	}
	return false;
}

bool Passwd::deleteUser(QRegExp &userRegExp, bool delGroup) {
	for(QStringList::Iterator it=passwdStringList.begin(); it!=passwdStringList.end(); ++it) {
		if(userRegExp.find((*it),0)!=-1) {
			splitPasswdEntry(*it);
			if(delGroup) this->delGroup(pw_gid);
			passwdStringList.remove(it);
			return true;
		}
	}
	return false;
}

bool Passwd::delUser(const char *username, bool delGroup) {
	QRegExp userRegExp(QString("^%1\\:").arg(username));
	return deleteUser(userRegExp,delGroup);
}

bool Passwd::delUser(int uid, bool delGroup) {
	QRegExp userRegExp(QString(":%1\\:").arg(uid));
	return deleteUser(userRegExp,delGroup);
}

bool Passwd::searchGroup(QRegExp &groupRegExp) {
	QStringList tempStringList(groupStringList.grep(groupRegExp));
	if((tempStringList.isEmpty())) {
		return false;
	} else {
		groupString=(*(tempStringList.begin()));
		splitGroupEntry(groupString);
	}
	return true;
}

bool Passwd::findGroup(const char *groupname) {
	QRegExp groupRegExp(QString("^%1\\:").arg(groupname));
	return searchGroup(groupRegExp);
}

bool Passwd::findGroup(int gid) {
	QRegExp groupRegExp(QString(":%1\\:").arg(gid));
	return searchGroup(groupRegExp);
}

bool Passwd::addGroup(QString gr_name, int gr_gid) {
	QString tempString;
	tempString=gr_name+":*:"+QString::number(gr_gid)+":";
	groupStringList.append(tempString);
	return 1;
}

bool Passwd::updateGroup(int gid) {
	QRegExp groupRegExp(QString(":%1\\:").arg(QString::number(gid)));
	for(QStringList::Iterator it=groupStringList.begin(); it!=groupStringList.end(); ++it) {
		if(groupRegExp.find((*it),0)!=-1) {
			*it=QString(gr_name+":*:"+QString::number(gr_gid)+":");
			for(QStringList::Iterator member=gr_mem.begin(); member!=gr_mem.end(); ++member) {
				*it+=*member;
				*it+=" ";
			}
			return true;
		}
	}
	return false;
}

bool Passwd::deleteGroup(QRegExp &groupRegExp) {
	for(QStringList::Iterator it=groupStringList.begin(); it!=groupStringList.end(); ++it) {
		if(groupRegExp.find((*it),0)!=-1) {
			groupStringList.remove(it);
			return true;
		}
	}
	return false;
}

bool Passwd::delGroup(const char *groupname) {
	QRegExp groupRegExp(QString("^%1\\:").arg(groupname));
	return deleteGroup(groupRegExp);
}

bool Passwd::delGroup(int gid) {
	QRegExp groupRegExp(QString(":%1\\:").arg(gid));
	return deleteGroup(groupRegExp);
}

bool Passwd::addGroupMember(QString groupname, QString member) {
	if(!(findGroup(groupname))) return false;
	gr_mem << member;
	if(!(updateGroup(gr_gid))) return false;
	return true;
}

bool Passwd::delGroupMember(QString groupname, QString member) {
	if(!(findGroup(groupname))) return false;
	for(QStringList::Iterator it=gr_mem.begin(); it!=gr_mem.end(); ++it) {
		if(*it==member) {
			gr_mem.remove(it);
			it=gr_mem.end();
		}
	}
	if(!(updateGroup(gr_gid))) return false;
	return true;
}

Passwd *accounts;
