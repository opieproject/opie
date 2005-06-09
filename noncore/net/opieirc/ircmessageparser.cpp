#include <qtextstream.h>
#include <qdatetime.h>

#include <opie2/ofiledialog.h>
#include <opie2/ofileselector.h>
#include <opie2/odebug.h>

#include "ircmessageparser.h"
#include "ircversion.h"
#include "ircchannelperson.h"
#include "dcctransfertab.h"
#include "ircservertab.h"

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
    { "TOPIC",   FUNC(parseLiteralTopic) },
    { 0 , 0 }
};

/* Lookup table for literal commands */
IRCCTCPMessageParserStruct IRCMessageParser::ctcpParserProcTable[] = {
    { "PING",    FUNC(parseCTCPPing) },
    { "VERSION", FUNC(parseCTCPVersion) },
    { "ACTION",  FUNC(parseCTCPAction) },
    { "DCC",     FUNC(parseCTCPDCC) },
    { 0 , 0 }
};

/* Lookup table for numerical commands
 * According to:
 * http://www.faqs.org/rfcs/rfc1459.html
 * http://www.faqs.org/rfcs/rfc2812.html
*/

IRCNumericalMessageParserStruct IRCMessageParser::numericalParserProcTable[] = {
    { 1, "%1", "1", FUNC(parseNumericalServerName) },       // RPL_WELCOME
    { 2, "%1", "1", 0 },                                    // RPL_YOURHOST
    { 3, "%1", "1", 0 },                                    // RPL_CREATED
    { 4, QT_TR_NOOP("Server %1 version %2 supports usermodes '%3' and channelmodes '%4'"), "1:4", FUNC(parseNumericalServerFeatures) },   // RPL_MYINFO
    { 5,  0, 0,  FUNC(parseNumericalServerProtocol) },      // RPL_BOUNCE, RPL_PROTOCTL
    { 250, "%1", "1", 0 },                                  // RPL_STATSCONN
    { 251, "%1", "1",  0 },                                 // RPL_LUSERCLIENT
    { 252, QT_TR_NOOP("There are %1 operators connected"), "1", 0 },    // RPL_LUSEROP
    { 253, QT_TR_NOOP("There are %1 unknown connection(s)"), "1", 0 },  // RPL_LUSERUNKNOWN
    { 254, QT_TR_NOOP("There are %1 channels formed"), "1", 0 },        // RPL_LUSERCHANNELS
    { 255, "%1", "1", 0 },                                  // RPL_LUSERME
    { 263, QT_TR_NOOP("Please wait a while and try again"), 0, 0 }, // RPL_TRYAGAIN
    { 265, "%1", "1", 0 },                                  // RPL_LOCALUSERS
    { 266, "%1", "1", 0 },                                  // RPL_GLOBALUSERS
    { 311, QT_TR_NOOP("Whois %1 (%2@%3)\nReal name: %4"), "1:3,5", 0 },    // RPL_WHOISUSER
    { 312, QT_TR_NOOP("%1 is using server %2"), "1,2", 0 },             // RPL_WHOISSERVER
    { 317, 0, 0, FUNC(parseNumericalWhoisIdle) },           // RPL_WHOISIDLE 
    { 318, "%1 :%2", "1,2", 0 },                            // RPL_ENDOFWHOIS
    { 319, QT_TR_NOOP("%1 is on channels: %2"), "1,2", 0 },             // RPL_WHOISCHANNELS
    { 320, "%1 %2", "1,2", 0},                              // RPL_WHOISVIRT
    { 332, 0, 0, FUNC(parseNumericalTopic) },               // RPL_TOPIC
    { 333, 0, 0, FUNC(parseNumericalTopicWhoTime) },        // RPL_TOPICWHOTIME*/
    { 353, QT_TR_NOOP("Names for %1: %2"), "2,3", FUNC(parseNumericalNames) },  // RPL_NAMREPLY
    { 366, "%1 :%2", "1,2", FUNC(parseNumericalEndOfNames) },       // RPL_ENDOFNAMES
    { 369, "%1 :%2", "1,2", 0 },                            // RPL_ENDOFWHOWAS
    { 372, "%1", "1", 0 },                                  // RPL_MOTD
    { 375, "%1", "1", 0 },                                  // RPL_MOTDSTART
    { 376, "%1", "1", 0 },                                  // RPL_ENDOFMOTD
    { 377, "%1", "1", 0 },                                  // RPL_MOTD2
    { 378, "%1", "1", 0 },                                  // RPL_MOTD3
    { 391, QT_TR_NOOP("Time on server %1 is %2"), "1,2", 0 },           // RPL_TIME
    { 401, QT_TR_NOOP("Channel or nick %1 doesn't exists"), "1", 0 },   // ERR_NOSUCHNICK
    { 403, QT_TR_NOOP("Channel %1 doesn't exists"), "1", 0 },           // ERR_ERR_NOSUCHCHANNEL
    { 406, QT_TR_NOOP("There is no history information for %1"), "1", 0 }, // ERR_WASNOSUCHNICK
    { 409, "%1", "1", 0 },                                  // ERR_NOORIGIN
    { 411, "%1", "1", 0 },                                  // ERR_NORECIPIENT
    { 412, "%1", "1", 0 },                                  // ERR_NOTEXTTOSEND
    { 421, QT_TR_NOOP("Unknown command: %1"), "1", 0 },     // ERR_ERR_UNKNOWNCOMMAND
    { 422, "%1", "1", 0 },                                  // ERR_NOMOTD
    { 433, QT_TR_NOOP("Can't change nick to %1: %2"), "1,2", FUNC(parseNumericalNicknameInUse) }, // ERR_NICKNAMEINUSE
    { 442, QT_TR_NOOP("You're not on channel %1"), "1", 0}, // ERR_NOTONCHANNEL
    { 477, "%1", "1", 0 },                                  // ERR_NOCHANMODES || ERR_NEEDREGGEDNICK
    { 482, QT_TR_NOOP("[%1] Operation not permitted, you don't have enough channel privileges"), "1", 0 }, //ERR_CHANOPRIVSNEEDED
    { 0, 0, 0, 0 }
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
                parseNumerical(message, i);
                return;
            }
        }
        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Received unhandled numeric command: %1").arg( QString::number(message->commandNumber()) )));
    } else if (message->isCTCP()) {
        for (int i=0; ctcpParserProcTable[i].commandName; i++) {
            if (message->ctcpCommand() == ctcpParserProcTable[i].commandName) {
                parseCTCP(message, i);
                return;
            }
        }
        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Received unhandled ctcp command: %1").arg( message->ctcpCommand())) );
    } else {
        for (int i=0; literalParserProcTable[i].commandName; i++) {
            if (message->command() == literalParserProcTable[i].commandName) {
                (this->*(literalParserProcTable[i].proc))(message);
                return;
            }
        }
        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Received unhandled literal command: %1").arg( message->command()) ));
    }
}

