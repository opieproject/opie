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

#ifndef __IRCCONNECTION_H
#define __IRCCONNECTION_H

#include <qsocket.h>
#include "ircserver.h"
#include "ircmessage.h"
#include "ircoutput.h"

/* IRCConnection acts as a wrapper around QSocket
   and is responsible for the communication with the
   IRC server */
class IRCConnection : public QObject {
    Q_OBJECT
public:
    IRCConnection(IRCServer *server);
    void doConnect();
    /* used to send commands to the IRC server */
    void sendLine(QString line);
    /* used to send CTCP commands to the IRC server */
    void sendCTCP(QString nick, QString line);
    void close();
    bool isConnected();
    bool isLoggedIn();
signals:
    /* triggered when the server sent us a message */
    void messageArrived(IRCMessage *message);
    /* Used to send commands to the UI (such as displaying text etc) */
    void outputReady(IRCOutput output);
protected slots:
    /* automatically executed after the connection is established */
    void login();
    /* called when there are socket errors */
    void error(int num);
    /* called when data arrived on m_socket (triggers messageArrived) */
    void dataReady();
    /* called when the IRC server closes the connection */
    void disconnect();
protected:
    IRCServer  *m_server;
    QSocket    *m_socket;
    bool        m_connected;
    bool        m_loggedIn;
};

#endif /* __IRCCONNECTION_H */
