#include <qtextstream.h>
#include "ircmessageparser.h"
#include "ircversion.h"
#include <stdio.h>

/* Lookup table for literal commands */
IRCLiteralMessageParserStruct IRCMessageParser::literalParserProcTable[] = { 
    { "PING",    FUNC(parseLiteralPing) },
    { "NOTICE",  FUNC(parseLiteralNotice) },
    { "JOIN",    FUNC(parseLiteralJoin) },
    { "PRIVMSG", FUNC(parseLiteralPrivMsg) },
    { "NICK",    FUNC(parseLiteralNick) },
    { "PART",    FUNC(parseLiteralPart) },
    { "QUIT",    FUNC(parseLiteralQuit) },
    { "ERROR",   FUNC(parseLiteralError) },
    { "ERROR:",  FUNC(parseLiteralError) },
    { "MODE",    FUNC(parseLiteralMode) },
    { "KICK",    FUNC(parseLiteralKick) },
    { 0 , 0 }
};

/* Lookup table for literal commands */
IRCCTCPMessageParserStruct IRCMessageParser::ctcpParserProcTable[] = { 
    { "PING",    FUNC(parseCTCPPing) },
    { "VERSION", FUNC(parseCTCPVersion) },
    { "ACTION",  FUNC(parseCTCPAction) },
    { 0 , 0 }
};
/* Lookup table for numerical commands */
IRCNumericalMessageParserStruct IRCMessageParser::numericalParserProcTable[] = { 
    { 1,   FUNC(parseNumerical001) },           // RPL_WELCOME
    { 2,   FUNC(parseNumerical002) },           // RPL_YOURHOST
    { 3,   FUNC(parseNumerical003) },           // RPL_CREATED
    { 4,   FUNC(parseNumerical004) },           // RPL_MYINFO
    { 5,   FUNC(parseNumerical005) },           // RPL_BOUNCE, RPL_PROTOCTL
    { 251, FUNC(parseNumericalStats) },         // RPL_LUSERCLIENT
    { 254, FUNC(nullFunc)},                     // RPL_LUSERCHANNELS
    { 255, FUNC(parseNumericalStats) },         // RPL_LUSERNAME
    { 353, FUNC(parseNumericalNames) },         // RPL_NAMREPLY
    { 366, FUNC(parseNumericalEndOfNames) },    // RPL_ENDOFNAMES
    { 375, FUNC(parseNumericalStats) },         // RPL_MOTDSTART
    { 372, FUNC(parseNumericalStats) },         // RPL_MOTD
    { 376, FUNC(parseNumericalStats) },         // RPL_ENDOFMOTD
    { 377, FUNC(parseNumericalStats) },         // RPL_MOTD2
    { 378, FUNC(parseNumericalStats) },         // RPL_MOTD3
    { 412, FUNC(parseNumericalStats) },         // ERNOTEXTTOSEND
    { 433, FUNC(parseNumericalNicknameInUse) }, // ERR_NICKNAMEINUSE
    { 0,   0 }
};

IRCMessageParser::IRCMessageParser(IRCSession *session) {
    m_session = session;
}

void IRCMessageParser::parse(IRCMessage *message) {
    /* Find out what kind of message we have here and call the appropriate handler using
       the parser tables. If no handler can be found, print out an error message */
    if (message->isNumerical()) {
        for (int i=0; i<numericalParserProcTable[i].commandNumber; i++) {
            if (message->commandNumber() == numericalParserProcTable[i].commandNumber) {
                (this->*(numericalParserProcTable[i].proc))(message);
                return;
            }
        }
        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Received unhandled numeric command : ")+QString::number(message->commandNumber())));
    } else if (message->isCTCP()) {
        for (int i=0; ctcpParserProcTable[i].commandName; i++) {
            if (message->ctcpCommand() == ctcpParserProcTable[i].commandName) {
                (this->*(ctcpParserProcTable[i].proc))(message);
                return;
            }
        }
        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Received unhandled ctcp command : ")+message->ctcpCommand()));
    } else {
        for (int i=0; literalParserProcTable[i].commandName; i++) {
            if (message->command() == literalParserProcTable[i].commandName) {
                (this->*(literalParserProcTable[i].proc))(message);
                return;
            }
        }
        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Received unhandled literal command : ")+message->command()));
    }
}

