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

#ifndef OVERSATILEVIEW_H
#define OVERSATILEVIEW_H

/* QT */

#include <qwidgetstack.h>
#include <qiconview.h>

/* OPIE */

#include <opie2/oapplication.h>

/* FORWARDS */

class QHeader;
class QIconSet;
class QIconViewItem;
class QListViewItem;
class QPopupMenu;
class QString;

#ifndef QT_NO_DRAGANDDROP
class QIconDragItem;
#endif

namespace Opie {
namespace Ui   {
class OListView;

class OVersatileView : public QWidgetStack
{
  Q_OBJECT

  friend class OVersatileViewItem;

  //==============================================================================================//
  // OVersatileView High Level API
  //==============================================================================================//

  public:
    OVersatileView( QWidget* parent = 0, const char* name = 0, int mode = 0 );
    ~OVersatileView();

    QPopupMenu* contextMenu() const;

    void setSynchronization( bool sync );
    bool synchronization();

    enum ViewMode { Tree = 0, Icons };
    int viewMode();

    QIconView* iconView() const;
    OListView* listView() const;

    enum WarningPolicy { None = 0, Warn, WarnReturn };

    void setWarningPolicy( int ) const;  // warn, if calling a method which doesn't apply to the current viewmode
    bool warningPolicy() const;

    void setDefaultPixmaps( int mode, QPixmap& leaf, QPixmap& opened, QPixmap& closed );

  public slots:
    void setViewMode( int mode );
    void setIconViewMode();
    void setTreeViewMode();

  protected:
    virtual bool isValidViewMode( int mode ) const;
    virtual void popupContextMenu( OVersatileViewItem* item, const QPoint& pos, int col = 0 );

  private:
    int _viewmode;
    bool _synchronization;
    mutable int _warningpolicy;

    OListView* _listview;
    QIconView* _iconview;

    QPixmap _treeleaf;
    QPixmap _treeopened;
    QPixmap _treeclosed;

    QPixmap _iconleaf;
    QPixmap _iconopened;
    QPixmap _iconclosed;

    QPopupMenu* _contextmenu;

    int _iconstyle;
    int _treestyle;

  private slots:

    void contextMenuRequested( QListViewItem*, const QPoint&, int );
    void contextMenuRequested( QIconViewItem*, const QPoint& );

    // type converting signal forwarders

    void selectionChanged( QListViewItem * );
    void currentChanged( QListViewItem * );
    void clicked( QListViewItem * );
    void pressed( QListViewItem * );
    void doubleClicked( QListViewItem * );
    void returnPressed( QListViewItem * );
    void onItem( QListViewItem * );

    void selectionChanged( QIconViewItem * );
    void currentChanged( QIconViewItem * );
    void clicked( QIconViewItem * );
    void pressed( QIconViewItem * );
    void doubleClicked( QIconViewItem * );
    void returnPressed( QIconViewItem * );
    void onItem( QIconViewItem * );

    void expanded( QListViewItem * item ); // QListView
    void collapsed( QListViewItem * item ); // QListView

  signals:

    void contextMenuRequested( OVersatileViewItem * item, const QPoint& pos, int col );

    /*#ifndef QT_NO_DRAGANDDROP
    void dropped( QDropEvent *e, const QValueList<QIconDragItem> &lst );  // QIconView
    #endif
    void itemRenamed( OVersatileViewItem *item, const QString & );  // QIconView
    void itemRenamed( OVersatileViewItem *item );  // QIconView
    */

  //==============================================================================================//
  // "Derived" API - Case 1: Methods existing either only in QListView or only in QIconView
  //==============================================================================================//

public:

    /*
    enum Arrangement {  // QIconView
    LeftToRight = 0,
    TopToBottom
    };
    enum ResizeMode {   // QIconView
    Fixed = 0,
    Adjust
    };
    enum ItemTextPos {  // QIconView
    Bottom = 0,
    Right
    };
    */

    //
    // only in QListView
    //

    int treeStepSize() const;  // QListView
    virtual void setTreeStepSize( int );  // QListView

    QHeader * header() const;  // QListView

