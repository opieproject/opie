/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers <eilers.stefan@epost.de>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include <opie2/ocheckitem.h>

/* QT */
#include <qpainter.h>

using namespace Opie;

/**
 * Constructs an CheckItem with a QTable as parent
 * and a sort key for.
 * The sort key will be used by QTable to sort the table later
 * @param t The parent QTable where the check item belongs
 * @param key A sort key
 */
OCheckItem::OCheckItem( QTable *t, const QString &key )
           :QTableItem( t, Never, "" ), m_checked( FALSE ), m_sortKey( key )
{}

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
void OCheckItem::paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool )
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
    if ( m_checked )
    {
        QPointArray a( 7*2 );
        int i, xx, yy;
        xx = x+1+marg;
        yy = y+2;
        for ( i=0; i<3; i++ )
        {
            a.setPoint( 2*i,   xx, yy );
            a.setPoint( 2*i+1, xx, yy+2 );
            xx++; yy++;
        }
        yy -= 2;
        for ( i=3; i<7; i++ )
        {
            a.setPoint( 2*i,   xx, yy );
            a.setPoint( 2*i+1, xx, yy+2 );
            xx++; yy--;
        }
        p->drawLineSegments( a );
    }
}
