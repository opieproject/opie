#ifndef IRCCHANNELPERSON_H
#define IRCCHANNELPERSON_H

class QString;
class IRCPerson;

/* This class encapsulates a IRCPerson and adds
 *    channel specific information */
class IRCChannelPerson {

public:
    /* Flags which a person can have inside a channel */
    enum IRCChannelPersonFlag {
            PERSON_FLAG_OP     = 0x01,
            PERSON_FLAG_VOICE  = 0x02,
            PERSON_FLAG_HALFOP = 0x04
    };

    IRCChannelPerson(IRCPerson *person = 0);
    ~IRCChannelPerson();
    
    QString setOp(const QString &nickname, bool set);
    QString setVoice(const QString &nickname, bool set);
    QString nick();
    const unsigned int flags();
    void setFlags(int flags);
    void setNick(const QString &nickname);

protected:
    IRCPerson *m_person;
    unsigned int m_flags;
};

#endif
