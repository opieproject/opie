/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@lists.sourceforge.net>
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

#include <unistd.h>

#include <opie2/odebug.h>
#include <opie2/opimrecurrence.h>
#include <opie2/opimnotifymanager.h>
#include <opie2/otodoaccessvcal.h>
#include <opie2/oapplicationfactory.h>
#include <opie2/opimalarmdlg.h>
#include <opie2/oresource.h>
#include <opie2/opimautoconvert.h>

#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/ir.h>
#include <qpe/qpemessagebox.h>
#include <qpe/alarmserver.h>
#include <qpe/qpeapplication.h>

#include <qaction.h>
#include <qlayout.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <qwhatsthis.h>

#include "quickeditimpl.h"
#include "todotemplatemanager.h"
#include "templatedialogimpl.h"
#include "tableview.h"

#include "textviewshow.h"
#include "todoeditor.h"
#include "newtaskdlg.h"
#include "mainwindow.h"

using Opie::Core::OApplicationFactory;
OPIE_EXPORT_APP( OApplicationFactory<Todo::MainWindow> )

using namespace Opie;
using namespace Todo;

MainWindow::MainWindow( QWidget* parent,
                        const char* name, WFlags )
    : Opie::OPimMainWindow( "Todolist", "Todo List", tr( "Task" ), "todo",
                            parent, name, WType_TopLevel | WStyle_ContextHelp )
{
    setCaption( tr( "Todo List" ) );
    if (!name)
        setName("todo window");

    m_syncing = false;
    m_showing = false;
    m_counter = 0;
    m_tempManager = new TemplateManager();
    m_tempManager->load();

    initBars();

    initConfig();
    initUI();
    initViews();
    initActions();
    initEditor();
    initShow();

    m_forceClose = false;
    if( ! OPimAutoConverter::promptConvertData( Pim::OPimGlobal::TODOLIST, this, caption() ) ) {
        m_forceClose = true;
        QTimer::singleShot(0, qApp, SLOT(quit() ) );
        return;
    }

    connect( qApp, SIGNAL( appMessage(const QCString&,const QByteArray&) ),
             this, SLOT( slotAppMessage(const QCString&,const QByteArray&) ) );

    raiseCurrentView();
    QTimer::singleShot( 0, this, SLOT(initStuff()) );
}

void MainWindow::initStuff()
{
    m_todoMgr.load();
    setViewCategory( m_curCat );
    setCategory( m_curCat );
}

void MainWindow::initActions()
{
    // Insert Task menu items
    QActionGroup *items = new QActionGroup( this, QString::null, false );

    m_deleteCompleteAction = new QAction( QString::null, QWidget::tr( "Delete completed" ),
                                          0, items, 0 );
    connect( m_deleteCompleteAction, SIGNAL(activated()), this, SLOT(slotDeleteCompleted()) );

    insertItemMenuItems( items );

    // Insert View menu items
    items = new QActionGroup( this, QString::null, false );

    m_completedAction = new QAction( QString::null, QWidget::tr("Show completed tasks"),
                                     0, items, 0, true );
    m_completedAction->setOn( showCompleted() );
    connect( m_completedAction, SIGNAL(toggled(bool)), this, SLOT(slotShowCompleted(bool)) );

    QAction *a = new QAction( QString::null, QWidget::tr("Show only over-due tasks"),
                              0, items, 0, true );
    a->setOn( showOverDue() );
    connect( a, SIGNAL(toggled(bool)), this, SLOT(slotShowDue(bool)) );

    m_showDeadLineAction = new QAction( QString::null, QWidget::tr("Show task deadlines"),
                                        0, items, 0, true );
    m_showDeadLineAction->setOn( showDeadline() );
    connect( m_showDeadLineAction, SIGNAL(toggled(bool)), this, SLOT(slotShowDeadLine(bool)) );

    m_showQuickTaskAction = new QAction( QString::null, QWidget::tr("Show quick task bar"),
                                         0, items, 0, true );
    m_showQuickTaskAction->setOn( showQuickTask() );
    connect( m_showQuickTaskAction, SIGNAL(toggled(bool)), this, SLOT(slotShowQuickTask(bool)) );

    insertViewMenuItems( items );
}

