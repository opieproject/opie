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

#ifndef __IRCCHANNELTAB_H
#define __IRCCHANNELTAB_H

#include <qpopupmenu.h>
#include <qpushbutton.h>
#include "irctab.h"
#include "ircsession.h"
#include "ircmisc.h"
#include "mainwindow.h"
#include "ircchannellist.h"

#define LISTWIDTH 70

class IRCServerTab;
class IRCChannelTab : public IRCTab {
    Q_OBJECT
public:
    /* IRCTab implementation */
    IRCChannelTab(IRCChannel *channel, IRCServerTab *parentTab, MainWindow *mainWindow, QWidget *parent = 0, const char *name = 0, WFlags f = 0);
    ~IRCChannelTab();
    QString title();
    IRCSession *session();
    IRCChannel *channel();
    IRCChannelList *list();
public:
    void appendText(QString text);
public slots:
    void remove();
    void settingsChanged();
protected slots:
    void processCommand();
    void toggleList();
    void mouseButtonPressed(int mouse, QListBoxItem *item, const QPoint &point);
    /* Popup slots */
    void popupQuery();
    void popupPing();
    void popupVersion();
    void popupWhois();
protected:
    IRCServerTab       *m_parentTab;
    IRCChannel         *m_channel;
    IRCChannelList     *m_list;
    QPushButton        *m_listButton;
    MainWindow         *m_mainWindow;
    QTextView          *m_textview;
    IRCHistoryLineEdit *m_field;
    QPopupMenu         *m_popup;
    bool                m_listVisible;
    int                 m_lines;
};

#endif /* __IRCCHANNELTAB_H */
