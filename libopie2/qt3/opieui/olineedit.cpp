/*
  This file                  Copyright (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
    is part of the           Copyright (C) 2001 Carsten Pfeiffer <pfeiffer@kde.org>, Dawit Alemayehu <adawit@kde.org>
       Opie Project          Copyright (C) 1999 Preston Brown <pbrown@kde.org>, Patrick Ward <PAT_WARD@HP-USA-om5.om.hp.com>
                             Copyright (C) 1997 Sven Radej (sven.radej@iname.com)
              =.
            .=l.             Originally part of the KDE Project
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

#include <qapplication.h>
#include <qclipboard.h>
#include <qtimer.h>
#include <qpopupmenu.h>

/* OPIE */

#include <opie2/ocompletionbox.h>
#include <opie2/olineedit.h>
#include <opie2/oglobalsettings.h>

typedef QString KURL; //FIXME: Revise for Opie

/*======================================================================================
 * OLineEditPrivate
 *======================================================================================*/

class OLineEdit::OLineEditPrivate
{
public:
    OLineEditPrivate()
    {
        grabReturnKeyEvents = false;
        handleURLDrops = true;
        completionBox = 0L;
    }
    ~OLineEditPrivate()
    {
        delete completionBox;
    }

    bool grabReturnKeyEvents;
    bool handleURLDrops;
    OCompletionBox *completionBox;
};


/*======================================================================================
 * OLineEdit
 *======================================================================================*/

OLineEdit::OLineEdit( const QString &string, QWidget *parent, const char *name )
    : QLineEdit( string, parent, name )
{
    init();
}

OLineEdit::OLineEdit( QWidget *parent, const char *name )
    : QLineEdit( parent, name )
{
    init();
}

OLineEdit::~OLineEdit ()
{
    delete d;
}

void OLineEdit::init()
{
    d = new OLineEditPrivate;
    possibleTripleClick = false;
    // Enable the context menu by default.
    setContextMenuEnabled( true );
    //OCursor::setAutoHideCursor( this, true, true );
    installEventFilter( this );
}

void OLineEdit::setCompletionMode( OGlobalSettings::Completion mode )
{
    OGlobalSettings::Completion oldMode = completionMode();
    if ( oldMode != mode && oldMode == OGlobalSettings::CompletionPopup &&
         d->completionBox && d->completionBox->isVisible() )
        d->completionBox->hide();

    // If the widgets echo mode is not Normal, no completion
    // feature will be enabled even if one is requested.
    if ( echoMode() != QLineEdit::Normal )
        mode = OGlobalSettings::CompletionNone; // Override the request.

    OCompletionBase::setCompletionMode( mode );
}

void OLineEdit::setCompletedText( const QString& t, bool marked )
{
    QString txt = text();
    if ( t != txt )
    {
        int curpos = marked ? txt.length() : t.length();
        validateAndSet( t, curpos, curpos, t.length() );
    }
}

void OLineEdit::setCompletedText( const QString& text )
{
    OGlobalSettings::Completion mode = completionMode();
    bool marked = ( mode == OGlobalSettings::CompletionAuto ||
                    mode == OGlobalSettings::CompletionMan ||
                    mode == OGlobalSettings::CompletionPopup );
    setCompletedText( text, marked );
}

void OLineEdit::rotateText( OCompletionBase::KeyBindingType type )
{
    OCompletion* comp = compObj();
    if ( comp &&
       (type == OCompletionBase::PrevCompletionMatch ||
        type == OCompletionBase::NextCompletionMatch ) )
    {
       QString input = (type == OCompletionBase::PrevCompletionMatch) ? comp->previousMatch() : comp->nextMatch();
       // Skip rotation if previous/next match is null or the same text
       if ( input.isNull() || input == displayText() )
            return;
       #if QT_VERSION > 290
       setCompletedText( input, hasSelectedText() );
       #else
       setCompletedText( input, hasMarkedText() );
       #endif
    }
}

