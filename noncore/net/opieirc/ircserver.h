/*
    OpieIRC - An embedded IRC client
    Copyright (C) 2002 Wenzel Jakob

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef __IRCSERVER_H
#define __IRCSERVER_H

#include <qstring.h>

/* IRCServer stores all information required to
   establish a connection to a server. */

class IRCServer {
public:
    IRCServer();

    void setHostname(QString hostname);
    void setDescription(QString description);
    void setPort(int port);
    void setUsername(QString username);
    void setPassword(QString password);
    void setNick(QString nick);
    void setRealname(QString realname);

    QString hostname();
    QString description();
    unsigned short int port();
    QString username();
    QString password();
    QString nick();
    QString realname();
protected:
    QString             m_hostname;
    QString             m_description;
    unsigned short int  m_port;
    QString             m_username;
    QString             m_password;
    QString             m_nick;
    QString             m_realname;
};

#endif /* __IRCSERVER_H */
