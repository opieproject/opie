#include <unistd.h>
#include <string.h>
#include "ircconnection.h"

IRCConnection::IRCConnection(IRCServer *server) {
    m_server = server;
    m_socket = new QSocket(this);
    m_connected = FALSE;
    m_loggedIn = FALSE;
    connect(m_socket, SIGNAL(connected()), this, SLOT(login()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(dataReady()));
    connect(m_socket, SIGNAL(error(int)), this, SLOT(error(int)));
    connect(m_socket, SIGNAL(connectionClosed()), this, SLOT(disconnect()));
    connect(m_socket, SIGNAL(delayedCloseFinished()), this, SLOT(disconnect()));
}

/* Connect to the IRC server */
void IRCConnection::doConnect() {
    ASSERT(!m_connected);
    m_socket->connectToHost(m_server->hostname(), m_server->port());
}

/* Send commands to the IRC server */
void IRCConnection::sendLine(QString line) {
    while((line.right(1) == "\n") || (line.right(1) == "\r"))
        line = line.left(line.length() - 1);
    line.append("\r\n");
    m_socket->writeBlock(line, line.length());
}

void IRCConnection::sendCTCP(QString nick, QString line) {
    sendLine("NOTICE " + nick + " :\001"+line+"\001"); 
}

/*
 * login() is called right after the connection
 * to the IRC server has been established
 */
void IRCConnection::login() {
    char hostname[256];
    QString loginString;
    
    emit outputReady(IRCOutput(OUTPUT_CLIENTMESSAGE, tr("Connected, logging in ..")));
    m_connected = TRUE;
    gethostname(hostname, sizeof(hostname)-1);
    hostname[sizeof (hostname) - 1] = 0;
    
    /* Create a logon string and send it */
    if (m_server->password().length()>0) {
        loginString += "PASS " + m_server->password() + "\r\n";
    }
    loginString += "NICK " + m_server->nick() + "\r\n" +
                   "USER " + m_server->username() + " " + hostname +
                   " " + m_server->hostname() + " :" + m_server->realname() + "\r\n";
    sendLine(loginString);
}

/* Called when data arrives on the socket */
void IRCConnection::dataReady() {
    while(m_socket->canReadLine()) {
        IRCMessage message(m_socket->readLine());
        if (!m_loggedIn && message.isNumerical() && message.commandNumber() == 1) {
            m_loggedIn = TRUE;
            emit outputReady(IRCOutput(OUTPUT_CLIENTMESSAGE, tr("Successfully logged in.")));
        }
        emit messageArrived(&message);
    }
}

/* Called if any type of socket error occurs */
void IRCConnection::error(int num) {
    emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Socket error : ") + strerror(num)));
}

void IRCConnection::disconnect() {
    m_connected = FALSE;
    m_loggedIn = FALSE;
    emit outputReady(IRCOutput(OUTPUT_CONNCLOSE, tr("Connection closed")));
}

bool IRCConnection::isConnected() {
    return m_connected;
}

bool IRCConnection::isLoggedIn() {
    return m_loggedIn;
}

void IRCConnection::close() {
    m_socket->close(); 
    if (m_socket->state()==QSocket::Idle) {
        disconnect();
    }
}
