#include <qpe/resource.h>
#include <qpixmap.h>
#include "ircchannellist.h"

IRCChannelList::IRCChannelList(IRCChannel *channel, QWidget *parent, const char *name, WFlags f) : QListBox(parent, name, f) {
    m_channel = channel;
}

void IRCChannelList::update() {
    QPixmap op = Resource::loadPixmap("opieirc/op");
    QPixmap hop = Resource::loadPixmap("opieirc/hop");
    QPixmap voice = Resource::loadPixmap("opieirc/voice");
    QListIterator<IRCChannelPerson> it = m_channel->people();
    clear();
    for (; it.current(); ++it) {
        IRCChannelPerson *person = it.current();
        if (person->flags & PERSON_FLAG_OP) {
            insertItem(op, person->person->nick());
        } else if (person->flags & PERSON_FLAG_HALFOP) {
            insertItem(op, person->person->nick());
        } else if (person->flags & PERSON_FLAG_VOICE) {
            insertItem(voice, person->person->nick());
        } else {
            insertItem(person->person->nick());
        }
    }
    sort();
}


bool IRCChannelList::hasPerson(QString nick) {
    for (unsigned int i=0; i<count(); i++) {
        if (text(i) == nick)
            return TRUE;
    }
    return FALSE;
}
