/*
Dagger - A Bible study program utilizing the Sword library.
Copyright (c) 2004 Dan Williams <drw@handhelds.org>

This file is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later version.

This file is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this
file; see the file COPYING. If not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef SWORDOPTIONLIST_H
#define SWORDOPTIONLIST_H

#include <qaction.h>
#include <qlist.h>

class SwordOptionList : public QList<QAction>
{
private:

    int compareItems( QCollection::Item item1, QCollection::Item item2 )
    {
        // Sort by QAction text
        QString act1 = reinterpret_cast<QAction*>(item1)->text();
        QString act2 = reinterpret_cast<QAction*>(item2)->text();
        if ( act1 < act2 )
            return -1;
        else if ( act1 == act2 )
            return 0;
        else
            return 1;
    }
};

typedef QListIterator<QAction> SwordOptionListIterator;

#endif
