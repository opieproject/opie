/*
                             This file is part of the Opie Project
              =.             (C) 2003 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

/* QT */

#include <qcolor.h>
#include <qheader.h>
#include <qpainter.h>
#include <qpixmap.h>

/* OPIE */

#include <opie2/odebug.h>
#include <opie2/olistview.h>

/*======================================================================================
 * OListView
 *======================================================================================*/

OListView::OListView( QWidget *parent, const char *name )
          :QListView( parent, name )
{
    //FIXME: get from global settings and calculate ==> see oglobalsettings.*

    m_alternateBackground = QColor( 238, 246, 255 );
    m_columnSeparator = QPen( QColor( 150, 160, 170 ), 0, DotLine );
    m_fullWidth = true;
    connect( this, SIGNAL(expanded(QListViewItem*)), SLOT(expand(QListViewItem*)));
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

void OListView::expand(QListViewItem *item)
{
 	((OListViewItem*)item)->expand();
}

OListViewItem* OListView::childFactory()
{
    return new OListViewItem( this );
}

#ifndef QT_NO_DATASTREAM
void OListView::serializeTo( QDataStream& s ) const
{
    #warning Caution... the binary format is still under construction...
    odebug << "storing OListView..." << oendl;

    // store number of columns and the labels
    s << columns();
    for ( int i = 0; i < columns(); ++i )
        s << columnText( i );

    // calculate the number of top-level items to serialize
    int items = 0;
    QListViewItem* item = firstChild();
    while ( item )
    {
        item = item->nextSibling();
        items++;
    }

    // store number of items and the items itself
    s << items;
    item = firstChild();
    for ( int i = 0; i < items; ++i )
    {
        s << *static_cast<OListViewItem*>( item );
        item = item->nextSibling();
    }

    odebug << "OListview stored." << oendl;
}

void OListView::serializeFrom( QDataStream& s )
{
    #warning Caution... the binary format is still under construction...
    odebug << "loading OListView..." << oendl;

    int cols;
    s >> cols;
    qDebug( "read number of columns = %d", cols );

    while ( columns() < cols ) addColumn( QString::null );

    for ( int i = 0; i < cols; ++i )
    {
        QString coltext;
        s >> coltext;
        qDebug( "read text '%s' for column %d", (const char*) coltext, i );
        setColumnText( i, coltext );
    }

    int items;
    s >> items;
    qDebug( "read number of items = %d", items );

    for ( int i = 0; i < items; ++i )
    {
        OListViewItem* item = childFactory();
        s >> *item;
    }

    odebug << "OListView loaded." << oendl;

}

QDataStream& operator<<( QDataStream& s, const OListView& lv )
{
    lv.serializeTo( s );
}

QDataStream& operator>>( QDataStream& s, OListView& lv )
{
    lv.serializeFrom( s );
}
#endif // QT_NO_DATASTREAM

/*======================================================================================
 * OListViewItem
 *======================================================================================*/

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
    /*if (! itemAbove()->inherits( "OListViewItem" )) return false;*/

    // check if we have a valid alternate background color
    if (!(lv && lv->alternateBackground().isValid())) return false;

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
            /* if ( !item.inherits( "OListViewItem" ) item = 0; */
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
            /* if (!item.inherits( "OListViewItem" ) ) break; */
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
        _cg.setBrush( QColorGroup::Base, QBrush(backgroundColor(), *pm) );
        p->setBrushOrigin( -listView()->contentsX(), -listView()->contentsY() );
    }
    else if ( isAlternate() )
    {
        _cg.setColor( QColorGroup::Base, static_cast<OListView*>( listView() )->alternateBackground() );
    }
    QListViewItem::paintCell( p, _cg, column, width, alignment );

    //FIXME: Use styling here!

    const QPen& pen = static_cast<OListView*>( listView() )->columnSeparator();
    p->setPen( pen );
    p->drawLine( width-1, 0, width-1, height() );
}


OListViewItem* OListViewItem::childFactory()
{
    return new OListViewItem( this );
}


#ifndef QT_NO_DATASTREAM
void OListViewItem::serializeTo( QDataStream& s ) const
{
    #warning Caution... the binary format is still under construction...
    odebug << "storing OListViewItem..." << oendl;

    // store item text
    for ( int i = 0; i < listView()->columns(); ++i )
    {
        s << text( i );
    }

    // calculate the number of children to serialize
    int items = 0;
    QListViewItem* item = firstChild();
    while ( item )
    {
        item = item->nextSibling();
        items++;
    }

    // store number of items and the items itself
    s << items;
    item = firstChild();
    for ( int i = 0; i < items; ++i )
    {
        s << *static_cast<OListViewItem*>( item );
        item = item->nextSibling();
    }

    odebug << "OListviewItem stored." << oendl;
}


