/*
    OpieIRC - An embedded IRC client
    Copyright (C) 2002 Wenzel Jakob

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef __IRCSERVERTAB_H
#define __IRCSERVERTAB_H

#include "irctab.h"
#include "ircsession.h"
#include "mainwindow.h"
#include "ircchanneltab.h"

class IRCServerTab : public IRCTab {
    Q_OBJECT
public:
    /* IRCTab implementation */
    IRCServerTab(IRCServer server, MainWindow *mainWindow, QWidget *parent = 0, const char *name = 0, WFlags f = 0);
    ~IRCServerTab();
    QString title();
    IRCSession *session();
    IRCServer *server();

    /* Start the server session */
    void doConnect();
    void removeChannelTab(IRCChannelTab *tab);

    /* Execute a user command such as /join */
    void executeCommand(IRCTab *tab, QString line);
protected:
    void appendText(QString text);
    IRCChannelTab *getTabForChannel(IRCChannel *channel);
public slots:
    void remove();
    void processCommand();
protected slots:
    void display(IRCOutput output);
protected:
    bool                  m_close;
    IRCServer             m_server;
    IRCSession           *m_session;
    MainWindow           *m_mainWindow;
    QTextView            *m_textview;
    QLineEdit            *m_field;
    QList<IRCChannelTab>  m_channelTabs;
};

#endif /* __IRCSERVERTAB_H */
