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

#ifndef __IRCMESSAGEPARSER_H
#define __IRCMESSAGEPARSER_H

#include "ircsession.h"

/* Macro to facilitate the parser table's creation */
#define FUNC(__proc) &IRCMessageParser::__proc

class IRCMessageParser;

/* Typedef representing a parser function */
typedef void (IRCMessageParser::*IRCMessageParseProc)(IRCMessage *);

/* Struct representing a literal command handler */
typedef struct IRCLiteralMessageParserStruct {
    char *commandName;
    IRCMessageParseProc proc;
};

/* Struct representing a ctcp command handler */
typedef struct IRCCTCPMessageParserStruct {
    char *commandName;
    IRCMessageParseProc proc;
};

/* Struct representing a numerical command handler */
typedef struct IRCNumericalMessageParserStruct {
    unsigned short commandNumber; 
    IRCMessageParseProc proc;
};

class IRCMessageParser : public QObject {
    Q_OBJECT
public:
    /* Create an IRCMessageParser object */
    IRCMessageParser(IRCSession *session);
    /* Parse a server message and take the appropriate actions */
    void parse(IRCMessage *message);
signals:
    /* Used to send commands to the UI (such as displaying text etc) */
    void outputReady(IRCOutput output);
private:
    /* Parser functions */
    void nullFunc(IRCMessage *message);
    void parseLiteralPing(IRCMessage *message);
    void parseLiteralNotice(IRCMessage *message);
    void parseLiteralJoin(IRCMessage *message);
    void parseLiteralPrivMsg(IRCMessage *message);
    void parseLiteralNick(IRCMessage *message);
    void parseLiteralPart(IRCMessage *message);
    void parseLiteralQuit(IRCMessage *message);
    void parseLiteralError(IRCMessage *message);
    void parseLiteralMode(IRCMessage *message);
    void parseLiteralKick(IRCMessage *message);
    void parseNumerical001(IRCMessage *message);
    void parseNumerical002(IRCMessage *message);
    void parseNumerical003(IRCMessage *message);
    void parseNumerical004(IRCMessage *message);
    void parseNumerical005(IRCMessage *message);
    void parseNumericalStats(IRCMessage *message);
    void parseNumericalNames(IRCMessage *message);
    void parseNumericalEndOfNames(IRCMessage *message);
    void parseNumericalNicknameInUse(IRCMessage *message);
    void parseCTCPPing(IRCMessage *message);
    void parseCTCPVersion(IRCMessage *message);
    void parseCTCPAction(IRCMessage *message);
protected:
    IRCSession *m_session;
    /* Parser tables */
    static IRCLiteralMessageParserStruct literalParserProcTable[];
    static IRCNumericalMessageParserStruct numericalParserProcTable[];
    static IRCCTCPMessageParserStruct ctcpParserProcTable[];
};

#endif /* __IRCMESSAGEPARSER_H */
