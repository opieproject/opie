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

void IRCOutput::addParam(void *data) {
    m_parameters.append(data);
}

void *IRCOutput::getParam(int index) {
    return m_parameters.at(index);
}