void OLineEdit::makeCompletion( const QString& text )
{
    OCompletion *comp = compObj();
    if ( !comp )
        return;  // No completion object...

    QString match = comp->makeCompletion( text );
    OGlobalSettings::Completion mode = completionMode();
    if ( mode == OGlobalSettings::CompletionPopup )
    {
        if ( match.isNull() )
        {
            if ( d->completionBox ) {
                d->completionBox->hide();
                d->completionBox->clear();
            }
        }
        else
            setCompletedItems( comp->allMatches() );
    }
    else
    {
        // all other completion modes
        // If no match or the same match, simply return without completing.
        if ( match.isNull() || match == text )
            return;

        setCompletedText( match );
    }
}

void OLineEdit::setReadOnly(bool readOnly)
{
    QPalette p = palette();
    if (readOnly)
    {
        QColor color = p.color(QPalette::Disabled, QColorGroup::Background);
        p.setColor(QColorGroup::Base, color);
        p.setColor(QColorGroup::Background, color);
    }
    else
    {
        QColor color = p.color(QPalette::Normal, QColorGroup::Base);
        p.setColor(QColorGroup::Base, color);
        p.setColor(QColorGroup::Background, color);
    }
    setPalette(p);

    QLineEdit::setReadOnly (readOnly);
}

void OLineEdit::keyPressEvent( QKeyEvent *e )
{
    qDebug( "OLineEdit::keyPressEvent()" );
    
    /*
    
    KKey key( e );

    if ( KStdAccel::copy().contains( key ) ) {
        copy();
	return;
    }
    else if ( KStdAccel::paste().contains( key ) ) {
        paste();
	return;
    }
    else if ( KStdAccel::cut().contains( key ) ) {
        cut();
	return;
    }
    else if ( KStdAccel::undo().contains( key ) ) {
        undo();
	return;
    }
    else if ( KStdAccel::redo().contains( key ) ) {
        redo();
	return;
    }
    else if ( KStdAccel::deleteWordBack().contains( key ) )
    {
        cursorWordBackward(TRUE);
        if ( hasSelectedText() )
            del();

        e->accept();
        return;
    }
    else if ( KStdAccel::deleteWordForward().contains( key ) )
    {
        // Workaround for QT bug where
        cursorWordForward(TRUE);
        if ( hasSelectedText() )
            del();

        e->accept();
        return;
    }
    */
    
    // Filter key-events if EchoMode is normal &
    // completion mode is not set to CompletionNone
    if ( echoMode() == QLineEdit::Normal &&
         completionMode() != OGlobalSettings::CompletionNone )
    {
        KeyBindingMap keys = getKeyBindings();
        OGlobalSettings::Completion mode = completionMode();
        bool noModifier = (e->state() == NoButton || e->state()== ShiftButton);

        if ( (mode == OGlobalSettings::CompletionAuto ||
              mode == OGlobalSettings::CompletionMan) && noModifier )
        {
            QString keycode = e->text();
            if ( !keycode.isNull() && keycode.unicode()->isPrint() )
            {
                QLineEdit::keyPressEvent ( e );
                QString txt = text();
                int len = txt.length();
                #if QT_VERSION > 290
                if ( !hasSelectedText() && len && cursorPosition() == len )
                #else
                if ( !hasMarkedText() && len && cursorPosition() == len )
                #endif
                {
                    if ( emitSignals() )
                        emit completion( txt );
                    if ( handleSignals() )
                        makeCompletion( txt );
                    e->accept();
                }
                return;
            }
        }

        else if ( mode == OGlobalSettings::CompletionPopup && noModifier )
        {
            qDebug( "OLineEdit::keyPressEvent() - global settings = CompletionPopup & noModifier" );

            QString old_txt = text();
            QLineEdit::keyPressEvent ( e );
            QString txt = text();
            int len = txt.length();
            QString keycode = e->text();


            if ( txt != old_txt && len && cursorPosition() == len &&
                 ( (!keycode.isNull() && keycode.unicode()->isPrint()) ||
                   e->key() == Key_Backspace ) )
            {
                if ( emitSignals() )
                    emit completion( txt ); // emit when requested...
                if ( handleSignals() )
                    makeCompletion( txt );  // handle when requested...
                e->accept();
            }
            else if (!len && d->completionBox && d->completionBox->isVisible())
                d->completionBox->hide();

            return;
        }

        /*else if ( mode == OGlobalSettings::CompletionShell )
        {
            // Handles completion.
            KShortcut cut;
            if ( keys[TextCompletion].isNull() )
                cut = KStdAccel::shortcut(KStdAccel::TextCompletion);
            else
                cut = keys[TextCompletion];

            if ( cut.contains( key ) )
            {
                // Emit completion if the completion mode is CompletionShell
                // and the cursor is at the end of the string.
                QString txt = text();
                int len = txt.length();
                if ( cursorPosition() == len && len != 0 )
                {
                    if ( emitSignals() )
                        emit completion( txt );
                    if ( handleSignals() )
                        makeCompletion( txt );
                    return;
                }
            }
            else if ( d->completionBox )
                d->completionBox->hide();
        }

        // handle rotation
        if ( mode != OGlobalSettings::CompletionNone )
        {
            // Handles previous match
            KShortcut cut;
            if ( keys[PrevCompletionMatch].isNull() )
                cut = KStdAccel::shortcut(KStdAccel::PrevCompletion);
            else
                cut = keys[PrevCompletionMatch];

            if ( cut.contains( key ) )
            {
                if ( emitSignals() )
                    emit textRotation( OCompletionBase::PrevCompletionMatch );
                if ( handleSignals() )
                    rotateText( OCompletionBase::PrevCompletionMatch );
                return;
            }

            // Handles next match
            if ( keys[NextCompletionMatch].isNull() )
                cut = KStdAccel::key(KStdAccel::NextCompletion);
            else
                cut = keys[NextCompletionMatch];

            if ( cut.contains( key ) )
            {
                if ( emitSignals() )
                    emit textRotation( OCompletionBase::NextCompletionMatch );
                if ( handleSignals() )
                    rotateText( OCompletionBase::NextCompletionMatch );
                return;
            }
        }

        // substring completion
        if ( compObj() )
        {
            KShortcut cut;
            if ( keys[SubstringCompletion].isNull() )
                cut = KStdAccel::shortcut(KStdAccel::SubstringCompletion);
            else
                cut = keys[SubstringCompletion];

            if ( cut.contains( key ) )
            {
                if ( emitSignals() )
                    emit substringCompletion( text() );
                if ( handleSignals() )
                {
                    setCompletedItems( compObj()->substringCompletion(text()));
                    e->accept();
                }
                return;
            }
        } */
    }   

    // Let QLineEdit handle any other keys events.
    QLineEdit::keyPressEvent ( e );
}

