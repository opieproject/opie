/*
  This file                  Copyright (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
    is part of the           Copyright (C) 2000 Carsten Pfeiffer <pfeiffer@kde.org>
       Opie Project          Copyright (C) 2000 Dawit Alemayehu <adawit@kde.org>

              =.             Originally part of the KDE Project
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

#include <qclipboard.h>
#include <qlistbox.h>
#include <qpopupmenu.h>

/* OPIE */

#include <opie2/ocompletionbox.h>
#include <opie2/olineedit.h>
#include <opie2/opixmapprovider.h>
#include <opie2/ocombobox.h>

/*======================================================================================
 * OComboBoxPrivate
 *======================================================================================*/

class OComboBox::OComboBoxPrivate
{
public:
    OComboBoxPrivate()
    {
        olineEdit = 0L;
    }
    ~OComboBoxPrivate()
    {
    }

    OLineEdit *olineEdit;
};

/*======================================================================================
 * OComboBox
 *======================================================================================*/

OComboBox::OComboBox( QWidget *parent, const char *name )
    : QComboBox( parent, name )
{
    init();
}

OComboBox::OComboBox( bool rw, QWidget *parent, const char *name )
    : QComboBox( rw, parent, name )
{
    init();

    if ( rw )
    {
        OLineEdit *edit = new OLineEdit( this, "combo lineedit" );
        setLineEdit( edit );
    }
}

OComboBox::~OComboBox()
{
    delete d;
}

void OComboBox::init()
{
    d = new OComboBoxPrivate;

    // Permanently set some parameters in the parent object.
    QComboBox::setAutoCompletion( false );

    // Initialize enable popup menu to false.
    // Below it will be enabled if the widget
    // is editable.
    m_bEnableMenu = false;

    m_trapReturnKey = false;

    // Enable context menu by default if widget
    // is editable.
    setContextMenuEnabled( true );

    // for wheelscrolling
    installEventFilter( this );
    if ( lineEdit() )
        lineEdit()->installEventFilter( this );
}


bool OComboBox::contains( const QString& _text ) const
{
    if ( _text.isEmpty() )
        return false;

    for (int i = 0; i < count(); i++ ) {
        if ( text(i) == _text )
            return true;
    }
    return false;
}

void OComboBox::setAutoCompletion( bool autocomplete )
{
    if ( d->olineEdit )
    {
        if ( autocomplete )
        {
            d->olineEdit->setCompletionMode( OGlobalSettings::CompletionAuto );
            setCompletionMode( OGlobalSettings::CompletionAuto );
        }
        else
        {
            d->olineEdit->setCompletionMode( OGlobalSettings::completionMode() );
            setCompletionMode( OGlobalSettings::completionMode() );
        }
    }
}

void OComboBox::setContextMenuEnabled( bool showMenu )
{
    if( d->olineEdit )
    {
        d->olineEdit->setContextMenuEnabled( showMenu );
        m_bEnableMenu = showMenu;
    }
}

/*
void OComboBox::setURLDropsEnabled( bool enable )
{
    if ( d->olineEdit )
        d->olineEdit->setURLDropsEnabled( enable );
}

bool OComboBox::isURLDropsEnabled() const
{
    return d->olineEdit && d->olineEdit->isURLDropsEnabled();
}
*/

void OComboBox::setCompletedText( const QString& text, bool marked )
{
    if ( d->olineEdit )
        d->olineEdit->setCompletedText( text, marked );
}

void OComboBox::setCompletedText( const QString& text )
{
    if ( d->olineEdit )
        d->olineEdit->setCompletedText( text );
}

void OComboBox::makeCompletion( const QString& text )
{
    if( d->olineEdit )
        d->olineEdit->makeCompletion( text );

    else // read-only combo completion
    {
        if( text.isNull() || !listBox() )
            return;

        int index = listBox()->index( listBox()->findItem( text ) );
        if( index >= 0 ) {
            setCurrentItem( index );
        }
    }
}

void OComboBox::rotateText( OCompletionBase::KeyBindingType type )
{
    if ( d->olineEdit )
        d->olineEdit->rotateText( type );
}

