/* This file is part of the KDE libraries
   Copyright (C) 2000 Daniel M. Duley <mosfet@kde.org>
   Copyright (C) 2002 Hamish Rodda <meddie@yoyo.its.monash.edu.au>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

/* QT */

#include <qdrawutil.h>
#include <qtimer.h>

/* OPIE */

#include <opie2/opopupmenu.h>
#include <opie2/oconfig.h>

using namespace Opie::Core;
using namespace Opie::Ui;

OPopupTitle::OPopupTitle(QWidget *parent, const char *name)
    : QWidget(parent, name)
{
    setMinimumSize(16, fontMetrics().height()+8);
}

OPopupTitle::OPopupTitle(OPixmapEffect::GradientType /* gradient */,
                         const QColor &/* color */, const QColor &/* textColor */,
                         QWidget *parent, const char *name)
   : QWidget(parent, name)
{
    setMinimumSize(16, fontMetrics().height()+8);
}

OPopupTitle::OPopupTitle(const OPixmap & /* background */, const QColor &/* color */,
                         const QColor &/* textColor */, QWidget *parent,
                         const char *name)
    : QWidget(parent, name)
{
    setMinimumSize(16, fontMetrics().height()+8);
}

void OPopupTitle::setTitle(const QString &text, const QPixmap *icon)
{
    titleStr = text;
    if (icon)
        miniicon = *icon;
    else
        miniicon.resize(0, 0);

    int w = miniicon.width()+fontMetrics().width(titleStr);
    int h = QMAX( fontMetrics().height(), miniicon.height() );
    setMinimumSize( w+16, h+8 );
}

void OPopupTitle::setText( const QString &text )
{
    titleStr = text;
    int w = miniicon.width()+fontMetrics().width(titleStr);
    int h = QMAX( fontMetrics().height(), miniicon.height() );
    setMinimumSize( w+16, h+8 );
}

void OPopupTitle::setIcon( const QPixmap &pix )
{
    miniicon = pix;
    int w = miniicon.width()+fontMetrics().width(titleStr);
    int h = QMAX( fontMetrics().height(), miniicon.height() );
    setMinimumSize( w+16, h+8 );
}

void OPopupTitle::paintEvent(QPaintEvent *)
{
    QRect r(rect());
    QPainter p(this);
    #if QT_VERSION > 290
    qApp->style().drawPrimitive(QStyle::PE_HeaderSection, &p, r, palette().active());
    #else
    #warning OPopupMenu is not fully functional on Qt2
    #endif

    if (!miniicon.isNull())
        p.drawPixmap(4, (r.height()-miniicon.height())/2, miniicon);

    if (!titleStr.isNull())
    {
        p.setPen(palette().active().text());
        QFont f = p.font();
        f.setBold(true);
        p.setFont(f);
        if(!miniicon.isNull())
        {
            p.drawText(miniicon.width()+8, 0, width()-(miniicon.width()+8),
                       height(), AlignLeft | AlignVCenter | SingleLine,
                       titleStr);
        }
        else
        {
            p.drawText(0, 0, width(), height(),
                       AlignCenter | SingleLine, titleStr);
        }
    }

    p.setPen(palette().active().highlight());
    p.drawLine(0, 0, r.right(), 0);
}

QSize OPopupTitle::sizeHint() const
{
    return(minimumSize());
}

class OPopupMenu::OPopupMenuPrivate
{
public:
    OPopupMenuPrivate ()
        : noMatches(false)
        , shortcuts(false)
        , autoExec(false)
        , lastHitIndex(-1)
        , m_ctxMenu(0)
    {}
    
    ~OPopupMenuPrivate ()
    {
        delete m_ctxMenu;
    }

    QString m_lastTitle;

    // variables for keyboard navigation
    QTimer clearTimer;

    bool noMatches : 1;
    bool shortcuts : 1;
    bool autoExec : 1;

    QString keySeq;
    QString originalText;

    int lastHitIndex;

