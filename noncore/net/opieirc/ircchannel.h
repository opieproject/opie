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

#ifndef __IRCCHANNEL_H
#define __IRCCHANNEL_H

#include <qobject.h>
#include <qlist.h>
#include <qstring.h>
#include "ircperson.h"

/* Flags which a person can have inside a channel */
enum IRCChannelPersonFlag {
    PERSON_FLAG_OP     = 0x01, 
    PERSON_FLAG_VOICE  = 0x02, 
    PERSON_FLAG_HALFOP = 0x04
};

/* This struct encapsulates a IRCPerson and adds
   channel specific information */
typedef struct IRCChannelPerson {
    IRCPerson *person;
    unsigned int flags;
};

/* IRCChannel is the object-oriented representation
   of an IRC channel. It basically acts as a container
   for IRCChannelPersons */
class IRCChannel : public QObject {
    Q_OBJECT
public:
    IRCChannel(QString channelname);
    ~IRCChannel();
   
    void addPerson(IRCChannelPerson *person);
    void removePerson(IRCChannelPerson *person);
    IRCChannelPerson *getPerson(QString nickname);
    QListIterator<IRCChannelPerson> people();

    /* hasPeople identifies whether the irc channel is
       done synchronizing with the current state -
       this is only relevant when joining a channel */
    void setHasPeople(bool hasPeople);
    QString channelname();
    bool hasPeople();
protected:
    QList<IRCChannelPerson>   m_people;
    QString                   m_channelname;
    bool                      m_hasPeople;
};

#endif /* __IRCCHANNEL_H */
