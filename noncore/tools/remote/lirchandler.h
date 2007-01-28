/*
Opie-Remote.  emulates remote controls on an iPaq (and maybe a Zaurus) in Opie.
Copyright (C) 2007 Paul Eggleton & Thomas Stephens

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef LIRCHANDLER_H
#define LIRCHANDLER_H

class LircHandler
{
private:
	bool connectLirc(void);
	const char *readPacket();
	
	struct sockaddr_un addr;
	int fd;
public:
	LircHandler(void);
	QStringList getRemotes(void);
	QStringList getButtons(const char *remoteName);
	int sendIR(const char *lircaction);
	bool startLircd(void);
	bool stopLircd(void);
	bool isLircdRunning(void);
};

#endif