void IRCMessageParser::nullFunc(IRCMessage *) {
    /* Do nothing */
}

void IRCMessageParser::parseLiteralPing(IRCMessage *message) {
    m_session->m_connection->sendLine("PONG " + message->allParameters());
}

void IRCMessageParser::parseLiteralNotice(IRCMessage *message) {
    emit outputReady(IRCOutput(OUTPUT_SERVERMESSAGE, message->allParameters()));
}

void IRCMessageParser::parseLiteralJoin(IRCMessage *message) {
    QString channelName = message->param(0);
    IRCPerson mask(message->prefix());
    IRCChannel *channel = m_session->getChannel(channelName);
    if (!channel) {
        /* We joined */
        if (mask.nick() == m_session->m_server->nick()) {
            channel = new IRCChannel(channelName);
            m_session->addChannel(channel);
        } else {
            emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Nonexistant channel join - desynchronized?")));
        }
    } else {
        /* Someone else joined */
        if (mask.nick() != m_session->m_server->nick()) {
            if (!channel->getPerson(mask.nick())) {
                IRCChannelPerson *chanperson = new IRCChannelPerson();
                IRCPerson *person = m_session->getPerson(mask.nick());
                if (!person) {
                    person = new IRCPerson(message->prefix());
                    m_session->addPerson(person);
                }
                chanperson->flags = 0;
                chanperson->person = person;
                channel->addPerson(chanperson);
                IRCOutput output(OUTPUT_OTHERJOIN, mask.nick() + tr(" joined channel ") + channelName);
                output.addParam(channel);
                output.addParam(chanperson);
                emit outputReady(output);
            } else {
               emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Person has already joined the channel - desynchronized?")));
            }
        } else {
           emit outputReady(IRCOutput(OUTPUT_ERROR, tr("You already joined the channel - desynchronized?")));
        }
    }
}

void IRCMessageParser::parseLiteralPart(IRCMessage *message) {
    QString channelName = message->param(0);
    IRCChannel *channel = m_session->getChannel(channelName);
    IRCPerson mask(message->prefix());
    if (channel) {
        if (mask.nick() == m_session->m_server->nick()) {
            m_session->removeChannel(channel);
            IRCOutput output(OUTPUT_SELFPART, tr("You left channel ") + channelName);
            output.addParam(channel);
            emit outputReady(output);
            delete channel;
        } else {
            IRCChannelPerson *person = channel->getPerson(mask.nick());
            if (person) {
                channel->removePerson(person);
                IRCOutput output(OUTPUT_OTHERPART, mask.nick() + tr(" left channel ") + channelName);
                output.addParam(channel);
                output.addParam(person);
                emit outputReady(output);
                delete person;
            } else {
                emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Parting person not found - desynchronized?")));
            }
        }
    } else {
        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Channel for part not found - desynchronized?")));
    }
}

