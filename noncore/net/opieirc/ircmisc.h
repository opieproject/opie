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

#ifndef __IRCMISC_H
#define __IRCMISC_H

#include <qtabwidget.h>
#include <qlineedit.h>
#include <qtabbar.h>
#include <qlabel.h>
#include <qcolor.h>
#include <qarray.h>

/* Custom colored QTabWidget */

class QExtTab : public QTab {
public:
    QColor color;
};

class IRCTabWidget : public QTabWidget {
    Q_OBJECT
public:
    IRCTabWidget(QWidget *parent = 0, const char *name = 0);
    void setTabColor(int index, QColor color);
};

class IRCTabBar : public QTabBar {
    Q_OBJECT
public:
    IRCTabBar(QWidget *parent = 0, const char *name = 0);
    void setTabColor(int index, QColor color);
protected:
    void paintLabel(QPainter*, const QRect&, QTab*, bool) const;
    int insertTab(QTab *, int index = -1);
protected:
    QArray<QColor> m_colors;
};

/* A QLineEdit with history functionality and tab completion */

class IRCHistoryLineEdit : public QLineEdit {
    Q_OBJECT
public:
    IRCHistoryLineEdit(QWidget *parent = 0, const char *name = 0);
    virtual bool eventFilter(QObject *object, QEvent *event);
protected:
    void keyPressEvent(QKeyEvent *);
protected:
    QStringList m_history;
    int         m_index;
};

#endif /* __IRCMISC_H */