/* m_curCat from Config */
void MainWindow::initConfig()
{
    Config config( "todo" );
    config.setGroup( "View" );
    m_completed =  config.readBoolEntry( "ShowComplete", true );
    m_curCat = config.readEntry( "Category", QString::null );
    m_deadline =  config.readBoolEntry( "ShowDeadLine", true);
    m_overdue = config.readBoolEntry("ShowOverDue", false );
    m_quicktask = config.readBoolEntry("ShowQuickTask", true);
}

void MainWindow::initUI()
{
    // Load priority pixmaps
    QString namestr;
    for ( unsigned int i = 1; i < 6; i++ ) {
        namestr = "todo/priority";
        namestr.append( QString::number( i ) );
        m_pic_priority.append( Opie::Core::OResource::loadPixmap( namestr ) );
    }

    // Create main widget stack
    m_stack = new Opie::Ui::OWidgetStack(this,  "main stack");
    setCentralWidget( m_stack );
    connect( this, SIGNAL(categorySelected(const QString&)),
            this, SLOT(setCategory(const QString&)) );

    // Create quick task toolbar
    m_curQuick = new QuickEditImpl( this, m_quicktask, m_pic_priority );
    addToolBar( (QToolBar *)m_curQuick->widget(), QWidget::tr( "QuickEdit" ),
                QMainWindow::Top, true );
    m_curQuick->signal()->connect( this, SLOT(slotQuickEntered()) );
}

void MainWindow::initViews()
{
    TableView* tableView = new TableView( this, m_stack, m_pic_priority );
    QWhatsThis::add( tableView, QWidget::tr( "This is a listing of all current tasks.\n\nThe list displays the following information:\n1. Completed - A green checkmark indicates task is completed.  Click here to complete a task.\n2. Priority - a graphical representation of task priority.  Double-click here to modify.\n3. Description - description of task.  Click here to select the task.\n4. Deadline - shows when task is due.  This column can be shown or hidden by selecting Options->'Show task deadlines' from the menu above." ) );
    m_stack->addWidget( tableView,  m_counter++ );
    m_views.append( tableView );
    m_curView = tableView;
    connectBase( tableView );
    /* add QString type + QString configname to
     * the View menu
     * and subdirs for multiple views
     */
}

void MainWindow::initEditor()
{
    m_curEdit = new Editor( m_pic_priority );
}

void MainWindow::initShow()
{
    m_curShow = new TextViewShow(this, this);
    m_stack->addWidget( m_curShow->widget() , m_counter++ );
}

MainWindow::~MainWindow()
{
    delete templateManager();
}

void MainWindow::connectBase( ViewBase* )
{
    // once templates and signals mix we'll use it again
}

QPopupMenu* MainWindow::contextMenu( int , bool /*recur*/ )
{
    return itemContextMenu();
}

OPimTodoAccess::List MainWindow::list() const
{
    return m_todoMgr.list();
}

OPimTodoAccess::List MainWindow::sorted( bool asc, int sortOrder )
{
    int cat = 0;
    if ( m_curCat != tr( "All" ) )
        cat = currentCatId();
    if ( m_curCat == tr( "Unfiled" ) )
        cat = -1;

    int filter = OPimTodoAccess::FilterCategory;

    if (!m_completed )
        filter |= OPimTodoAccess::DoNotShowCompleted;
    if (m_overdue)
        filter |= OPimTodoAccess::OnlyOverDue;

    return m_todoMgr.sorted( asc, sortOrder, filter, cat );
}

OPimTodoAccess::List MainWindow::sorted( bool asc, int sortOrder, int addFilter)
{
    int cat = 0;
    if ( m_curCat != tr( "All" ) )
        cat = currentCatId();

    if ( m_curCat == tr( "Unfiled" ) )
        cat = -1;

    return m_todoMgr.sorted(asc, sortOrder, addFilter,  cat );
}

OPimTodo MainWindow::event( int uid )
{
    return m_todoMgr.event( uid );
}

bool MainWindow::isSyncing() const
{
    return m_syncing;
}

TemplateManager* MainWindow::templateManager()
{
    return m_tempManager;
}

Editor* MainWindow::currentEditor()
{
    return m_curEdit;
}

TodoShow* MainWindow::currentShow()
{
    return m_curShow;
}

void MainWindow::slotReload()
{
    m_syncing = false;
    m_todoMgr.reload();
    currentView()->updateView( );
    raiseCurrentView();
}

