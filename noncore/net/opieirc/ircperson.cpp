#include "ircperson.h"

IRCPerson::IRCPerson() {
    m_nick = "";
    m_user = "";
    m_host = "";
}

IRCPerson::IRCPerson(const IRCPerson &person) {
    m_nick = person.m_nick;
    m_user = person.m_user;
    m_host = person.m_host;
}

IRCPerson::IRCPerson(QString mask) {
    IRCPerson();
    fromMask(mask);
}

void IRCPerson::fromMask(QString mask) {
    int sep1 = mask.find("!");
    int sep2 = mask.find("@");
    
    m_nick = mask.left(sep1);
    m_user = mask.mid(sep1+1, sep2-sep1-1);
    m_host = mask.right(mask.length()-sep2-1);
}

QString IRCPerson::toMask() {
    return m_nick + "!" + m_user + "@" + m_host; 
}

void IRCPerson::setNick(QString nick) {
    m_nick = nick;
}

void IRCPerson::setUser(QString user) {
    m_user = user;
}

void IRCPerson::setHost(QString host) {
    m_host = host;
}

QString IRCPerson::nick() {
    return m_nick;
}

QString IRCPerson::user() {
    return m_user;
}

QString IRCPerson::host() {
    return m_host;
}
