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

#ifndef TODO_VIEW_H
#define TODO_VIEW_H

#include <qarray.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qwidget.h>

#include <opie/tododb.h>

#include "smalltodo.h"

using namespace Opie;

namespace Todo {

    /**
     * According to tronical it's not possible
     * to have Q_OBJECT in a template at all
     * so this is a hack widget not meant
     * to be public
     */
    class InternQtHack : public QObject {
        Q_OBJECT
    public:
        InternQtHack() : QObject() {};
        void emitShow(int uid) { emit showTodo(uid);  }
        void emitEdit(int uid) { emit edit(uid ); }
        void emitUpdate( int uid,
                         const SmallTodo& to) {
            emit update(uid, to );
        }
        void emitUpdate( int uid,
                         const ToDoEvent& ev ){
            emit update(uid, ev );
        }
        void emitRemove( int uid ) {
            emit remove( uid );
        }
        void emitUpdate( QWidget* wid ) {
            emit update( wid );
        }
    signals:
        void showTodo(int uid );
        void edit(int uid );
        void update( int uid, const SmallTodo& );
        void update( int uid, const ToDoEvent& );
        /* sorry you need to cast */;
        void update( QWidget* wid );
        void remove( int uid );

    };
    class MainWindow;

    /**
     * due to inheretince problems we need this base class
     */
    class ViewBase {
    public:
        virtual QWidget* widget() = 0;
        virtual QString type()const = 0;
        virtual int current() = 0;
        virtual QString currentRepresentation() = 0;
        virtual void showOverDue( bool ) = 0;
        virtual void setTodos( ToDoDB::Iterator it,
                               ToDoDB::Iterator end ) = 0;
        virtual void setTodo(int uid,  const ToDoEvent& ) = 0;
        virtual void addEvent( const ToDoEvent& ) = 0;
        virtual void replaceEvent( const ToDoEvent& ) = 0;
        virtual void removeEvent( int uid ) = 0;
        virtual void setShowCompleted( bool ) = 0;
        virtual void setShowDeadline( bool ) = 0;
        virtual void setShowCategory( const QString& = QString::null ) = 0;
        virtual void clear() = 0;
        virtual QArray<int> completed() = 0;
        virtual void newDay() = 0;

        virtual void connectShow( QObject*, const char* ) = 0;
        virtual void connectEdit( QObject*, const char* ) = 0;
        virtual void connectUpdateSmall( QObject*, const char* ) = 0;
        virtual void connectUpdateBig( QObject*, const char* ) = 0;
        virtual void connectUpdateView( QObject*, const char*) = 0;

    };

    /**
     * A base class for all TodoView which are showing
     * a list of todos.
     * Either in a QTable, QListView or any other QWidget
     * derived class
     * Through the MainWindow( dispatcher ) one can access
     * the relevant informations
     *
     * It's not possible to have signal and slots from within
     * templates this way you've to register for a signal
     */
    class TodoView : public ViewBase{

    public:
        /**
         * c'tor
         */
        TodoView( MainWindow* win );

        /**
         *d'tor
         */
        virtual ~TodoView();

        /* connect to the show signal */
        void connectShow(QObject* obj,
                         const char* slot );

        /* connect to edit */
        void connectEdit( QObject* obj,
                          const char* slot );
        void connectUpdateSmall( QObject* obj,
                                 const char* slot );
        void connectUpdateBig( QObject* obj,
                               const char* slot ) ;
        void connectUpdateView( QObject* obj,
                                const char* slot );
    protected:
        MainWindow* todoWindow();
        ToDoEvent event(int uid );
        ToDoDB::Iterator begin();
        ToDoDB::Iterator end();

        /*
          These things needs to be implemented
          in a implementation
    signals:
        */
    protected:
        void showTodo( int uid ) { hack->emitShow(uid); }
        void edit( int uid ) { hack->emitEdit(uid); }
        void update(int uid, const SmallTodo& to ) {
            hack->emitUpdate(uid, to );
        }
        void update(int uid, const ToDoEvent& ev) {
            hack->emitUpdate(uid, ev );
        }
        void remove( int uid ) {
            hack->emitRemove( uid );
        }
    private:
        InternQtHack* hack;
        MainWindow *m_main;
    };
};

#endif