    // support for RMB menus on menus
    QPopupMenu* m_ctxMenu;
    static bool s_continueCtxMenuShow;
    static int s_highlightedItem;
    static OPopupMenu* s_contextedMenu;
};

int OPopupMenu::OPopupMenuPrivate::s_highlightedItem(-1);
OPopupMenu* OPopupMenu::OPopupMenuPrivate::s_contextedMenu(0);
bool OPopupMenu::OPopupMenuPrivate::s_continueCtxMenuShow(true);

OPopupMenu::OPopupMenu(QWidget *parent, const char *name)
    : QPopupMenu(parent, name)
{
    d = new OPopupMenuPrivate;
    resetKeyboardVars();
    connect(&(d->clearTimer), SIGNAL(timeout()), SLOT(resetKeyboardVars()));
}

OPopupMenu::~OPopupMenu()
{
    if (OPopupMenuPrivate::s_contextedMenu == this)
    {
        OPopupMenuPrivate::s_contextedMenu = 0;
        OPopupMenuPrivate::s_highlightedItem = -1;
    }
    
    delete d;
}

int OPopupMenu::insertTitle(const QString &text, int id, int index)
{
    OPopupTitle *titleItem = new OPopupTitle();
    titleItem->setTitle(text);
    int ret = insertItem(titleItem, id, index);
    setItemEnabled(id, false);
    return ret;
}

int OPopupMenu::insertTitle(const QPixmap &icon, const QString &text, int id,
                            int index)
{
    OPopupTitle *titleItem = new OPopupTitle();
    titleItem->setTitle(text, &icon);
    int ret = insertItem(titleItem, id, index);
    setItemEnabled(id, false);
    return ret;
}

void OPopupMenu::changeTitle(int id, const QString &text)
{
    QMenuItem *item = findItem(id);
    if(item){
        if(item->widget())
            ((OPopupTitle *)item->widget())->setTitle(text);
#ifndef NDEBUG
        else
            qWarning( "KPopupMenu: changeTitle() called with non-title id %d", id );
#endif
    }
#ifndef NDEBUG
    else
        qWarning( "KPopupMenu: changeTitle() called with invalid id %d", id );
#endif
}

void OPopupMenu::changeTitle(int id, const QPixmap &icon, const QString &text)
{
    QMenuItem *item = findItem(id);
    if(item){
        if(item->widget())
            ((OPopupTitle *)item->widget())->setTitle(text, &icon);
#ifndef NDEBUG
        else
            qWarning( "KPopupMenu: changeTitle() called with non-title id %d", id );
#endif
    }
#ifndef NDEBUG
    else
        qWarning( "KPopupMenu: changeTitle() called with invalid id %d", id );
#endif
}

QString OPopupMenu::title(int id) const
{
    if(id == -1) // obsolete
        return(d->m_lastTitle);
    QMenuItem *item = findItem(id);
    if(item){
        if(item->widget())
            return(((OPopupTitle *)item->widget())->title());
        else
            qWarning("OPopupMenu: title() called with non-title id %d.", id);
    }
    else
        qWarning("OPopupMenu: title() called with invalid id %d.", id);
    return(QString::null);
}

QPixmap OPopupMenu::titlePixmap(int id) const
{
    QMenuItem *item = findItem(id);
    if(item){
        if(item->widget())
            return(((OPopupTitle *)item->widget())->icon());
        else
            qWarning("KPopupMenu: titlePixmap() called with non-title id %d.", id);
    }
    else
        qWarning("KPopupMenu: titlePixmap() called with invalid id %d.", id);
    QPixmap tmp;
    return(tmp);
}

/**
 * This is re-implemented for keyboard navigation.
 */
void OPopupMenu::closeEvent(QCloseEvent*e)
{
    if (d->shortcuts)
        resetKeyboardVars();
    QPopupMenu::closeEvent(e);
}

