/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
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

#include "categorymenu.h"
#include "backend/categories.h"
#include "categoryselect.h"
#include <qstring.h>
#include <qmap.h>

CategoryMenu::CategoryMenu( const QString &n, bool ig = TRUE,
	QWidget *parent = 0, const char *name = 0 ) : 
    QPopupMenu(parent, name),
	appName(n),
	includeGlobal(ig)
{
    currentMid = 1;
    reload();
    connect(this, SIGNAL(activated(int)), this, SLOT(mapMenuId(int)));
}

CategoryMenu::~CategoryMenu( ) 
{
}

void CategoryMenu::reload()
{
    clear();
    Categories c;

    c.load(categoryFileName());

    QStringList sl = c.labels(appName, includeGlobal);
    int mid = 1;

    insertItem(tr("All"), mid);
    mid++;
    insertItem(tr("Unfiled"), mid);
    mid++;

    for (QStringList::Iterator it = sl.begin();
	    it != sl.end(); ++it ) {
	int cid = c.id(appName, *it);
	insertItem(*it, mid);
	menuToId.insert(mid, cid);
	idToMenu.insert(cid, mid);
	mid++;
    }

    setItemChecked(currentMid, TRUE );
}

void CategoryMenu::mapMenuId(int id) 
{
    if (id == currentMid)
	return;
    setItemChecked( currentMid, FALSE );
    setItemChecked( id, TRUE );
    currentMid = id;

    emit categoryChange();
}

bool CategoryMenu::isSelected(const QArray<int> &cUids) const
{
    if (currentMid == 1)
	return TRUE;

    if (currentMid == 2 && cUids.count() == 0) 
	return TRUE;

    if (cUids.contains(menuToId[currentMid]))
	return TRUE;

    return FALSE;
}

void CategoryMenu::setCurrentCategory( int newCatUid )
{
    if (!idToMenu.contains(newCatUid)) 
	return;

    mapMenuId(idToMenu[newCatUid]);
}

void CategoryMenu::setCurrentCategoryAll( )
{
    mapMenuId(1);
}

void CategoryMenu::setCurrentCategoryUnfiled( )
{
    mapMenuId(2);
}
