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

#ifndef __CATEGORYMENU_H__
#define __CATEGORYMENU_H__

#include <qpopupmenu.h>
#include <qstring.h>
#include <qarray.h>
#include <qmap.h>

class CategoryMenuPrivate;
class CategoryMenu : public QPopupMenu
{
    Q_OBJECT
public:
    CategoryMenu( const QString &appName, bool, QWidget *parent = 0, 
	    const char *name = 0 );
    ~CategoryMenu();

    bool isSelected(const QArray<int> &cUids) const;

    void setCurrentCategory( int newCatUid );
    void setCurrentCategoryAll( );
    void setCurrentCategoryUnfiled( );

signals:
    void categoryChange();

public slots:
    void reload();

private slots:
    void mapMenuId(int);

private:
    QString appName;
    bool includeGlobal;

    QMap<int, int> menuToId;
    QMap<int, int> idToMenu;
    int currentMid;

    CategoryMenuPrivate *d;
};

#endif
