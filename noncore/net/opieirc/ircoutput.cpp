#include <qtopia/stringutil.h>

#include "ircoutput.h"


IRCOutputEscapeSecuences IRCOutput::m_escapeSecuences[] = {
    { '\002', "<b>", "</b>"},
    { '\037', "<u>", "</u>"},
    { 0, 0, 0},
};

IRCOutput::IRCOutput(IRCOutputType type, QString message) {
    m_type = type;
    /* Filter color escape sequences, since they aren't implemented yet */
    m_message = message.replace(QRegExp("\003[1-9]*,*[1-9]*"), "");
}

IRCOutputType IRCOutput::type() {
    return m_type;
}

QString IRCOutput::message() {
    return m_message;
}

void IRCOutput::setType(IRCOutputType type) {
    m_type = type;
}

void IRCOutput::setMessage(const QString &message) {
    m_message = message;
}

QString IRCOutput::htmlMessage() {
    return toHTML(m_message);
}

QString IRCOutput::toHTML(const QString &message) {
    QString htmlMessage = Qtopia::escapeString(message);
    
    for(int i=0; m_escapeSecuences[i].escape != 0;++i) {
        int pos = 0;
        bool isOpen = false;
        while( (pos = htmlMessage.find(m_escapeSecuences[i].escape, pos)) != -1) {
            htmlMessage.remove(pos, 1);
            if(isOpen)
                htmlMessage.insert(pos, m_escapeSecuences[i].close);
            else
                htmlMessage.insert(pos, m_escapeSecuences[i].open);
        
            isOpen = !isOpen;
        }
        if(isOpen)
            htmlMessage.append(m_escapeSecuences[i].close);
    }
    
    htmlMessage = htmlMessage.replace(QRegExp("\n"), "<br>");
    return htmlMessage;
}

void IRCOutput::addParam(void *data) {
    m_parameters.append(data);
}

void *IRCOutput::getParam(int index) {
    return m_parameters.at(index);
}