void IRCMessageParser::parseLiteralPrivMsg(IRCMessage *message) {
    if (m_session->m_server->nick() == message->param(0)) {
        /* IRC Query message detected, verify sender and display it */
        IRCPerson mask(message->prefix());
        IRCPerson *person = m_session->getPerson(mask.nick());
        if (!person) {
            /* Person not yet known, create and add to the current session */
            person = new IRCPerson(message->prefix());
            m_session->addPerson(person);
        }
        IRCOutput output(OUTPUT_CHANPRIVMSG, message->param(1));
        output.addParam(person);
        emit outputReady(output);
    } else if (message->param(0).at(0) == '#') {
        /* IRC Channel message detected, verify sender, channel and display it */
        IRCChannel *channel = m_session->getChannel(message->param(0));
        if (channel) {
            IRCPerson mask(message->prefix());
            IRCChannelPerson *person = channel->getPerson(mask.nick());
            if (person) {
                IRCOutput output(OUTPUT_CHANPRIVMSG, message->param(1));
                output.addParam(channel);
                output.addParam(person);
                emit outputReady(output);
            } else {
                emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Channel message with unknown sender")));
            }
        } else {
            emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Channel message with unknown channel")));
        }
    } else {
        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Received PRIVMSG of unknown type")));
    }
}

void IRCMessageParser::parseLiteralNick(IRCMessage *message) {
    IRCPerson mask(message->prefix());

    if (mask.nick() == m_session->m_server->nick()) {
        /* We are changing our nickname */
        m_session->m_server->setNick(message->param(0));
        IRCOutput output(OUTPUT_NICKCHANGE, tr("You are now known as ")+message->param(0));
        output.addParam(0);
        emit outputReady(output);
    } else {
        /* Someone else is */
        IRCPerson *person = m_session->getPerson(mask.nick());
        if (person) {
            IRCOutput output(OUTPUT_NICKCHANGE, mask.nick() + tr(" is now known as ") + message->param(0));
            output.addParam(person);
            emit outputReady(output);
        } else {
            emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Nickname change of an unknown person")));
        }
    }
}

void IRCMessageParser::parseLiteralQuit(IRCMessage *message) {
    IRCPerson mask(message->prefix());
    IRCPerson *person = m_session->getPerson(mask.nick());
    if (person) {
        QList<IRCChannel> channels;
        m_session->getChannelsByPerson(person, channels);
        QListIterator<IRCChannel> it(channels);
        for (;it.current(); ++it) { 
            IRCChannelPerson *chanperson = it.current()->getPerson(mask.nick());
            it.current()->removePerson(chanperson);
        }
        m_session->removePerson(person);
        IRCOutput output(OUTPUT_QUIT, mask.nick() + tr(" has quit ") + "(" + message->param(0) + ")");
        output.addParam(person);
        emit outputReady(output);
    } else {
         emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Unknown person quit - desynchronized?")));
    }
}

void IRCMessageParser::parseLiteralError(IRCMessage *message) {
    emit outputReady(IRCOutput(OUTPUT_ERROR, message->allParameters()));
}

void IRCMessageParser::parseCTCPPing(IRCMessage *message) {
    IRCPerson mask(message->prefix());
    m_session->m_connection->sendCTCP(mask.nick(), "PING " + message->allParameters());
    emit outputReady(IRCOutput(OUTPUT_CTCP, tr("Received a CTCP PING from ")+mask.nick()));
}

void IRCMessageParser::parseCTCPVersion(IRCMessage *message) {
    IRCPerson mask(message->prefix());
    m_session->m_connection->sendCTCP(mask.nick(), APP_VERSION " "  APP_COPYSTR);
    emit outputReady(IRCOutput(OUTPUT_CTCP, tr("Received a CTCP VERSION from ")+mask.nick()));
}

