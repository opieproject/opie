/**********************************************************************
** Copyright (C) 2002 Stefan Eilers (se, eilers.stefan@epost.de
**
** This file may be distributed and/or modified under the terms of the
** GNU Library General Public License version 2 as published by the
**  Free Software Foundation and appearing in the file LICENSE.GPL
** included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**********************************************************************/


#include "ocheckitem.h"

/**
 * Constructs an CheckItem with a QTable as parent
 * and a sort key for.
 * The sort key will be used by QTable to sort the table later
 * @param t The parent QTable where the check item belongs
 * @param key A sort key
 */
OCheckItem::OCheckItem( QTable *t, const QString &key )
    : QTableItem( t, Never, "" ), m_checked( FALSE ), m_sortKey( key )
{
}

/**
 * reimplemted for internal reasons
 * @return Returns the sort key of the Item
 * @see QTableItem
 */
QString OCheckItem::key() const
{
    return m_sortKey;
}

/**
 * This method can check or uncheck the item. It will
 * call QTable to update the cell.
 *
 * @param b Whether to check or uncheck the item
 */
void OCheckItem::setChecked( bool b )
{
    m_checked = b;
    table()->updateCell( row(), col() );
}

/**
 * This will toggle the item. If it is checked it'll get
 * unchecked by this method or vice versa.
 */
void OCheckItem::toggle()
{
    m_checked = !m_checked;
}

/**
 * This will return the state of the item.
 *
 * @return Returns true if the item is checked
 */
bool OCheckItem::isChecked() const
{
    return m_checked;
}

/**
 * @internal
 * This paints the item
 */
void OCheckItem::paint( QPainter *p, const QColorGroup &cg, const QRect &cr,
		       bool )
{
    p->fillRect( 0, 0, cr.width(), cr.height(), cg.brush( QColorGroup::Base ) );

    int marg = ( cr.width() - BoxSize ) / 2;
    int x = 0;
    int y = ( cr.height() - BoxSize ) / 2;
    p->setPen( QPen( cg.text() ) );
    p->drawRect( x + marg, y, BoxSize, BoxSize );
    p->drawRect( x + marg+1, y+1, BoxSize-2, BoxSize-2 );
    p->setPen( darkGreen );
    x += 1;
    y += 1;
    if ( m_checked ) {
	QPointArray a( 7*2 );
	int i, xx, yy;
	xx = x+1+marg;
	yy = y+2;
	for ( i=0; i<3; i++ ) {
	    a.setPoint( 2*i,   xx, yy );
	    a.setPoint( 2*i+1, xx, yy+2 );
	    xx++; yy++;
	}
	yy -= 2;
	for ( i=3; i<7; i++ ) {
	    a.setPoint( 2*i,   xx, yy );
	    a.setPoint( 2*i+1, xx, yy+2 );
	    xx++; yy--;
	}
	p->drawLineSegments( a );
    }
}