void IRCMessageParser::parseNumerical(IRCMessage *message, int position) {
    QString out = tr(numericalParserProcTable[position].message);
    QString paramString = numericalParserProcTable[position].params;
    
    if(!out.isEmpty() && !paramString.isEmpty()) {
        QStringList params = message->params(numericalParserProcTable[position].params);
    
        QStringList::Iterator end = params.end();
        for (QStringList::Iterator it = params.begin(); it != end; ++it) {
            out = out.arg(*it);
        }
    
        emit outputReady(IRCOutput(OUTPUT_SERVERMESSAGE, out));
    }
    
    if(numericalParserProcTable[position].proc)
        (this->*(numericalParserProcTable[position].proc))(message);
}

void IRCMessageParser::parseCTCP(IRCMessage *message, int position) {
    if(ctcpParserProcTable[position].proc)
        (this->*(ctcpParserProcTable[position].proc))(message);
}

	

void IRCMessageParser::parseNumericalServerName(IRCMessage *message) {
    emit outputReady(IRCOutput(OUTPUT_TITLE, tr("Connected to")+" <b>" + message->prefix() + "</b>"));
    /* Register EFFECTIVE nickname, some networks (as irc-hispano) use nick:password
     * for authentication and the parser gets confused */
    m_session->m_server->setNick(message->param(0));

}

