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

#ifndef HAZE_OBAR_EXTENSION
#define HAZE_OBAR_EXTENSION

#include <qstring.h>
#include <qwidget.h>

class QAction;
class QPopupMenu;

/*
 * ### TODO
 * -Consider Life Updates
 * -Make ValueBased like Opie-featurelist
 */

/**
 * The different screen sizes have different look and feel. On bigger
 * screens only a QToolBar feels strange. One is at least known to have
 * a Help-Menu, a File Menu with Quit/Exit but instead of providing two
 * different ways / two action sets we will group actions to gether and give
 * them attributes when and where we might want them to be visible.
 * We can also group actions. For example you could Group All Actions
 * into the File Group. This means with many actions you would only have more toolbar
 * actions but many sub menus in File. On bigger screen this would automatically
 * expand to a full blown MenuBar and ToolButtons.
 *
 * @short Grouping of actions for dynamic Bar Generation
 * @version 0.01
 * @author hOlgAr
 */
class OBarGroup {
public:
    enum Preference { Allways, Never, IfPlace };
    OBarGroup( const QString& name,  enum Preference groupPreference );
    ~OBarGroup();

    void add( QAction* action, enum Preference menuPreference,
              enum Preference toolpreference );
    void add( const QString&, QPopupMenu* );

    void add( OBarGroup* );

    void remove( QAction* action );
    void remove( QMap* );

protected:
    OBarGroup* parent()const;

private:
};


/**
 * Add your groups in order to the bar manager
 * and either call createGUI() on it, or tell it
 * it the mainwindow and it is listening to the show event
 * and then creates the toolbar
 */
class OBarManager : public QObject{
public:
    OBarManager( QWindow* parent );
    ~OBarManager();

    void add( OBarGroup*, int pos = -1 );
    void remove( OBarGroup* );

    bool eventFilter( QObject* , QEvent* );

public slots:
    void createGUI();
};

/*
 * ### TODO GUI merging
 */

#endif


