/*
                            This file is part of the Opie Project

                 Copyright (c)  2002 S. Prud'homme <prudhomme@laposte.net>
                                     Dan Williams <williamsdr@acm.org>
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

#ifndef COLORPOPUPMENU_H
#define COLORPOPUPMENU_H

#include <qframe.h>
#include <qpopupmenu.h>

class QWidget;
class QGridLayout;

/**
 * @class ColorPanelButton
 * @brief The ColorPanelButton class provides a button for color selection.
 *
 * @see ColorPopupMenu
 *
 * The ColorPanelButton class provides a button for color selection.  The button
 * is drawn with the desired color and no border.  This class is used internally
 * by the ColorPopupMenu class to displaying colors in its menu.
 */
class ColorPanelButton : public QFrame
{
    Q_OBJECT

public:

/**
 * @fn ColorPanelButton( const QColor& color, QWidget* parent = 0, const char* name = 0 )
 * @brief Object constructor.
 *
 * @param color Desired color.
 * @param parent Pointer to parent of this control.
 * @param name Name of control.
 *
 * Constructs a new ColorPanelButton control with parent, name and desired color.
 */
    ColorPanelButton(const QColor& color, QWidget* parent = 0, const char* name = 0);

/**
 * @fn ~ColorPanelButton()
 * @brief Object destructor.
 */
    ~ColorPanelButton();

/**
 * @fn setActive( bool active )
 * @brief Sets button selection state.
 *
 * @param active Boolean indicator of new button state.
 *
 * Changes button selection state.  If button is selected, a highlighted border
 * is drawn.
 */
    void setActive(bool active);

/**
 * @fn enterEvent( QEvent* e )
 * @brief Reimplemented for internal reasons.
 *
 * @param e Event currently being processed.
 *
 * Reimplemented to ensure correct display of button based on whether it is
 * active or not.
 */
    void enterEvent(QEvent* e);

/**
 * @fn leaveEvent( QEvent* e )
 * @brief Reimplemented for internal reasons.
 *
 * @param e Event currently being processed.
 *
 * Reimplemented to ensure correct display of button based on whether it is
 * active or not.
 */
    void leaveEvent(QEvent* e);

/**
 * @fn paintEvent( QEvent* e )
 * @brief Reimplemented for internal reasons.
 *
 * @param e Event currently being processed.
 *
 * Reimplemented to ensure correct display of button.
 */
    void paintEvent(QPaintEvent* e);

/**
 * @fn mouseReleaseEvent( QMouseEvent* e )
 * @brief Slot executed when button is pressed.
 *
 * @param e Mouse event currently being processed.
 *
 * @see selected()
 *
 * This slot executes when the button has been pressed.  It emits the selected
 * signal as notification that it has been pressed.
 */
    void mouseReleaseEvent(QMouseEvent* e);

signals:

/**
 * @fn selected( const QColor& color )
 * @brief Signal to indicate button has been pressed.
 *
 * @param color Button color.
 *
 * This signal is emitted when the button is pressed.  It provides the color
 * associated to this button.
 */
    void selected(const QColor&);

private:
    QColor m_color;
    bool m_active : 1;
    class ColorPanelButtonPrivate;
    ColorPanelButtonPrivate *d;
};

/**
 * @class ColorPopupMenu
 * @brief The ColorPopupMenu class provides a small color selection
 *        popup menu.
 *
 * ColorPopupMenu is a derivation of TrollTech's QPopupMenu and provides
 * a small color selection popup menu which can be attached to another control
 * such as a toolbar button of menu item.
 *
 * The popup menu displays 30 default colors available in a grid, and also
 * includes an option at the bottom to display a color selection dialog box for
 * finer color control.
 */
class ColorPopupMenu : public QPopupMenu
{
    Q_OBJECT

public:

/**
 * @fn ColorPopupMenu( const QColor& color, QWidget* parent = 0, const char* name = 0 )
 * @brief Object constructor.
 *
 * @param color Initial color selected in menu.
 * @param parent Pointer to parent of this control.
 * @param name Name of control.
 *
 * Constructs a new ColorPopupMenu control with parent, name and initial color selected.
 */
    ColorPopupMenu( const QColor& color, QWidget* parent = 0, const char* name = 0 );

/**
 * @fn ~ColorPopupMenu()
 * @brief Object destructor.
 */
    ~ColorPopupMenu();

private:
    class ColorPopupMenuPrivate;
    ColorPopupMenuPrivate *d;
    QColor       m_color;
    QWidget*     colorPanel;
    QGridLayout* colorLayout;

/**
 * @fn addColor( const QColor& color, int row, int col )
 * @brief Adds color selection option to popup menu.
 *
 * @param color Color to be displayed in menu.
 * @param row Row where color is to appear in menu.
 * @param col Column where color is to appear in menu.
 *
 * Adds a color selection option to popup menu.  Used internally when
 * initially constructing the menu control.
 */
    void addColor( const QColor& color, int row, int col );

signals:

/**
 * @fn colorSelected( const QColor& color )
 * @brief Signal to indicate color chosen from the menu.
 *
 * @param color Color selected from the menu.
 *
 * This signal is emitted when a color has been selected either directly from
 * the menu, or chosen from the color selection dialog.
 */
    void colorSelected( const QColor& color );

protected slots:

/**
 * @fn buttonSelected( const QColor& color )
 * @brief Slot to process selected color.
 *
 * @param color Color selected from the menu.
 *
 * @see colorSelected()
 *
 * This slot executes when a color has been selected from the menu.  It performs
 * two functions:
 * - Emit the colorSelected signal with the color selected.
 * - Hide the menu.
 */
    void buttonSelected( const QColor& color );

/**
 * @fn moreColorClicked()
 * @brief Slot to process display color selection dialog.
 *
 * @see colorSelected()
 *
 * This slot executes when the 'More...' option is selected at the bottom of the menu.
 * It performs the following functions:
 * - Constructs and executes a OColorDialog to allow finer color selection.
 * - Emit the colorSelected signal with the color selected.
 * - Hide the menu.
 */
    void moreColorClicked();
};

#endif // COLORPOPUPMENUANEL_H