void MainWindow::closeEvent( QCloseEvent* e )
{
    if ( m_forceClose ) {
        e->accept();
        return;
    }
    
    if ( m_stack->visibleWidget() == currentShow()->widget() ) {
        m_showing = false;
        raiseCurrentView();
        e->ignore();
        return;
    }
    /*
     * we should have flushed and now we're still saving
     * so there is no need to flush
     */
    if (m_syncing ) {
        e->accept();
        return;
    }
    bool quit = false;
    if ( m_todoMgr.saveAll() ){
        quit = true;
    }
    else {
        if ( QMessageBox::critical( this, QWidget::tr("Out of space"),
                                    QWidget::tr("Todo was unable\n"
                                       "to save your changes.\n"
                                       "Free up some space\n"
                                       "and try again.\n"
                                       "\nQuit Anyway?"),
                                    QMessageBox::Yes|QMessageBox::Escape,
                                    QMessageBox::No|QMessageBox::Default)
             != QMessageBox::No ) {
            e->accept();
            quit = true;
        }
        else
            e->ignore();

    }

    if (quit ) {
        Config config( "todo" );
        config.setGroup( "View" );
        config.writeEntry( "ShowComplete", showCompleted() );
        config.writeEntry( "Category", currentCategory() );
        config.writeEntry( "ShowDeadLine", showDeadline());
        config.writeEntry( "ShowOverDue", showOverDue() );
        config.writeEntry( "ShowQuickTask", showQuickTask() );
        /* save templates */
        templateManager()->save();
        e->accept();
        QTimer::singleShot(0, qApp, SLOT(closeAllWindows()) );
    }
}

void MainWindow::slotItemNew()
{
    QStringList templateList = templateManager()->templates();
    if( templateList.isEmpty() ) {
        // No templates, just create a blank task
        create();
    }
    else {
        // There are templates, so allow the user to select one
        NewTaskDlg dlg( templateList, this );
        if ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted ) {
            QString tempName = dlg.tempSelected();
            if ( tempName.isNull() ) {
                // Create new, blank task
                create();
            }
            else {
                // Create new task from the template selected
                OPimTodo event = templateManager()->templateEvent( tempName );
                event = currentEditor()->edit( this, event );
                if ( currentEditor()->accepted() ) {
                        event.setUid( 1 );
                        handleAlarms( OPimTodo(), event );
                        m_todoMgr.add( event );
                        currentView()->addEvent( event );

                        reloadCategories();
                }
                raiseCurrentView();
            }
        }
    }
}

void MainWindow::slotItemEdit()
{
    slotEdit( currentView()->current() );
}

void MainWindow::slotItemDuplicate() {
    if(m_syncing) {
        QMessageBox::warning(this, QWidget::tr("Todo"),
                             QWidget::tr("Data can not be edited, currently syncing"));
        return;
    }
    OPimTodo ev = m_todoMgr.event( currentView()->current() );
    /* let's generate a new uid */
    ev.setUid(1);
    m_todoMgr.add( ev );

    currentView()->addEvent( ev );
    raiseCurrentView();
}

void MainWindow::slotItemDelete()
{
    if ( !currentView()->current() )
        return;

    if( m_syncing ) {
        QMessageBox::warning(this, QWidget::tr("Todo"),
                             QWidget::tr("Data can not be edited, currently syncing"));
        return;
    }
    QString strName = currentView()->currentRepresentation();
    if (!QPEMessageBox::confirmDelete(this, QWidget::tr("Todo"), strName ) )
        return;

    handleAlarms( m_todoMgr.event( currentView()->current() ), OPimTodo() );
    m_todoMgr.remove( currentView()->current() );
    currentView()->removeEvent( currentView()->current() );
    raiseCurrentView();
}

static const char *beamfile = "/tmp/opie-todo.vcs";

void MainWindow::slotItemBeam()
{
    beam( currentView()->current() );
}

void MainWindow::slotItemFind()
{
}

void MainWindow::slotConfigure()
{
    TemplateDialogImpl dlg( this, m_tempManager );
    if ( QPEApplication::execDialog( &dlg ) != QDialog::Accepted )
        m_tempManager->load();
}

void MainWindow::slotDelete(int uid )
{
    if( uid == 0 )
        return;

    if( m_syncing ) {
        QMessageBox::warning(this, QWidget::tr("Todo"),
                             QWidget::tr("Data can not be edited, currently syncing"));
        return;
    }

    OPimTodo to = m_todoMgr.event(uid);
    if (!QPEMessageBox::confirmDelete(this, QWidget::tr("Todo"), to.toShortText() ) )
        return;

    handleAlarms(to, OPimTodo() );
    m_todoMgr.remove( to.uid() );
    currentView()->removeEvent( to.uid() );
    raiseCurrentView();
}