void IRCMessageParser::parseCTCPAction(IRCMessage *message) {
    IRCPerson mask(message->prefix());
    QString dest = message->ctcpDestination();
    if (dest.startsWith("#")) {
        IRCChannel *channel = m_session->getChannel(dest);
        if (channel) {
            IRCChannelPerson *person = channel->getPerson(mask.nick());
            if (person) {
                IRCOutput output(OUTPUT_CHANACTION, "*" + mask.nick() + message->param(0));
                output.addParam(channel);
                output.addParam(person);
                emit outputReady(output);
            } else {
                emit outputReady(IRCOutput(OUTPUT_ERROR, tr("CTCP ACTION with unknown person - Desynchronized?")));
            }
        } else {
            emit outputReady(IRCOutput(OUTPUT_ERROR, tr("CTCP ACTION with unknown channel - Desynchronized?")));
        }
    } else {
        if (message->ctcpDestination() == m_session->m_server->nick()) {
            IRCPerson *person = m_session->getPerson(mask.nick());
            if (!person) {
                /* Person not yet known, create and add to the current session */
                person = new IRCPerson(message->prefix());
                m_session->addPerson(person);
            }
            IRCOutput output(OUTPUT_QUERYACTION, "*" + mask.nick() + message->param(0));
            output.addParam(person);
            emit outputReady(output);
        } else {
            emit outputReady(IRCOutput(OUTPUT_ERROR, tr("CTCP ACTION with bad recipient")));
        }
    }
}
    
void IRCMessageParser::parseLiteralMode(IRCMessage *message) {
    IRCPerson mask(message->prefix());

    if (message->param(0).startsWith("#")) {
        IRCChannel *channel = m_session->getChannel(message->param(0));
        if (channel) {
            QString temp, parameters = message->allParameters().right(message->allParameters().length() - channel->channelname().length() - 1);
            QTextIStream stream(&parameters);
            bool set = FALSE;
            while (!stream.atEnd()) {
                stream >> temp;
                if (temp.startsWith("+")) {
                    set = TRUE;
                    temp = temp.right(1);
                } else if (temp.startsWith("-")) {
                    set = FALSE;
                    temp = temp.right(1);
                } else {
                    emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Mode change has unknown type")));
                    return;
                }
                if (temp == "o") {
                    stream >> temp;
                    IRCChannelPerson *person = channel->getPerson(temp);
                    if (person) {
                        if (set) {
                            person->flags |= PERSON_FLAG_OP;
                            IRCOutput output(OUTPUT_CHANPERSONMODE, mask.nick() + tr(" gives channel operator status to " + person->person->nick()));
                            output.addParam(channel);
                            output.addParam(person);
                            emit outputReady(output);
                        } else {
                            person->flags &= 0xFFFF - PERSON_FLAG_OP;
                            IRCOutput output(OUTPUT_CHANPERSONMODE, mask.nick() + tr(" removes channel operator status from " + person->person->nick()));
                            output.addParam(channel);
                            output.addParam(person);
                            emit outputReady(output);
                        }
                    } else {
                        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Mode change with unknown person - Desynchronized?")));
                    }
                } else if (temp == "v") {
                    stream >> temp;
                    IRCChannelPerson *person = channel->getPerson(temp);
                    if (person) {
                        if (set) {
                            person->flags |= PERSON_FLAG_VOICE;
                            IRCOutput output(OUTPUT_CHANPERSONMODE, mask.nick() + tr(" gives voice to " + person->person->nick()));
                            output.addParam(channel);
                            output.addParam(person);
                            emit outputReady(output);
                        } else {
                            person->flags &= 0xFFFF - PERSON_FLAG_VOICE;
                            IRCOutput output(OUTPUT_CHANPERSONMODE, mask.nick() + tr(" removes voice from " + person->person->nick()));
                            output.addParam(channel);
                            output.addParam(person);
                            emit outputReady(output);
                        }
                    } else {
                        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Mode change with unknown person - Desynchronized?")));
                    }
                } else {
                    emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Mode change with unknown flag")));
                }
            }
        } else {
            emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Mode change with unknown kannel - Desynchronized?")));
        }
    } else {
        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("User modes not supported yet")));
    }
}

