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

/* OPIE */

#include <opie2/odebug.h>
#include <opie2/oversatileview.h>
#include <opie2/oversatileviewitem.h>
#include <opie2/olistview.h>

/* QT */

#include <qaction.h>
#include <qpopupmenu.h>

/* XPM */
static const char * view_icon_xpm[] = {
"16 16 16 1",
" 	c None",
".	c #87BD88",
"+	c #8BBE8B",
"@	c #81BA81",
"#	c #6DAF6D",
"$	c #87BD87",
"%	c #FCFDFC",
"&	c #AED0AE",
"*	c #4E9C4C",
"=	c #91BD91",
"-	c #72B172",
";	c #448643",
">	c #519F50",
",	c #499247",
"'	c #356A35",
")	c #686868",
"                ",
"  .+@#    .+@#  ",
"  $%&*    $%&*  ",
"  @=-;    @=-;  ",
"  #>,'    #>,'  ",
"                ",
" ))))))  )))))) ",
"                ",
"                ",
"  .+@#    .+@#  ",
"  $%&*    $%&*  ",
"  @=-;    @=-;  ",
"  #>,'    #>,'  ",
"                ",
" ))))))  )))))) ",
"                "};

/* XPM */
static const char * view_tree_xpm[] = {
"16 16 17 1",
" 	c None",
".	c #3A3A3A",
"+	c #87BD88",
"@	c #8BBE8B",
"#	c #81BA81",
"$	c #6DAF6D",
"%	c #87BD87",
"&	c #FCFDFC",
"*	c #AED0AE",
"=	c #4E9C4C",
"-	c #91BD91",
";	c #72B172",
">	c #448643",
",	c #686868",
"'	c #519F50",
")	c #499247",
"!	c #356A35",
" .              ",
" .              ",
" .  +@#$        ",
" .  %&*=        ",
" .. #-;>  ,, ,,,",
" .  $')!        ",
" .              ",
" .              ",
" .              ",
" .  +@#$        ",
" .  %&*=        ",
" .. #-;>  ,, ,,,",
"    $')!        ",
"                ",
"                ",
"                "};