void IRCMessageParser::parseNumericalServerFeatures(IRCMessage *message) {
    m_session->setValidUsermodes(message->param(2));
    m_session->setValidChannelmodes(message->param(3));

}

void IRCMessageParser::parseNumericalServerProtocol(IRCMessage *message) {
    /* XXX: Add some usefull features here */
    QString out = message->allParameters();
    out = out.mid(out.find(' ')+1);
    emit outputReady(IRCOutput(OUTPUT_SERVERMESSAGE, out));
}
void IRCMessageParser::parseNumericalWhoisIdle(IRCMessage *message) {
    QDateTime dt;
    QTime t;
    t = t.addSecs(message->param(2).toInt());
    dt.setTime_t(message->param(3).toInt());
    
    emit outputReady(IRCOutput(OUTPUT_SERVERMESSAGE, tr("%1 has been idle for %2").arg(message->param(1)).arg(t.toString())));
    emit outputReady(IRCOutput(OUTPUT_SERVERMESSAGE, tr("%1 signed on %2").arg(message->param(1)).arg(dt.toString())));
    
}

void IRCMessageParser::parseLiteralPing(IRCMessage *message) {
    m_session->m_connection->sendLine("PONG " + message->allParameters());
}

void IRCMessageParser::parseLiteralNotice(IRCMessage *message) {
    emit outputReady(IRCOutput(OUTPUT_SERVERMESSAGE, message->allParameters()));
}