bool OComboBox::eventFilter( QObject* o, QEvent* ev )
{
    QLineEdit *edit = lineEdit();

    int type = ev->type();

    if ( o == edit )
    {
        //OCursor::autoHideEventFilter( edit, ev );

        if ( type == QEvent::KeyPress )
        {
            QKeyEvent *e = static_cast<QKeyEvent *>( ev );

            if ( e->key() == Key_Return || e->key() == Key_Enter)
            {
                // On Return pressed event, emit both
                // returnPressed(const QString&) and returnPressed() signals
                emit returnPressed();
                emit returnPressed( currentText() );
                if ( d->olineEdit && d->olineEdit->completionBox(false) &&
                     d->olineEdit->completionBox()->isVisible() )
                    d->olineEdit->completionBox()->hide();

                return m_trapReturnKey;
            }
        }
    }


    // wheel-scrolling changes the current item
    if ( type == QEvent::Wheel ) {
        if ( !listBox() || listBox()->isHidden() ) {
            QWheelEvent *e = static_cast<QWheelEvent*>( ev );
            static const int WHEEL_DELTA = 120;
            int skipItems = e->delta() / WHEEL_DELTA;
            if ( e->state() & ControlButton ) // fast skipping
                skipItems *= 10;

            int newItem = currentItem() - skipItems;

            if ( newItem < 0 )
                newItem = 0;
            else if ( newItem >= count() )
                newItem = count() -1;

            setCurrentItem( newItem );
            if ( !text( newItem ).isNull() )
                emit activated( text( newItem ) );
            emit activated( newItem );
            e->accept();
            return true;
        }
    }

    return QComboBox::eventFilter( o, ev );
}

void OComboBox::setTrapReturnKey( bool grab )
{
    m_trapReturnKey = grab;
}

bool OComboBox::trapReturnKey() const
{
    return m_trapReturnKey;
}

/*
void OComboBox::setEditURL( const OURL& url )
{
    QComboBox::setEditText( url.prettyURL() );
}

void OComboBox::insertURL( const OURL& url, int index )
{
    QComboBox::insertItem( url.prettyURL(), index );
}

void OComboBox::insertURL( const QPixmap& pixmap, const OURL& url, int index )
{
    QComboBox::insertItem( pixmap, url.prettyURL(), index );
}

void OComboBox::changeURL( const OURL& url, int index )
{
    QComboBox::changeItem( url.prettyURL(), index );
}

void OComboBox::changeURL( const QPixmap& pixmap, const OURL& url, int index )
{
    QComboBox::changeItem( pixmap, url.prettyURL(), index );
}
*/


void OComboBox::setCompletedItems( const QStringList& items )
{
    if ( d->olineEdit )
        d->olineEdit->setCompletedItems( items );
}


OCompletionBox * OComboBox::completionBox( bool create )
{
    if ( d->olineEdit )
        return d->olineEdit->completionBox( create );
    return 0;
}

// QWidget::create() turns off mouse-Tracking which would break auto-hiding
void OComboBox::create( WId id, bool initializeWindow, bool destroyOldWindow )
{
    QComboBox::create( id, initializeWindow, destroyOldWindow );
    //OCursor::setAutoHideCursor( lineEdit(), true, true );
}

void OComboBox::setLineEdit( OLineEdit *edit )
{
    #if QT_VERSION > 290
    QComboBox::setLineEdit( edit );
    if ( !edit->inherits( "OLineEdit" ) )
        d->olineEdit = 0;
    else
        d->olineEdit = static_cast<OLineEdit*>( edit );
    setDelegate( d->olineEdit );

    // forward some signals. We only emit returnPressed() ourselves.
    if ( d->olineEdit ) {
        connect( d->olineEdit, SIGNAL( completion(const QString&)),
                 SIGNAL( completion(const QString&)) );
        connect( d->olineEdit, SIGNAL( substringCompletion(const QString&)),
                 SIGNAL( substringCompletion(const QString&)) );
        connect( d->olineEdit,
                 SIGNAL( textRotation(OCompletionBase::KeyBindingType)),
                 SIGNAL( textRotation(OCompletionBase::KeyBindingType)) );
        connect( d->olineEdit,
                 SIGNAL( completionModeChanged(OGlobalSettings::Completion)),
                 SIGNAL( completionModeChanged(OGlobalSettings::Completion)));

        connect( d->olineEdit,
                 SIGNAL( aboutToShowContextMenu(QPopupMenu*)),
                 SIGNAL( aboutToShowContextMenu(QPopupMenu*)) );
    }
    #else
    #warning OComboBox is not fully functional with Qt2
    #endif
}

