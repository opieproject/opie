/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef MENUBUTTON_H
#define MENUBUTTON_H

#include <qpushbutton.h>
#include <qstringlist.h>

class MenuButton : public QPushButton {
    Q_OBJECT
public:
    MenuButton( QWidget* parent, const char* name=0);
    MenuButton( const QStringList& items, QWidget* parent, const char* name=0);

    void clear();

    int currentItem() const;
    QString currentText() const;

    void insertItems( const QStringList& items );
    void insertItem( const QIconSet& icon, const QString& text=QString::null );
    void insertItem( const QString& text );
    void insertSeparator();

    void setLabel(const QString& label);

signals:
    void selected(int);
    void selected(const QString&);

public slots:
    void select(int);
    void select(const QString&);

private:
    void init();
    QStringList txts;
    QPopupMenu* pop;
    int nitems;
    int cur;
    QString lab;
    void updateLabel();
};

#endif

