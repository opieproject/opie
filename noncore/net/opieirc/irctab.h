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

#ifndef __IRCTAB_H
#define __IRCTAB_H

#include <qwidget.h>
#include <qtextview.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include "ircsession.h"

/* This is the base class for any tabs which need to be integrated into
   the main GUI tab widget */

class IRCTab : public QWidget {
    Q_OBJECT
public:
    IRCTab(QWidget *parent = 0, const char *name = 0, WFlags f = 0);
    void setID(int id);
    int id();
    virtual QString title() = 0;
    virtual IRCSession *session() = 0;
    virtual void appendText(QString text) = 0;
signals:
    void changed(IRCTab *);
public slots:
    virtual void remove() = 0;
    virtual void settingsChanged() = 0;
protected:
    QLabel      *m_description;
    QVBoxLayout *m_layout;
    int          m_id;
public:
    /* Configuration shared accross all instances - contains HTML style colors (#rrggbb) */ 
    static QString m_errorColor;
    static QString m_serverColor;
    static QString m_textColor;
    static QString m_backgroundColor;
    static QString m_selfColor;
    static QString m_otherColor;
    static QString m_notificationColor;
};

#endif /* __IRCTAB_H */