OVersatileView::OVersatileView( QWidget* parent, const char* name, int mode )
               :QWidgetStack( parent, name ),
                _viewmode( mode ), _warningpolicy( None ),
                _treeleaf(), _treeopened(), _treeclosed(),
                _iconleaf(), _iconopened(), _iconclosed()
{
    //
    // Create child widgets and set some reasonable default styles
    //

    _listview = new OListView( this, "oversatileview embedded listview" );
    _iconview = new QIconView( this, "oversatileview embedded iconview" );

    _listview->setAllColumnsShowFocus( true );
    _listview->setRootIsDecorated( true );
    _listview->setShowSortIndicator( true );
    _iconview->setGridX( 90 );
    _iconview->setGridY( 42 );
    _iconview->setAutoArrange( true );
    
    #ifdef QWS  // TODO: Let this depend on current geometry (rotation)
    _iconview->setArrangement( QIconView::TopToBottom );
    #else
    _iconview->setArrangement( QIconView::LeftToRight );
    #endif
    
    _iconview->setResizeMode( QIconView::Adjust );
    
    // qt-embedded: map stylus right on hold to right button press

    #ifdef QWS
    ( (QPEApplication*) qApp)->setStylusOperation( _iconview->viewport(), QPEApplication::RightOnHold );
    ( (QPEApplication*) qApp)->setStylusOperation( _listview->viewport(), QPEApplication::RightOnHold );
    #endif

    setViewMode( mode ); // TODO: Read last style from config
    // setSynchronization( true ); // TODO: Implement this

    // create context menu allowing to switch between the views

    _contextmenu = new QPopupMenu( 0, "oversatileview contextmenu" );
    _contextmenu->setCaption( "Style" );
    _contextmenu->setCheckable( true );
    QActionGroup* ag = new QActionGroup( _contextmenu, "style option group" );
    QAction* a1 = new QAction( "View Items in Icon Style", QIconSet( QPixmap( view_icon_xpm ) ),
                               "View Icons", 0, ag, "viewicon action", true );
    QAction* a2 = new QAction( "View Items in Tree Style", QIconSet( QPixmap( view_tree_xpm ) ),
                                "View Tree", 0, ag, "viewtree action", true );
    ag->addTo( _contextmenu );
    if ( mode == Icons )
        a1->setOn( true );
    else if ( mode == Tree )
        a2->setOn( true );
    connect( a1, SIGNAL( activated() ), this, SLOT( setIconViewMode() ) );
    connect( a2, SIGNAL( activated() ), this, SLOT( setTreeViewMode() ) );
    
    #if (QT_VERSION >= 0x030000)
    connect( _listview, SIGNAL( contextMenuRequested(QListViewItem*,const QPoint&,int) ), this, SLOT( contextMenuRequested(QListViewItem*,const QPoint&,int) ) );
    connect( _iconview, SIGNAL( contextMenuRequested(QIconViewItem*,const QPoint&) ), this, SLOT( contextMenuRequested(QIconViewItem*,const QPoint&) ) );
    #else
    connect( _listview, SIGNAL( rightButtonPressed(QListViewItem*,const QPoint&,int) ), this, SLOT( contextMenuRequested(QListViewItem*,const QPoint&,int) ) );
    connect( _iconview, SIGNAL( rightButtonPressed(QIconViewItem*,const QPoint&) ), this, SLOT( contextMenuRequested(QIconViewItem*,const QPoint&) ) );
    #endif
        
    //
    // signal forwarders
    //
    // unfortunately we can't short-circuit all the QListView and QIconView signals
    // to OVersatileView signals, because the signal/slot mechanism doesn't allow
    // type-conversion :-(

    // common signals for listview
    
    connect( _listview, SIGNAL( selectionChanged() ), this, SIGNAL( selectionChanged() ) );
    connect( _listview, SIGNAL( selectionChanged(QListViewItem*) ), this, SLOT( selectionChanged(QListViewItem*) ) );
    connect( _listview, SIGNAL( currentChanged(QListViewItem*) ), this, SLOT( currentChanged(QListViewItem*) ) );   
    connect( _listview, SIGNAL( clicked(QListViewItem*) ), this, SLOT( clicked(QListViewItem*) ) );
    connect( _listview, SIGNAL( pressed(QListViewItem*) ), this, SLOT( pressed(QListViewItem*) ) );
    
    connect( _listview, SIGNAL( doubleClicked(QListViewItem*) ), this, SLOT( doubleClicked(QListViewItem*) ) );
    connect( _listview, SIGNAL( returnPressed(QListViewItem*) ), this, SLOT( returnPressed(QListViewItem*) ) );
    
    connect( _listview, SIGNAL( onItem(QListViewItem*) ), this, SLOT( onItem(QListViewItem*) ) );
    connect( _listview, SIGNAL( onViewport() ), this, SIGNAL( onViewport() ) );

    // common signals for iconview
    
    connect( _iconview, SIGNAL( selectionChanged() ), this, SIGNAL( selectionChanged() ) );
    connect( _iconview, SIGNAL( selectionChanged(QIconViewItem*) ), this, SLOT( selectionChanged(QIconViewItem*) ) );
    connect( _iconview, SIGNAL( currentChanged(QIconViewItem*) ), this, SLOT( currentChanged(QIconViewItem*) ) );   
    connect( _iconview, SIGNAL( clicked(QIconViewItem*) ), this, SLOT( clicked(QIconViewItem*) ) );
    connect( _iconview, SIGNAL( pressed(QIconViewItem*) ), this, SLOT( pressed(QIconViewItem*) ) );
    
    connect( _iconview, SIGNAL( doubleClicked(QIconViewItem*) ), this, SLOT( doubleClicked(QIconViewItem*) ) );
    connect( _iconview, SIGNAL( returnPressed(QIconViewItem*) ), this, SLOT( returnPressed(QIconViewItem*) ) );
    
    connect( _iconview, SIGNAL( onItem(QIconViewItem*) ), this, SLOT( onItem(QIconViewItem*) ) );
    connect( _iconview, SIGNAL( onViewport() ), this, SIGNAL( onViewport() ) );
    
    // listview only signals
    
    connect( _listview, SIGNAL( expanded(QListViewItem*) ), this, SLOT( expanded(QListViewItem*) ) );
    connect( _listview, SIGNAL( collapsed(QListViewItem*) ), this, SLOT( collapsed(QListViewItem*) ) );
    
    // iconview only signals
    
    connect( _iconview, SIGNAL( moved() ), this, SIGNAL( moved() ) );
}

