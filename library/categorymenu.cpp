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

#include "categorymenu.h"
#include "backend/categories.h"
#include "categoryselect.h"
#include <qstring.h>
#include <qmap.h>

/*!
  \class CategoryMenu
  \brief The CategoryMenu widget aids in filtering records or files by Category.

  The CategoryMenu widget provides a popup menu that will make filtering records
  or files by category much easier.  The widget will lookup the available
  categories for an application, populate the menu, and keep a track of which
  categories are being filtered against.  A set of categories can be tested
  by the isSelected() function to see if a record or file containing those
  categories would be allowed through by the filter.

  \warning Currently this class is not suitable for extending.

  \ingroup qtopiaemb
*/

/*!
  \fn void CategoryMenu::categoryChange()
  This signal is emitted when the user selects a different category in the
  menu, hence changing what records or files should be selected.
*/

/*!
  Creates a new CategoryMenu with \a parent and \a name.  The menu will be
  populated with the available categories for \a application.

  If \a globals is TRUE then it will also poplulate the menu with the
  global categories.
*/
CategoryMenu::CategoryMenu( const QString &application, bool globals = TRUE,
	QWidget *parent = 0, const char *name = 0 ) :
    QPopupMenu(parent, name),
	appName(application),
	includeGlobal(globals)
{
    currentMid = 1;
    reload();
    connect(this, SIGNAL(activated(int)), this, SLOT(mapMenuId(int)));
}

/*!
  Destroys a CategoryMenu.
*/
CategoryMenu::~CategoryMenu( )
{
}

/*!
  Repopulates the widget's list of available categories.
*/
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

/*!
  \internal
*/
void CategoryMenu::mapMenuId(int id)
{
    if (id == currentMid)
	return;
    setItemChecked( currentMid, FALSE );
    setItemChecked( id, TRUE );
    currentMid = id;

    emit categoryChange();
}

/*!
  Returns TRUE if a record or file with the set of category ids \a cUids
  is allowed by the current selection in the CategoryMenu.
  Otherwise returns FALSE.
*/
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

/*!
  Sets the menu to have \a newCatUid as the currently selected Category.
*/
void CategoryMenu::setCurrentCategory( int newCatUid )
{
    if (!idToMenu.contains(newCatUid))
	return;

    mapMenuId(idToMenu[newCatUid]);
}

/*!
  Sets the menu to allow all category sets.
*/
void CategoryMenu::setCurrentCategoryAll( )
{
    mapMenuId(1);
}

/*!
  Sets the menu to allow only empty category sets.
*/
void CategoryMenu::setCurrentCategoryUnfiled( )
{
    mapMenuId(2);
}
