/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Holger Freyther <zecke@handhelds.org>
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

#ifndef TODO_MAIN_WINDOW_H
#define TODO_MAIN_WINDOW_H

#include <qlist.h>
#include <qmainwindow.h>

#include <opie/tododb.h>
#include <opie/todoevent.h>

#include "todoview.h"
#include "todomanager.h"

class QPopupMenu;
class QMenuBar;
class QToolBar;
class QAction;
class QWidgetStack;
class Ir;

using namespace Opie;

namespace Todo {
    typedef TodoView<QWidget> View;
    class MainWindow : public QMainWindow {
        Q_OBJECT
    public:
        MainWindow( QWidget *parent = 0,
                    const char* name = 0 );
        ~MainWindow();

        /** return a context menu for a ToDoEvent */
        QPopupMenu* contextMenu(int uid );
        QPopupMenu* options();
        QPopupMenu* edit();
        QPopupMenu* view();
        QToolBar*   toolbar();


        ToDoDB::Iterator begin();
        ToDoDB::Iterator end();
        ToDoEvent event(int uid );

        bool isSyncing()const;
        bool showCompleted()const;
        bool showDeadline()const;
        bool showOverDue()const;
        QString currentCategory()const;
        int currentCatId();
private slots:
        void slotReload();
        void slotFlush();

    protected:
        void closeEvent( QCloseEvent* e );

    private:
        void connectBase( ViewBase* );
        void initUI();
        void initActions();
        void initConfig();
        void initViews();
        void populateCategories();
        void raiseCurrentView();
        ViewBase* currentView();
        ViewBase* m_curView;

        QMenuBar* m_bar;
        QToolBar* m_tool;
        QAction* m_editAction,
            *m_deleteAction,
            *m_findAction,
            *m_completedAction,
            *m_showDeadLineAction,
            *m_deleteAllAction,
            *m_deleteCompleteAction,
            *m_duplicateAction,
            *m_showOverDueAction,
            *m_effectiveAction;
        QWidgetStack *m_stack;
        QPopupMenu* m_catMenu,
            *m_edit,
            *m_options,
            *m_view;

        bool m_syncing:1;
        bool m_deadline:1;
        bool m_completed:1;
        bool m_overdue:1;
        TodoManager m_todoMgr;
        QString m_curCat;
        QList<ViewBase> m_views;
        uint m_counter;

     private slots:
        void slotNew();
        void slotDuplicate();
        void slotDelete();
        void slotDeleteAll();
        void slotDeleteCompleted();

        void slotEdit();
        void slotFind();

        void setCategory( int );

        void slotShowDeadLine( bool );
        void slotShowCompleted( bool );

        void setDocument( const QString& );


        void slotBeam();
        void beamDone( Ir* );
        void slotShowDetails();
        void slotShowDue( bool );
    };
};

#endif