void OLineEdit::mouseDoubleClickEvent( QMouseEvent* e )
{
    if ( e->button() == Qt::LeftButton  )
    {
        possibleTripleClick=true;
        QTimer::singleShot( QApplication::doubleClickInterval(),this,
                            SLOT(tripleClickTimeout()) );
    }
    QLineEdit::mouseDoubleClickEvent( e );
}

void OLineEdit::mousePressEvent( QMouseEvent* e )
{
    if ( possibleTripleClick && e->button() == Qt::LeftButton )
    {
        selectAll();
        return;
    }
    QLineEdit::mousePressEvent( e );
}

void OLineEdit::tripleClickTimeout()
{
    possibleTripleClick=false;
}

QPopupMenu *OLineEdit::createPopupMenu()
{
    // Return if popup menu is not enabled !!
    if ( !m_bEnableMenu )
        return 0;

    #if QT_VERSION > 290
    QPopupMenu *popup = QLineEdit::createPopupMenu();
    #else
    QPopupMenu *popup = new QPopupMenu();
    #warning OLineEdit is not fully functional on Qt2
    #endif

    // completion object is present.
    if ( compObj() )
    {
        QPopupMenu *subMenu = new QPopupMenu( popup );
        connect( subMenu, SIGNAL( activated( int ) ),
                 this, SLOT( completionMenuActivated( int ) ) );

        popup->insertSeparator();
        //popup->insertItem( SmallIconSet("completion"), i18n("Text Completion"),
        //                   subMenu );

        popup->insertItem( tr("Text Completion"), subMenu );

        subMenu->insertItem( tr("None"), NoCompletion );
        subMenu->insertItem( tr("Manual"), ShellCompletion );
        subMenu->insertItem( tr("Automatic"), AutoCompletion );
        subMenu->insertItem( tr("Dropdown List"), PopupCompletion );
        subMenu->insertItem( tr("Short Automatic"), SemiAutoCompletion );

        //subMenu->setAccel( KStdAccel::completion(), ShellCompletion );
        subMenu->setAccel( Key_Tab, ShellCompletion );

        OGlobalSettings::Completion mode = completionMode();
        subMenu->setItemChecked( NoCompletion,
                                 mode == OGlobalSettings::CompletionNone );
        subMenu->setItemChecked( ShellCompletion,
                                 mode == OGlobalSettings::CompletionShell );
        subMenu->setItemChecked( PopupCompletion,
                                 mode == OGlobalSettings::CompletionPopup );
        subMenu->setItemChecked( AutoCompletion,
                                 mode == OGlobalSettings::CompletionAuto );
        subMenu->setItemChecked( SemiAutoCompletion,
                                 mode == OGlobalSettings::CompletionMan );
        if ( mode != OGlobalSettings::completionMode() )
        {
            subMenu->insertSeparator();
            subMenu->insertItem( tr("Default"), Default );
        }
    }
    // ### do we really need this?  Yes, Please do not remove!  This
    // allows applications to extend the popup menu without having to
    // inherit from this class! (DA)
    emit aboutToShowContextMenu( popup );

    return popup;
}