OVersatileView::~OVersatileView()
{
}

QPopupMenu* OVersatileView::contextMenu() const
{
    return _contextmenu;
}

void OVersatileView::contextMenuRequested( QListViewItem* item, const QPoint& pos, int col )
{
    // can't use QObject::inherits here, because ListViewItems, beit Q, O or K,
    // do not inherit from QObject - assuming here the programmer is
    // disciplined enough to only add OVersatileViewItems to an OVersatileView
    popupContextMenu( static_cast<OVersatileViewItem*>( item ), pos, col );
}

void OVersatileView::contextMenuRequested( QIconViewItem* item, const QPoint& pos )
{
    // see above
    popupContextMenu( static_cast<OVersatileViewItem*>( item ), pos, -1 );
}

void OVersatileView::popupContextMenu( OVersatileViewItem* item, const QPoint& pos, int col )
{
    if ( !item )
        _contextmenu->exec( pos );
    else
        emit( contextMenuRequested( item, pos, col ) );
}

void OVersatileView::setSynchronization( bool sync )
{
    _synchronization = sync;
}

bool OVersatileView::synchronization()
{
    return _synchronization;
}

void OVersatileView::setDefaultPixmaps( int mode, QPixmap& leaf, QPixmap& opened, QPixmap& closed )
{
    if ( mode == Tree )
    {
        _treeleaf = leaf;
        _treeopened = opened;
        _treeclosed = closed;
    }
    else if ( mode == Icons )
    {
        _iconleaf = leaf;
        _iconopened = opened;
        _iconclosed = closed;
    }
    else
    {
        odebug << "OVersatileView::setDefaultPixmaps(): invalid mode" << oendl;
    }
}

QIconView* OVersatileView::iconView() const
{
    return _iconview;
}

OListView* OVersatileView::listView() const
{
    return _listview;
}

void OVersatileView::setViewMode( int mode )
{
    if ( mode == Tree )
    {
        _viewmode = mode;
        raiseWidget( _listview );
    }
    else if ( mode == Icons )
    {
        _viewmode = mode;
        raiseWidget( _iconview );
    }
    else
    {
        odebug << "OVersatileView::setViewMode(): invalid mode" << oendl;
    }
}

void OVersatileView::setIconViewMode()
{
    setViewMode( Icons );
}

void OVersatileView::setTreeViewMode()
{
    setViewMode( Tree );
}

bool OVersatileView::isValidViewMode( int mode ) const
{
    switch ( _warningpolicy )
    {
        case OVersatileView::None:
        {
            return true;
        }
        case OVersatileView::Warn:
        {
            if ( _viewmode != mode )
            {
                odebug << "OVersatileView::isValidViewMode(): Requested operation not valid in current mode." << oendl;
                return true;
            }
        }
        case OVersatileView::WarnReturn:
        {
            if ( _viewmode != mode )
            {
                odebug << "OVersatileView::isValidViewMode(): Requested operation not valid in current mode." << oendl;
                return false;
            }
        }
        default:
        {
            owarn << "OVersatileView::isValidViewMode(): Inconsistent object state!" << oendl;
            return true;
        }
    }
}
void OVersatileView::setWarningPolicy( int policy ) const
{
    _warningpolicy = policy;
}
bool OVersatileView::warningPolicy() const
{
    return _warningpolicy;
}
//==============================================================================================//
// Stupid Signal forwarders...
// Folks, this is why I like python with its dynamic typing:
// I can code the following dozens of lines C++ in four Python lines...
//==============================================================================================//

