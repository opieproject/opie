#include "ircperson.h"
#include "ircchannelperson.h"

#include <qstring.h>
#include <qobject.h>

IRCChannelPerson::IRCChannelPerson(IRCPerson *person)
{
    m_person = person;
    m_flags = 0;
}

IRCChannelPerson::~IRCChannelPerson()
{
    //if(m_person)
    //    delete m_person;
}
    
QString IRCChannelPerson::setOp(const QString &nickname, bool set)
{
    if(set) {
        m_flags |= PERSON_FLAG_OP;
        return ( nickname + QObject::tr(" gives channel operator status to ") + nick());
    }

    m_flags &= 0xFFFF - PERSON_FLAG_OP;
    return ( nickname + QObject::tr(" removes channel operator status from ") + nick());
}

QString IRCChannelPerson::setVoice(const QString &nickname, bool set)
{
    if(set) {
        m_flags |= PERSON_FLAG_VOICE;
        return ( nickname + QObject::tr(" gives voice to ") + nick() );
    }

    m_flags &= 0xFFFF - PERSON_FLAG_VOICE;
    return ( nickname + QObject::tr(" removes voice from ") + nick());
}

QString IRCChannelPerson::nick()
{
    if(m_person)
        return m_person->nick();
    
    return QString::null;
}

void IRCChannelPerson::setFlags(int flags)
{
    m_flags = flags;
}

void IRCChannelPerson::setNick(const QString &nickname)
{
    m_person->setNick(nickname);
}

const unsigned int IRCChannelPerson::flags()
{
    return m_flags;
}