// Temporary functions until QT3 appears. - Seth Chaiklin 20 may 2001
void OComboBox::deleteWordForward()
{
    lineEdit()->cursorWordForward(TRUE);
    #if QT_VERSION > 290
    if ( lineEdit()->hasSelectedText() )
    #else
    if ( lineEdit()->hasMarkedText() )
    #endif
    {
        lineEdit()->del();
    }
}

void OComboBox::deleteWordBack()
{
    lineEdit()->cursorWordBackward(TRUE);
    #if QT_VERSION > 290
    if ( lineEdit()->hasSelectedText() )
    #else
    if ( lineEdit()->hasMarkedText() )
    #endif
    {
        lineEdit()->del();
    }
}

void OComboBox::setCurrentItem( const QString& item, bool insert, int index )
{
    int sel = -1;
    for (int i = 0; i < count(); ++i)
        if (text(i) == item)
        {
            sel = i;
            break;
        }
    if (sel == -1 && insert)
    {
        insertItem(item, index);
        if (index >= 0)
            sel = index;
        else
            sel = count() - 1;
    }
    setCurrentItem(sel);
}

void OComboBox::setCurrentItem(int index)
{
    QComboBox::setCurrentItem(index);
}


/*======================================================================================
 * OHistoryCombo
 *======================================================================================*/

// we are always read-write
OHistoryCombo::OHistoryCombo( QWidget *parent, const char *name )
    : OComboBox( true, parent, name )
{
    init( true ); // using completion
}

// we are always read-write
OHistoryCombo::OHistoryCombo( bool useCompletion,
                              QWidget *parent, const char *name )
    : OComboBox( true, parent, name )
{
    init( useCompletion );
}

void OHistoryCombo::init( bool useCompletion )
{
    if ( useCompletion )
        completionObject()->setOrder( OCompletion::Weighted );

    setInsertionPolicy( NoInsertion );
    myIterateIndex = -1;
    myRotated = false;
    myPixProvider = 0L;

    connect( this, SIGNAL(aboutToShowContextMenu(QPopupMenu*)),
             SLOT(addContextMenuItems(QPopupMenu*)) );
    connect( this, SIGNAL( activated(int) ), SLOT( slotReset() ));
    connect( this, SIGNAL( returnPressed(const QString&) ), SLOT(slotReset()));
}

OHistoryCombo::~OHistoryCombo()
{
    delete myPixProvider;
}

void OHistoryCombo::setHistoryItems( QStringList items,
                                     bool setCompletionList )
{
    OComboBox::clear();

    // limit to maxCount()
    while ( (int) items.count() > maxCount() && !items.isEmpty() )
        items.remove( items.begin() );

    insertItems( items );

    if ( setCompletionList && useCompletion() ) {
        // we don't have any weighting information here ;(
        OCompletion *comp = completionObject();
        comp->setOrder( OCompletion::Insertion );
        comp->setItems( items );
        comp->setOrder( OCompletion::Weighted );
    }

    clearEdit();
}

QStringList OHistoryCombo::historyItems() const
{
    QStringList list;
    for ( int i = 0; i < count(); i++ )
        list.append( text( i ) );

    return list;
}

void OHistoryCombo::clearHistory()
{
    OComboBox::clear();
    if ( useCompletion() )
        completionObject()->clear();
}

void OHistoryCombo::addContextMenuItems( QPopupMenu* menu )
{
    if ( menu &&!lineEdit()->text().isEmpty())
    {
        menu->insertSeparator();
        menu->insertItem( tr("Empty Contents"), this, SLOT( slotClear()));
    }
}

void OHistoryCombo::addToHistory( const QString& item )
{
    if ( item.isEmpty() || (count() > 0 && item == text(0) ))
        return;

    // remove all existing items before adding
    if ( !duplicatesEnabled() ) {
        for ( int i = 0; i < count(); i++ ) {
            if ( text( i ) == item )
                removeItem( i );
        }
    }

    // now add the item
    if ( myPixProvider )
        //insertItem( myPixProvider->pixmapFor(item, KIcon::SizeSmall), item, 0);
        insertItem( myPixProvider->pixmapFor(item, 16), item, 0);
    else
        insertItem( item, 0 );

    int last;
    QString rmItem;

    bool useComp = useCompletion();
    while ( count() > maxCount() && count() > 0 ) {
        // remove the last item, as long as we are longer than maxCount()
        // remove the removed item from the completionObject if it isn't
        // anymore available at all in the combobox.
        last = count() - 1;
        rmItem = text( last );
        removeItem( last );
        if ( useComp && !contains( rmItem ) )
            completionObject()->removeItem( rmItem );
    }

    if ( useComp )
        completionObject()->addItem( item );
}