void OLineEdit::completionMenuActivated( int id )
{
    OGlobalSettings::Completion oldMode = completionMode();

    switch ( id )
    {
        case Default:
           setCompletionMode( OGlobalSettings::completionMode() ); break;
        case NoCompletion:
           setCompletionMode( OGlobalSettings::CompletionNone ); break;
        case AutoCompletion:
            setCompletionMode( OGlobalSettings::CompletionAuto ); break;
        case SemiAutoCompletion:
            setCompletionMode( OGlobalSettings::CompletionMan ); break;
        case ShellCompletion:
            setCompletionMode( OGlobalSettings::CompletionShell ); break;
        case PopupCompletion:
            setCompletionMode( OGlobalSettings::CompletionPopup ); break;
        default: return;
    }

    if ( oldMode != completionMode() )
    {
        if ( oldMode == OGlobalSettings::CompletionPopup &&
             d->completionBox && d->completionBox->isVisible() )
            d->completionBox->hide();
        emit completionModeChanged( completionMode() );
    }
}

/*void OLineEdit::dropEvent(QDropEvent *e)
{
    KURL::List urlList;
    if( d->handleURLDrops && KURLDrag::decode( e, urlList ) )
    {
        QString dropText = text();
        KURL::List::ConstIterator it;
        for( it = urlList.begin() ; it != urlList.end() ; ++it )
        {
            if(!dropText.isEmpty())
                dropText+=' ';

            dropText += (*it).prettyURL();
        }

        validateAndSet( dropText, dropText.length(), 0, 0);

        e->accept();
    }
    else
        QLineEdit::dropEvent(e);
}*/

bool OLineEdit::eventFilter( QObject* o, QEvent* ev )
{
    if( o == this )
    {
        //OCursor::autoHideEventFilter( this, ev );
        if ( ev->type() == QEvent::AccelOverride )
        {
            QKeyEvent *e = static_cast<QKeyEvent *>( ev );
          //  if (overrideAccel (e))
          //  {
          //      e->accept();
          //      return true;
          //  }
        }
        else if( ev->type() == QEvent::KeyPress )
        {
            QKeyEvent *e = static_cast<QKeyEvent *>( ev );

            if( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter )
            {
                bool trap = d->completionBox && d->completionBox->isVisible();

                // Qt will emit returnPressed() itself if we return false
                if ( d->grabReturnKeyEvents || trap )
                    emit QLineEdit::returnPressed();

                emit returnPressed( displayText() );

                if ( trap )
                    d->completionBox->hide();

                // Eat the event if the user asked for it, or if a completionbox was visible
                return d->grabReturnKeyEvents || trap;
            }
        }
    }
    return QLineEdit::eventFilter( o, ev );
}


