/*
    This file is part of OpieIRC - An embedded IRC client
    Copyright (C) 2005 Alberto 'Skyhusker' García Hierro
    <skyhusker@handhelds.org>

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

#include "ircperson.h"
#include "ircchannelperson.h"

#include <qstring.h>
#include <qobject.h>

IRCChannelPerson::IRCChannelPerson(IRCPerson *person)
{
    m_person = person;
    m_flags = 0;
}

IRCChannelPerson::~IRCChannelPerson()
{
    //if(m_person)
    //    delete m_person;
}
    
QString IRCChannelPerson::setOp(const QString &nickname, bool set)
{
    if(set) {
        m_flags |= PERSON_FLAG_OP;
        return ( nickname + QObject::tr(" gives channel operator status to ") + nick());
    }

    m_flags &= 0xFFFF - PERSON_FLAG_OP;
    return ( nickname + QObject::tr(" removes channel operator status from ") + nick());
}

QString IRCChannelPerson::setVoice(const QString &nickname, bool set)
{
    if(set) {
        m_flags |= PERSON_FLAG_VOICE;
        return ( nickname + QObject::tr(" gives voice to ") + nick() );
    }

    m_flags &= 0xFFFF - PERSON_FLAG_VOICE;
    return ( nickname + QObject::tr(" removes voice from ") + nick());
}

QString IRCChannelPerson::nick()
{
    if(m_person)
        return m_person->nick();
    
    return QString::null;
}

void IRCChannelPerson::setFlags(int flags)
{
    m_flags = flags;
}

void IRCChannelPerson::setNick(const QString &nickname)
{
    m_person->setNick(nickname);
}

const unsigned int IRCChannelPerson::flags()
{
    return m_flags;
}