void MainWindow::slotDeleteAll()
{
    if( m_syncing ) {
        QMessageBox::warning(this, QWidget::tr("Todo"),
                             QWidget::tr("Data can not be edited, currently syncing"));
        return;
    }


    if ( !QPEMessageBox::confirmDelete( this, QWidget::tr( "Todo" ), QWidget::tr("all tasks?") ) )
        return;

    m_todoMgr.removeAll();
    currentView()->clear();

    raiseCurrentView();
}

void MainWindow::slotDeleteCompleted()
{
    if( m_syncing ) {
        QMessageBox::warning(this, QWidget::tr("Todo"),
                             QWidget::tr("Data can not be edited, currently syncing"));
        return;
    }

    if ( !QPEMessageBox::confirmDelete( this, QWidget::tr( "Todo" ), QWidget::tr("all completed tasks?") ) )
        return;

    m_todoMgr.removeCompleted();
    currentView()->updateView( );
}

/*
 * set the category
 */
void MainWindow::setCategory( const QString &category )
{
    m_curCat = category;
    if ( m_curCat == tr( "All" ) )
        m_curCat = QString::null;

    currentView()->setShowCategory( m_curCat );
    raiseCurrentView();
}

void MainWindow::slotShowDeadLine( bool dead )
{
    m_deadline = dead;
    currentView()->setShowDeadline( dead );
}

void MainWindow::slotShowCompleted( bool show )
{
    m_completed = show;
    currentView()->setShowCompleted( m_completed );
}

void MainWindow::slotShowQuickTask( bool show )
{
    m_quicktask = show;

    bool showVisible = ( m_stack->visibleWidget() == currentShow()->widget() );
    showHideQuickTask( showVisible );
}

void MainWindow::showHideQuickTask( bool showVisible )
{
    if ( m_quicktask && !showVisible ) {
        if( !m_curQuick->widget()->isVisible() )
            m_curQuick->widget()->show();
    }
    else {
        if( m_curQuick->widget()->isVisible() )
            m_curQuick->widget()->hide();
    }
}

bool MainWindow::showOverDue() const
{
    return m_overdue;
}

void MainWindow::setDocument( const QString& fi)
{
    DocLnk doc(fi);
    if ( doc.isValid() )
        receiveFile(doc.file() );
    else
        receiveFile(fi );
}

void MainWindow::beamDone( Ir* ir)
{
    delete ir;
    ::unlink( beamfile );
}

void MainWindow::receiveFile( const QString& filename )
{
    OPimTodoAccessVCal* cal = new OPimTodoAccessVCal(filename );

    OPimTodoAccess acc( cal );
    OPimTodoAccess::List list;
    if ( acc.load() )
	list = acc.allRecords();

    if (list.count()) {

        QString message = QWidget::tr("<P>%1 new tasks arrived.<p>Would you like to add them to your Todolist?").arg(list.count() );

        if ( QMessageBox::information(this, QWidget::tr("New Tasks"),
                                      message, QMessageBox::Ok,
                                      QMessageBox::Cancel ) == QMessageBox::Ok ) {
            OPimTodoAccess::List::Iterator it;
            for ( it = list.begin(); it != list.end(); ++it )
                m_todoMgr.add( (*it) );

            currentView()->updateView();
        }
    }
}

void MainWindow::slotFlush() {
    m_syncing = true;
    m_todoMgr.save();
}

void MainWindow::slotShowDetails()
{
    slotShow( currentView()->current() );
}

bool MainWindow::showCompleted() const
{
    return m_completed;
}

bool MainWindow::showDeadline() const
{
    return m_deadline;
}

bool MainWindow::showQuickTask() const
{
    return m_quicktask;
}

QString MainWindow::currentCategory() const
{
    return m_curCat;
}

int MainWindow::currentCatId()
{
    return m_todoMgr.catId( m_curCat );
}

ViewBase* MainWindow::currentView()
{
    return m_curView;
}

void MainWindow::raiseCurrentView()
{
    // due QPE/Application/todolist show(int)
    // we might not have the populateCategories slot called once
    // we would show the otodo but then imediately switch to the currentView
    // if we're initially showing we shouldn't raise the table
    // in returnFromView we fix up m_showing
    if ( m_showing )
        return;

    m_stack->raiseWidget( m_curView->widget() );
}