    virtual int addColumn( const QString &label, int size = -1);  // QListView
    virtual int addColumn( const QIconSet& iconset, const QString &label, int size = -1);  // QListView
    void removeColumn( int index ); // #### make virtual in next major release!  // QListView
    virtual void setColumnText( int column, const QString &label );  // QListView
    virtual void setColumnText( int column, const QIconSet& iconset, const QString &label );  // QListView
    QString columnText( int column ) const;  // QListView
    virtual void setColumnWidth( int column, int width );  // QListView
    int columnWidth( int column ) const;  // QListView
    enum WidthMode { Manual, Maximum };  // QListView
    virtual void setColumnWidthMode( int column, WidthMode );  // QListView
    WidthMode columnWidthMode( int column ) const;  // QListView
    int columns() const;  // QListView

    virtual void setColumnAlignment( int, int );  // QListView
    int columnAlignment( int ) const;  // QListView

    OVersatileViewItem * itemAt( const QPoint & screenPos ) const;  // QListView
    QRect itemRect( const OVersatileViewItem * ) const;  // QListView
    int itemPos( const OVersatileViewItem * );  // QListView

    bool isSelected( const OVersatileViewItem * ) const;  // QListView  // also in QIconViewItem but not in QIconView *shrug*

    virtual void setMultiSelection( bool enable );  // QListView
    bool isMultiSelection() const;  // QListView

    OVersatileViewItem * selectedItem() const;  // QListView
    virtual void setOpen( OVersatileViewItem *, bool );  // QListView
    bool isOpen( const OVersatileViewItem * ) const;  // QListView

    OVersatileViewItem * firstChild() const;  // QListView
    int childCount() const;  // QListView

    virtual void setAllColumnsShowFocus( bool );  // QListView
    bool allColumnsShowFocus() const;  // QListView

    virtual void setItemMargin( int );  // QListView
    int itemMargin() const;  // QListView

    virtual void setRootIsDecorated( bool );  // QListView
    bool rootIsDecorated() const;  // QListView

    void setShowSortIndicator( bool show );  // QListView
    bool showSortIndicator() const;  // QListView

    int index( const OVersatileViewItem *item ) const;  // QIconView

  public slots:
    void triggerUpdate();  // QListView

  signals:
    void expanded( OVersatileViewItem *item ); // QListView
    void collapsed( OVersatileViewItem *item ); // QListView

    //
    // only in QIconView
    //

  public:
    uint count() const;  // QIconView

    OVersatileViewItem *firstItem() const;  // QIconView
    OVersatileViewItem *lastItem() const;  // QIconView

    OVersatileViewItem *findItem( const QPoint &pos ) const; // QIconView
    OVersatileViewItem *findItem( const QString &text ) const; // QIconView

    OVersatileViewItem* findFirstVisibleItem( const QRect &r ) const;  // QIconView
    OVersatileViewItem* findLastVisibleItem( const QRect &r ) const;  // QIconView

    virtual void setGridX( int rx );  // QIconView
    virtual void setGridY( int ry );  // QIconView
    int gridX() const;  // QIconView
    int gridY() const;  // QIconView
    virtual void setSpacing( int sp );  // QIconView
    int spacing() const;  // QIconView
    virtual void setItemTextPos( QIconView::ItemTextPos pos );  // QIconView
    QIconView::ItemTextPos itemTextPos() const;  // QIconView
    virtual void setItemTextBackground( const QBrush &b );  // QIconView
    QBrush itemTextBackground() const;  // QIconView
    virtual void setArrangement( QIconView::Arrangement am );  // QIconView
    QIconView::Arrangement arrangement() const;  // QIconView
    virtual void setResizeMode( QIconView::ResizeMode am );  // QIconView
    QIconView::ResizeMode resizeMode() const;  // QIconView
    virtual void setMaxItemWidth( int w );  // QIconView
    int maxItemWidth() const;  // QIconView
    virtual void setMaxItemTextLength( int w );  // QIconView
    int maxItemTextLength() const;  // QIconView
    virtual void setAutoArrange( bool b );  // QIconView
    bool autoArrange() const;  // QIconView
    virtual void setShowToolTips( bool b );  // QIconView
    bool showToolTips() const;  // QIconView

    bool sorting() const;  // QIconView
    bool sortDirection() const;  // QIconView

    virtual void setItemsMovable( bool b );  // QIconView
    bool itemsMovable() const;  // QIconView
    virtual void setWordWrapIconText( bool b );  // QIconView
    bool wordWrapIconText() const;  // QIconView