void IRCMessageParser::parseLiteralJoin(IRCMessage *message) {
    QString channelName = message->param(0).lower();
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
                IRCPerson *person = m_session->getPerson(mask.nick());
                if (!person) {
                    person = new IRCPerson(message->prefix());
                    m_session->addPerson(person);
                }
                IRCChannelPerson *chanperson = new IRCChannelPerson(person);
                channel->addPerson(chanperson);
                IRCOutput output(OUTPUT_OTHERJOIN ,tr("%1 joined channel %2").arg( mask.nick() ).arg( channelName ));
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
    QString channelName = message->param(0).lower();
    IRCChannel *channel = m_session->getChannel(channelName);
    IRCPerson mask(message->prefix());
    if (channel) {
        if (mask.nick() == m_session->m_server->nick()) {
            m_session->removeChannel(channel);
            IRCOutput output(OUTPUT_SELFPART, tr("You left channel %1").arg( channelName ));
            output.addParam(channel);
            emit outputReady(output);
            delete channel;
        } else {
            IRCChannelPerson *person = channel->getPerson(mask.nick());
            if (person) {
                channel->removePerson(person);
                IRCOutput output(OUTPUT_OTHERPART, tr("%1 left channel %2").arg( mask.nick() ).arg( channelName) );
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
    if (m_session->m_server->nick().lower() == message->param(0).lower() ) {
        /* IRC Query message detected, verify sender and display it */
        IRCPerson mask(message->prefix());
        IRCPerson *person = m_session->getPerson(mask.nick());
        if (!person) {
            /* Person not yet known, create and add to the current session */
            person = new IRCPerson(message->prefix());
            m_session->addPerson(person);
        }
        IRCOutput output(OUTPUT_QUERYPRIVMSG, message->param(1));
        output.addParam(person);
        emit outputReady(output);
    } 
    else 
        if (IRCChannel::isValid(message->param(0))) {
            /* IRC Channel message detected, verify sender, channel and display it */
            IRCChannel *channel = m_session->getChannel(message->param(0).lower());
            if (channel) {
                IRCPerson mask(message->prefix());
                IRCChannelPerson *person = channel->getPerson(mask.nick());
                if (person) {
                    IRCOutput output(OUTPUT_CHANPRIVMSG, message->param(1));
                    output.addParam(channel);
                    output.addParam(person);
                    emit outputReady(output);
                } 
                else {
                    emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Channel message with unknown sender")));
                }
            } 
            else {
                emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Channel message with unknown channel %1").arg(message->param(0).lower()) ));
            }
        } 
        else {emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Received PRIVMSG of unknown type")));
    }
}

void IRCMessageParser::parseLiteralNick(IRCMessage *message) {
    IRCPerson mask(message->prefix());
    m_session->updateNickname(mask.nick(), message->param(0));
    /* this way of handling nick changes really sucks 
    if (mask.nick() == m_session->m_server->nick()) {
         We are changing our nickname 
        m_session->m_server->setNick(message->param(0));
        IRCOutput output(OUTPUT_NICKCHANGE, tr("You are now known as %1").arg( message->param(0)));
        output.addParam(0);
        emit outputReady(output);
    } else {
         Someone else is 
        RCPerson *person = m_session->getPerson(mask.nick());
        if (person) {
            //IRCOutput output(OUTPUT_NICKCHANGE, tr("%1 is now known as %2").arg( mask.nick() ).arg( message->param(0)));

             new code starts here -- this removes the person from all channels 
            QList<IRCChannel> channels;
            m_session->getChannelsByPerson(person, channels);
            QListIterator<IRCChannel> it(channels);
            for (;it.current(); ++it) {
              IRCChannelPerson *chanperson = it.current()->getPerson(mask.nick());
              it.current()->removePerson(chanperson);
              chanperson->person->setNick(message->param(0));
              it.current()->addPerson(chanperson);
              IRCOutput output(OUTPUT_NICKCHANGE, tr("%1 is now known as %2").arg( mask.nick() ).arg( message->param(0)));
              output.addParam(person);
              emit outputReady(output);
            }
             new code ends here 
        } else {
            emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Nickname change of an unknown person")));
        }
    }*/
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
            delete chanperson;
        }
        m_session->removePerson(person);
        IRCOutput output(OUTPUT_QUIT, tr("%1 has quit (%2)" ).arg( mask.nick() ).arg( message->param(0) ));
        output.addParam(person);
        emit outputReady(output);
        delete person;
    } else {
         emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Unknown person quit - desynchronized?")));
    }
}

void IRCMessageParser::parseLiteralTopic(IRCMessage *message) {
    IRCPerson mask(message->prefix());
    IRCChannel *channel = m_session->getChannel(message->param(0).lower());
    if (channel) {
        IRCOutput output(OUTPUT_TOPIC, mask.nick() + tr(" changed topic to ") + "\"" + message->param(1) + "\"");
        output.addParam(channel);
        emit outputReady(output);
    } else {
        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Unknown channel topic - desynchronized?")));
    }
}

void IRCMessageParser::parseLiteralError(IRCMessage *message) {
    emit outputReady(IRCOutput(OUTPUT_ERROR, message->allParameters()));
}

void IRCMessageParser::parseCTCPPing(IRCMessage *message) {
    IRCPerson mask(message->prefix());
    if(message->isCTCPReply()) {
        unsigned int sentTime = message->param(0).toUInt();
        QDateTime tm;
        tm.setTime_t(0);
        unsigned int receivedTime = tm.secsTo(QDateTime::currentDateTime());
        emit outputReady(IRCOutput(OUTPUT_CTCP, tr("Received a CTCP PING reply from %1: %2 seconds").arg(mask.nick()).arg(receivedTime-sentTime)));
        return;
    } 
    m_session->m_connection->sendCTCPReply(mask.nick(), "PING", message->allParameters());
    emit outputReady(IRCOutput(OUTPUT_CTCP, tr("Received a CTCP PING request from %1").arg(mask.nick())));

    //IRCPerson mask(message->prefix());
    QString dest = message->ctcpDestination();
    if (dest.startsWith("#")) {
        IRCChannel *channel = m_session->getChannel(dest.lower());
        if (channel) {
            IRCChannelPerson *person = channel->getPerson(mask.nick());
            if (person) {
                IRCOutput output(OUTPUT_CHANACTION, tr("Received a CTCP PING from ")+ mask.nick()) ;
                output.addParam(channel);
                output.addParam(person);
                emit outputReady(output);
            } else {
                emit outputReady(IRCOutput(OUTPUT_ERROR, tr("CTCP PING with unknown person - Desynchronized?")));
            }
        } else {
            emit outputReady(IRCOutput(OUTPUT_ERROR, tr("CTCP PING with unknown channel - Desynchronized?")));
        }
    } else {
        if (message->ctcpDestination() == m_session->m_server->nick()) {
            IRCPerson *person = m_session->getPerson(mask.nick());
            if (!person) {
                /* Person not yet known, create and add to the current session */
                person = new IRCPerson(message->prefix());
                m_session->addPerson(person);
            }
            IRCOutput output(OUTPUT_QUERYACTION, tr("Received a CTCP PING from ")+ mask.nick() );
            output.addParam(person);
            emit outputReady(output);
        } else {
            emit outputReady(IRCOutput(OUTPUT_ERROR, tr("CTCP PING with bad recipient")));
        }
    }

}

void IRCMessageParser::parseCTCPVersion(IRCMessage *message) {
    IRCPerson mask(message->prefix());
    IRCOutput output(OUTPUT_CTCP);
    if(message->isCTCPRequest()) {
        m_session->m_connection->sendCTCPReply(mask.nick(), "VERSION", APP_VERSION " "  APP_COPYSTR);
        output.setMessage(tr("Received a CTCP VERSION request from ") + mask.nick());
    }

    else { 
        output.setMessage("Received CTCP VERSION reply from " + mask.nick() + ":" + message->param(0));
    }
    emit outputReady(output);
}

void IRCMessageParser::parseCTCPAction(IRCMessage *message) {
    IRCPerson mask(message->prefix());
    QString dest = message->ctcpDestination();
    if (dest.startsWith("#")) {
        IRCChannel *channel = m_session->getChannel(dest.lower());
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

void IRCMessageParser::parseCTCPDCC(IRCMessage *message) {
    QStringList params = QStringList::split(' ', message->param(0).stripWhiteSpace());
    
    if(params[0] == "SEND") {
        QString nickname = IRCPerson(message->prefix()).nick();
        if( params.count() != 5) {
            emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Malformed DCC request from %1").arg(nickname)));
            return;
        }
        bool accepted = DCCTransferTab::confirm(static_cast<QWidget*>(m_session->parent()), nickname, params[1], params[4].toUInt());
        if(!accepted)
            return;
        QString filename = Opie::Ui::OFileDialog::getSaveFileName(Opie::Ui::OFileSelector::EXTENDED_ALL,
                QString::null, params[1], MimeTypes(), 0, tr("Save As"));
        if(filename.isEmpty())
            return;

        odebug << "Receiving file " << filename << " from " << nickname << oendl;
        static_cast<IRCServerTab*>(m_session->parent())->mainwindow()->addDCC(DCCTransfer::Recv, params[2].toUInt(), params[3].toUInt(),
                filename, nickname, params[4].toUInt());
    }
}

void IRCMessageParser::parseLiteralMode(IRCMessage *message) {
    IRCPerson mask(message->prefix());

    if (IRCChannel::isValid(message->param(0))) {
        IRCChannel *channel = m_session->getChannel(message->param(0).lower());
        if (channel) {
            QString temp, parameters = message->allParameters().right(message->allParameters().length() - channel->channelname().length() - 1);
            QTextIStream stream(&parameters);
            bool set = FALSE;
            while (!stream.atEnd()) {
                stream >> temp;
                if (temp.startsWith("+")) {
                    set = TRUE;
                    temp = temp.right(1);
                } 
                else
                    if (temp.startsWith("-")) {
                        set = FALSE;
                        temp = temp.right(1);
                    } 
                    else {
                        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Mode change has unknown type")));
                        return;
                    }
                if (temp == "o") {
                    stream >> temp;
                    IRCChannelPerson *person = channel->getPerson(temp);
                    if (person) {
                        IRCOutput output(OUTPUT_CHANPERSONMODE, person->setOp(mask.nick(), set));
                        output.addParam(channel);
                        output.addParam(person);
                        emit outputReady(output);
                    } 
                    else {
                        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Mode change with unknown person - Desynchronized?")));
                    }
                } 
                else 
                    if (temp == "v") {
                        stream >> temp;
                        IRCChannelPerson *person = channel->getPerson(temp);
                        if (person) {
                            IRCOutput output(OUTPUT_CHANPERSONMODE, person->setVoice(mask.nick(), set));
                            output.addParam(channel);
                            output.addParam(person);
                            emit outputReady(output);
                        }
                        else {
                            emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Mode change with unknown person - Desynchronized?")));
                        }
                    } 
                    else {
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
    IRCChannel *channel = m_session->getChannel(message->param(0).lower());
    if (channel) {
        IRCChannelPerson *person = channel->getPerson(message->param(1));
        if (person) {
            if (person->nick() == m_session->m_server->nick()) {
                m_session->removeChannel(channel);
                IRCOutput output(OUTPUT_SELFKICK, tr("You were kicked from ") + channel->channelname() + tr(" by ") + mask.nick() + " (" + message->param(2) + ")");
                output.addParam(channel);
                emit outputReady(output);
            } else {
              /* someone else got kicked */
                channel->removePerson(person);
                IRCOutput output(OUTPUT_OTHERKICK, person->nick() + tr(" was kicked from ") + channel->channelname() + tr(" by ") + mask.nick()+ " (" + message->param(2) + ")");
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

void IRCMessageParser::parseNumericalNames(IRCMessage *message) {
    /* Name list sent when joining a channel */
    IRCChannel *channel = m_session->getChannel(message->param(2).lower());
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
                    case '@': flag = IRCChannelPerson::PERSON_FLAG_OP;     break;
                    case '+': flag = IRCChannelPerson::PERSON_FLAG_VOICE;  break;
                    case '%': flag = IRCChannelPerson::PERSON_FLAG_HALFOP; break;
                    default : flag = 0; break;
                }
            } else {
                nick = temp;
            }

            IRCPerson *person = m_session->getPerson(nick);
            if (person == 0) {
                person = new IRCPerson();
                person->setNick(nick);
                m_session->addPerson(person);
            }
            IRCChannelPerson *chan_person = new IRCChannelPerson(person);
            chan_person->setFlags(flag);
            channel->addPerson(chan_person);
        }
    } else {
        emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Server message with unknown channel")));
    }
}

void IRCMessageParser::parseNumericalEndOfNames(IRCMessage *message) {
    /* Done syncing to channel */
    IRCChannel *channel = m_session->getChannel(message->param(1).lower());
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
    /* If we are connnected this error is not critical */
    if(m_session->isLoggedIn())
        return;
    
    emit outputReady(IRCOutput(OUTPUT_ERROR, tr("Nickname is in use, please reconnect with a different nickname")));
    m_session->endSession();
}

void IRCMessageParser::parseNumericalNoSuchNick(IRCMessage *) {
    emit outputReady(IRCOutput(OUTPUT_ERROR, tr("No such nickname")));
}

void IRCMessageParser::parseNumericalTopic(IRCMessage *message) {
    IRCChannel *channel = m_session->getChannel(message->param(1).lower());
    if (channel) {
        IRCOutput output(OUTPUT_TOPIC, tr("Topic for channel " + channel->channelname() + " is \"" + message->param(2) + "\""));
        output.addParam(channel);
        emit outputReady(output);
    } else {
        IRCOutput output(OUTPUT_TOPIC, tr("Topic for channel " + message->param(1) + " is \"" + message->param(2) + "\""));
        output.addParam(0);
        emit outputReady(output);
    }
}

void IRCMessageParser::parseNumericalTopicWhoTime(IRCMessage *) {
}

    
