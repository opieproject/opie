/*
                             This file is part of the Opie Project
                             Copyright (C) 2003 Michael Lauer <mickey@tm.informatik.uni-frankfurt.de>
                             Inspired by KDE OGlobalSettings
                             Copyright (C) 2000 David Faure <faure@kde.org>
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

#ifndef OGLOBALSETTINGS_H
#define OGLOBALSETTINGS_H

#include <qstring.h>
#include <qcolor.h>
#include <qfont.h>

/**
 * \todo make real const values
 */
#define OPIE_DEFAULT_SINGLECLICK true
#define OPIE_DEFAULT_INSERTTEAROFFHANDLES true
#define OPIE_DEFAULT_AUTOSELECTDELAY -1
#define OPIE_DEFAULT_CHANGECURSOR true
#define OPIE_DEFAULT_LARGE_CURSOR false
#define OPIE_DEFAULT_VISUAL_ACTIVATE true
#define OPIE_DEFAULT_VISUAL_ACTIVATE_SPEED 50

//FIXME: There's still a whole lot of stuff in here which has to be revised
//FIXME: before public usage... lack of time to do it at once - so it will
//FIXME: happen step-by-step. ML.
// we should not habe too much configure options!!!!!! -zecke

namespace Opie {
namespace Core {

/**
 * Access the OPIE global configuration settings.
 *
 */
class OGlobalSettings
{
  public:

    /**
     * Returns a threshold in pixels for drag & drop operations.
     * As long as the mouse movement has not exceeded this number
     * of pixels in either X or Y direction no drag operation may
     * be started. This prevents spurious drags when the user intended
     * to click on something but moved the mouse a bit while doing so.
     *
     * For this to work you must save the position of the mouse (oldPos)
     * in the @ref QWidget::mousePressEvent().
     * When the position of the mouse (newPos)
     * in a  @ref QWidget::mouseMoveEvent() exceeds this threshold
     * you may start a drag
     * which should originate from oldPos.
     *
     * Example code:
     * <pre>
     * void OColorCells::mousePressEvent( QMouseEvent *e )
     * {
     *    mOldPos = e->pos();
     * }
     *
     * void OColorCells::mouseMoveEvent( QMouseEvent *e )
     * {
     *    if( !(e->state() && LeftButton)) return;
     *
     *    int delay = OGlobalSettings::dndEventDelay();
     *    QPoint newPos = e->pos();
     *    if(newPos.x() > mOldPos.x()+delay || newPos.x() < mOldPos.x()-delay ||
     *       newPos.y() > mOldPos.y()+delay || newPos.y() < mOldPos.y()-delay)
     *    {
     *       // Drag color object
     *       int cell = posToCell(mOldPos); // Find color at mOldPos
     *       if ((cell != -1) && colors[cell].isValid())
     *       {
     *          OColorDrag *d = OColorDrag::makeDrag( colors[cell], this);
     *          d->dragCopy();
     *       }
     *    }
     * }
     * </pre>
     *
     */

    // we do not support DND at the momemt -zecke
    static int dndEventDelay();

    /**
     * Returns whether OPIE runs in single (default) or double click
     * mode.
     *
     * @return @p true if single click mode, or @p false if double click mode.
     *
     * see @ref http://opie.handhelds.org/documentation/standards/opie/style/mouse/index.html
     **/
    static bool singleClick();

    /**
     * Returns whether tear-off handles are inserted in OPopupMenus.
     **/
    // would clutter the small screen -zecke
    static bool insertTearOffHandle();

    /**
     * @return the OPIE setting for "change cursor over icon"
     */
    static bool changeCursorOverIcon();

    /**
     * @return whether to show some feedback when an item (specifically an
     * icon) is activated.
     */
    static bool visualActivate();
    static unsigned int visualActivateSpeed();

    /**
     * Returns the OPIE setting for the auto-select option
     *
     * @return the auto-select delay or -1 if auto-select is disabled.
     */
    static int autoSelectDelay();

    /**
     * Returns the OPIE setting for the shortcut key to open
     * context menus.
     *
     * @return the key that pops up context menus.
     */
    static int contextMenuKey();

    /**
     * Returns the OPIE setting for context menus.
     *
     * @return whether context menus should be shown on button press
     * or button release (click).
     */
    static bool showContextMenusOnPress ();

    /**
     * This enum describes the completion mode used for by the @ref OCompletion class.
     * See <a href="http://opie.handhelds.org/documentation/standards/opie/style/keys/completion.html">
     * the styleguide</a>.
     **/
    enum Completion {
       /**
        * No completion is used.
        */
        CompletionNone=1,
       /**
        * Text is automatically filled in whenever possible.
        */
        CompletionAuto,
       /**
        * Same as automatic except shortest match is used for completion.
        */
        CompletionMan,
       /**
        * Complete text much in the same way as a typical *nix shell would.
        */
        CompletionShell,
       /**
        * Lists all possible matches in a popup list-box to choose from.
        */
        CompletionPopup,
       /**
        * Lists all possible matches in a popup list-box to choose from, and automatically
        * fill the result whenever possible.
        */
        CompletionPopupAuto
    };
    /**
     * Returns the preferred completion mode setting.
     *
     * @return @ref Completion.  Default is @p CompletionPopup.
     */
    static Completion completionMode();