void IRCMessageParser::parseLiteralKick(IRCMessage *message) {
    IRCPerson mask(message->prefix());
    IRCChannel *channel = m_session->getChannel(message->param(0));
    if (channel) {
        IRCChannelPerson *person = channel->getPerson(message->param(1));
        if (person) {
            if (person->person->nick() == m_session->m_server->nick()) {
                m_session->removeChannel(channel);
                IRCOutput output(OUTPUT_SELFKICK, tr("You were kicked from ") + channel->channelname() + tr(" by ") + mask.nick() + " (" + message->param(2) + ")");
                output.addParam(channel);
                emit outputReady(output);
            } else {
                channel->removePerson(person);
                IRCOutput output(OUTPUT_OTHERKICK, person->person->nick() + tr(" was kicked from ") + channel->channelname() + tr(" by ") + mask.nick()+ " (" + message->param(2) + ")");
                output.addParam(channel);
                output.addParam(person);
                emit outputReady(output);
            }
        } else {
            emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Unknown person kick - desynchronized?")));
        }
    } else {
         emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Unknown channel kick - desynchronized?")));
    }
}

void IRCMessageParser::parseNumerical001(IRCMessage *message) {
    /* Welcome to IRC message, display */
    emit outputReady(IRCOutput(OUTPUT_SERVERMESSAGE, message->param(1)));
}

void IRCMessageParser::parseNumerical002(IRCMessage *message) {
    emit outputReady(IRCOutput(OUTPUT_SERVERMESSAGE, message->param(1)));
}

void IRCMessageParser::parseNumerical003(IRCMessage *message) {
    emit outputReady(IRCOutput(OUTPUT_SERVERMESSAGE, message->param(1)));
}

void IRCMessageParser::parseNumerical004(IRCMessage *message) {
    emit outputReady(IRCOutput(OUTPUT_SERVERMESSAGE, message->allParameters()));
}

void IRCMessageParser::parseNumerical005(IRCMessage *message) {
    emit outputReady(IRCOutput(OUTPUT_SERVERMESSAGE, message->allParameters()));
}

void IRCMessageParser::parseNumericalStats(IRCMessage *message) {
    emit outputReady(IRCOutput(OUTPUT_SERVERMESSAGE, message->param(1)));
}

void IRCMessageParser::parseNumericalNames(IRCMessage *message) {
    /* Name list sent when joining a channel */
    IRCChannel *channel = m_session->getChannel(message->param(2));
    if (channel != 0) {
        QString people = message->param(3);
        QTextIStream stream(&people);
        QString temp;

        while (!stream.atEnd()) {
            stream >> temp;
            
            char flagch = temp.at(0).latin1();
            int flag = 0;
            QString nick;
            /* Parse person flags */
            if (flagch == '@' || flagch == '+' || flagch=='%' || flagch == '*') {
                
                nick = temp.right(temp.length()-1);
                switch (flagch) {
                    case '@': flag = PERSON_FLAG_OP;     break;
                    case '+': flag = PERSON_FLAG_VOICE;  break;
                    case '%': flag = PERSON_FLAG_HALFOP; break;
                    default : flag = 0; break;
                }
            } else {
                nick = temp;
            }
            
            IRCChannelPerson *chan_person = new IRCChannelPerson();
            IRCPerson *person = m_session->getPerson(nick);
            if (person == 0) {
                person = new IRCPerson();
                person->setNick(nick);
                m_session->addPerson(person);
            }
            chan_person->person = person;
            chan_person->flags = flag;
            channel->addPerson(chan_person);
        }
    } else {
        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Server message with unknown channel")));
    }
}

void IRCMessageParser::parseNumericalEndOfNames(IRCMessage *message) {
    /* Done syncing to channel */
    IRCChannel *channel = m_session->getChannel(message->param(1));
    if (channel) {
        channel->setHasPeople(TRUE);
        /* Yes, we want the names before anything happens inside the GUI */
        IRCOutput output(OUTPUT_SELFJOIN, tr("You joined channel ") + channel->channelname());
        output.addParam(channel);
        emit outputReady(output);
    } else {
        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Server message with unknown channel")));
    }
}


void IRCMessageParser::parseNumericalNicknameInUse(IRCMessage *) {
    emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Nickname is in use, please reconnect with a different nickname")));
    m_session->endSession();
}
