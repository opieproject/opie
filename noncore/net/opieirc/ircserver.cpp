#include "ircserver.h"

IRCServer::IRCServer() {
    m_port = 6667;
}

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

QString IRCServer::hostname() {
    return m_hostname;
}

QString IRCServer::name() {
    return m_name;
}

unsigned short int IRCServer::port() {
    return m_port;
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
    return m_realname;
}

