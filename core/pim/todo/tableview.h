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

#include "tableitems.h"
#include "todoview.h"

class QTimer;

namespace Todo {
    class CheckItem;
    class DueTextItem;
    class TableView : public QTable, public TodoView {
        Q_OBJECT
    public:
        TableView( MainWindow*, QWidget* parent );
        ~TableView();

        void updateFromTable( const OTodo&, CheckItem* = 0 );
        OTodo find(int uid);

        QString type()const;
        int current();
        QString currentRepresentation();

        void showOverDue( bool );
        void setTodos( OTodoAccess::List::Iterator it,
                       OTodoAccess::List::Iterator end );
        void setTodo( int uid, const OTodo& );
        void addEvent( const OTodo& event );
        void replaceEvent( const OTodo& );
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
        /* reimplented for internal reasons */
        void viewportPaintEvent( QPaintEvent* );
        inline void insertTodo( const OTodo& );
        CheckItem* checkItem( int row );
        DueTextItem* dueItem( int row );
        QTimer *m_menuTimer;
        QMap<int, CheckItem*> m_cache;
        bool m_enablePaint:1;

private slots:
        void slotShowMenu();
        void slotClicked(int, int, int,
                         const QPoint& );
        void slotPressed(int, int, int,
                         const QPoint& );
        void slotValueChanged(int, int);
        void slotCurrentChanged(int, int );
    };
    inline void TableView::insertTodo( const OTodo& event ) {
        int row = numRows();
        setNumRows( row + 1 );


        QString sortKey =  (char) ( (event.isCompleted() ? 'a' : 'A' )
                                    + event.priority() )
                           +  Qtopia::buildSortKey( event.description() );
        CheckItem *chk = new CheckItem( this, sortKey, event.uid(), event.categories() );
        chk->setChecked( event.isCompleted() );

        ComboItem *cmb = new ComboItem(this,  QTableItem::WhenCurrent );
        cmb->setText( QString::number( event.priority() ) );

        QString sum = event.summary();
        QTableItem* ti = new TodoTextItem( this, sum.isEmpty() ?
                                           event.description().left(40).simplifyWhiteSpace() :
                                           sum );
        ti->setReplaceable( FALSE );

        DueTextItem *due = new DueTextItem(this, event );

        setItem( row, 0, chk );
        setItem( row, 1, cmb );
        setItem( row, 2, ti  );
        setItem( row, 3, due );

        m_cache.insert( event.uid(), chk );
    }
};

#endif