  public slots:
    virtual void arrangeItemsInGrid( const QSize &grid, bool update = TRUE );  // QIconView
    virtual void arrangeItemsInGrid( bool update = TRUE );  // QIconView
    virtual void updateContents();  // QIconView

  signals:
    /*#ifndef QT_NO_DRAGANDDROP
    void dropped( QDropEvent *e, const QValueList<QIconDragItem> &lst );  // QIconView
    #endif
    */
    void moved();  // QIconView
    void itemRenamed( OVersatileViewItem *item, const QString & );  // QIconView
    void itemRenamed( OVersatileViewItem *item );  // QIconView

  //==============================================================================================//
  // "Derived" API - Case 2: Methods existing in QListView and QIconView with the same signatures
  //==============================================================================================//

  public:
    enum SelectionMode {
    Single = 0,
    Multi,
    Extended,
    NoSelection
    };

    virtual void clear();

    virtual void setFont( const QFont & );
    virtual void setPalette( const QPalette & );

    virtual void takeItem( OVersatileViewItem * );

    void setSelectionMode( SelectionMode mode );
    SelectionMode selectionMode() const;

    virtual void selectAll( bool select );
    virtual void clearSelection();
    virtual void invertSelection();

    void ensureItemVisible( const OVersatileViewItem * );
    virtual void repaintItem( const OVersatileViewItem * ) const;

    virtual void setCurrentItem( OVersatileViewItem * );
    OVersatileViewItem * currentItem() const;

    // bool eventFilter( QObject * o, QEvent * );   // use QWidgetStack implementation

    // QSize minimumSizeHint() const;               // use QWidgetStack implementation
    // QSizePolicy sizePolicy() const;              // use QWidgetStack implementation
    // QSize sizeHint() const;                      // use QWidgetStack implementation

  signals:
    void selectionChanged();
    void selectionChanged( OVersatileViewItem * );
    void currentChanged( OVersatileViewItem * );
    void clicked( OVersatileViewItem * );
    void pressed( OVersatileViewItem * );

    void doubleClicked( OVersatileViewItem * );
    void returnPressed( OVersatileViewItem * );

    void onItem( OVersatileViewItem * );
    void onViewport();

  //==============================================================================================//
  // "Derived" API - Case 2: Methods existing in QListView and QIconView with differing signatures
  //==============================================================================================//

  /*

  public:
    virtual void insertItem( OVersatileViewItem * );  // QListView
    virtual void insertItem( OVersatileViewItem *item, OVersatileViewItem *after = 0L );  // QIconView

    virtual void setSelected( OVersatileViewItem *, bool );  // QListView
    virtual void setSelected( OVersatileViewItem *item, bool s, bool cb = FALSE );  // QIconView

    virtual void setSorting( int column, bool increasing = TRUE );  // QListView
    void setSorting( bool sort, bool ascending = TRUE );  // QIconView

    void sort(); // #### make virtual in next major release  // QListView
    virtual void sort( bool ascending = TRUE );  // QIconView

  */

  signals:
    void clicked( OVersatileViewItem *, const QPoint &, int );  // QListView
    void clicked( OVersatileViewItem *, const QPoint & );  // QIconView

    void pressed( OVersatileViewItem *, const QPoint &, int );  // QListView
    void pressed( OVersatileViewItem *, const QPoint & );  // QIconView

    void rightButtonClicked( OVersatileViewItem* item, const QPoint& pos );  // QIconView
    void rightButtonClicked( OVersatileViewItem *, const QPoint&, int );  // QListView

    void rightButtonPressed( OVersatileViewItem* item, const QPoint& pos );  // QIconView
    void rightButtonPressed( OVersatileViewItem *, const QPoint&, int );  // QListView

    void mouseButtonPressed( int, OVersatileViewItem *, const QPoint& , int );  // QListView
    void mouseButtonPressed( int button, OVersatileViewItem* item, const QPoint& pos );  // QIconView

    void mouseButtonClicked( int, OVersatileViewItem *,  const QPoint&, int );  // QListView
    void mouseButtonClicked( int button, OVersatileViewItem* item, const QPoint& pos );  // QIconView

};

}
}
#endif