void OVersatileView::selectionChanged( QListViewItem * item )
{
    emit( selectionChanged( static_cast<OVersatileViewItem*>( item ) ) );
}

void OVersatileView::selectionChanged( QIconViewItem * item )
{
    emit( selectionChanged( static_cast<OVersatileViewItem*>( item ) ) );
}

void OVersatileView::currentChanged( QListViewItem * item )
{
    emit( currentChanged( static_cast<OVersatileViewItem*>( item ) ) );
}

void OVersatileView::currentChanged( QIconViewItem * item )
{
    emit( currentChanged( static_cast<OVersatileViewItem*>( item ) ) );
}

void OVersatileView::clicked( QListViewItem * item )
{
    emit( clicked( static_cast<OVersatileViewItem*>( item ) ) );
}

void OVersatileView::clicked( QIconViewItem * item )
{
    emit( clicked( static_cast<OVersatileViewItem*>( item ) ) );
}

void OVersatileView::pressed( QListViewItem * item )
{
    emit( pressed( static_cast<OVersatileViewItem*>( item ) ) );
}

void OVersatileView::pressed( QIconViewItem * item )
{
    emit( pressed( static_cast<OVersatileViewItem*>( item ) ) );
}

void OVersatileView::doubleClicked( QListViewItem * item )
{
    emit( doubleClicked( static_cast<OVersatileViewItem*>( item ) ) );
}

void OVersatileView::doubleClicked( QIconViewItem * item )
{
    emit( doubleClicked( static_cast<OVersatileViewItem*>( item ) ) );
}

void OVersatileView::returnPressed( QListViewItem * item )
{
    emit( returnPressed( static_cast<OVersatileViewItem*>( item ) ) );
}
  
void OVersatileView::returnPressed( QIconViewItem * item )
{
    emit( returnPressed( static_cast<OVersatileViewItem*>( item ) ) );
}

void OVersatileView::onItem( QListViewItem * item )
{
    emit( onItem( static_cast<OVersatileViewItem*>( item ) ) );
}

void OVersatileView::onItem( QIconViewItem * item )
{
    emit( onItem( static_cast<OVersatileViewItem*>( item ) ) );
}

void OVersatileView::expanded( QListViewItem *item ) // QListView
{
    //odebug << "OVersatileView::expanded(): opening tree..." << oendl;
    if ( !_treeopened.isNull() )
        item->setPixmap( 0, _treeopened );
    emit( expanded( static_cast<OVersatileViewItem*>( item ) ) );
}
void OVersatileView::collapsed( QListViewItem *item ) // QListView
{
    if ( !_treeclosed.isNull() )
        item->setPixmap( 0, _treeclosed );
    emit( collapsed( static_cast<OVersatileViewItem*>( item ) ) );
}

//=============================================================================================//
// OVersatileView Case I - API only existing in QListView or QIconView but not in both!
//==============================================================================================//
    
int OVersatileView::treeStepSize() const  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return -1;
    }
    return _listview->treeStepSize();
}
 void OVersatileView::setTreeStepSize( int size )  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return;
    }
    _listview->setTreeStepSize( size );
}

QHeader * OVersatileView::header() const  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return 0;
    }
    return _listview->header();
}

 int OVersatileView::addColumn( const QString &label, int size )  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return -1;
    }
    return _listview->addColumn( label, size );
}

 int OVersatileView::addColumn( const QIconSet& iconset, const QString &label, int size )  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return -1;
    }
    return _listview->addColumn( iconset, label, size );
}

