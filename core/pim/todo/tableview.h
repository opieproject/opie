/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 <>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
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

#ifndef OPIE_TABLE_VIEW_H
#define OPIE_TABLE_VIEW_H

#include <qtable.h>
#include <qmap.h>

#include "todoview.h"

using namespace Opie;

class QTimer;
namespace Todo {
    class CheckItem;
    class DueTextItem;
    class TableView : public QTable, public TodoView {
        Q_OBJECT
    public:
        TableView( MainWindow*, QWidget* parent );
        ~TableView();

        void updateFromTable( const ToDoEvent&, CheckItem* = 0 );
        ToDoEvent find(int uid);

        QString type()const;
        int current();
        QString currentRepresentation();

        void showOverDue( bool );
        void setTodos( ToDoDB::Iterator it,
                       ToDoDB::Iterator end );
        void setTodo( int uid, const ToDoEvent& );
        void addEvent( const ToDoEvent& event );
        void replaceEvent( const ToDoEvent& );
        void removeEvent( int uid );
        void setShowCompleted( bool );
        void setShowDeadline( bool );

        void setShowCategory(const QString& =QString::null );
        void clear();
        void newDay();
        QArray<int> completed();
        QWidget* widget();
        void sortColumn(int, bool, bool );
    private:
        QMap<int, CheckItem*> m_cache;
        void insertTodo( const ToDoEvent& );
        CheckItem* checkItem( int row );
        DueTextItem* dueItem( int row );
        QTimer *m_menuTimer;
private slots:
        void slotShowMenu();
        void slotClicked(int, int, int,
                         const QPoint& );
        void slotPressed(int, int, int,
                         const QPoint& );
        void slotValueChanged(int, int);
        void slotCurrentChanged(int, int );
    };
};

#endif
