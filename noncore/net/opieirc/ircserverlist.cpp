#include <qlayout.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include "ircserverlist.h"
#include "ircservereditor.h"

class IRCListBoxServer : public QListBoxText {
public:
    IRCListBoxServer(IRCServer server);
    QString text();
};

IRCServerList::IRCServerList(QWidget* parent, const char *name, bool modal, WFlags f) : QDialog(parent, name, modal, f) {
    QVBoxLayout *layout = new QVBoxLayout(this, 5, 5);
    setCaption(tr("Serverlist Browser"));
    m_config = new Config("OpieIRC");
    m_config->setGroup("OpieIRC");
    QLabel *label = new QLabel(tr("Please choose a server profile"), this);
    layout->addWidget(label);
    m_list = new QListBox(this);
    layout->addWidget(m_list);
    QHBox *buttons = new QHBox(this);
    QPushButton *del = new QPushButton(tr("Delete"), buttons);
    QPushButton *edit = new QPushButton(tr("Edit"), buttons);
    QPushButton *add = new QPushButton(tr("Add"), buttons);
    connect(del, SIGNAL(clicked()), this, SLOT(delServer()));
    connect(edit, SIGNAL(clicked()), this, SLOT(editServer()));
    connect(add, SIGNAL(clicked()), this, SLOT(addServer()));
    layout->addWidget(buttons);
    showMaximized();
}

void IRCServerList::addServer() {
    IRCServer server;
    IRCServerEditor editor(server, this, "ServerEditor", TRUE);
    if (editor.exec() == QDialog::Accepted) {
        server = editor.getServer();
        //m_servers->append(server);
        update();
    }
}

void IRCServerList::delServer() {
}

void IRCServerList::editServer() {
}

void IRCServerList::update() {
}

IRCServerList::~IRCServerList() {
    delete m_config;
}