void OVersatileView::removeColumn( int index )  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return;
    }
    _listview->removeColumn( index );
}
 void OVersatileView::setColumnText( int column, const QString &label )  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return;
    }
    _listview->setColumnText( column, label );
}
 void OVersatileView::setColumnText( int column, const QIconSet& iconset, const QString &label )  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return;
    }
    _listview->setColumnText( column, iconset, label );
}
QString OVersatileView::columnText( int column ) const  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return QString::null;
    }
    return _listview->columnText( column );
}
 void OVersatileView::setColumnWidth( int column, int width )  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return;
    }
    _listview->setColumnWidth( column, width );
}
int OVersatileView::columnWidth( int column ) const  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return -1;
    }
    return _listview->columnWidth( column );
}
 void OVersatileView::setColumnWidthMode( int column, WidthMode mode )  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return;
    }
    _listview->setColumnWidth( column, mode );
}
int OVersatileView::columns() const  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return -1;
    }
    return _listview->columns();
}

 void OVersatileView::setColumnAlignment( int column, int align )  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return;
    }
    _listview->setColumnAlignment( column, align );
}
int OVersatileView::columnAlignment( int column ) const  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return -1;
    }
    return _listview->columnAlignment( column );
}

OVersatileViewItem * OVersatileView::itemAt( const QPoint & screenPos ) const  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return 0;
    }
    return static_cast<OVersatileViewItem*>( _listview->itemAt( screenPos ) );
}
QRect OVersatileView::itemRect( const OVersatileViewItem * item ) const  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return QRect( -1, -1, -1, -1 );
    }
    return _listview->itemRect( item );
}
int OVersatileView::itemPos( const OVersatileViewItem * item )  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return -1;
    }
    return _listview->itemPos( item );
}

bool OVersatileView::isSelected( const OVersatileViewItem * item ) const  // QListView  // also in QIconViewItem but !in QIconView *shrug*
{
    if ( !isValidViewMode( Tree ) )
    {
        return false;
    }
    return _listview->isSelected( item );
}

 void OVersatileView::setMultiSelection( bool enable )
{
    _listview->setMultiSelection( enable );
}
bool OVersatileView::isMultiSelection() const
{
    return _listview->isMultiSelection();
}

OVersatileViewItem * OVersatileView::selectedItem() const  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return 0;
    }
    return static_cast<OVersatileViewItem*>( _listview->selectedItem() );
}
 void OVersatileView::setOpen( OVersatileViewItem * item, bool open )  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return;
    }
    _listview->setOpen( item, open );
}
bool OVersatileView::isOpen( const OVersatileViewItem * item ) const  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return false;
    }
    return _listview->isOpen( item );
}

OVersatileViewItem * OVersatileView::firstChild() const  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return 0;
    }
    return static_cast<OVersatileViewItem*>( _listview->firstChild() );
}
int OVersatileView::childCount() const  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return -1;
    }
    return _listview->childCount();
}

 void OVersatileView::setAllColumnsShowFocus( bool focus )  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return;
    }
    _listview->setAllColumnsShowFocus( focus );
}
bool OVersatileView::allColumnsShowFocus() const  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return false;
    }
    return _listview->allColumnsShowFocus();
}

 void OVersatileView::setItemMargin( int margin )  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return;
    }
   _listview->setItemMargin( margin );
}
int OVersatileView::itemMargin() const  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return -1;
    }
    return _listview->itemMargin();
}

 void OVersatileView::setRootIsDecorated( bool decorate )  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return;
    }
    _listview->setRootIsDecorated( decorate );
}
bool OVersatileView::rootIsDecorated() const  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return false;
    }
    return _listview->rootIsDecorated();
}

void OVersatileView::setShowSortIndicator( bool show )  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return;
    }
    _listview->setShowSortIndicator( show );
}
bool OVersatileView::showSortIndicator() const  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return false;
    }
    return _listview->showSortIndicator();
}

void OVersatileView::triggerUpdate()  // QListView
{
    if ( !isValidViewMode( Tree ) )
    {
        return;
    }
    _listview->triggerUpdate();
}

//
// only in QIconView
//
    
uint OVersatileView::count() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return 0;
    }
    return _iconview->count();
}

int OVersatileView::index( const OVersatileViewItem *item ) const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return -1;
    }
    return _iconview->index( item );
}

