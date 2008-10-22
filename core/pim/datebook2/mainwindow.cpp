
#include <qcopchannel_qws.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qaction.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qobjectlist.h>

#include <qpe/qpeapplication.h>
#include <qpe/ir.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qpe/qpemessagebox.h>
#include <opie2/oresource.h>

#include "editor.h"
#include "show.h"
#include "templatemanager.h"
#include "bookmanager.h"
#include "mainwindow.h"
#include "datebooksettings.h"

#include "dateentryimpl.h"
#include "views/day/dayview.h"

using namespace Opie;
using namespace Datebook;

MainWindow::MainWindow()
    : OPimMainWindow( "Datebook", 0, 0, 0, 0, 0, WType_TopLevel | WStyle_ContextHelp ), m_descMan( "Descriptions" ),  m_locMan( "Locations" )
{
    initBars();
    initUI();
    initViews();
    initManagers();
    initConfig();

    m_edit = NULL;

    setCaption( tr("Calendar") );
    setIcon( Opie::Core::OResource::loadPixmap( "datebook_icon" ) );

    raiseCurrentView();
    
    QTimer::singleShot(0, this, SLOT(populate() ) );

    // FIXME OPimMainWindow already registers QCop receivers
    QCopChannel* chan = new QCopChannel( "QPE/System", this );
    connect( chan, SIGNAL( received(const QCString&,const QByteArray&) ),
             this, SLOT( slotReceive(const QCString&,const QByteArray&) ) );

    chan  = new QCopChannel( "QPE/Datebook", this );
    connect( chan, SIGNAL( received(const QCString&,const QByteArray&) ),
             this, SLOT( slotReceive(const QCString&,const QByteArray&) ) );
}

MainWindow::~MainWindow() {
    m_tempMan.save();
    m_locMan.save();
    m_descMan.save();

    manager()->save();
    delete m_manager;
}

void MainWindow::initBars() {
    // do nothing
}

void MainWindow::doSetDocument( const QString& str ) {

}

void MainWindow::flush() {
    manager()->save();
}

void MainWindow::reload() {
    manager()->reload();
}

int MainWindow::create() {
    return 0;
}

bool MainWindow::remove( int uid ) {
    manager()->remove( uid );
    return true;
}

void MainWindow::beam( int uid ) {

}

void MainWindow::show( int uid ) {

    eventShow()->show( manager()->event( uid ) );
}

void MainWindow::add( const OPimRecord& ad) {
    manager()->add( ad );
}

void MainWindow::edit() {
    edit ( currentView()->currentItem() );
}

void MainWindow::edit( int uid ) {
    
}

/*
 * init tool bars layout and so on
 */
