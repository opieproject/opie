/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 hOlgAr <zecke@handhelds.org>
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

#include <qvaluelist.h>
#include <qvbox.h>

#include <opie/otabwidget.h>

#include "osplitter.h"


struct OSplitterContainer {
    QWidget* widget;
    const QString& icon
    const QString& name;
};


/**
 *
 * This is the constructor of OSplitter
 * You might want to call setSizeChange to tell
 * OSplitter to change its layout when a specefic
 * mark was crossed. OSplitter sets a default value.
 *
 * You cann add widget with addWidget to the OSplitter.
 *
 * @param orient The orientation wether to layout horizontal or vertical
 * @param parent The parent of this widget
 * @param name   The name passed on to QObject
 * @param fl     Additional widgets flags passed to QWidget
 *
 * @short single c'tor of the OSplitter
 */
OSplitter::OSplitter( Orientation orient, QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    m_orient = orient;
    m_hbox = 0;
    m_tabWidget = 0;
    m_size_policy = 330;
}


/**
 * Destructor destructs this object and cleans up. All child
 * widgets will be deleted
 * @see addWidget
 */
OSplitter::~OSplitter() {
    delete m_hbox;
    delete m_tabWidget;
}


/**
 * This function sets the size change policy of the splitter.
 * If this size marked is crossed the splitter will relayout.
 * Note that depending on the set Orientation it'll either look
 * at the width or height.
 *
 * @param width_height The mark that will be watched. Interpreted depending on the Orientation of the Splitter.
 * @return void
 */
void OSplitter::setSizeChange( int width_height ) {
    m_size_policy = width_height;
    relayout();
}

/**
 * Adds a widget to the Splitter. The widgets gets inserted
 * at the end of either the Box or TabWidget.
 * Ownership gets transfered and the widgets gets reparented.
 * Note: icon and label is only available on small screensizes
 *  if size is smaller than the mark
 * Warning: No null checking of the widget is done. Only on debug
 * a message will be outputtet
 *
 * @param wid The widget which will be added
 * @param icon The icon of the possible Tab
 * @param label The label of the possible Tab
 */
void OSplitter::addWidget( QWidget* wid, const QString& icon, const QString& label ) {
#ifdef DEBUG
    if (!wid ) {
        qWarning("Widget is not valid!");
        return;
    }
#endif
    OSplitterContainer cont;
    cont.widget = wid;
    cont.icon =icon;
    cont.label = label;

    m_container.append( cont );

    if (m_hbox )
        addToBox( cont );
    else
        addToTab( cont );
}


/**
 * Removes the widget from the tab widgets. OSplitter drops ownership
 * of this widget and the widget will be reparented to 0.
 * The widget will not be deleted.
 *
 * @param w The widget to be removed
 */
void OSplitter::removeWidget( QWidget* w) {
    /* if not widget nor parent or parent not any of my master childs return */
    if (!w && w->parent() && ( w->parent() != m_hbox || w->parent() != m_tabWidget ) )
        return;

    /* only tab needs to be removed.. box recognizes it */
    if ( !m_hbox )
        removeFromTab( w );


    /* Find the widget, reparent it and remove it from our list */
    ContainerList::Iterator it;
    for ( it = m_container.begin(); it != m_container.end(); ++it )
        if ( (*it).widget == w ) {
            w.reparent( 0, w.getWFlags );
            it = m_container.remove( it );
            break;
        }


}


/**
 * This method will give focus to the widget. If in a tabwidget
 * the tabbar will be changed
 *
 * @param w The widget which will be set the current one
 */
void OSplitter::setCurrentWidget( QWidget*  w) {
    if (m_tabWidget )
        m_tabWidget->setCurrentWidget( w );
    else
        m_hbox->setFocus( w );

}

/**
 * This is an overloaded member function and only differs in the argument it takes.
 * Searches list of widgets for label. It'll pick the first label it finds
 *
 * @param label Label to look for. First match will be taken
 */
void OSplitter::setCurrentWidget( const QString& label ) {
    ContainerList::Iterator it;
    for (it = m_container.begin(); it != m_container.end(); ++it ) {
        if ( (*it).name == label ) {
            setCurrentWidget( (*it).widget );
            break;
        }
    }
}

/**
 * return the currently activated widget if in tab widget moud
 * or null because all widgets are visible
 */
QWidget* OSplitter::currentWidget() {
    if ( m_hbox )
        return 0l;
    else
        return m_tabWidget->currentWidget();
}


/**
 * @reimplented for internal reasons
 * returns the sizeHint of one of its sub widgets
 */
QSize OSplitter::sizeHint()const {
}

/**
 * @reimplemented for internal reasons
 */
void OSplitter::resizeEvent( QResizeEvent* res ) {

}