OVersatileViewItem* OVersatileView::firstItem() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return 0;
    }
    return static_cast<OVersatileViewItem*>( _iconview->firstItem() );
}
OVersatileViewItem* OVersatileView::lastItem() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return 0;
    }
    return static_cast<OVersatileViewItem*>( _iconview->lastItem() );
}

OVersatileViewItem* OVersatileView::findItem( const QPoint &pos ) const // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return 0;
    }
    return static_cast<OVersatileViewItem*>( _iconview->findItem( pos ) );
}
OVersatileViewItem* OVersatileView::findItem( const QString &text ) const // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return 0;
    }
    return static_cast<OVersatileViewItem*>( _iconview->findItem( text ) );
}

OVersatileViewItem* OVersatileView::findFirstVisibleItem( const QRect &r ) const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return 0;
    }
    return static_cast<OVersatileViewItem*>( _iconview->findFirstVisibleItem( r ) );
}
OVersatileViewItem* OVersatileView::findLastVisibleItem( const QRect &r ) const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return 0;
    }
    return static_cast<OVersatileViewItem*>( _iconview->findLastVisibleItem( r ) );
}

 void OVersatileView::setGridX( int rx )  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return;
    }
    _iconview->setGridX( rx );
}
 void OVersatileView::setGridY( int ry )  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return;
    }
    _iconview->setGridY( ry );
}
int OVersatileView::gridX() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return -1;
    }
    return _iconview->gridX();
}
int OVersatileView::gridY() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return -1;
    }
    return _iconview->gridY();
}
 void OVersatileView::setSpacing( int sp )  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return;
    }
    _iconview->setSpacing( sp );
}
int OVersatileView::spacing() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return -1;
    }
    return _iconview->spacing();
}
 void OVersatileView::setItemTextPos( QIconView::ItemTextPos pos )  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return;
    }
    _iconview->setItemTextPos( pos );
}
QIconView::ItemTextPos OVersatileView::itemTextPos() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return (QIconView::ItemTextPos) -1;
    }
    return _iconview->itemTextPos();
}
 void OVersatileView::setItemTextBackground( const QBrush &b )  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return;
    }
    _iconview->setItemTextBackground( b );
}
QBrush OVersatileView::itemTextBackground() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return QBrush();
    }
    return _iconview->itemTextBackground();
}
 void OVersatileView::setArrangement( QIconView::Arrangement am )  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return;
    }
    _iconview->setArrangement( am );
}
QIconView::Arrangement OVersatileView::arrangement() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return (QIconView::Arrangement) -1;
    }
    return _iconview->arrangement();
}
 void OVersatileView::setResizeMode( QIconView::ResizeMode am )  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return;
    }
    _iconview->setResizeMode( am );
}
QIconView::ResizeMode OVersatileView::resizeMode() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return (QIconView::ResizeMode) -1;
    }
    return _iconview->resizeMode();
}
 void OVersatileView::setMaxItemWidth( int w )  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return;
    }
    _iconview->setMaxItemWidth( w );
}
int OVersatileView::maxItemWidth() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return -1;
    }
    return _iconview->maxItemWidth();
}
 void OVersatileView::setMaxItemTextLength( int w )  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return;
    }
    _iconview->setMaxItemTextLength( w );
}
int OVersatileView::maxItemTextLength() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return -1;
    }
    return _iconview->maxItemTextLength();
}
 void OVersatileView::setAutoArrange( bool b )  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return;
    }
    _iconview->setAutoArrange( b );
}
bool OVersatileView::autoArrange() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return false;
    }
    return _iconview->autoArrange();
}
 void OVersatileView::setShowToolTips( bool b )  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return;
    }
    _iconview->setShowToolTips( b );
}
bool OVersatileView::showToolTips() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return false;
    }
    return _iconview->showToolTips();
}

