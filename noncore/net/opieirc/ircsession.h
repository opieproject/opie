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

#ifndef __IRCSESSION_H
#define __IRCSESSION_H

#include <qstring.h>
#include <qlist.h>
#include "ircserver.h"
#include "ircconnection.h"
#include "ircmessage.h"
#include "ircchannel.h"
#include "ircoutput.h"

class IRCMessageParser;

/* The IRCSession stores all information relating to the connection
   to one IRC server. IRCSession makes it possible to run multiple
   IRC server connections from within the same program */

class IRCSession : public QObject {
friend class IRCMessageParser;
    Q_OBJECT
public:
    IRCSession(IRCServer *server);
    ~IRCSession();
    
    void join(QString channel);
    void part(IRCChannel *channel);
    void beginSession();
    bool isSessionActive();
    void endSession();

    void sendMessage(IRCPerson *person, QString message);
    void sendMessage(IRCChannel *channel, QString message);
    void sendAction(IRCPerson *person, QString message);
    void sendAction(IRCChannel *channel, QString message);
    IRCChannel *getChannel(QString channelname);
    IRCPerson  *getPerson(QString nickname);
protected:
    void addPerson(IRCPerson *person);
    void addChannel(IRCChannel *channel);
    void removeChannel(IRCChannel *channel);
    void removePerson(IRCPerson *person);
    void getChannelsByPerson(IRCPerson *person, QList<IRCChannel> &channels);
protected slots:
    void handleMessage(IRCMessage *message);
signals:
    void outputReady(IRCOutput output);
protected:
    IRCServer             *m_server;
    IRCConnection         *m_connection;
    IRCMessageParser      *m_parser;
    QList<IRCChannel>      m_channels;
    QList<IRCPerson>       m_people;
};

#endif /* __IRCSESSION_H */
