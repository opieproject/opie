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

#ifndef __IRCSERVEREDITOR
#define __IRCSERVEREDITOR

#include <qdialog.h>
#include <qlineedit.h>
#include "ircserver.h"

class IRCServerEditor : public QDialog {
    Q_OBJECT
public:
    IRCServerEditor(IRCServer server, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags f = 0);
    IRCServer getServer();
protected:
    QLineEdit *m_hostname;
    QLineEdit *m_port;
    QLineEdit *m_description;
    QLineEdit *m_nickname;
    QLineEdit *m_username;
    QLineEdit *m_password;
    QLineEdit *m_realname;
};

#endif /* __IRCSERVEREDITOR_H */
