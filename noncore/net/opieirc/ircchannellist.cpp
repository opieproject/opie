#include <qpe/resource.h>

#include "ircchannellist.h"
#include "ircchannelperson.h"

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
        if (person->flags() & IRCChannelPerson::PERSON_FLAG_OP) {
            insertItem(op, "1" + person->nick());
        } else if (person->flags() & IRCChannelPerson::PERSON_FLAG_HALFOP) {
            insertItem(op, "2" + person->nick());
        } else if (person->flags() & IRCChannelPerson::PERSON_FLAG_VOICE) {
            insertItem(voice, "3" + person->nick());
        } else {
            insertItem("4" + person->nick());
        }
    }
    sort();
    adjustNicks();
}


bool IRCChannelList::hasPerson(QString nick) {
    for (unsigned int i=0; i<count(); i++) {
        if (text(i) == nick)
            return TRUE;
    }
    return FALSE;
}

bool IRCChannelList::removePerson(QString nick) {
    for (unsigned int i=0; i<count(); i++) {
        if (text(i) == nick){
            removeItem(i);
            return TRUE;
            }
    }
    return FALSE;
}

void IRCChannelList::adjustNicks() {
    QString txt;
    QPixmap pm;

    for(unsigned int i=0; i<count(); i++) {
        txt = text(i).remove(0,1);
        if(pixmap(i)) {
            pm = *pixmap(i);
            removeItem(i);
            insertItem(pm, txt, i);
        }
        else {
            removeItem(i);
            insertItem(txt, i);
        }
    }
}
