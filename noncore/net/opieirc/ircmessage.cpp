#include <qtextstream.h>
#include "ircmessage.h"

/*
 * Create a new IRCMessage by evaluating
 * a received string 
 */

IRCMessage::IRCMessage(QString line) {
    /* Remove CRs from the message */
    while((line.right(1) == "\n") || (line.right(1) == "\r"))
        line = line.left(line.length() - 1);
    QTextIStream stream(&line);
    QString temp;
    
    stream >> temp;
    if (temp.startsWith(":")) {
        /* extract the prefix */
        m_prefix = temp.right(temp.length()-1);
        stream >> temp;
        m_command = temp.upper();
        m_allParameters = line.right(line.length() - m_prefix.length() - m_command.length() - 3);
    } else {
        m_command = temp.upper();
        m_allParameters = line.right(line.length() - m_command.length() - 1);
    }
    
    /* Create a list of all parameters */
    while(!(stream.atEnd())) {
        stream >> temp;
        if (temp.startsWith(":")) {
            /* last parameter */
            m_trailing = line.right(line.length() - line.find(QChar(':'), 1) - 1);
            m_parameters << m_trailing;
            break;
        } else {
            m_parameters << temp;
        }
    }

    
    m_commandNumber = m_command.toInt(&m_isNumerical);
    /* Is this a CTCP command */
    if ((m_command == "PRIVMSG" || m_command == "NOTICE") && m_trailing.length()>0 && m_trailing.left(1) == QChar(1)) {
        m_ctcp = TRUE;
        /* Strip CTCP \001 characters */
        m_allParameters = m_allParameters.replace(QRegExp(QChar(1)), "");
        QTextIStream ctcpStream(&m_allParameters);
        if (m_command == "PRIVMSG")
            ctcpStream >> m_ctcpDestination;
        ctcpStream >> temp;
        m_ctcpCommand = temp.upper().right(temp.length()-1);
        m_parameters.clear();
        int length = m_allParameters.length() - m_ctcpCommand.length() - 1;
        if (m_command == "PRIVMSG")
            length -= m_ctcpDestination.length() + 1;
        if (length <= 0) {
            m_allParameters = "";
        } else {
            m_allParameters = m_allParameters.right(length);
            m_parameters << m_allParameters;
        }
    } else {
        m_ctcp = FALSE;
    }

    /*
       -- Uncomment to debug --

    printf("Parsed : '%s'\n", line.ascii());
    printf("Prefix : '%s'\n", m_prefix.ascii());
    printf("Command : '%s'\n", m_command.ascii());
    printf("Allparameters : '%s'\n", m_allParameters.ascii());
    for (unsigned int i=0; i<m_parameters.count(); i++) {
        printf("Parameter %i : '%s'\n", i, m_parameters[i].ascii());
    }
    printf("CTCP Command : '%s'\n", m_ctcpCommand.latin1());
    printf("CTCP Destination : '%s'\n", m_ctcpDestination.latin1());
    printf("CTCP param  count is : '%i'\n", m_parameters.count());
    
    */
}

QString IRCMessage::param(int param) {
    return m_parameters[param];
}

QString IRCMessage::prefix() {
    return m_prefix;
}

QString IRCMessage::command() {
    return m_command;
}

QString IRCMessage::ctcpCommand() {
    return m_ctcpCommand;
}

QString IRCMessage::ctcpDestination() {
    return m_ctcpDestination;
}

unsigned short IRCMessage::commandNumber() {
    return m_commandNumber;
}

bool IRCMessage::isNumerical() {
    return m_isNumerical;
}

bool IRCMessage::isCTCP() {
    return m_ctcp;
}

QString IRCMessage::trailing() {
    return m_trailing;
}

QString IRCMessage::allParameters() {
    return m_allParameters;
}

