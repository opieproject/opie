/*
                            This file is part of the Opie Project

                             Copyright (c)  2002 Dan Williams <williamsdr@acm.org>
              =.
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

#ifndef OTABBAR_H
#define OTABBAR_H

#include <qtabbar.h>

/**
 * @class OTabBar
 * @brief The OTabBar class is a derivative of QTabBar.
 *
 * OTabBar is a derivation of TrollTech's QTabBar which provides
 * a row of tabs for selection.  The only difference between this
 * class and QTabBar is that there is no dotted line box around
 * the label of the tab with the current focus.
 */
class OTabBar : public QTabBar
{
    Q_OBJECT

public:
/**
 * @fn OTabBar( QWidget *parent = 0, const char *name = 0 )
 * @brief Object constructor.
 *
 * @param parent Pointer to parent of this control.
 * @param name Name of control.
 *
 * Constructs a new OTabBar control with parent and name.
 */
    OTabBar( QWidget * = 0, const char * = 0 );

protected:
/**
 * @fn paintLabel( QPainter* p, const QRect& br, QTab* t, bool has_focus )
 * @brief Internal function to draw a tab's label.
 *
 * @param p Pointer to QPainter used for drawing.
 * @param br QRect providing region to draw label in.
 * @param t Tab to draw label for.
 * @param has_focus Boolean value not used, retained for compatibility reasons.
 */
    void paintLabel( QPainter *, const QRect &, QTab *, bool ) const;
};

#endif
