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

#ifndef __IRCMESSAGE_H
#define __IRCMESSAGE_H

#include <qstring.h>
#include <qstringlist.h>

/* IRCMessage objects are used to encapsulate information
   which the IRC server sent to us. */

class IRCMessage {
public:
    /* Parse an IRC message and create the IRCMessage object */
    IRCMessage(QString line);

    /* Return the IRC message prefix (usually sender etc) */
    QString prefix();
    /* Check if this IRCMessage's command is literal or numerical */
    bool isNumerical();
    /* CHeck if this IRCMessage is a CTCP message */
    bool isCTCP();
    /* Return the IRC command (literal commands) */
    QString command();
    /* Return the CTCP command */
    QString ctcpCommand();
    /* Return the CTCP destination if applicable (channel/person) */
    QString ctcpDestination();
    /* Return the IRC command (numerical commands) */
    unsigned short commandNumber();
    /* Return the trailing parameter string */
    QString trailing();
    /* Return the complete parameter string */
    QString allParameters();
    /* Return one parameter */ 
    QString param(int param);
protected:
    QString         m_prefix;
    QString         m_command;
    QString         m_ctcpCommand;
    QString         m_ctcpDestination;
    unsigned short  m_commandNumber;
    QString         m_allParameters;
    QString         m_trailing;
    QStringList     m_parameters;
    bool            m_isNumerical;
    bool            m_ctcp;
};

#endif
