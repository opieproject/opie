/*
                             This file is part of the Opie Project
              =.             (C) 2003-2004 Michael 'Mickey' Lauer <mickey@vanille.de>
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

#ifndef OLISTVIEW_H
#define OLISTVIEW_H

#include <qcolor.h>
#include <qlistview.h>
#include <qpen.h>
#include <qdatastream.h>
#include <qstringlist.h>


namespace Opie {
namespace Ui   {
class OListViewItem;


/*======================================================================================
 * OListView
 *======================================================================================*/

/**
 * @brief A list/tree widget.
 *
 * A @ref QListView variant featuring visual and functional enhancements
 * like an alternate background for odd rows, an autostretch mode
 * for the width of the widget ( >= Qt 3 only ) and persistence capabilities.
 *
 * @author Michael 'Mickey' Lauer <mickey@vanille.de>
 */
class OListView: public QListView
{
    Q_OBJECT
  public:
   /**
    * Constructor.
    *
    * The parameters @a parent and @a name are handled by
    * @ref QListView, as usual.
    */
    OListView( QWidget* parent = 0, const char* name = 0 );
   /**
    * Destructor.
    */
    virtual ~OListView();
   /**
    * Let the last column fit exactly all the available width.
    */
    void setFullWidth( bool fullWidth );
   /**
    * Returns whether the last column is set to fit the available width.
    */
    bool fullWidth() const;
   /**
    * Reimplemented for full width support
    */
    virtual int addColumn( const QString& label, int width = -1 );
   /**
    * Reimplemented for full width support
    */
    virtual int addColumn( const QIconSet& iconset, const QString& label, int width = -1 );
   /**
    * Reimplemented for full width support
    */
    virtual void removeColumn(int index);
   /**
    * Set the alternate background background @a color.
    * Set to an invalid color to disable alternate colors.
    * This only has an effect if the items are OListViewItems
    */
    void setAlternateBackground( const QColor& color );
   /**
    * Sets the column separator @a pen.
    */
    void setColumnSeparator( const QPen& pen );

   /**
    * @returns the alternate background color
    */
    const QColor& alternateBackground() const;
   /**
    * @return the column separator pen
    */
    const QPen& columnSeparator() const;
   /**
    * Create a list view item as child of this object
    * @returns the new object
    */
    virtual OListViewItem* childFactory();
#ifndef QT_NO_DATASTREAM
   /**
    * Serialize this object to @ref QDataStream @a stream
    */
    virtual void serializeTo( QDataStream& stream ) const;
   /**
    * Serialize this object from a @ref QDataStream @a stream
    */
    virtual void serializeFrom( QDataStream& s );
#endif

  public slots:
    /**
     * Expand all items
     */
     void expand();
    /**
     * Collapse all items
     */
     void collapse();

  protected slots:
    /**
     * expand the current OListViewItem
     */
    void expand(QListViewItem*);

  private:
    QColor m_alternateBackground;
    bool m_fullWidth : 1;
    QPen m_columnSeparator;
    class Private;
    Private *d;
};

#ifndef QT_NO_DATASTREAM
/**
  * @relates OListView
  * Writes @a listview to the @a stream and returns a reference to the stream.
  */
QDataStream& operator<<( QDataStream& stream, const OListView& listview );
/**
  * @relates OListView
  * Reads @a listview from the @a stream and returns a reference to the stream.
  */
QDataStream& operator>>( QDataStream& stream, OListView& listview );
#endif // QT_NO_DATASTREAM

/*======================================================================================
 * OListViewItem
 *======================================================================================*/

class OListViewItem: public QListViewItem
{
    friend class OCheckListItem;
  public:
    /**
     * Constructors.
     */
    OListViewItem( QListView * parent );
    OListViewItem( QListViewItem * parent );
    OListViewItem( QListView * parent, QListViewItem * after );
    OListViewItem( QListViewItem * parent, QListViewItem * after );

    OListViewItem( QListView * parent,
        QString,     QString = QString::null,
        QString = QString::null, QString = QString::null,
        QString = QString::null, QString = QString::null,
        QString = QString::null, QString = QString::null );

    OListViewItem( QListViewItem * parent,
        QString,     QString = QString::null,
        QString = QString::null, QString = QString::null,
        QString = QString::null, QString = QString::null,
        QString = QString::null, QString = QString::null );

    OListViewItem( QListView * parent, QListViewItem * after,
        QString,     QString = QString::null,
        QString = QString::null, QString = QString::null,
        QString = QString::null, QString = QString::null,
        QString = QString::null, QString = QString::null );

    OListViewItem( QListViewItem * parent, QListViewItem * after,
        QString,     QString = QString::null,
        QString = QString::null, QString = QString::null,
        QString = QString::null, QString = QString::null,
        QString = QString::null, QString = QString::null );
    /**
     * Destructor.
     */
    virtual ~OListViewItem();
    /**
     * @returns the background color of the list item.
     */
    const QColor& backgroundColor();
    /**
     * @returns true, if the item is at an odd position and
     * thus have to be painted with the alternate background color.
     */
    bool isAlternate();
    /**
     * @note: Reimplemented for internal purposes - the API is not affected
     *
     */
    void paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int alignment );
    /**
     * Perform object initialization.
     */
    void init();
   /**
    * create a list view item as child of this object
    * @returns the new object
    */
    virtual OListViewItem* childFactory();
    #ifndef QT_NO_DATASTREAM
   /**
    * serialize this object to or from a @ref QDataStream
    * @param s the stream used to serialize this object.
    */
    virtual void serializeTo( QDataStream& s ) const;

   /**
    * serialize this object to or from a @ref QDataStream
    * @param s the stream used to serialize this object.
    */
    virtual void serializeFrom( QDataStream& s );
    #endif

    /**
     * expand the the item
     */
    virtual void expand(){};

  private:
    bool m_known : 1;
    bool m_odd   : 1;
    class Private;
    Private *d;
};

