/*
Opie-Remote.  emulates remote controlls on an iPaq (and maybe a Zaurus) in Opie.
Copyright (C) 2002 Thomas Stephens

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <qdialog.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#define PACKET_SIZE 256
#define TIMEOUT 3

class ButtonDialog : public QDialog
{
	Q_OBJECT
public:
	ButtonDialog(QString buttonName, QWidget *parent=0, const char*name=0, bool modal=FALSE, WFlags f=0);
	void setIRSocket(int newfd);
	const char *readPacket();
	QStringList getRemotes();
	QStringList getButtons(const char *remoteName);
	QStringList getList();
	QString getLabel();
public slots:
	void remoteSelected(const QString &string);
	void buttonSelected(const QString &string);
private:
	QComboBox *remote;
	QComboBox *button;
	QStringList list;
	QLineEdit *label;
	int fd;
	int timeout;
	struct sockaddr_un addr;
};
