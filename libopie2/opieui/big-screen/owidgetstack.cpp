/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2003 hOlgAr <zecke@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
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

#include "owidgetstack.h"

/* OPIE */
#include <opie2/odebug.h>

/* QT */
#include <qapplication.h>
#include <qwidgetstack.h>

namespace Opie {
namespace Ui   {
    const int mode_size = 330;



/**
 * This is the standard widget. For simple usage see the example. Normally this widget
 * is the central widget of a QMainWindow.
 * Use removeWidget before you delete a widget yourself. OWidgetStack does not
 * yet recognize removal of children.
 *
 * @param parent The parent widget. It maybe 0 but then you need to take care of deletion.
 *               Or you use QPEApplication::showMainWidget().
 * @param name   Name will be passed on to QObject
 * @param fl     Additional window flags passed to QFrame. see @Qt::WFlags
 */
OWidgetStack::OWidgetStack( QWidget* parent, const char* name, WFlags fl)
    : QFrame( parent, name, fl )
{
    m_last = m_mWidget = 0;
    m_forced = false;

    QApplication::desktop()->installEventFilter( this );
    setFontPropagation   ( AllChildren );
    setPalettePropagation( AllChildren );

    /* sets m_mode and initializes more */
    /* if you change this call change switchTop as well */
    m_stack = 0;
    switchStack();
}

/**
 * The destructor. It deletes also all added widgets.
 *
 */
OWidgetStack::~OWidgetStack() {
    if (m_mode == BigScreen && !m_list.isEmpty() ) {
        QMap<int, QWidget*>::Iterator it = m_list.begin();
        for ( ; it != m_list.end(); ++it )
            delete it.data();
    }
    m_list.clear();

}

/**
 * return the mode of the desktop. There are currently two modes. SmallScreen
 * with a normal PDA resolution and BigScreen with resolutions greater than
 * 330 for width and height.
 * You can also force the mode this widget is in with forceMode()
 * Note that NoForce will be never returned from here
 */
enum OWidgetStack::Mode OWidgetStack::mode()const {
    return m_mode;
}

/**
 *  You can also force one of the modes and then
 *  this widget stops on listening to size changes. You
 *  can revert to the scanning behaviour by setting mode
 *  to NoForce
 */
void OWidgetStack::forceMode( enum Mode mode) {
    m_forced = mode != NoForce;

    /* we need to see which mode we're in */
    if (!m_forced ) {
        if ( QApplication::desktop()->width() >=
             mode_size )
            mode = BigScreen;
        else
            mode = SmallScreen;
    }
    switch( mode ) {
    case NoForce:
    case SmallScreen:
        switchStack();
        break;
    case BigScreen:
        switchTop();
        break;

    }

    m_mode = mode;
}

/**
 * Adds a widget to the stack. The first widget added is considered
 * to be the mainwindow. This is important because if Opie is in
 * BigScreen mode the sizeHint of the MainWindow will be returned.
 * In Small Screen the sizeHint of the QWidgetStack is returned.
 * See QWidgetStack::sizeHint.
 * This widget takes ownership of the widget and may even reparent.
 * All windows will be hidden
 *
 * @param wid The QWidget to be added
 * @param id  An ID for the Widget. If the ID is duplicated the
              last set widget will be related to the id
 *
 */
void OWidgetStack::addWidget( QWidget* wid, int id) {
    if (!wid)
        return;

    /* set our main widget */
    if (!m_mWidget)
        m_mWidget = wid;

    m_list.insert( id, wid );

    /**
     * adding does not raise any widget
     * But for our mainwidget we prepare
     * the right position with the right parent
     */
    if (m_mode == SmallScreen )
        m_stack->addWidget( wid,id );
    else if ( m_mWidget == wid ) {
        wid->reparent(this, 0, contentsRect().topLeft() );
        wid->hide();
    }else {
        wid->reparent(0, WType_TopLevel, QPoint(10, 10) );
        wid->hide();
    }
}


/**
 * Remove the widget from the stack it'll be reparented to 0
 * and ownership is dropped. You need to delete it.
 * If the removed widget was the mainwindow consider
 * to call setMainWindow.
 *
 * @param wid The QWidget to be removed
 */
void OWidgetStack::removeWidget( QWidget* wid) {
    if (!wid)
        return;

    if (m_mode == SmallScreen )
        m_stack->removeWidget( wid );


    wid->reparent(0, 0, QPoint(0, 0) );
    m_list.remove( id(wid) );

    if ( wid == m_mWidget )
        m_mWidget = 0;
}

#if 0
/**
 * @internal_resons
 */
QSizeHint OWidgetStack::sizeHint()const {

}

/**
 * @internal_reasons
 */
QSizeHint OWidgetStack::minimumSizeHint()const {

}
#endif

/**
 * This function tries to find the widget with the id.
 * You supplied a possible id in addWIdget. Note that not
 * QWidget::winId() is used.
 *
 * @param id The id to search for
 *
 * @return The widget or null
 * @see addWidget
 */
QWidget* OWidgetStack::widget( int id) const {
    return m_list[id];
}

/**
 * Tries to find the assigned id for the widget
 * or returns -1 if no widget could be found
 * @param wid The widget to look for
 */
int OWidgetStack::id( QWidget* wid)const{
    if (m_list.isEmpty() )
        return -1;

    QMap<int, QWidget*>::ConstIterator it = m_list.begin();
    for ( ; it != m_list.end(); ++it )
        if ( it.data() == wid )
            break;

    /* if not at the end return the key */
    return it == m_list.end() ? -1 : it.key();
}


/**
 * This function returns the currently visible
 * widget. In BigScreen mode the mainwindow
 * is returned
 */
QWidget* OWidgetStack::visibleWidget()const {
    if (m_mode == SmallScreen )
        return m_stack->visibleWidget();
    else
        return m_mWidget;

}

/**
 * This method raises the widget wit the specefic id.
 * Note that in BigScreen mode the widget is made visible
 * but the other ( previous) visible widget(s) will not
 * be made invisible. If you need this use hideWidget().
 *
 * @param id Raise the widget with id
 */
void OWidgetStack::raiseWidget( int id) {
    return raiseWidget( widget( id ) );
}

/**
 * This is an overloaded function and only differs in its parameters.
 * @see raiseWidget( int )
 */
void OWidgetStack::raiseWidget( QWidget* wid) {
    m_last = wid;
    if (m_mode == SmallScreen )
        m_stack->raiseWidget( wid );
    else {
        int ide;
        emit aboutToShow( wid );
        /* if someone is connected and the widget is actually available */
        if ( receivers( SIGNAL(aboutToShow(int) ) ) &&
             ( (ide = id( wid ) ) != -1 ) )
            emit aboutToShow( ide );

        /* ### FIXME PLACE THE WIDGET right */
        wid->show();
    }
}

/**
 * This will hide the currently visible widget
 * and raise the widget specified by the parameter.
 * Note that this method does not use visibleWIdget but remembers
 * the last raisedWidget
 */
void OWidgetStack::hideWidget( int id) {
    /* hiding our main widget wouldn't be smart */
    if ( m_mode == BigScreen && m_last != m_mWidget )
        m_last->hide();
    raiseWidget( id );
}

/**
 * This is overloaded and only differs in the parameters
 * it takes.
 */
void OWidgetStack::hideWidget( QWidget* wid) {
    /* still not smart */
    if ( m_mode == BigScreen && m_last != m_mWidget )
        m_last->hide();

    raiseWidget( wid );
}


bool OWidgetStack::eventFilter( QObject* obj, QEvent* e) {
    owarn << " " << obj->name() << " " << obj->className() << "" << oendl;
    if ( e->type() == QEvent::Resize ) {
        QResizeEvent *res = static_cast<QResizeEvent*>( e );
        QSize size = res->size();
        if ( size.width() >= mode_size )
            switchTop();
        else
            switchStack();
    }
    return false;
}


/**
 * @internal_resons
 */
void OWidgetStack::resizeEvent( QResizeEvent* ev ) {
    QFrame::resizeEvent( ev );
    if (m_mode == SmallScreen )
        m_stack->setGeometry( frameRect() );
    else
        if (m_mWidget )
            m_mWidget->setGeometry( frameRect() );

}

/**
 * setMainWindow gives the OWidgetStack a hint which
 * window should always stay inside the stack.
 * Normally the first added widget is considered to be
 * the mainwindow but you can change this with this
 * function.
 * If in BigScreen mode the current mainwindow will be reparented
 * and hidden. The position will be taken by the new one.
 * If the old MainWindow was hidden the new window will
 * also be hidden. If the window was visible the new mainwindow
 * will be made visible too and the old one hidden. If there
 * was no mainwindow it will be hidden as well.
 *
 * @param wid The new mainwindow
 */
void OWidgetStack::setMainWindow( QWidget* wid ) {
    if (m_mode == BigScreen ) {
        bool wasVisible = false;
        if (m_mWidget ) {
            wasVisible = !m_mWidget->isHidden();
            /* hidden by default */
            m_mWidget->reparent(0, WType_TopLevel, QPoint(10, 10) );
        }
        wid->reparent(this, 0, frameRect().topLeft() );

        if (wasVisible)
            wid->show();
    }

    m_mWidget = wid;
}

/**
 * this is an overloaded member and only differs
 * in the type of arguments.
 * @see setMainWindow(QWidget*)
 */
void OWidgetStack::setMainWindow( int id) {
    setMainWindow( widget( id ) );
}


/*
 * this function switches to a stack ;)
 */
void OWidgetStack::switchStack() {
    if (m_stack ) {
        m_stack->setGeometry( frameRect() );
        return;
    }

    m_mode = SmallScreen;
    m_stack = new QWidgetStack(this);

    connect(m_stack, SIGNAL(aboutToShow(QWidget*) ),
            this, SIGNAL(aboutToShow(QWidget*) ) );
    connect(m_stack, SIGNAL(aboutToShow(int) ),
            this, SIGNAL(aboutToShow(int) ) );

    /* now reparent the widgets... luckily QWidgetSatck does most of the work */
    if (m_list.isEmpty() )
        return;

    QMap<int, QWidget*>::Iterator it = m_list.begin();
    for ( ; it != m_list.end(); ++it )
        m_stack->addWidget( it.data(), it.key() );


}

/*
 * we will switch to top level mode
 * reparent the list of widgets and then delete the stack
 */
void OWidgetStack::switchTop() {
    m_mode = BigScreen;
    /* this works because it is guaranteed that switchStack was called at least once*/
    if (!m_stack && m_mWidget) {
        m_mWidget->setGeometry( frameRect() );
        return;
    }else if (!m_stack)
        return;

    if (!m_list.isEmpty() ) {
        QMap<int, QWidget*>::Iterator it = m_list.begin();
        for ( ; it != m_list.end(); ++it ) {
            /* better than reparenting twice */
            if ( it.data() == m_mWidget ) {
                m_mWidget->reparent(this, 0, frameRect().topLeft() );
                m_mWidget->setGeometry( frameRect() );
                m_mWidget->show();
            }else
                /* ### FIXME we need to place the widget better */
                it.data()->reparent(0, WType_TopLevel, QPoint(10, 10) );
        }
    }

    delete m_stack;
    m_stack = 0;
}

}
}