void MainWindow::initUI() {
    setToolBarsMovable( false );

    m_toolBar = new QToolBar( this );
    m_toolBar->setHorizontalStretchable( TRUE );
    
    QMenuBar* mb = new QMenuBar( m_toolBar );

    m_itemNewAction->addTo( m_toolBar );

    m_popTemplate = new QPopupMenu( this );
    m_popTemplate->setCheckable( TRUE );
    connect( m_popTemplate, SIGNAL(activated(int) ),
             this, SLOT(slotNewFromTemplate(int) ) );
//     m_popView->insertItem(tr("New from template"), m_popTemplate, -1, 0);

//    m_itemEditAction->addTo( m_popView );

    m_toolBar->addSeparator();

    QAction *a = new QAction( tr("Today" ), Opie::Core::OResource::loadPixmap( "datebook/to_day", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, this, 0 );
    a->addTo( m_toolBar );
    connect(a, SIGNAL( activated() ), this, SLOT( slotGoToNow() ) );

    m_toolBar->addSeparator();

    m_viewsBar = new QToolBar( this );
    m_viewsBar->setHorizontalStretchable( FALSE );

    m_toolBar2 = new QToolBar( this );
    m_toolBar2->setHorizontalStretchable( TRUE );

    m_toolBar2->addSeparator();

    a = new QAction( tr("Find"), Opie::Core::OResource::loadPixmap( "mag", Opie::Core::OResource::SmallIcon ),
                     QString::null, 0, this, 0 );
    a->addTo( m_toolBar2 );
    connect(a, SIGNAL( activated() ), this, SLOT( slotFind() ) );

    m_configureAction->addTo( m_toolBar2 );

/*
    a = new QAction( tr("Configure Locations"), QString::null, 0, 0 );
    a->addTo( m_popSetting );
    connect(a, SIGNAL( activated() ), this, SLOT( slotConfigureLocs() ) );

    a = new QAction( tr("Configure Descriptions"), QString::null, 0, 0 );
    a->addTo( m_popSetting );
    connect(a, SIGNAL( activated() ), this, SLOT(slotConfigureDesc() ) );

    a = new QAction( tr("Configure Templates"), QString::null, 0, 0 );
    a->addTo( m_popSetting );
    connect(a, SIGNAL( activated() ), this, SLOT(slotConfigureTemp() ) );
*/
    connect( qApp, SIGNAL(clockChanged(bool) ),
             this, SLOT(slotClockChanged(bool) ) );
    connect( qApp, SIGNAL(weekChanged(bool) ),
             this, SLOT(slotWeekChanged(bool) ) );

    connect( qApp, SIGNAL(appMessage(const QCString&,const QByteArray&) ),
             this, SLOT(slotAppMessage(const QCString&,const QByteArray&) ) );

    m_stack = new QWidgetStack( this );
    setCentralWidget( m_stack );

}

void MainWindow::initConfig() {

}

void MainWindow::initViews() {
    m_views.append(new DayView(this, m_stack));

    m_viewsGroup = new QActionGroup( this );

    bool firstact = TRUE;
    for ( QListIterator<View> it(m_views); it.current(); ++it ) {
        QAction *a = new ViewSelectAction( it.current(), 0, m_viewsGroup, it.current()->type());
        if(firstact) // FIXME should be looking for user-selected default view
            a->setOn(TRUE);
        firstact = FALSE;
        connect(a, SIGNAL( activated() ), this, SLOT( slotChangeView() ) );
    }
    m_viewsGroup->addTo( m_viewsBar );
}

void MainWindow::initManagers() {
    m_manager = new BookManager;

    m_tempMan.load();
    m_locMan.load();
    m_descMan.load();

    setTemplateMenu();
}

void MainWindow::raiseCurrentView() {
    m_stack->raiseWidget( currentView()->widget() );
}

/*
 * populate the view
 */
void MainWindow::populate() {
    if (!manager()->isLoaded() )
        manager()->load();
}

void MainWindow::slotGoToNow() {
    currentView()->showDay( QDate::currentDate() );
}

View* MainWindow::currentView() {
    QObjectListIt itact( *(m_viewsGroup->children()) );
    QAction *a;
    while ( (a=(QAction *)itact.current()) ) {
        ++itact;
        if ( a->isOn() ) {
            for ( QListIterator<View> it(m_views); it.current(); ++it ) {
                if(it.current()->type() == a->name())
                    return it.current();
            }    
            
            break;
        }
    }
    return NULL;
}

void MainWindow::slotFind() {

}

void MainWindow::slotConfigure() {
    DateBookSettings frmSettings( m_ampm, this );
/*X    frmSettings.setStartTime( startTime );
    frmSettings.setAlarmPreset( aPreset, presetTime );
    frmSettings.setJumpToCurTime( bJumpToCurTime );
    frmSettings.setRowStyle( rowStyle );
    frmSettings.comboDefaultView->setCurrentItem(defaultView-1);
    frmSettings.comboWeekListView->setCurrentItem(weeklistviewconfig);
    frmSettings.setPluginList(db_holiday->pluginManager(),db_holiday->pluginLoader());

    bool found=false;
    for (int i=0; i<(frmSettings.comboLocation->count()); i++) {
        if ( frmSettings.comboLocation->text(i) == defaultLocation ) {
            frmSettings.comboLocation->setCurrentItem(i);
            found=true;
            break;
        }
    }
    if(!found) {
        frmSettings.comboLocation->insertItem(defaultLocation);
        frmSettings.comboLocation->setCurrentItem(frmSettings.comboLocation->count()-1);
    }
    frmSettings.comboCategory->setCategories(defaultCategories,"Calendar", tr("Calendar"));
*/
    if ( QPEApplication::execDialog( &frmSettings ) ) {
        frmSettings.savePlugins();
/*X        db_holiday->pluginManager()->save();
        db_holiday->reloadPlugins();

        aPreset = frmSettings.alarmPreset();
        presetTime = frmSettings.presetTime();
        startTime = frmSettings.startTime();
        bJumpToCurTime = frmSettings.jumpToCurTime();
        rowStyle = frmSettings.rowStyle();
        defaultView = frmSettings.comboDefaultView->currentItem()+1;
        defaultLocation = frmSettings.comboLocation->currentText();
        defaultCategories = frmSettings.comboCategory->currentCategories();

        saveSettings();
*/
        for ( QListIterator<View> it(m_views); it.current(); ++it ) {
            
        }    
    }
}

void MainWindow::slotClockChanged( bool ) {

}

void MainWindow::slotWeekChanged(bool ) {

}

void MainWindow::slotAppMessage( const QCString&, const QByteArray& ) {

}

void MainWindow::slotReceive( const QCString&, const QByteArray& ) {

}

void MainWindow::slotItemNew() {
    if( editor()->newEvent( currentView()->defaultDate() ) ) {
        add( editor()->event() );
        currentView()->reschedule();
    }
}

void MainWindow::slotItemEdit() {
}

void MainWindow::slotItemDuplicate() {
}

void MainWindow::slotItemDelete() {
}

void MainWindow::slotItemBeam() {
}

BookManager* MainWindow::manager() {
    return m_manager;
}

TemplateManager MainWindow::templateManager() {
    return m_tempMan;
}

LocationManager MainWindow::locationManager() {
    return m_locMan;
}

DescriptionManager MainWindow::descriptionManager() {
    return m_descMan;
}

void MainWindow::setLocationManager( const LocationManager& loc) {
    m_locMan = loc;
}

void MainWindow::setDescriptionManager( const DescriptionManager& dsc ) {
    m_descMan = dsc;
}

Show* MainWindow::eventShow() {
    return m_show;
}

void MainWindow::slotChangeView() {
    odebug << "View changed: " << currentView()->type() << oendl;
    raiseCurrentView();
}

void MainWindow::slotConfigureLocs() {
    LocationManagerDialog dlg( locationManager() );
    dlg.setCaption( tr("Configure Locations") );
    if ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted ) {
        setLocationManager( dlg.manager() );
    }
}

