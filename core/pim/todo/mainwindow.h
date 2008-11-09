/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@handhelds.org>
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

#ifndef TODO_MAIN_WINDOW_H
#define TODO_MAIN_WINDOW_H

#include <qlist.h>

#include <opie2/otodoaccess.h>
#include <opie2/opimtodo.h>
#include <opie2/opimmainwindow.h>
#include <opie2/owidgetstack.h>

#include "smalltodo.h"
#include "todoview.h"
#include "quickedit.h"
#include "todomanager.h"

class QPopupMenu;
class QMenuBar;
class QToolBar;
class QAction;
class Ir;
class QVBox;
class QLineEdit;

namespace Todo {
    typedef TodoView View;
    class TemplateManager;
    class Editor;
    class TodoShow;
    struct QuickEditBase;

    class MainWindow : public Opie::OPimMainWindow {
        Q_OBJECT
        friend class TodoView; // avoid QObject here....
        friend class TodoShow; // avoid QObject
    public:
        /* Opie::Core::OApplicationFactory application Name */
        static QString appName() { return QString::fromLatin1("todolist"); }

        MainWindow( QWidget *parent = 0,
                    const char* name = 0, WFlags fl = 0 );
        ~MainWindow();

        /** return a context menu for an OPimTodo */
        QPopupMenu* contextMenu(int uid, bool doesRecur = FALSE );

        void updateList();
        OPimTodoAccess::List list()const;
        OPimTodoAccess::List sorted( bool asc, int sortOrder );
        OPimTodoAccess::List sorted( bool asc, int sortOrder, int addFilter );

        OPimTodo event(int uid );

        bool isSyncing()const;
        bool showCompleted()const;
        bool showDeadline()const;
        bool showOverDue()const;
        bool showQuickTask()const;
        QString currentCategory()const;
        int currentCatId();
        TemplateManager* templateManager();
        QuickEditBase* quickEditor();

        void updateTodo( const OPimTodo& );
        Editor* currentEditor();
        void setReadAhead(uint count );
private slots:
        void slotQuickEntered();
        void slotReload();
        void slotFlush();

    protected:
        void closeEvent( QCloseEvent* e );

    private:
        /* handle setting and removing alarms */
        void handleAlarms( const OPimTodo& oldTodo, const OPimTodo& newTodo );
        void receiveFile( const QString& filename );
        void connectBase( ViewBase* );
        void initUI();
        void initActions();
        void initConfig();
        void initViews();
        void initEditor();
        void initShow();
        void raiseCurrentView();
        ViewBase* currentView();
        ViewBase* m_curView;
        bool m_showing : 1; // used to see if we show a todo in the cases we shouldn't change the table
        QuickEditBase* m_curQuick;
        Editor* m_curEdit;
        TodoShow* currentShow();
        TodoShow* m_curShow;

        QAction* m_editAction,
            *m_deleteAction,
            *m_findAction,
            *m_completedAction,
            *m_showDeadLineAction,
            *m_deleteAllAction,
            *m_deleteCompleteAction,
            *m_duplicateAction,
            *m_showOverDueAction,
            *m_showQuickTaskAction,
            *m_effectiveAction;
        Opie::Ui::OWidgetStack *m_stack;

        bool m_syncing:1;
        bool m_deadline:1;
        bool m_completed:1;
        bool m_overdue:1;
        bool m_quicktask:1;
        TodoManager m_todoMgr;
        QString m_curCat;
        QList<ViewBase> m_views;
        uint m_counter;
        TemplateManager* m_tempManager;


     private slots:
        void initStuff();

        void slotItemNew();
        void slotItemEdit();
        void slotItemDuplicate();
        void slotItemDelete();
        void slotItemBeam();
        void slotItemFind();
        void slotConfigure();

        void slotShow(int);
        void slotEdit(int);
        void slotUpdate3( QWidget* );
        void slotComplete( int uid );
        void slotComplete( const OPimTodo& ev );

        void slotDelete(int uid );
        void slotDeleteAll();
        void slotDeleteCompleted();


        void setCategory( const QString &category );

        void slotShowDeadLine( bool );
        void slotShowCompleted( bool );
        void slotShowQuickTask( bool );

        void setDocument( const QString& );


        void beamDone( Ir* );
        void slotShowDetails();
        void slotShowNext();
        void slotShowPrev();
        void slotShowDue( bool );
        void slotReturnFromView(); // for TodoShow...
        /* reimplementation from opimmainwindow */
    protected slots:
        void flush();
        void reload();
        int create();
        bool remove( int uid );
        void beam(int uid);
        void show( int uid );
        void edit( int uid );
        void add( const Opie::OPimRecord& );
        bool doAlarm( const QDateTime& dt, int uid );
    };
}

#endif
