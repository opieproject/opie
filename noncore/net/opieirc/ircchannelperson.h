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

#ifndef IRCCHANNELPERSON_H
#define IRCCHANNELPERSON_H

class QString;
class IRCPerson;

/* This class encapsulates a IRCPerson and adds
 *    channel specific information */
class IRCChannelPerson {

public:
    /* Flags which a person can have inside a channel */
    enum IRCChannelPersonFlag {
            PERSON_FLAG_OP     = 0x01,
            PERSON_FLAG_VOICE  = 0x02,
            PERSON_FLAG_HALFOP = 0x04
    };

    IRCChannelPerson(IRCPerson *person = 0);
    ~IRCChannelPerson();

    QString setOp(const QString &nickname, bool set);
    QString setVoice(const QString &nickname, bool set);
    QString nick();
    const unsigned int flags();
    void setFlags(int flags);
    void setNick(const QString &nickname);

protected:
    IRCPerson *m_person;
    unsigned int m_flags;
};

#endif
