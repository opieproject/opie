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

#ifndef __IRCQUERYTAB_H
#define __IRCQUERYTAB_H

#include "mainwindow.h"
#include "ircsession.h"
#include "ircmisc.h"

class IRCServerTab;
class IRCQueryTab : public IRCTab {
    Q_OBJECT
public:
    /* IRCTab implementation */
    IRCQueryTab(IRCPerson *person, IRCServerTab *parentTab, MainWindow *mainWindow, QWidget *parent = 0, const char *name = 0, WFlags f = 0);
    ~IRCQueryTab();
    QString title();
    IRCSession *session();
    IRCPerson *person();
    void appendText(QString text);
public slots:
    void remove();
    void processCommand();
    void display(IRCOutput output);
    void settingsChanged();
protected:
    bool                  m_close;
    MainWindow           *m_mainWindow;
    IRCServerTab         *m_parentTab;
    IRCPerson            *m_person;
    QTextView            *m_textview;
    IRCHistoryLineEdit   *m_field;
    int                   m_lines;
};

#endif /* __IRCQUERYTAB_H */