bool OVersatileView::sorting() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return false;
    }
    return _iconview->sorting();
}
bool OVersatileView::sortDirection() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return false;
    }
    return _iconview->sortDirection();
}

 void OVersatileView::setItemsMovable( bool b )  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return;
    }
    _iconview->setItemsMovable( b );
}
bool OVersatileView::itemsMovable() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return false;
    }
    return _iconview->itemsMovable();
}
void OVersatileView::setWordWrapIconText( bool b )  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return;
    }
    _iconview->setWordWrapIconText( b );
}
bool OVersatileView::wordWrapIconText() const  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return false;
    }
    return _iconview->wordWrapIconText();
}

void OVersatileView::arrangeItemsInGrid( const QSize &grid, bool update )  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return;
    }
    _iconview->arrangeItemsInGrid( grid, update );
}
void OVersatileView::arrangeItemsInGrid( bool update )  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return;
    }
    _iconview->arrangeItemsInGrid( update );
}
void OVersatileView::updateContents()  // QIconView
{
    if ( !isValidViewMode( Icons ) )
    {
        return;
    }
    _iconview->updateContents();
}

//==============================================================================================//
// OVersatileView Case II - QListView / QIconView common API
//==============================================================================================//

void OVersatileView::clear()
{
    _iconview->clear();
    _listview->clear();
}

void OVersatileView::setFont( const QFont & font )
{
    _iconview->setFont( font );
    _listview->setFont( font );
}
void OVersatileView::setPalette( const QPalette & palette )
{
    _iconview->setPalette( palette );
    _listview->setPalette( palette );
}

void OVersatileView::takeItem( OVersatileViewItem * item )
{
    _iconview->takeItem( item );
    _listview->takeItem( item );
}

void OVersatileView::setSelectionMode( SelectionMode mode )
{
    _iconview->setSelectionMode( (QIconView::SelectionMode) mode );
    _listview->setSelectionMode( (QListView::SelectionMode) mode );
}
OVersatileView::SelectionMode OVersatileView::selectionMode() const
{
    return (OVersatileView::SelectionMode) _iconview->selectionMode();
}

void OVersatileView::selectAll( bool select )
{
    _iconview->selectAll( select );
}
void OVersatileView::clearSelection()
{
    _iconview->clearSelection();
    _listview->clearSelection();
}
void OVersatileView::invertSelection()
{
    _iconview->invertSelection();
    _listview->invertSelection();
}

void OVersatileView::ensureItemVisible( const OVersatileViewItem * item )
{
    _iconview->ensureItemVisible( const_cast<OVersatileViewItem*>( item ) );
    _listview->ensureItemVisible( item );
}
void OVersatileView::repaintItem( const OVersatileViewItem * item ) const
{
    _iconview->repaintItem( const_cast<OVersatileViewItem*>( item ) );
    _listview->repaintItem( item );
}

void OVersatileView::setCurrentItem( OVersatileViewItem * item )
{
    _iconview->setCurrentItem( item );
    _listview->setCurrentItem( item );
}
OVersatileViewItem * OVersatileView::currentItem() const
{
    return static_cast<OVersatileViewItem*>( _listview->currentItem() );
}

// bool eventFilter( QObject * o, QEvent * )   // use QWidgetStack implementation

// QSize minimumSizeHint() const               // use QWidgetStack implementation
// QSizePolicy sizePolicy() const              // use QWidgetStack implementation
// QSize sizeHint() const                      // use QWidgetStack implementation

//==============================================================================================//
// OVersatileView Case III - APIs which differ slightly
//==============================================================================================//

/*

 void OVersatileView::insertItem( OVersatileViewItem * )  // QListView
 void OVersatileView::insertItem( OVersatileViewItem *item, OVersatileViewItem *after = 0L )  // QIconView

 void OVersatileView::setSelected( OVersatileViewItem *, bool )  // QListView
 void OVersatileView::setSelected( OVersatileViewItem *item, bool s, bool cb = FALSE )  // QIconView

 void OVersatileView::setSorting( int column, bool increasing = TRUE )  // QListView
void OVersatileView::setSorting( bool sort, bool ascending = TRUE )  // QIconView

void OVersatileView::sort() // #### make  in next major release  // QListView
 void OVersatileView::sort( bool ascending = TRUE )  // QIconView

*/