#ifndef QT_NO_DATASTREAM
/**
  * @relates QListViewItem
  * Writes listview @a item and all subitems recursively to @a stream
  * and returns a reference to the stream.
  */
QDataStream& operator<<( QDataStream& stream, const OListViewItem& item );
/**
  * @relates QListViewItem
  * Reads listview @a item from @a stream and returns a reference to the stream.
  */
QDataStream& operator>>( QDataStream& stream, OListViewItem& item );
#endif // QT_NO_DATASTREAM


/*======================================================================================
 * OCheckListItem
 *======================================================================================*/

class OCheckListItem : public QCheckListItem
{
  public:

    OCheckListItem( QCheckListItem *parent, const QString &text,
                    Type = Controller );
    OCheckListItem( QListViewItem *parent, const QString &text,
                    Type = Controller );
    OCheckListItem( QListView *parent, const QString &text,
                    Type = Controller );
    OCheckListItem( QListViewItem *parent, const QString &text,
                    const QPixmap & );
    OCheckListItem( QListView *parent, const QString &text,
                    const QPixmap & );
    ~OCheckListItem();
    /**
     * @returns the background color of the list item.
     */
    const QColor& backgroundColor();
    /**
     * @returns true, if the item is at an odd position and
     * thus have to be painted with the alternate background color.
     */
    bool isAlternate();
    /**
     * @note: Reimplemented for internal purposes - the API is not affected
     *
     */
    void paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int alignment );
    /**
     * Perform object initialization.
     */
    void init();

  private:
    bool m_known;
    bool m_odd;
};


/*======================================================================================
 * ONamedListView
 *======================================================================================*/

class ONamedListViewItem;

/**
 * @brief An OListView variant with named columns.
 *
 * This class provides a higher-level interface to an OListView.
 *
 * @author Michael 'Mickey' Lauer <mickey@vanille.de>
 */
class ONamedListView: public OListView
{
  public:
    /**
     * Constructor.
     *
     * The parameters @a parent and @a name are handled by
     * @ref OListView, as usual.
     */
    ONamedListView( QWidget* parent = 0, const char* name = 0 );
    /**
     * Destructor.
     */
    virtual ~ONamedListView();
    /**
     * Add a number of @a columns to the listview.
     */
    virtual void addColumns( const QStringList& columns );
    /**
     * @returns the column index matching to @a text or -1 if not found.
     */
    virtual int findColumn( const QString& text ) const;
    /**
     * @returns the first item which has a @a text in column @a column.
     * Set @a recurse to indicate how much subchild levels to search, e.g.<ul>
     * <li>set it to 0 to search only among direct childs,
     * <li>set it to 1 to search direct childs and all 1st order subchilds
     * <li>set it to -1 for maximum recursion.
     * </ul>
     * @sa ONamedListViewItem::find()
     */
    virtual ONamedListViewItem* find( ONamedListViewItem* start, int column, const QString& text, int recurse = -1 ) const;
    virtual ONamedListViewItem* find( int column, const QString& text, int recurse = -1 ) const;

    virtual ONamedListViewItem* find( ONamedListViewItem* start, const QString& column, const QString& text, int recurse = -1 ) const;
    virtual ONamedListViewItem* find( const QString& column, const QString& text, int recurse = -1 ) const;
  private:
    class Private;
    Private *d;
};

/*======================================================================================
 * ONamedListViewItem
 *======================================================================================*/

/**
 * @brief An OListView variant with named columns.
 *
 * This class provides a higher-level interface to an OListViewItem.
 *
 * @author Michael 'Mickey' Lauer <mickey@vanille.de>
 */
class ONamedListViewItem: public OListViewItem
{
  public:
    /**
     * Constructor. Accepts the same parameters as a @ref OListViewItem,
     * plus a @ref QStringList which holds an arbitrary number of @a texts.
     */
    ONamedListViewItem( QListView* parent, const QStringList& texts );
    ONamedListViewItem( QListViewItem* parent, const QStringList& texts );
    ONamedListViewItem( QListView* parent, QListViewItem* after, const QStringList& texts );
    ONamedListViewItem( QListViewItem* parent, QListViewItem* after, const QStringList& texts );
    /**
     * Destructor.
     */
    virtual ~ONamedListViewItem();
    /**
     * Sets the text in column @a column to @a text.
     * This method differs from @ref QListViewItem::setText() in that it
     * accepts a string as column indicator instead of an int.
     */
    virtual void setText( const QString& column, const QString& text );
    /**
     * Sets a number of @a texts for this item.
     */
    virtual void setText( const QStringList& texts );
    /**
     * @returns the first child which has a @a text in column @a column.
     * Set @a recurse to indicate how much subchild levels to search, e.g.<ul>
     * <li>set it to 0 to search only among direct childs,
     * <li>set it to 1 to search direct childs and all 1st order subchilds
     * <li>set it to -1 for maximum recursion.
     * </ul>
     * @sa ONamedListView::find()
     */
    virtual ONamedListViewItem* find( int column, const QString& text, int recurse = -1 ) const;
    virtual ONamedListViewItem* find( const QString& column, const QString& text, int recurse = -1 ) const;

  private:
    class Private;
    Private *d;

};
}
}

#endif // OLISTVIEW_H