void OPopupMenu::keyPressEvent(QKeyEvent* e)
{
    if (!d->shortcuts) {
        // continue event processing by Qpopup
        //e->ignore();
        QPopupMenu::keyPressEvent(e);
        return;
    }

    int i = 0;
    bool firstpass = true;
    QString keyString = e->text();

    // check for common commands dealt with by QPopup
    int key = e->key();
    if (key == Key_Escape || key == Key_Return || key == Key_Enter
            || key == Key_Up || key == Key_Down || key == Key_Left
            || key == Key_Right || key == Key_F1) {

        resetKeyboardVars();
        // continue event processing by Qpopup
        //e->ignore();
        QPopupMenu::keyPressEvent(e);
        return;
    }

    // check to see if the user wants to remove a key from the sequence (backspace)
    // or clear the sequence (delete)
    if (!d->keySeq.isNull()) {

        if (key == Key_Backspace) {

            if (d->keySeq.length() == 1) {
                resetKeyboardVars();
                return;
            }

            // keep the last sequence in keyString
            keyString = d->keySeq.left(d->keySeq.length() - 1);

            // allow sequence matching to be tried again
            resetKeyboardVars();

        } else if (key == Key_Delete) {
            resetKeyboardVars();

            // clear active item
            setActiveItem(0);
            return;

        } else if (d->noMatches) {
            // clear if there are no matches
            resetKeyboardVars();

            // clear active item
            setActiveItem(0);

        } else {
            // the key sequence is not a null string
            // therefore the lastHitIndex is valid
            i = d->lastHitIndex;
        }
    } else if (key == Key_Backspace && parentMenu) {
        // backspace with no chars in the buffer... go back a menu.
        hide();
        resetKeyboardVars();
        return;
    }

    d->keySeq += keyString;
    int seqLen = d->keySeq.length();

    for (; i < (int)count(); i++) {
        // compare typed text with text of this entry
        int j = idAt(i);

        // don't search disabled entries
        if (!isItemEnabled(j))
            continue;

        QString thisText;

        // retrieve the right text
        // (the last selected item one may have additional ampersands)
        if (i == d->lastHitIndex)
            thisText = d->originalText;
        else
            thisText = text(j);

        // if there is an accelerator present, remove it
        if ((int)accel(j) != 0)
            thisText = thisText.replace(QRegExp("&"), "");

        // chop text to the search length
        thisText = thisText.left(seqLen);

        // do the search
        if (thisText.find(d->keySeq, 0, false) == 0) {

            if (firstpass) {
                // match
                setActiveItem(i);

                // check to see if we're underlining a different item
                if (d->lastHitIndex != i)
                    // yes; revert the underlining
                    changeItem(idAt(d->lastHitIndex), d->originalText);

                // set the original text if it's a different item
                if (d->lastHitIndex != i || d->lastHitIndex == -1)
                    d->originalText = text(j);

                // underline the currently selected item
                changeItem(j, underlineText(d->originalText, d->keySeq.length()));

                // remeber what's going on
                d->lastHitIndex = i;

                // start/restart the clear timer
                d->clearTimer.start(5000, true);

                // go around for another try, to see if we can execute
                firstpass = false;
            } else {
                // don't allow execution
                return;
            }
        }

        // fall through to allow execution
    }

    if (!firstpass) {
        if (d->autoExec) {
            // activate anything
            activateItemAt(d->lastHitIndex);
            resetKeyboardVars();

        } else if (findItem(idAt(d->lastHitIndex)) &&
                 findItem(idAt(d->lastHitIndex))->popup()) {
            // only activate sub-menus
            activateItemAt(d->lastHitIndex);
            resetKeyboardVars();
        }

        return;
    }

    // no matches whatsoever, clean up
    resetKeyboardVars(true);
    //e->ignore();
    QPopupMenu::keyPressEvent(e);
}

QString OPopupMenu::underlineText(const QString& text, uint length)
{
    QString ret = text;
    for (uint i = 0; i < length; i++) {
        if (ret[2*i] != '&')
            ret.insert(2*i, "&");
    }
    return ret;
}

void OPopupMenu::resetKeyboardVars(bool noMatches /* = false */)
{
    // Clean up keyboard variables
    if (d->lastHitIndex != -1) {
        changeItem(idAt(d->lastHitIndex), d->originalText);
        d->lastHitIndex = -1;
    }

    if (!noMatches) {
        d->keySeq = QString::null;
    }

    d->noMatches = noMatches;
}