void MainWindow::slotShowDue( bool ov )
{
    m_overdue = ov;
    currentView()->showOverDue( ov );
    raiseCurrentView();
}

void MainWindow::slotShow( int uid )
{
    if ( uid == 0 )
        return;

    currentShow()->slotShow( event( uid ) );
    m_stack->raiseWidget( currentShow()->widget() );

    showHideQuickTask( true );
}

void MainWindow::slotShowNext()
{
    int l = currentView()->next();
    if (l!=0)
        slotShow(l);
}

void MainWindow::slotShowPrev()
{
    int l = currentView()->prev();
    if (l!=0)
        slotShow(l);
}

void MainWindow::slotEdit( int uid )
{
    if ( uid == 0 )
        return;

    if( m_syncing ) {
        QMessageBox::warning(this, QWidget::tr("Todo"),
                             QWidget::tr("Data can't be edited, currently syncing"));
        return;
    }

    OPimTodo old_todo = m_todoMgr.event( uid );

    OPimTodo todo = currentEditor()->edit(this, old_todo );

    /* if completed */
    if ( currentEditor()->accepted() ) {
        handleAlarms( old_todo, todo );
        m_todoMgr.update( todo.uid(), todo );
        currentView()->replaceEvent( todo );
        /* a Category might have changed */
        reloadCategories();
    }

    raiseCurrentView();
}

/*
void MainWindow::slotUpdate1( int uid, const SmallTodo& ev)
{
    m_todoMgr.update( uid, ev );
}
*/

void MainWindow::updateTodo(  const OPimTodo& ev)
{
    m_todoMgr.update( ev.uid() , ev );
}

/* The view changed it's configuration
 * update the view menu
 */
void MainWindow::slotUpdate3( QWidget* )
{
}

void MainWindow::updateList()
{
    m_todoMgr.updateList();
}

void MainWindow::setReadAhead( uint count )
{
    if (m_todoMgr.todoDB() )
        m_todoMgr.todoDB()->setReadAhead( count );
}

void MainWindow::slotQuickEntered()
{
    OPimTodo todo = quickEditor()->todo();
    if ( todo.isEmpty() )
        return;

    m_todoMgr.add( todo );
    currentView()->addEvent( todo );
    raiseCurrentView();
}

QuickEditBase* MainWindow::quickEditor()
{
    return m_curQuick;
}

void MainWindow::slotComplete( int uid )
{
    slotComplete( event(uid) );
}

void MainWindow::slotComplete( const OPimTodo& todo )
{
    OPimTodo to = todo;
    to.setCompleted( !to.isCompleted() );
    to.setCompletedDate( QDate::currentDate() );

    /*
     * if the item does recur
     * we need to spin it off
     * and update the items duedate to the next
     * possible recurrance of this item...
     * the spinned off one will loose the
     * recurrence.
     * We calculate the difference between the old due date and the
     * new one and add this diff to start, completed and alarm dates
     * -zecke
     */
    if ( to.hasRecurrence() && to.isCompleted() ) {
        OPimTodo to2( to );

        /* the spinned off one won't recur anymore */
        to.setRecurrence( OPimRecurrence() );

        OPimRecurrence rec = to2.recurrence();
        rec.setStart( to.dueDate() );
        to2.setRecurrence( rec );
        /*
         * if there is a next occurence
         * from the duedate of the last recurrance
         */
        QDate date;
        if ( to2.recurrence().nextOcurrence( to2.dueDate().addDays(1), date ) ) {
            int dayDiff = to.dueDate().daysTo( date );
            QDate inval;
            /* generate a new uid for the old record */
            to.setUid( 1 );

            /* add the old one cause it has a new UID here cause it was spin off */
            m_todoMgr.add( to );

            /*
             * update the due date
             * start date
             * and complete date
             */
            to2.setDueDate( date );
            rec.setStart( date );
            to2.setRecurrence( rec );  // could be Monday, TuesDay, Thursday every week

            /* move start date */
            if (to2.hasStartDate() )
                to2.setStartDate( to2.startDate().addDays( dayDiff ) );

            /* now the alarms */
            if (to2.hasNotifiers() ) {
                OPimNotifyManager::Alarms _als = to2.notifiers().alarms();
                OPimNotifyManager::Alarms als;

                /* for every alarm move the day */
                for ( OPimNotifyManager::Alarms::Iterator it = _als.begin(); it != _als.end(); ++it ) {
                    OPimAlarm al = (*it);
                    al.setDateTime( al.dateTime().addDays( dayDiff ) );
                    als.append( al );
                }
                to2.notifiers().setAlarms( als );
                handleAlarms( OPimTodo(), todo );
            }
            to2.setCompletedDate( inval );
            to2.setCompleted( false );

            updateTodo( to2 );
        }
        else
            updateTodo( to );
    }
    else
        updateTodo( to );

    currentView()->updateView();
    raiseCurrentView();
}

