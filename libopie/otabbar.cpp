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

#include "otabbar.h"

OTabBar::OTabBar( QWidget *parent = 0, const char *name = 0 )
     : QTabBar( parent, name )
{
}

void OTabBar::paintLabel( QPainter* p, const QRect& br, QTab* t, bool has_focus ) const
{

    QRect r = br;
    if ( t->iconset)
    {
        QIconSet::Mode mode = (t->enabled && isEnabled()) ? QIconSet::Normal : QIconSet::Disabled;
        if ( mode == QIconSet::Normal && has_focus ) {
            mode = QIconSet::Active;
        }
        QPixmap pixmap = t->iconset->pixmap( QIconSet::Small, mode );
        int pixw = pixmap.width();
        int pixh = pixmap.height();
        r.setLeft( r.left() + pixw + 2 );
        p->drawPixmap( br.left()+2, br.center().y()-pixh/2, pixmap );
    }

    QRect tr = r;
    if ( t->id == currentTab() )
    {
        tr.setBottom( tr.bottom() - style().defaultFrameWidth() );
    }

    if ( t->enabled && isEnabled()  )
    {
        p->setPen( colorGroup().foreground() );
        p->drawText( tr, AlignCenter | ShowPrefix, t->label );
    }
    else if ( style() == MotifStyle )
    {
        p->setPen( palette().disabled().foreground() );
        p->drawText( tr, AlignCenter | ShowPrefix, t->label );
    }
    else
    {
        p->setPen( colorGroup().light() );
        QRect wr = tr;
        wr.moveBy( 1, 1 );
        p->drawText( wr, AlignCenter | ShowPrefix, t->label );
        p->setPen( palette().disabled().foreground() );
        p->drawText( tr, AlignCenter | ShowPrefix, t->label );
    }
}
