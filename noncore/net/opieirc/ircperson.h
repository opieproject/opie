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

#ifndef __IRCPERSON
#define __IRCPERSON

#include <qstring.h>

/* This class requires all required information relating to a person
   on the IRC network. This class can be used as an input mask for
   IRCMessage-prefixes too
*/

class IRCPerson {
public:
    IRCPerson();
    /* Create an IRCPerson from an IRC style description (nick!user@host) */
    IRCPerson(QString mask);
    /* Copy constructor */
    IRCPerson(const IRCPerson &person);

    void fromMask(QString mask);
    void setNick(QString name);
    void setUser(QString user);
    void setHost(QString host);
    QString toMask();
    QString nick();
    QString user();
    QString host();
protected:
    QString m_nick;
    QString m_user;
    QString m_host;
};

#endif /* __IRCPERSON */