bool OHistoryCombo::removeFromHistory( const QString& item )
{
    if ( item.isEmpty() )
        return false;

    bool removed = false;
    QString temp = currentText();
    for ( int i = 0; i < count(); i++ ) {
        while ( item == text( i ) ) {
            removed = true;
            removeItem( i );
        }
    }

    if ( removed && useCompletion() )
        completionObject()->removeItem( item );

    setEditText( temp );
    return removed;
}

void OHistoryCombo::keyPressEvent( QKeyEvent *e )
{
    // save the current text in the lineedit
    if ( myIterateIndex == -1 )
        myText = currentText();

    // going up in the history, rotating when reaching QListBox::count()
    //if ( OStdAccel::isEqual( e, OStdAccel::rotateUp() ) ) {
    if ( e->key() == Qt::Key_Up ) {
        myIterateIndex++;

        // skip duplicates/empty items
        while ( myIterateIndex < count()-1 &&
                (currentText() == text( myIterateIndex ) ||
                text( myIterateIndex ).isEmpty()) )
            myIterateIndex++;

        if ( myIterateIndex >= count() ) {
            myRotated = true;
            myIterateIndex = -1;

            // if the typed text is the same as the first item, skip the first
            if ( myText == text(0) )
                myIterateIndex = 0;

            setEditText( myText );
        }
        else
            setEditText( text( myIterateIndex ));
    }


    // going down in the history, no rotation possible. Last item will be
    // the text that was in the lineedit before Up was called.
    //else if ( OStdAccel::isEqual( e, OStdAccel::rotateDown() ) ) {
    else if ( e->key() == Qt::Key_Down ) {
        myIterateIndex--;

        // skip duplicates/empty items
        while ( myIterateIndex >= 0 &&
                (currentText() == text( myIterateIndex ) ||
                text( myIterateIndex ).isEmpty()) )
            myIterateIndex--;


        if ( myIterateIndex < 0 ) {
            if ( myRotated && myIterateIndex == -2 ) {
                myRotated = false;
                myIterateIndex = count() - 1;
                setEditText( text(myIterateIndex) );
            }
            else { // bottom of history
                if ( myIterateIndex == -2 ) {
                    qDebug( "ONotifyClient is not implemented yet." );
                    //ONotifyClient::event( ONotifyClient::notification,
                    //                  i18n("No further item in the history."));
                }

                myIterateIndex = -1;
                if ( currentText() != myText )
                    setEditText( myText );
            }
        }
        else
            setEditText( text( myIterateIndex ));
    }

    else
        OComboBox::keyPressEvent( e );
}

void OHistoryCombo::slotReset()
{
    myIterateIndex = -1;
    myRotated = false;
}


void OHistoryCombo::setPixmapProvider( OPixmapProvider *prov )
{
    if ( myPixProvider == prov )
        return;

    delete myPixProvider;
    myPixProvider = prov;

    // re-insert all the items with/without pixmap
    // I would prefer to use changeItem(), but that doesn't honour the pixmap
    // when using an editable combobox (what we do)
    if ( count() > 0 ) {
        QStringList items( historyItems() );
        clear();
        insertItems( items );
    }
}

void OHistoryCombo::insertItems( const QStringList& items )
{
    QStringList::ConstIterator it = items.begin();
    QString item;
    while ( it != items.end() ) {
        item = *it;
        if ( !item.isEmpty() ) { // only insert non-empty items
            if ( myPixProvider )
                // insertItem( myPixProvider->pixmapFor(item, OIcon::SizeSmall), item );
                insertItem( myPixProvider->pixmapFor(item, 16), item );
            else
                insertItem( item );
        }
        ++it;
    }
}

void OHistoryCombo::slotClear()
{
    clearHistory();
    emit cleared();
}