void MainWindow::flush()
{
    slotFlush();
}

void MainWindow::reload()
{
    slotReload();
}

int MainWindow::create()
{
    int uid = 0;

    if( m_syncing ) {
        QMessageBox::warning(this, QWidget::tr("Todo"),
                             QWidget::tr("Data can not be edited, currently syncing"));
        return uid;
    }

    m_todoMgr.load();


    OPimTodo todo = currentEditor()->newTodo( currentCatId(),
                                           this );

    if ( currentEditor()->accepted() ) {
        //todo.assignUid();
        uid = todo.uid();
        handleAlarms( OPimTodo(), todo );
        m_todoMgr.add( todo );
        currentView()->addEvent( todo );


        // I'm afraid we must call this every time now, otherwise
        // spend expensive time comparing all these strings...
        // but only call if we changed something -zecke
        reloadCategories();
    }
    raiseCurrentView( );

    return uid;
}

/* delete it silently... */
bool MainWindow::remove( int uid )
{
    if (m_syncing) return false;

    /* argh need to get the whole OEvent... to disable alarms -zecke */
    handleAlarms( OPimTodo(), m_todoMgr.event( uid ) );

    return m_todoMgr.remove( uid );
}

void MainWindow::beam( int uid )
{
    if( uid == 0 ) return;

    ::unlink( beamfile );
    m_todoMgr.load();

    OPimTodo todo = event( uid );
    OPimTodoAccessVCal* cal = new OPimTodoAccessVCal(QString::fromLatin1(beamfile) );
    OPimTodoAccess acc( cal );
    if ( acc.load() ) {
        acc.add( todo );
        acc.save();
    }
    Ir* ir = new Ir(this );
    connect(ir, SIGNAL(done(Ir*) ),
            this, SLOT(beamDone(Ir*) ) );
    ir->send(beamfile, todo.summary(), "text/x-vCalendar" );
}

void MainWindow::show( int uid )
{
    m_todoMgr.load(); // might not be loaded yet
    m_showing = true;
    slotShow( uid );
    raise();
    QPEApplication::setKeepRunning();
}

void MainWindow::edit( int uid )
{
    m_todoMgr.load();
    slotEdit( uid );
}

void MainWindow::add( const OPimRecord& rec )
{
    OPimTodo test;
    if ( rec.rtti() != test.rtti() ) return;
    m_todoMgr.load(); // might not be loaded

    const OPimTodo& todo = static_cast<const OPimTodo&>(rec);

    m_todoMgr.add(todo );
    currentView()->addEvent( todo );


    // I'm afraid we must call this every time now, otherwise
    // spend expensive time comparing all these strings...
    // but only call if we changed something -zecke
    reloadCategories();
}

void MainWindow::slotReturnFromView()
{
    m_showing = false;
    showHideQuickTask( false );
    raiseCurrentView();
}

namespace {
    OPimNotifyManager::Alarms findNonMatching( const OPimNotifyManager::Alarms& oldAls,
                                               const OPimNotifyManager::Alarms& newAls )
    {
        OPimNotifyManager::Alarms nonMatching;
        OPimNotifyManager::Alarms::ConstIterator oldIt = oldAls.begin();
        OPimNotifyManager::Alarms::ConstIterator newIt;
        for ( ; oldIt != oldAls.end(); ++oldIt ) {
            bool found = false;
            QDateTime oldDt = (*oldIt).dateTime();
            for (newIt= newAls.begin(); newIt != newAls.end(); ++newIt ) {
                if ( oldDt == (*newIt).dateTime() ) {
                    found = true;
                    break;
                    }
            }
            if (!found)
                nonMatching.append( (*oldIt) );
        }
        return nonMatching;
    }

