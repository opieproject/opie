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

#include <opie/otodoaccess.h>
#include <opie/otodo.h>

#include "smalltodo.h"
#include "todoview.h"
#include "todomanager.h"

class QPopupMenu;
class QMenuBar;
class QToolBar;
class QAction;
class QWidgetStack;
class Ir;


namespace Todo {
    typedef TodoView View;
    class TemplateManager;
    class Editor;
    class TodoShow;
    class TemplateEditor;

    class MainWindow : public QMainWindow {
        Q_OBJECT
    public:
        MainWindow( QWidget *parent = 0,
                    const char* name = 0 );
        ~MainWindow();

        /** return a context menu for an OTodo */
        QPopupMenu* contextMenu(int uid );
        QPopupMenu* options();
        QPopupMenu* edit();
        QPopupMenu* view();
        QToolBar*   toolbar();


        void updateList();
        OTodoAccess::List::Iterator begin();
        OTodoAccess::List::Iterator  end();
//        OTodoAccess::List::Iterator &iterator();

        OTodo event(int uid );

        bool isSyncing()const;
        bool showCompleted()const;
        bool showDeadline()const;
        bool showOverDue()const;
        QString currentCategory()const;
        int currentCatId();
        TemplateManager* templateManager();

        void updateTodo( const OTodo& );
        void populateTemplates();
        Editor* currentEditor();
private slots:
        void populateCategories();
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
        void initEditor();
        void initShow();
        void initTemplate();
        void raiseCurrentView();
        ViewBase* currentView();
        ViewBase* m_curView;
        Editor* m_curEdit;
        TodoShow* currentShow();
        TodoShow* m_curShow;
        TemplateEditor* currentTemplateEditor();
        TemplateEditor* m_curTempEd;

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
            *m_view,
            *m_template;

        bool m_syncing:1;
        bool m_deadline:1;
        bool m_completed:1;
        bool m_overdue:1;
        TodoManager m_todoMgr;
        QString m_curCat;
        QList<ViewBase> m_views;
        uint m_counter;
        TemplateManager* m_tempManager;

     private slots:
        void slotShow(int);
        void slotEdit(int);
private slots:
        void slotUpdate3( QWidget* );
        void slotNewFromTemplate(int id );
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
