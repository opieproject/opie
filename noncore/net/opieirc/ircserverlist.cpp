#include <qlayout.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>
#include "ircserverlist.h"
#include "ircservereditor.h"

class IRCListBoxServer : public QListBoxText {
public:
    IRCListBoxServer(IRCServer server) : QListBoxText(server.name()) {
        m_server = server;
    }

    IRCServer server() {
        return m_server;
    }
    
    void setServer(IRCServer server) {
        m_server = server;
        setText(m_server.name());
    }
protected:
    IRCServer m_server;
};

IRCServerList::IRCServerList(QWidget* parent, const char *name, bool modal, WFlags) : QDialog(parent, name, modal, WStyle_ContextHelp) {
    QVBoxLayout *layout = new QVBoxLayout(this, 5, 5);
    setCaption(tr("Serverlist Browser"));
    QLabel *label = new QLabel(tr("Please choose a server profile"), this);
    label->setAlignment(AlignHCenter);
    layout->addWidget(label);
    m_list = new QListBox(this);
    QWhatsThis::add(m_list, tr("Select a server profile from this list and then tap on OK in the upper-right corner"));
    layout->addWidget(m_list);
    QHBox *buttons = new QHBox(this);
    QPushButton *del = new QPushButton(tr("Delete"), buttons);
    QPushButton *edit = new QPushButton(tr("Edit"), buttons);
    QPushButton *add = new QPushButton(tr("Add"), buttons);
    QWhatsThis::add(del, tr("Delete the currently selected server profile"));
    QWhatsThis::add(edit, tr("Edit the currently selected server profile"));
    QWhatsThis::add(add, tr("Add a new server profile"));
    connect(del, SIGNAL(clicked()), this, SLOT(delServer()));
    connect(edit, SIGNAL(clicked()), this, SLOT(editServer()));
    connect(add, SIGNAL(clicked()), this, SLOT(addServer()));
    layout->addWidget(buttons);
    /* Load the configuration file */
    m_config = new Config("OpieIRC");
    m_config->setGroup("OpieIRC");
    int count = m_config->readNumEntry("ServerCount", 0);
    if (count) {
        for (int i=0; i<count; i++) {
            m_config->setGroup("OpieIRC");
            QString name = m_config->readEntry("Server"+QString::number(i));
            if (name.length() > 0) {
                IRCServer server;
                m_config->setGroup(name);
                server.setName(name);
                server.setHostname(m_config->readEntry("Hostname"));
                server.setPort(m_config->readNumEntry("Port"));
                server.setUsername(m_config->readEntry("Username"));
                server.setPassword(m_config->readEntry("Password"));
                server.setNick(m_config->readEntry("Nick"));
                server.setRealname(m_config->readEntry("Realname"));
                server.setChannels(m_config->readEntry("Channels"));
                m_list->insertItem(new IRCListBoxServer(server));
            }
        }
    }
    
    showMaximized();
}

void IRCServerList::addServer() {
    IRCServer server;
    IRCServerEditor editor(server, this, "ServerEditor", TRUE);
    if (editor.exec() == QDialog::Accepted) {
        server = editor.getServer();
        /* Gets deleted by QListBox, so this is ok */
        m_list->insertItem(new IRCListBoxServer(server));
    }
}

void IRCServerList::delServer() {
    int index = m_list->currentItem();
    if (index != -1) {
        m_list->removeItem(index);
    }
}

void IRCServerList::editServer() {
    int index = m_list->currentItem();
    if (index != -1) {
        IRCListBoxServer *item = (IRCListBoxServer *)m_list->item(index);
        IRCServer server = item->server();
        IRCServerEditor editor(server, this, "ServerEditor", TRUE);
        if (editor.exec() == QDialog::Accepted) {
            server = editor.getServer();
            item->setServer(server);
        }
    }
}

int IRCServerList::exec() {
    int returncode = QDialog::exec();
    /* Now save the changes */
    m_config->setGroup("OpieIRC");
    m_config->writeEntry("ServerCount", QString::number(m_list->count()));
    for (unsigned int i=0; i<m_list->count(); i++) {
        IRCServer server = ((IRCListBoxServer *)m_list->item(i))->server();
        m_config->setGroup("OpieIRC");
        m_config->writeEntry("Server"+QString::number(i), server.name());
        m_config->setGroup(server.name());
        m_config->writeEntry("Hostname", server.hostname());
        m_config->writeEntry("Port", QString::number(server.port()));
        m_config->writeEntry("Username", server.username());
        m_config->writeEntry("Password", server.password());
        m_config->writeEntry("Nick", server.nick());
        m_config->writeEntry("Realname", server.realname());
        m_config->writeEntry("Channels", server.channels());
    }
    return returncode;
}

bool IRCServerList::hasServer() {
    return (m_list->currentItem() != -1);
}

IRCServer IRCServerList::server() {
    return ((IRCListBoxServer *)m_list->item(m_list->currentItem()))->server();
}

IRCServerList::~IRCServerList() {
    delete m_config;
}
