#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qwhatsthis.h>
#include "ircservereditor.h"

IRCServerEditor::IRCServerEditor(IRCServer server, QWidget* parent, const char* name, bool modal, WFlags) : QDialog(parent, name, modal, WStyle_ContextHelp) {
    QGridLayout *layout = new QGridLayout(this, 7, 2, 5, 5);
    QLabel *label = new QLabel(tr("Profile name :"), this);
    m_name = new QLineEdit(server.name(), this);
    QWhatsThis::add(m_name, tr("The name of this server profile in the overview"));
    layout->addWidget(label, 0, 0);
    layout->addWidget(m_name, 0, 1);
    label = new QLabel(tr("Hostname :"), this);
    m_hostname = new QLineEdit(server.hostname(), this);
    QWhatsThis::add(m_hostname, tr("The server to connect to - can be any valid host name or IP address"));
    layout->addWidget(label, 1, 0);
    layout->addWidget(m_hostname, 1, 1);
    label = new QLabel(tr("Port :"), this);
    m_port = new QLineEdit(QString::number(server.port()), this);
    QWhatsThis::add(m_port, tr("The server port to connect to. Usually 6667"));
    layout->addWidget(label, 2, 0);
    layout->addWidget(m_port, 2, 1);
    label = new QLabel(tr("Nickname :"), this);
    m_nickname = new QLineEdit(server.nick(), this);
    QWhatsThis::add(m_nickname, tr("Your nick name on the IRC network"));
    layout->addWidget(label, 3, 0);
    layout->addWidget(m_nickname, 3, 1);
    label = new QLabel(tr("Realname :"), this);
    m_realname = new QLineEdit(server.realname(), this);
    QWhatsThis::add(m_realname, tr("Your real name"));
    layout->addWidget(label, 4, 0);
    layout->addWidget(m_realname, 4, 1);
    label = new QLabel(tr("Password :"), this);
    m_password = new QLineEdit(server.password(), this);
    QWhatsThis::add(m_password, tr("Password to connect to the server (if required)"));
    layout->addWidget(label, 5, 0);
    layout->addWidget(m_password, 5, 1);
    label = new QLabel(tr("Channels :"), this);
    m_channels = new QLineEdit(server.channels(), this);
    QWhatsThis::add(m_channels, tr("Comma-Separated list of all channels you would like to join automatically"));
    layout->addWidget(label, 6, 0);
    layout->addWidget(m_channels, 6, 1);
    setCaption(tr("Edit server information"));
    showMaximized();
}


void IRCServerEditor::accept() {
    if (m_name->text().length()==0)
        QMessageBox::critical(this, tr("Error"), tr("Profile name required"));
    else if (m_hostname->text().length()==0)
        QMessageBox::critical(this, tr("Error"), tr("Host name required"));
    else if (m_port->text().toInt()<=0)
        QMessageBox::critical(this, tr("Error"), tr("Port required"));
    else if (m_nickname->text().length()==0)
        QMessageBox::critical(this, tr("Error"), tr("Nickname required"));
    else if (m_realname->text().length()==0)
        QMessageBox::critical(this, tr("Error"), tr("Realname required"));
    else {
        /* Now verify whether the channel list has a valid format */
        QStringList channels = QStringList::split(QChar(','), m_channels->text());
        for (QStringList::Iterator it = channels.begin(); it != channels.end(); ++it) {
            QString channelName = (*it).stripWhiteSpace();
            if (!channelName.startsWith("#") && !channelName.startsWith("+")) {
                QMessageBox::critical(this, tr("Error"), tr("The channel list needs to contain a\ncomma separated list of channel\n names which start with either '#' or '+'"));
                return;
            }
        }
        QDialog::accept();
    }
}

IRCServer IRCServerEditor::getServer() {
    IRCServer server;
    server.setName(m_name->text());
    server.setHostname(m_hostname->text());
    server.setPort(m_port->text().toInt());
    server.setNick(m_nickname->text());
    server.setRealname(m_realname->text());
    server.setUsername(m_nickname->text());
    server.setPassword(m_password->text());
    server.setChannels(m_channels->text());
    return server;
}
