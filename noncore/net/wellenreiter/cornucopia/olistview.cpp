/*
                             This file is part of the Opie Project

                             Copyright (C) 2003 Michael 'Mickey' Lauer
                             <mickey@tm.informatik.uni-frankfurt.de>
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

#include <qcolor.h>
#include <qheader.h>
#include <qpainter.h>
#include <qpixmap.h>

#include "olistview.h"

//****************************** OListView **************************************************************************

OListView::OListView( QWidget *parent, const char *name )
          :QListView( parent, name )
{
    //FIXME: get from global settings and calculate ==> see oglobalsettings.*

    m_alternateBackground = QColor( 238, 246, 255 );
    m_columnSeparator = QPen( QColor( 150, 160, 170 ), 0, DotLine );
    m_fullWidth = true;
}

OListView::~OListView()
{
}

void OListView::setFullWidth( bool fullWidth )
{
    m_fullWidth = m_fullWidth;
    #if QT_VERSION > 290
    header()->setStretchEnabled( fullWidth, columns()-1 );
    #endif
}

bool OListView::fullWidth() const
{
    return m_fullWidth;
}

int OListView::addColumn( const QString& label, int width )
{
    int result = QListView::addColumn( label, width );
    #if QT_VERSION > 290
    if (m_fullWidth) {
        header()->setStretchEnabled( false, columns()-2 );
        header()->setStretchEnabled( true, columns()-1  );
    }
    #endif
    return result;
}

int OListView::addColumn( const QIconSet& iconset, const QString& label, int width )
{
    int result = QListView::addColumn( iconset, label, width );
    #if QT_VERSION > 290
    if (m_fullWidth) {
        header()->setStretchEnabled( false, columns()-2 );
        header()->setStretchEnabled( true, columns()-1 );
    }
    #endif
    return result;
}

void OListView::removeColumn( int index )
{
    QListView::removeColumn(index);
    #if QT_VERSION > 290
    if ( m_fullWidth && index == columns() )
    {
        header()->setStretchEnabled( true, columns()-1 );
    }
    #endif
}

const QColor& OListView::alternateBackground() const
{
    return m_alternateBackground;
}

void OListView::setAlternateBackground( const QColor &c )
{
    m_alternateBackground = c;
    repaint();
}

const QPen& OListView::columnSeparator() const
{
    return m_columnSeparator;
}

void OListView::setColumnSeparator( const QPen& p )
{
    m_columnSeparator = p;
    repaint();
}

//****************************** OListViewItem ***********************************************************************

OListViewItem::OListViewItem(QListView *parent)
  : QListViewItem(parent)
{
    init();
}

OListViewItem::OListViewItem(QListViewItem *parent)
  : QListViewItem(parent)
{
    init();
}

OListViewItem::OListViewItem(QListView *parent, QListViewItem *after)
  : QListViewItem(parent, after)
{
    init();
}

OListViewItem::OListViewItem(QListViewItem *parent, QListViewItem *after)
  : QListViewItem(parent, after)
{
    init();
}

OListViewItem::OListViewItem(QListView *parent,
    QString label1, QString label2, QString label3, QString label4,
    QString label5, QString label6, QString label7, QString label8)
  : QListViewItem(parent, label1, label2, label3, label4, label5, label6, label7, label8)
{
    init();
}

OListViewItem::OListViewItem(QListViewItem *parent,
    QString label1, QString label2, QString label3, QString label4,
    QString label5, QString label6, QString label7, QString label8)
  : QListViewItem(parent, label1, label2, label3, label4, label5, label6, label7, label8)
{
    init();
}

OListViewItem::OListViewItem(QListView *parent, QListViewItem *after,
    QString label1, QString label2, QString label3, QString label4,
    QString label5, QString label6, QString label7, QString label8)
  : QListViewItem(parent, after, label1, label2, label3, label4, label5, label6, label7, label8)
{
    init();
}

OListViewItem::OListViewItem(QListViewItem *parent, QListViewItem *after,
    QString label1, QString label2, QString label3, QString label4,
    QString label5, QString label6, QString label7, QString label8)
  : QListViewItem(parent, after, label1, label2, label3, label4, label5, label6, label7, label8)
{
    init();
}

OListViewItem::~OListViewItem()
{
}

void OListViewItem::init()
{
    m_known = false;
}

const QColor &OListViewItem::backgroundColor()
{
    return isAlternate() ? static_cast<OListView*>(listView())->alternateBackground() :
                           listView()->viewport()->colorGroup().base();
}

bool OListViewItem::isAlternate()
{
    OListView *lv = static_cast<OListView*>( listView() );

    // check if the item above is an OListViewItem
    OListViewItem *above = static_cast<OListViewItem*>( itemAbove() );
    /*if (not itemAbove()->inherits( "OListViewItem" )) return false;*/

    // check if we have a valid alternate background color
    if (not (lv && lv->alternateBackground().isValid())) return false;

    m_known = above ? above->m_known : true;
    if (m_known)
    {
       m_odd = above ? !above->m_odd : false;
    }
    else
    {
        OListViewItem *item;
        bool previous = true;
        if (parent())
        {
            item = static_cast<OListViewItem *>(parent());
            if ( item /*&& item->inherits( "OListViewItem" )*/ ) previous = item->m_odd;
            item = static_cast<OListViewItem *>(parent()->firstChild());
            /* if !( item.inherits( "OListViewItem" ) item = 0; */
        }
        else
        {
            item = static_cast<OListViewItem *>(lv->firstChild());
        }

        while(item)
        {
            item->m_odd = previous = !previous;
            item->m_known = true;
            item = static_cast<OListViewItem *>(item->nextSibling());
            /* if !(item.inherits( "OListViewItem" ) ) break; */
        }
    }
    return m_odd;
}

void OListViewItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
{
    QColorGroup _cg = cg;
    const QPixmap *pm = listView()->viewport()->backgroundPixmap();
    if (pm && !pm->isNull())
    {
        _cg.setBrush(QColorGroup::Base, QBrush(backgroundColor(), *pm));
        p->setBrushOrigin( -listView()->contentsX(), -listView()->contentsY() );
    }
    else if ( isAlternate() )
    {
        _cg.setColor( QColorGroup::Base, static_cast<OListView*>( listView() )->alternateBackground() );
    }
    QListViewItem::paintCell(p, _cg, column, width, alignment);

    //FIXME: Use styling here?

    const QPen& pen = static_cast<OListView*>( listView() )->columnSeparator();
    p->setPen( pen );
    p->drawLine( width-1, 0, width-1, height() );
}