void MainWindow::slotConfigureDesc() {
    DescriptionManagerDialog dlg( descriptionManager() );
    dlg.setCaption( tr("Configure Descriptions") );
    if ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted ) {
        setDescriptionManager( dlg.manager() );
    }
}

void MainWindow::slotConfigureTemp() {
    TemplateDialog dlg( templateManager(), editor() );
    dlg.setCaption( tr("Configure Templates") );
    if ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted ) {
        m_tempMan = dlg.manager();
        setTemplateMenu();
    }
}

void MainWindow::hideShow() {

}

void MainWindow::viewPopup( const OPimOccurrence &ev, const QPoint &pt ) {
    OPimEvent eve = ev.toEvent();
    QPopupMenu m;
    m.insertItem( tr( "Edit" ), 1 );
    m.insertItem( tr( "Duplicate" ), 4 );
    m.insertItem( tr( "Delete" ), 2 );
    if(Ir::supported()) {
        m.insertItem( tr( "Beam" ), 3 );
        if( eve.hasRecurrence() ) 
            m.insertItem( tr( "Beam this occurrence"), 5 );
    }
    int r = m.exec( pt );
    if ( r == 1 ) {
        odebug << "edit" << oendl;
//X        emit editMe( ev );
    } else if ( r == 2 ) {
//X        emit deleteMe( ev );
    } else if ( r == 3 ) {
//X        emit beamMe( eve );
    } else if ( r == 4 ) {
//X        emit duplicateMe( eve );
    } else if ( r == 5 ) {
        // create an OPimEvent and beam it...
        /*
            * Start with the easy stuff. If start and  end date is the same we can just use
            * the values of effective events
            * If it is a multi day event we need to find the real start and end date...
            */
        if ( ev.toEvent().startDateTime().date() == ev.toEvent().endDateTime().date() ) {
            OPimEvent event( ev.toEvent() );

            QDateTime dt( ev.date(), ev.startTime() );
            event.setStartDateTime( dt );

            dt.setTime( ev.endTime() );
            event.setEndDateTime( dt );
//X            emit beamMe( event );
        }else {
            /*
                * at least the the Times are right now
                */
            QDateTime start( ev.toEvent().startDateTime() );
            QDateTime end  ( ev.toEvent().endDateTime() );


            /*
                * ok we know the start date or we need to find it
                */
            if ( ev.startTime() != QTime( 0, 0, 0 ) ) {
                start.setDate( ev.date() );
            }else {
//X                    QDate dt = DateBookDay::findRealStart( ev.event().uid(), ev.date(), dateBook->db );
//X                    start.setDate( dt );
            }


            /*
                * ok we know now the end date...
                * else
                *   get to know the offset btw the real start and real end
                *   and then add it to the new start date...
                */
            if ( ev.endTime() != QTime(23, 59, 59 ) ) {
                end.setDate( ev.date() );
            }else{
                int days = ev.toEvent().startDateTime().date().daysTo( ev.toEvent().endDateTime().date() );
                end.setDate( start.date().addDays( days ) );
            }



            OPimEvent event( ev.toEvent() );
            event.setStartDateTime( start );
            event.setEndDateTime( end   );


//X            emit beamMe( event );
        }
    }
}

void MainWindow::viewAdd(const QDate& ) {

}

void MainWindow::viewAdd( const QDateTime&, const QDateTime& ) {

}

bool MainWindow::viewAP()const{
    return m_ampm;
}

bool MainWindow::viewStartMonday()const {

}

void MainWindow::setTemplateMenu() {
    m_popTemplate->clear();

    QStringList list = templateManager().names();
}

/*
 * get the name of the item with the id id
 * then ask for an OPimEvent from the manager
 */
void MainWindow::slotNewFromTemplate(int id ) {
    QString name = m_popTemplate->text( id );

    OPimEvent ev = templateManager().value( name );

    if ( editor()->edit( ev ) ) {
        ev =  editor()->event();
        ev.setUid( -1 );
        manager()->add( ev );

        /*
         * no we'll find out if the current view
         * should show the new event
         * and then we will ask it to refresh
         * FIXME for now we'll call a refresh
         */
        currentView()->reschedule();
        raiseCurrentView();
    }
}

Editor* MainWindow::editor() {
    if(!m_edit)
        m_edit = new DateEntryEditor(this, this);
    return m_edit;
}
