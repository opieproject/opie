#include "ircchannel.h"

IRCChannel::IRCChannel(QString channelname) {
    m_hasPeople = FALSE;
    m_channelname = channelname;
}

IRCChannel::~IRCChannel() {
    /* We want this to get deleted */
    m_people.setAutoDelete(TRUE);
}

QString IRCChannel::channelname() {
    return m_channelname;
}

bool IRCChannel::hasPeople() {
    return m_hasPeople;
}

void IRCChannel::setHasPeople(bool hasPeople) {
    m_hasPeople = hasPeople;
}

void IRCChannel::addPerson(IRCChannelPerson *person) {
    m_people.append(person);
}

void IRCChannel::removePerson(IRCChannelPerson *person) {
    m_people.remove(person);
}

QListIterator<IRCChannelPerson> IRCChannel::people()  {
    QListIterator<IRCChannelPerson> it(m_people);
    return it;
}

IRCChannelPerson *IRCChannel::getPerson(QString nickname) {
    QListIterator<IRCChannelPerson> it(m_people);
    for (; it.current(); ++it) {
        if (it.current()->person->nick() == nickname) {
            return it.current();
        }
    }
    return 0;
}