void OLineEdit::setURLDropsEnabled(bool enable)
{
    d->handleURLDrops=enable;
}

bool OLineEdit::isURLDropsEnabled() const
{
    return d->handleURLDrops;
}

void OLineEdit::setTrapReturnKey( bool grab )
{
    d->grabReturnKeyEvents = grab;
}

bool OLineEdit::trapReturnKey() const
{
    return d->grabReturnKeyEvents;
}

/*void OLineEdit::setURL( const KURL& url )
{
    QLineEdit::setText( url.prettyURL() );
}*/

void OLineEdit::makeCompletionBox()
{
    if ( d->completionBox )
        return;

    d->completionBox = new OCompletionBox( this, "completion box" );
    if ( handleSignals() )
    {
        connect( d->completionBox, SIGNAL(highlighted( const QString& )),
                 SLOT(setText( const QString& )) );
        connect( d->completionBox, SIGNAL(userCancelled( const QString& )),
                 SLOT(setText( const QString& )) );

        // Nice lil' hacklet ;) KComboBox doesn't know when the completionbox
        // is created (childEvent() is even more hacky, IMHO), so we simply
        // forward the completionbox' activated signal from here.
        if ( parentWidget() && parentWidget()->inherits("KComboBox") )
            connect( d->completionBox, SIGNAL( activated( const QString& )),
                     parentWidget(), SIGNAL( activated( const QString & )));
    }
}

/*bool OLineEdit::overrideAccel (const QKeyEvent* e)
{
  KShortcut scKey;

  KKey key( e );
  KeyBindingMap keys = getKeyBindings();

  if (keys[TextCompletion].isNull())
    scKey = KStdAccel::shortcut(KStdAccel::TextCompletion);
  else
    scKey = keys[TextCompletion];

  if (scKey.contains( key ))
    return true;

  if (keys[NextCompletionMatch].isNull())
    scKey = KStdAccel::shortcut(KStdAccel::NextCompletion);
  else
    scKey = keys[NextCompletionMatch];

  if (scKey.contains( key ))
    return true;

  if (keys[PrevCompletionMatch].isNull())
    scKey = KStdAccel::shortcut(KStdAccel::PrevCompletion);
   else
    scKey = keys[PrevCompletionMatch];

  if (scKey.contains( key ))
    return true;

  if (KStdAccel::deleteWordBack().contains( key ))
    return true;
  if (KStdAccel::deleteWordForward().contains( key ))
    return true;

  if (d->completionBox && d->completionBox->isVisible ())
    if (e->key () == Key_Backtab)
      return true;

  return false;
}*/

void OLineEdit::setCompletedItems( const QStringList& items )
{
    QString txt = text();
    if ( !items.isEmpty() &&
         !(items.count() == 1 && txt == items.first()) )
    {
        if ( !d->completionBox )
            makeCompletionBox();

        if ( !txt.isEmpty() )
            d->completionBox->setCancelledText( txt );
        d->completionBox->setItems( items );
        d->completionBox->popup();
    }
    else
    {
        if ( d->completionBox && d->completionBox->isVisible() )
            d->completionBox->hide();
    }
}

OCompletionBox * OLineEdit::completionBox( bool create )
{
    if ( create )
        makeCompletionBox();

    return d->completionBox;
}

void OLineEdit::setCompletionObject( OCompletion* comp, bool hsig )
{
    OCompletion *oldComp = compObj();
    if ( oldComp && handleSignals() )
        disconnect( oldComp, SIGNAL( matches( const QStringList& )),
                    this, SLOT( setCompletedItems( const QStringList& )));

    if ( comp && hsig )
      connect( comp, SIGNAL( matches( const QStringList& )),
               this, SLOT( setCompletedItems( const QStringList& )));

    OCompletionBase::setCompletionObject( comp, hsig );
}

// QWidget::create() turns off mouse-Tracking which would break auto-hiding
void OLineEdit::create( WId id, bool initializeWindow, bool destroyOldWindow )
{
    QLineEdit::create( id, initializeWindow, destroyOldWindow );
    //OCursor::setAutoHideCursor( this, true, true );
}

void OLineEdit::clear()
{
    setText( QString::null );
}
