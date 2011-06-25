/*
                             This file is part of the Opie Project
               =.            Copyright (C) 2005 Holger Freyther <freyther@handhelds.org>
             .=l.            Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can 
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.        
    .i_,=:_.      -<s.       This library is distributed in the hope that  
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-        
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB. 
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include <qwidget.h>
#include <qtoolbutton.h>
#include <qmap.h>

class QPopupMenu;

namespace Opie {
namespace Core {
class OStorageInfo;
}
}

namespace Opie {
namespace Ui   {

/**
 * @short a toolbar button for selecting filesystems
 *
 * Provides a QToolButton that drops down a file system selection menu.
 *
 * @see QToolButton
 * @see OFileDialog
 */
class OFileSystemButton : public QToolButton
{
    Q_OBJECT
public:
    /**
     * Constructs a new OFileSystemButton
     * @param parent the parent widget
     * @param showHome true to show the user's home directory as the first item, false otherwise
     */
    OFileSystemButton( QWidget *parent, bool showHome = false );
    ~OFileSystemButton();
signals:
    /**
     * emitted when a user selects a filesystem from the popup menu
     */
    void changeDir( const QString &dir );
protected slots:
    void slotFSActivated( int id );
    void slotPopUp();
    void slotDisksChanged();
protected:
    QPopupMenu *m_popup;
    QMap<QString, QString> m_dev;
    Opie::Core::OStorageInfo *m_storage;
    bool m_showHome;
};

}
}
