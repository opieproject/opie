#include <qlayout.h>
#include <qlabel.h>
#include "ircservereditor.h"

IRCServerEditor::IRCServerEditor(IRCServer server, QWidget* parent, const char* name, bool modal = FALSE, WFlags f) : QDialog(parent, name, modal, f) {
    QGridLayout *layout = new QGridLayout(this, 6, 2, 5, 5);
    QLabel *label = new QLabel(tr("Hostname :"), this);
    m_hostname = new QLineEdit(server.hostname(), this);
    layout->addWidget(label, 0, 0);
    layout->addWidget(m_hostname, 0, 1);
    label = new QLabel(tr("Port :"), this);
    m_port = new QLineEdit(QString::number(server.port()), this);
    layout->addWidget(label, 1, 0);
    layout->addWidget(m_port, 1, 1);
    label = new QLabel(tr("Nickname :"), this);
    m_nickname = new QLineEdit(server.nick(), this);
    layout->addWidget(label, 2, 0);
    layout->addWidget(m_nickname, 2, 1);
    label = new QLabel(tr("Description :"), this);
    m_description = new QLineEdit(server.description(), this);
    layout->addWidget(label, 3, 0);
    layout->addWidget(m_description, 3, 1);
    label = new QLabel(tr("Realname :"), this);
    m_realname = new QLineEdit(server.realname(), this);
    layout->addWidget(label, 4, 0);
    layout->addWidget(m_realname, 4, 1);
    label = new QLabel(tr("Username :"), this);
    m_username = new QLineEdit(server.username(), this);
    layout->addWidget(label, 3, 0);
    layout->addWidget(m_username, 3, 1);
    label = new QLabel(tr("Password :"), this);
    m_password = new QLineEdit(server.password(), this);
    layout->addWidget(label, 5, 0);
    layout->addWidget(m_password, 5, 1);
    showMaximized();
}


IRCServer IRCServerEditor::getServer() {
    IRCServer server;
    server.setHostname(m_hostname->text());
    server.setPort(m_port->text().toInt());
    server.setNick(m_nickname->text());
    server.setDescription(m_description->text());
    server.setRealname(m_realname->text());
    server.setUsername(m_username->text());
    server.setPassword(m_password->text());
    return server;
}