    /**
     * This enum describes the debug mode used for by the @ref odbgstream class.
     * See <a href="http://opie.handhelds.org/documentation/standards/opie/style/debug/debug.html">
     * the styleguide</a>.
     **/
    enum Debug {
       /**
        * Debug messages are ignored.
        */
        DebugNone=-1,
       /**
        * Debug output is sent to files /var/log/.
        */
        DebugFiles=0,
       /**
        * Debug output is written in a QMessageBox.
        */
        DebugMsgBox=1,
       /**
        * Debug output is sent to stderr.
        */
        DebugStdErr=2,
       /**
        * Debug output is sent to syslog.
        */
        DebugSysLog=3,
       /**
        * Debug output is sent via udp over a socket.
        */
        DebugSocket=4
    };
    /**
     * Returns the preferred debug mode setting.
     *
     * @return @ref Debug.  Default is @p DebugStdErr.
     */
    static Debug debugMode();

    /**
     * Returns additional information for debug output (dependent on the debug mode).
     *
     * @return Additional debug output information.
     */
    static QString debugOutput();
    /**
     * This is a structure containing the possible mouse settings.
     */
    struct OMouseSettings
    {
        enum { RightHanded = 0, LeftHanded = 1 };
        int handed; // left or right
    };

    /**
     * This returns the current mouse settings.
     */
    static OMouseSettings & mouseSettings();

    /**
     * The path to the desktop directory of the current user.
     */
     // below handled by Global stuff and QPEApplication
    static QString desktopPath() { initStatic(); return *s_desktopPath; }

    /**
     * The path to the autostart directory of the current user.
     */
    static QString autostartPath() { initStatic(); return *s_autostartPath; }

    /**
     * The path to the trash directory of the current user.    
     */
     // we do not have that concept -zecke
    static QString trashPath() { initStatic(); return *s_trashPath; }

    /**
     * The path where documents are stored of the current user.
     */
    static QString documentPath() { initStatic(); return *s_documentPath; }


    /**
     * The default color to use when highlighting toolbar buttons
     */
    static QColor toolBarHighlightColor();
    static QColor inactiveTitleColor();
    static QColor inactiveTextColor();
    static QColor activeTitleColor();
    static QColor activeTextColor();
    static int contrast();

    /**
     * The default colors to use for text and links.
     */
    static QColor baseColor(); // Similair to QColorGroup::base()
    static QColor textColor(); // Similair to QColorGroup::text()
    static QColor linkColor();
    static QColor visitedLinkColor();
    static QColor highlightedTextColor(); // Similair to QColorGroup::hightlightedText()
    static QColor highlightColor(); // Similair to QColorGroup::highlight()

    /**
     * Returns the alternate background color used by @ref OListView with
     * @ref OListViewItem. Any other list that uses alternating background
     * colors should use this too, to obey to the user's preferences. Returns
     * an invalid color if the user doesn't want alternating backgrounds.
     * @see #calculateAlternateBackgroundColor
     */
    static QColor alternateBackgroundColor();
    /**
     * Calculates a color based on @p base to be used as alternating
     * color for e.g. listviews.
     * @see #alternateBackgroundColor
     */
    static QColor calculateAlternateBackgroundColor(const QColor& base);


    static QFont generalFont();
    static QFont fixedFont();
    static QFont toolBarFont();
    static QFont menuFont();
    static QFont windowTitleFont();
    static QFont taskbarFont();

    /**
     * Returns if the user specified multihead. In case the display
     * has multiple screens, the return value of this function specifies
     * if the user wants OPIE to run on all of them or just on the primary
     */
    static bool isMultiHead();

private:
    /**
     * reads in all paths from kdeglobals
     */
    static void initStatic();
    /**
     * initialise kde2Blue
     */
    static void initColors();
    /**
     * drop cached values for fonts (called by OApplication)
     */
    static void rereadFontSettings();
    /**
     * drop cached values for paths (called by OApplication)
     */
    static void rereadPathSettings();
    /**
     * drop cached values for mouse settings (called by OApplication)
     */
    static void rereadMouseSettings();


    static QString* s_desktopPath;
    static QString* s_autostartPath;
    static QString* s_trashPath;
    static QString* s_documentPath;
    static QFont *_generalFont;
    static QFont *_fixedFont;
    static QFont *_toolBarFont;
    static QFont *_menuFont;
    static QFont *_windowTitleFont;
    static QFont *_taskbarFont;
    static QColor * kde2Gray;
    static QColor * kde2Blue;
    static QColor * kde2AlternateColor;
    static OMouseSettings *s_mouseSettings;

    static QColor * OpieGray;
    static QColor * OpieBlue;
    static QColor * OpieAlternate;   
    static QColor * OpieHighlight;
    
    friend class OApplication;
private:
    class Private;
    Private *d;
};

}
}

#endif
