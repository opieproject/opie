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

#include <opie/otodoaccess.h>

#include "smalltodo.h"


namespace Todo {
    class MainWindow;

    /**
     * due to inheretince problems we need this base class
     */
    struct ViewBase {
    public:
        virtual QWidget* widget() = 0;
        virtual QString type()const = 0;
        virtual int current() = 0;
        virtual QString currentRepresentation() = 0;
        virtual void showOverDue( bool ) = 0;

        /*
         * update the view
         */
        virtual void updateView() = 0;

        virtual void addEvent( const OTodo& ) = 0;
        virtual void replaceEvent( const OTodo& ) = 0;
        virtual void removeEvent( int uid ) = 0;
        virtual void setShowCompleted( bool ) = 0;
        virtual void setShowDeadline( bool ) = 0;
        virtual void setShowCategory( const QString& = QString::null ) = 0;
        virtual void clear() = 0;
        virtual void newDay() = 0;
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

    protected:
        MainWindow* todoWindow();
        OTodo event(int uid );
        OTodoAccess::List list();
        OTodoAccess::List sorted()const;
        void sort();
        void sort(int sort );
        void setSortOrder( int order );
        void setAscending( bool );

        /*
         * These things needs to be implemented
         * in a implementation
         */
        void showTodo( int uid );
        void edit( int uid );
        void update(int uid, const SmallTodo& to );
        void update(int uid, const OTodo& ev);
        void remove( int uid );
        void complete( int uid );
        void complete( const OTodo& ev );
    private:
        MainWindow *m_main;
        OTodoAccess::List m_sort;
        bool m_asc : 1;
        int m_sortOrder;
    };
};

#endif
