#include "ircoutput.h"

IRCOutput::IRCOutput(IRCOutputType type, QString message) {
    m_type = type;
    m_message = message;
}

IRCOutputType IRCOutput::type() {
    return m_type;
}

QString IRCOutput::message() {
    return m_message;
}

QString IRCOutput::htmlMessage() {
    QString htmlMessage =m_message.replace(QRegExp("&"), "&amp;");
    htmlMessage = htmlMessage.replace(QRegExp(">"), "&gt;");
    htmlMessage = htmlMessage.replace(QRegExp("<"), "&lt;");
    return htmlMessage;
}

void IRCOutput::addParam(void *data) {
    m_parameters.append(data);
}

void *IRCOutput::getParam(int index) {
    return m_parameters.at(index);
}
