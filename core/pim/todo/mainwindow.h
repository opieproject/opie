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

#include <opie/otodoaccess.h>
#include <opie/otodo.h>
#include <opie/opimmainwindow.h>

#include "smalltodo.h"
#include "todoview.h"
#include "quickedit.h"
#include "todomanager.h"

class QPopupMenu;
class QMenuBar;
class QToolBar;
class QAction;
class QWidgetStack;
class Ir;
class QVBox;

namespace Todo {
    typedef TodoView View;
    class TemplateManager;
    class Editor;
    class TodoShow;
    class TemplateEditor;
    struct QuickEditBase;

    class MainWindow : public OPimMainWindow {
        Q_OBJECT
        friend class TodoView; // avoid QObject here....
    public:
        MainWindow( QWidget *parent = 0,
                    const char* name = 0 );
        ~MainWindow();

        /** return a context menu for an OTodo */
        QPopupMenu* contextMenu(int uid, bool doesRecur = FALSE );
        QPopupMenu* options();
        QPopupMenu* edit();
        QToolBar*   toolbar();


        void updateList();
        OTodoAccess::List list()const;
        OTodoAccess::List sorted( bool asc, int sortOrder );
        OTodoAccess::List sorted( bool asc, int sortOrder, int addFilter );

        OTodo event(int uid );

        bool isSyncing()const;
        bool showCompleted()const;
        bool showDeadline()const;
        bool showOverDue()const;
        QString currentCategory()const;
        int currentCatId();
        TemplateManager* templateManager();
        QuickEditBase* quickEditor();

        void updateTodo( const OTodo& );
        void populateTemplates();
        Editor* currentEditor();
        void setReadAhead(uint count );
private slots:
        void slotQuickEntered();
        void populateCategories();
        void slotReload();
        void slotFlush();

    protected:
        void closeEvent( QCloseEvent* e );

    private:
        void receiveFile( const QString& filename );
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
        QuickEditBase* m_curQuick;
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
            *m_template;
        /* box with two rows
         * top will be the quick edit
         * this will bite my ass once
         * we want to have all parts
         * exchangeable
         */
        QVBox* m_mainBox;

        bool m_syncing:1;
        bool m_deadline:1;
        bool m_completed:1;
        bool m_overdue:1;
        TodoManager m_todoMgr;
        QString m_curCat;
        QList<ViewBase> m_views;
        QList<QuickEditBase> m_quickEdit;
        uint m_counter;
        TemplateManager* m_tempManager;


     private slots:
        void slotShow(int);
        void slotEdit(int);
        void slotUpdate3( QWidget* );
        void slotComplete( int uid );
        void slotComplete( const OTodo& ev );
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
        /* reimplementation from opimmainwindow */
    protected slots:
        void flush();
        void reload();
        int create();
        bool remove( int uid );
        void beam(int uid, int transport = IrDa );
        void show( int uid );
        void edit( int uid );
        void add( const OPimRecord& );
        OPimRecord* record( int rtti, const QByteArray& );
    };
};

#endif
