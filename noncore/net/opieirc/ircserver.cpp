#include <qpe/global.h>
#include <qpe/contact.h>

#include <qobject.h>
#include <qfile.h>

#include "ircserver.h"
#include "ircversion.h"


IRCServer::IRCServer() {
    m_port = 6667;
}

/* Setter implementations */

void IRCServer::setHostname(QString hostname) {
    m_hostname = hostname;
}

void IRCServer::setName(QString name) {
    m_name = name;
}

void IRCServer::setPort(int port) {
    m_port = port;
}

void IRCServer::setUsername(QString username) {
    m_username = username;
}

void IRCServer::setPassword(QString password) {
    m_password = password;
}

void IRCServer::setNick(QString nick) {
    m_nick = nick;
}

void IRCServer::setRealname(QString realname) {
    m_realname = realname;
}

void IRCServer::setChannels(QString channels) {
    m_channels = channels;
}

/* Getter implementations */

QString IRCServer::hostname() {
    return m_hostname;
}

QString IRCServer::name() {
    return m_name;
}

unsigned short int IRCServer::port() {
    if(m_port)
        return m_port;

    return 6667;
}

QString IRCServer::username() {
    return m_username;
}

QString IRCServer::password() {
    return m_password;
}

QString IRCServer::nick() {
    return m_nick;
}

QString IRCServer::realname() {
    if(!m_realname.isEmpty())
        return m_realname;
    // Try to fetch realname from addressbook
    QString file = Global::applicationFileName( "addressbook", "businesscard.vcf" );
    if ( QFile::exists( file ) ) {
        Contact cont = Contact::readVCard( file )[0];
        QString realName = cont.fullName();
        if(!realName.isEmpty())
            return realName;
    }

    return QString(APP_VERSION + QObject::tr(" User"));
}

QString IRCServer::channels() {
    return m_channels;
}