void OPopupMenu::setKeyboardShortcutsEnabled(bool enable)
{
    d->shortcuts = enable;
}

void OPopupMenu::setKeyboardShortcutsExecute(bool enable)
{
    d->autoExec = enable;
}
/**
 * End keyboard navigation.
 */

/**
 * RMB menus on menus
 */
QPopupMenu* OPopupMenu::contextMenu()
{
    if (!d->m_ctxMenu)
    {
        d->m_ctxMenu = new QPopupMenu(this);
        installEventFilter(this);
        connect(d->m_ctxMenu, SIGNAL(aboutToHide()), this, SLOT(ctxMenuHiding()));
    }

    return d->m_ctxMenu;
}

void OPopupMenu::cancelContextMenuShow()
{
    OPopupMenuPrivate::s_continueCtxMenuShow = false;
}

int OPopupMenu::contextMenuFocusItem()
{
    return OPopupMenuPrivate::s_highlightedItem;
}

OPopupMenu* OPopupMenu::contextMenuFocus()
{
    return OPopupMenuPrivate::s_contextedMenu;
}

void OPopupMenu::itemHighlighted(int /* whichItem */)
{
    if (!d->m_ctxMenu || !d->m_ctxMenu->isVisible())
    {
        return;
    }

    d->m_ctxMenu->hide();
    showCtxMenu(mapFromGlobal(QCursor::pos()));
}

void OPopupMenu::showCtxMenu(QPoint pos)
{
    OPopupMenuPrivate::s_highlightedItem = idAt(pos);

    if (OPopupMenuPrivate::s_highlightedItem == -1)
    {
        OPopupMenuPrivate::s_contextedMenu = 0;
        return;
    }

    emit aboutToShowContextMenu(this, OPopupMenuPrivate::s_highlightedItem, d->m_ctxMenu);

    if (!OPopupMenuPrivate::s_continueCtxMenuShow)
    {
        OPopupMenuPrivate::s_continueCtxMenuShow = true;
        return;
    }

    OPopupMenuPrivate::s_contextedMenu = this;
    d->m_ctxMenu->popup(this->mapToGlobal(pos));
    connect(this, SIGNAL(highlighted(int)), this, SLOT(itemHighlighted(int)));
}

void OPopupMenu::ctxMenuHiding()
{
    disconnect(this, SIGNAL(highlighted(int)), this, SLOT(itemHighlighted(int)));
    OPopupMenuPrivate::s_continueCtxMenuShow = true;
}

bool OPopupMenu::eventFilter(QObject* obj, QEvent* event)
{
    if (d->m_ctxMenu && obj == this)
    {
        if (event->type() == QEvent::MouseButtonRelease)
        {
            if (d->m_ctxMenu->isVisible())
            {
                return true;
            }
        }
        #if QT_VERSION > 290
        else if (event->type() == QEvent::ContextMenu)
        #else
        else if ( (event->type() == QEvent::MouseButtonPress) &&
                  ( (QMouseEvent*) event )->button() == QMouseEvent::RightButton )
        #endif
        {
            showCtxMenu(mapFromGlobal(QCursor::pos()));
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void OPopupMenu::hideEvent(QHideEvent*)
{
    if (d->m_ctxMenu)
    {
        d->m_ctxMenu->hide();
    }
}
/**
 * end of RMB menus on menus support
 */

// Obsolete
OPopupMenu::OPopupMenu(const QString& title, QWidget *parent, const char *name)
    : QPopupMenu(parent, name)
{
    d = new OPopupMenuPrivate;
    setTitle(title);
}

// Obsolete
void OPopupMenu::setTitle(const QString &title)
{
    OPopupTitle *titleItem = new OPopupTitle();
    titleItem->setTitle(title);
    insertItem(titleItem);
    d->m_lastTitle = title;
}

void OPopupTitle::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

void OPopupMenu::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