    void addAlarms( const OPimNotifyManager::Alarms& als, int uid )
    {
        OPimNotifyManager::Alarms::ConstIterator it;
        for ( it = als.begin(); it != als.end(); ++it ) {
            AlarmServer::addAlarm( (*it).dateTime(), "QPE/Application/todolist", "alarm(QDateTime,int)", uid );
        }

    }

    void removeAlarms( const OPimNotifyManager::Alarms& als, int uid )
    {
        OPimNotifyManager::Alarms::ConstIterator it;
        for ( it = als.begin(); it != als.end(); ++it ) {
            AlarmServer::deleteAlarm( (*it).dateTime(), "QPE/Application/todolist", "alarm(QDateTime,int)", uid );
        }
    }
}

void MainWindow::handleAlarms( const OPimTodo& oldTodo, const OPimTodo& newTodo)
{
    /*
     * if oldTodo is not empty and has notifiers we need to find the deleted ones
     */
    if( !oldTodo.isEmpty() && oldTodo.hasNotifiers() ) {
        OPimNotifyManager::Alarms removed;
        OPimNotifyManager::Alarms oldAls = oldTodo.notifiers().alarms();
        if (!newTodo.hasNotifiers() )
            removed = oldAls;
        else
            removed = findNonMatching( oldAls, newTodo.notifiers().alarms() );

        removeAlarms( removed, oldTodo.uid() );
    }
    if ( newTodo.hasNotifiers() ) {
        OPimNotifyManager::Alarms added;
        if ( oldTodo.isEmpty() || !oldTodo.hasNotifiers() )
            added = newTodo.notifiers().alarms();
        else
            added = findNonMatching( newTodo.notifiers().alarms(), oldTodo.notifiers().alarms() );

        addAlarms( added, newTodo.uid() );
    }
}

void MainWindow::handleAllAlarms()
{
    if( !m_todoMgr.isLoaded() )
        m_todoMgr.load();
    
    // Delete all todo alarms
    AlarmServer::deleteAlarm( QDateTime(), "QPE/Application/todolist", QCString(), -1 );

    // Register all todo alarms
    m_todoMgr.updateList();
    OPimTodoAccess::List list = m_todoMgr.list();
    OPimTodoAccess::List::Iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
        const OPimTodo& todo = (*it);
        if ( todo.hasNotifiers() )
            addAlarms( todo.notifiers().alarms(), todo.uid() );
    }
}

/* we might have not loaded the db */
bool MainWindow::doAlarm( const QDateTime& dt, int uid )
{
    bool needshow = false;

    m_todoMgr.load();

    OPimTodo todo = m_todoMgr.event( uid );
    if (!todo.hasNotifiers() )
        return false;

    /*
     * let's find the right alarm and find out if silent
     * then show alarm dialog
     */
    bool found = FALSE;
    OPimAlarm alarm = todo.notifiers().alarmAtDateTime( dt, found );
    if ( found ) {
        bool bSound = FALSE;
        if ( alarm.sound() != OPimAlarm::Silent ) {
            startAlarm();
            bSound = TRUE;
        }

        QDateTime occdt = alarm.occurrenceDateTime();
        if( occdt.isNull() )
            occdt = dt;

        QString msg = todo.toRichText();

        Config qpeconfig( "qpe" );
        qpeconfig.setGroup("Time");
        bool ampm = qpeconfig.readBoolEntry( "AMPM", TRUE );

        OPimAlarmDlg dlg( occdt, tr("Todo Alarm"), msg, 5, 0, ampm, TRUE, TRUE, this );
        connect( &dlg, SIGNAL(viewItem(int)), this, SLOT(edit(int)) );
        QPEApplication::execDialog( &dlg );

        if (bSound)
            killAlarm();

        todo.notifiers().remove( alarm );
        if( dlg.response() == OPimAlarmDlg::Snooze ) {
            todo.notifiers().add( OPimAlarm( alarm.sound(), dlg.snoozeDateTime(), 0, uid, occdt ) );
        }
        m_todoMgr.update( uid, todo );
        m_todoMgr.save();
        handleAlarms( OPimTodo(), todo );

        if( dlg.response() == OPimAlarmDlg::View ) {
            slotShow( uid );
            needshow = TRUE;
        }
    }
    else
        owarn << "Started for alarm at " << dt << " (uid=" << uid << ") that does not exist!" << oendl;

    return needshow;
}

void MainWindow::slotAppMessage( const QCString& msg, const QByteArray& data )
{
    if( msg == "registerAllAlarms()" ) {
        handleAllAlarms();
    }
}