void OListViewItem::serializeFrom( QDataStream& s )
{
    #warning Caution... the binary format is still under construction...
    odebug << "loading OListViewItem..." << oendl;

    for ( int i = 0; i < listView()->columns(); ++i )
    {
        QString coltext;
        s >> coltext;
        qDebug( "read text '%s' for column %d", (const char*) coltext, i );
        setText( i, coltext );
    }

    int items;
    s >> items;
    qDebug( "read number of items = %d", items );

    for ( int i = 0; i < items; ++i )
    {
        OListViewItem* item = childFactory();
        s >> (*item);
    }

    odebug << "OListViewItem loaded." << oendl;
}


QDataStream& operator<<( QDataStream& s, const OListViewItem& lvi )
{
    lvi.serializeTo( s );
}


QDataStream& operator>>( QDataStream& s, OListViewItem& lvi )
{
    lvi.serializeFrom( s );
}
#endif // QT_NO_DATASTREAM


/*======================================================================================
 * ONamedListView
 *======================================================================================*/

ONamedListView::ONamedListView( QWidget *parent, const char *name )
               :OListView( parent, name )
{
}


ONamedListView::~ONamedListView()
{
}


void ONamedListView::addColumns( const QStringList& columns )
{
    for ( QStringList::ConstIterator it = columns.begin(); it != columns.end(); ++it )
    {
        qDebug( "adding column %s", (const char*) *it );
        addColumn( *it );
    }
}


int ONamedListView::findColumn( const QString& text ) const
{
    //FIXME: If used excessively, this will slow down performance of updates
    //FIXME: because of the linear search over all column texts.
    //FIXME: I will optimize later by using a hash map.
    for ( int i = 0; i < columns(); ++i )
        if ( columnText( i ) == text )
            return i;
    return -1;
}


ONamedListViewItem* ONamedListView::find( int column, const QString& text, int recurse ) const
{
    return find( (ONamedListViewItem*) firstChild(), column, text, recurse );
}


ONamedListViewItem* ONamedListView::find( ONamedListViewItem* item, int column, const QString& text, int recurse ) const
{
    ONamedListViewItem* result;
    while ( item && item->text( column ) != text )
    {
        qDebug( "checked %s", (const char*) item->text( column ) );

        if ( recurse < 0 || recurse > 0 )
        {
            qDebug( "recursion is %d - recursing into...", recurse );
            result = find( (ONamedListViewItem*) item->firstChild(), column, text, recurse-1 );
            if ( result ) return result;
        }


        item = (ONamedListViewItem*) item->itemBelow();
    }
    if ( item && item->text( column ) == text )
        return item;
    else
        return 0;
}


ONamedListViewItem* ONamedListView::find( const QString& column, const QString& text, int recurse ) const
{
    int col = findColumn( column );
    if ( col != -1 )
        return find( (ONamedListViewItem*) firstChild(), col, text, recurse );
    else
        return 0;
}


ONamedListViewItem* ONamedListView::find( ONamedListViewItem* item, const QString& column, const QString& text, int recurse ) const
{
    int col = findColumn( column );
    if ( col != -1 )
        return find( item, col, text, recurse );
    else
        return 0;
}


/*======================================================================================
 * ONamedListViewItem
 *======================================================================================*/

ONamedListViewItem::ONamedListViewItem( QListView* parent, const QStringList& texts )
                   :OListViewItem( parent )
{
    setText( texts );
}


ONamedListViewItem::ONamedListViewItem( QListViewItem* parent, const QStringList& texts )
                   :OListViewItem( parent )
{
    setText( texts );
}


ONamedListViewItem::ONamedListViewItem( QListView* parent, QListViewItem* after, const QStringList& texts )
                   :OListViewItem( parent, after )
{
    setText( texts );
}


ONamedListViewItem::ONamedListViewItem( QListViewItem* parent, QListViewItem* after, const QStringList& texts )
                   :OListViewItem( parent, after )
{
    setText( texts );
}


ONamedListViewItem::~ONamedListViewItem()
{
}


void ONamedListViewItem::setText( const QStringList& texts )
{
    int col = 0;
    for ( QStringList::ConstIterator it = texts.begin(); it != texts.end(); ++it )
    {
        qDebug( "setting column %d = text %s", col, (const char*) *it );
        OListViewItem::setText( col++, *it );
    }

}


void ONamedListViewItem::setText( const QString& column, const QString& text )
{
    //FIXME: If used excessively, this will slow down performance of updates
    //FIXME: because of the linear search over all column texts.
    //FIXME: I will optimize later by using a hash map.
    int col = ( (ONamedListView*) listView() )->findColumn( column );
    if ( col != -1 )
        OListViewItem::setText( col, text );
    else
        qWarning( "ONamedListViewItem::setText(): Warning! Columntext '%s' not found.", (const char*) column );
}


ONamedListViewItem* ONamedListViewItem::find( int column, const QString& text, int recurse ) const
{
    return ( (ONamedListView*) listView() )->find( (ONamedListViewItem*) firstChild(), column, text, recurse );
}


ONamedListViewItem* ONamedListViewItem::find( const QString& column, const QString& text, int recurse ) const
{
    int col = ( (ONamedListView*) listView() )->findColumn( column );
    if ( col != -1 )
        return ( (ONamedListView*) listView() )->find( (ONamedListViewItem*) firstChild(), col, text, recurse );
    else
        return 0;
}

