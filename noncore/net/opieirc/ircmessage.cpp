#include <opie2/odebug.h>

using namespace Opie::Core;


#include <qtextstream.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qregexp.h>

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
        if (m_command == "PRIVMSG") {
	    m_ctcpRequest = TRUE;
        }
        else {
            m_ctcpRequest = FALSE;
        }
        
        /* Strip CTCP \001 characters */
        m_allParameters = m_allParameters.replace(QRegExp(QChar(1)), "");
        QTextIStream ctcpStream(&m_allParameters);
        ctcpStream >> m_ctcpDestination;
        ctcpStream >> temp;
        m_ctcpCommand = temp.upper().right(temp.length()-1);
        m_parameters.clear();
        int length = m_allParameters.length() - m_ctcpCommand.length() - 1;
        length -= m_ctcpDestination.length() + 1;
        if (length <= 0) {
            m_allParameters = "";
        } 
        else {
            m_allParameters = m_allParameters.right(length);
            m_parameters << m_allParameters;
        }
    } 
    else {
        m_ctcp = FALSE;
    }

    
    odebug << "Parsed: " << line << oendl;
    odebug << "Prefix: " << m_prefix << oendl;
    odebug << "Command: " << m_command << oendl;
    odebug << "Allparameters: " << m_allParameters << oendl;
    for (unsigned int i=0; i<m_parameters.count(); i++) {
        odebug << "Parameter " << i << ":" << m_parameters[i] << oendl;
    }
    odebug << "CTCP Command: " << m_ctcpCommand << oendl;
    odebug << "CTCP Destination: " << m_ctcpDestination << oendl;
    odebug << "CTCP param  count is: " << m_parameters.count() << oendl;
    
}

QString IRCMessage::param(int param) {
    return m_parameters[param];
}

QStringList IRCMessage::params(const QString &paramstring) const {
    QStringList params, retvalue;
    params = QStringList::split(',', paramstring);
    QStringList::Iterator end = params.end(); 
    
    for (QStringList::Iterator it = params.begin(); it != end; ++it) {
        int pos = (*it).find(':');
        if(pos < 0) {    
            if((*it).toUInt() < m_parameters.count())
                retvalue << m_parameters[(*it).toUInt()];
        }

        else {
            unsigned int start, end;
            start = (*it).left(pos).toUInt();
            end = (*it).mid(pos+1).toUInt();
            for (unsigned int i=start;i<=end && i < m_parameters.count() ;++i) {
                retvalue << m_parameters[i];
            }
        }      
    }
    
    return retvalue;
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

bool IRCMessage::isCTCPRequest() {
    return m_ctcpRequest;
}

bool IRCMessage::isCTCPReply() {
    return !m_ctcpRequest;
}

QString IRCMessage::trailing() {
    return m_trailing;
}

QString IRCMessage::allParameters() {
    return m_allParameters;
}

