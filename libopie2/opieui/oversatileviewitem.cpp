/*
                             This file is part of the Opie Project

              =.             (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
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

#include <opie2/oversatileviewitem.h>
#include <opie2/oversatileview.h>

OVersatileViewItem::OVersatileViewItem( OVersatileView * parent )
              :OListViewItem( parent->_listview ), QIconViewItem( parent->_iconview ),
              _versatileview( parent )
{
    init();
}

OVersatileViewItem::OVersatileViewItem( OVersatileView * parent, OVersatileViewItem * after )
              :OListViewItem( parent->_listview, after ), QIconViewItem( parent->_iconview, after ),
              _versatileview( parent )
{
    init();
}

OVersatileViewItem::OVersatileViewItem( OVersatileView * parent,
                                        QString a, QString b, QString c, QString d,
                                        QString e, QString f, QString g, QString h )
                   :OListViewItem( parent->_listview, a, b, c, d, e, f, g, h ),
                    QIconViewItem( parent->_iconview, a ),
                    _versatileview( parent )
{
    init();
}

OVersatileViewItem::OVersatileViewItem( OVersatileView * parent, OVersatileViewItem* after,
                                        QString a, QString b, QString c, QString d,
                                        QString e, QString f, QString g, QString h )
                   :OListViewItem( parent->_listview, after, a, b, c, d, e, f, g, h ),
                    QIconViewItem( parent->_iconview, after, a ),
                    _versatileview( parent )
{
    init();
}

OVersatileViewItem::OVersatileViewItem( OVersatileViewItem * parent,
                                        QString a, QString b, QString c, QString d,
                                        QString e, QString f, QString g, QString h )
                   :OListViewItem( parent, a, b, c, d, e, f, g, h ),
                    QIconViewItem( parent->_versatileview->_iconview, a ),
                    _versatileview( parent->_versatileview )
{
    init();
}

OVersatileViewItem::OVersatileViewItem( OVersatileViewItem * parent, OVersatileViewItem* after,
                                        QString a, QString b, QString c, QString d,
                                        QString e, QString f, QString g, QString h )
                   :OListViewItem( parent, after, a, b, c, d, e, f, g, h ),
                    QIconViewItem( parent->_versatileview->_iconview, after, a ),
                    _versatileview( parent->_versatileview )
{
    init();
}

OVersatileViewItem::~OVersatileViewItem()
{
}

OVersatileView* OVersatileViewItem::versatileView() const
{
    return _versatileview;
}

void OVersatileViewItem::init()
{
    if ( !firstChild() )
    {
        // I'm a sweet yellow and browne autumn leaf
        
            OListViewItem::setPixmap( 0, _versatileview->_treeleaf );
            QIconViewItem::setPixmap( _versatileview->_iconleaf );
    }
    else
    {
        // I'm a node and I have a little baby child
        
        if ( isOpen() )
        {        
                OListViewItem::setPixmap( 0, _versatileview->_treeopened );
                QIconViewItem::setPixmap( _versatileview->_iconopened );
        }
        else
        {        
                OListViewItem::setPixmap( 0, _versatileview->_treeclosed );
                QIconViewItem::setPixmap( _versatileview->_iconclosed );
        }
    }
    
}

void OVersatileViewItem::setRenameEnabled( bool allow )
{
    #if (QT_VERSION >= 0x030000)
    OListViewItem::setRenameEnabled( 0, allow );    // TODO: Backport to Qt-Embedded 2.x?
    #endif
    QIconViewItem::setRenameEnabled( allow );
}

  
