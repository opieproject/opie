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

#ifndef OCHECKITEM_H_
#define OCHECKITEM_H_

/* QT */
#include <qtable.h>

namespace Opie
{

/**
 * This class represents a checkable QTableItem. This can
 * be added to any QTable.
 *
 *
 * @see QTable
 * @see QTableItem
 * @short An checkable QTableItem
 * @version 1.0
 * @author Stefan Eilers ( eilers@handhelds.org )
 **/

class OCheckItem : public QTableItem
{
public:
    /** The size of a box currently unused */
    enum Size { BoxSize = 10 };
    OCheckItem( QTable *t, const QString &sortkey );

    virtual void setChecked( bool b );
    virtual void toggle();
    bool isChecked() const;
    /**
     * @short Set the sort key
     * @reimp
     */
    void setKey( const QString &key ) { m_sortKey = key; }
    virtual QString key() const;

    /**
     * @short Paint the Checkitem
     * @reimp
     */
    void paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected );

    //static const int BoxSize = 10;

private:
    class OCheckItemPrivate;
    OCheckItemPrivate *d;
    bool m_checked : 1;
    QString m_sortKey;

};

};

#endif
