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

#ifndef __IRCOUTPUT_H
#define __IRCOUTPUT_H

#include <qstring.h>
#include <qlist.h>
#include "ircchannel.h"

/* Types of possible IRC output */
enum IRCOutputType {
    OUTPUT_ERROR = -1,          /* parameters : none */
    OUTPUT_SERVERMESSAGE  = 0,   /* parameters : none */
    OUTPUT_CLIENTMESSAGE  = 1,   /* parameters : none */
    OUTPUT_CHANPRIVMSG    = 2,   /* parameters : channel (IRCChannel), person (IRCChannelPerson) */
    OUTPUT_QUERYPRIVMSG   = 3,   /* parameters : person (IRCPerson) */
    OUTPUT_NICKCHANGE     = 4,   /* parameters : person (IRCPerson) */
    OUTPUT_SELFJOIN       = 5,   /* parameters : channel (IRCChannel) */
    OUTPUT_OTHERJOIN      = 6,   /* parameters : channel (IRCChannel), person (IRCChannelPerson) */
    OUTPUT_SELFPART       = 7,   /* parameters : channel (IRCChannel) */
    OUTPUT_OTHERPART      = 8,   /* parameters : channel (IRCChannel), person (IRCChannelPerson) */
    OUTPUT_QUIT           = 9,   /* parameters : person (IRCPerson) */
    OUTPUT_CONNCLOSE      = 10,  /* parameters : none */
    OUTPUT_CTCP           = 11,  /* parameters : none */
    OUTPUT_SELFKICK       = 12,  /* parameters : channel (IRCChannel) */
    OUTPUT_OTHERKICK      = 13,  /* parameters : channel (IRCChannel) person (IRCChannelPerson) */
    OUTPUT_CHANACTION     = 14,  /* parameters : channel (IRCChannel) person (IRCChannelPerson) */
    OUTPUT_QUERYACTION    = 15,  /* parameters : person (IRCPerson) */
    OUTPUT_CHANPERSONMODE = 16   /* parameters : channel (IRCCHannel) person (IRCChannelPerson) */
};

/* The IRCOutput class is used as a kind of message which is sent by the
   IRC parser to inform the GUI of changes. This could for example be a 
   channel message or a nickname change */

class IRCOutput {
public:
    IRCOutput(IRCOutputType type, QString message);
    /* Used to add a parameter to this IRCOutput. Parameters are dependent
       on which IRCOutputType we are using (see above) */
    void addParam(void *data);
    
    IRCOutputType type();
    QString message();
    void *getParam(int index);
protected:
    IRCOutputType      m_type;
    QString            m_message;
    QList<void>        m_parameters;
};

#endif
