#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include "ircservereditor.h"

IRCServerEditor::IRCServerEditor(IRCServer server, QWidget* parent, const char* name, bool modal = FALSE, WFlags f) : QDialog(parent, name, modal, f) {
    QGridLayout *layout = new QGridLayout(this, 6, 2, 5, 5);
    QLabel *label = new QLabel(tr("Profile name :"), this);
    m_name = new QLineEdit(server.name(), this);
    layout->addWidget(label, 0, 0);
    layout->addWidget(m_name, 0, 1);
    label = new QLabel(tr("Hostname :"), this);
    m_hostname = new QLineEdit(server.hostname(), this);
    layout->addWidget(label, 1, 0);
    layout->addWidget(m_hostname, 1, 1);
    label = new QLabel(tr("Port :"), this);
    m_port = new QLineEdit(QString::number(server.port()), this);
    layout->addWidget(label, 2, 0);
    layout->addWidget(m_port, 2, 1);
    label = new QLabel(tr("Nickname :"), this);
    m_nickname = new QLineEdit(server.nick(), this);
    layout->addWidget(label, 3, 0);
    layout->addWidget(m_nickname, 3, 1);
    label = new QLabel(tr("Realname :"), this);
    m_realname = new QLineEdit(server.realname(), this);
    layout->addWidget(label, 4, 0);
    layout->addWidget(m_realname, 4, 1);
    label = new QLabel(tr("Password :"), this);
    m_password = new QLineEdit(server.password(), this);
    layout->addWidget(label, 5, 0);
    layout->addWidget(m_password, 5, 1);
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
    else
        QDialog::accept();
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
    return server;
}
