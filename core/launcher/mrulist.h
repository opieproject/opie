/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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

#ifndef __MRU_LIST_H__
#define __MRU_LIST_H__


#include <qpe/applnk.h>

#include <qframe.h>
#include <qlist.h>
#include <qpixmap.h>


class MRUList : public QFrame
{
public:
    MRUList( QWidget *parent );
    ~MRUList();
    virtual QSize sizeHint() const;
    static void addTask( const AppLnk *appLnk );
    static void removeTask(const QString &appName );
    bool quitOldApps();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent( QPaintEvent *event );

private:
    static QList<MRUList>   *MRUListWidgets;
    static QList<AppLnk>    *task;
    int selected;
    int oldsel;
};


#endif // __MRU_LIST_H__

