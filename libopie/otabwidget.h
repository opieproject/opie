/*
                            This file is part of the Opie Project

                             Copyright (c)  2002 Dan Williams <williamsdr@acm.org>
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

#ifndef OTABWIDGET_H
#define OTABWIDGET_H

#include "otabinfo.h"

#include <qwidget.h>
#include <qlist.h>

class QComboBox;
class QPixmap;
class QTabBar;
class QWidgetStack;

/**
 * @class OTabWidget
 * @brief The OTabWidget class provides a stack of widgets.
 *
 * OTabWidget is a derivation of TrollTech's QTabWidget which provides
 * a stack of widgets.  Widgets can be selected using either a tab bar or
 * drop down list box.
 *
 * The normal way to use OTabWidget is to do the following in the
 * constructor:
 * - Create a OTabWidget.
 * - Create a QWidget for each of the pages in the control, insert
 *   children into it, set up geometry management for it, and use addTab()
 *   to add the widget.
 */
class OTabWidget : public QWidget
{
    Q_OBJECT
public:

/**
 * @enum TabStyle
 * @brief Defines how the widget selection control is displayed.
 *
 * Valid values:
 * - Global: use globally selected options (qpe.conf - TabStyle & TabPosition)
 * - TextTab: Tabbed widget selection with text labels
 * - IconTab: Tabbed widget selection with icon labels, text label for active widget
 *            (similar to Opie launcher)
 * - TextList: Drop down list widget selection with text labels
 * - IconList: Drop down list widget selection with icon & text labels
 */
    enum TabStyle { Global, TextTab, IconTab, TextList, IconList };

/**
 * @enum TabPosition
 * @brief Defines where the widget selection control is drawn.
 *
 * Valid values:
 * - Top: Widget selection control is drawn above widgets
 * - Bottom: Widget selection control is drawn below widgets
 */
    enum TabPosition { Top, Bottom };

/**
 * @fn OTabWidget( QWidget *parent = 0, const char *name = 0, TabStyle s = Global, TabPosition p = Top )
 * @brief Object constructor.
 *
 * @param parent Pointer to parent of this control.
 * @param name Name of control.
 * @param s Style of widget selection control.
 * @param p Position of the widget selection control.
 *
 * Constructs a new OTabWidget control with parent and name.  The style and position parameters
 * determine how the widget selection control will be displayed.
 */
    OTabWidget( QWidget * = 0, const char * = 0, TabStyle = Global, TabPosition = Top );

/**
 * @fn ~OTabWidget()
 * @brief Object destructor.
 */
    ~OTabWidget();

/**
 * @fn addTab( QWidget *child, const QString &icon, const QString &label )
 * @brief Add new widget to control.
 *
 * @param child Widget control.
 * @param icon Path to icon.
 * @param label Text label.
 */
    void addTab( QWidget *, const QString &, const QString & );

/**
 * @fn tabStyle()
 * @brief Returns current widget selection control style.
 */
    TabStyle tabStyle() const;

/**
 * @fn setTabStyle( TabStyle s )
 * @brief Set the current widget selection control style.
 *
 * @param s New style to be used.
 */
    void setTabStyle( TabStyle );

/**
 * @fn tabPosition()
 * @brief Returns current widget selection control position.
 */
    TabPosition tabPosition() const;

/**
 * @fn setTabPosition( TabPosition p )
 * @brief Set the current widget selection control position.
 *
 * @param p New position of widget selection control.
 */
    void setTabPosition( TabPosition );

/**
 * @fn setCurrentTab( QWidget *childwidget )
 * @brief Selects and brings to top the desired widget by using widget pointer.
 *
 * @param childwidget Widget to select.
 */
    void setCurrentTab( QWidget * );

/**
 * @fn setCurrentTab( const QString &tabname )
 * @brief Selects and brings to top the desired widget, by using label.
 *
 * @param tabname Text label for widget to select.
 */
    void setCurrentTab( const QString & );

/**
 * @fn sizeHint()
 * @brief Reimplemented for internal purposes.
 */
    QSize sizeHint() const;


protected:

/**
 * @fn resizeEvent( QResizeEvent * )
 * @brief Reimplemented for internal purposes.
 */
    void resizeEvent( QResizeEvent * );

private:
    OTabInfoList  tabs;
    OTabInfo     *currentTab;

    TabStyle    tabBarStyle;
    TabPosition tabBarPosition;

    QWidgetStack *tabBarStack;
    QTabBar      *tabBar;
    QComboBox    *tabList;

    QWidgetStack *widgetStack;

/**
 * @fn loadSmooth( const QString &name )
 * @brief Loads icon for widget.
 *
 * @param name Name of icon image file.
 */
    QPixmap loadSmooth( const QString & );

/**
 * @fn selectTab( OTabInfo *tab )
 * @brief Internal function to select desired widget.
 *
 * @param tab Pointer to data for widget.
 */
    void    selectTab( OTabInfo * );

/**
 * @fn setUpLayout()
 * @brief Internal function to adjust layout.
 */
    void    setUpLayout();

private slots:

/**
 * @fn slotTabBarSelected( int id )
 * @brief Slot which is called when a tab is selected.
 *
 * @param id ID of widget selected.
 */
    void slotTabBarSelected( int );

/**
 * @fn slotTabListSelected( int index )
 * @brief Slot which is called when a drop down selection is made.
 *
 * @param id Index of widget selected.
 */
    void slotTabListSelected( int );
};

#endif
