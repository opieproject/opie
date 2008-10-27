
#include <qcopchannel_qws.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qaction.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qobjectlist.h>

#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/ir.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qpe/qpemessagebox.h>
#include <qmessagebox.h>
#include <opie2/oresource.h>

#include "editor.h"
#include "show.h"
#include "templatemanager.h"
#include "bookmanager.h"
#include "mainwindow.h"
#include "datebooksettings.h"

#include "dateentryimpl.h"
#include "views/day/dayview.h"
#include "views/week/weekview.h"
#include "views/weeklst/weeklstview.h"
#include "views/month/monthview.h"

using namespace Opie;
using namespace Datebook;

MainWindow::MainWindow()
    : OPimMainWindow( "Datebook", 0, 0, 0, 0, 0, WType_TopLevel | WStyle_ContextHelp ), m_descMan( "Descriptions" ),  m_locMan( "Locations" )
{
    m_currView = NULL;
    
    initConfig();
    initBars();
    initUI();
    initViews();
    initManagers();

    m_edit = NULL;

    setCaption( tr("Calendar") );
    setIcon( Opie::Core::OResource::loadPixmap( "datebook_icon" ) );
   
    QTimer::singleShot(0, this, SLOT(populate() ) );

    QCopChannel* chan = new QCopChannel( "QPE/System", this );
    connect( chan, SIGNAL( received(const QCString&,const QByteArray&) ),
             this, SLOT( slotSystemReceive(const QCString&,const QByteArray&) ) );

    connect( qApp, SIGNAL( appMessage(const QCString&,const QByteArray&) ),
             this, SLOT( slotAppMessage(const QCString&,const QByteArray&) ) );
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

}

void MainWindow::edit() {
   
}

void MainWindow::edit( int uid ) {
    OPimEvent event = manager()->event( uid );
    if ( editor()->edit( event ) ) {
        event = editor()->event();
        manager()->update(event);
    }
}

void MainWindow::showDayView() {
    QObjectListIt itact( *(m_viewsGroup->children()) );
    QAction *a;
    while ( (a=(QAction *)itact.current()) ) {
        ++itact;
        if ( a->name() == QString("DayView") /* no tr */ ) {
            a->setOn(true);
            slotChangeView();
            break;
        }
    }
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
//X     m_popView->insertItem(tr("New from template"), m_popTemplate, -1, 0);

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

/*X
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

    m_stack = new QWidgetStack( this );
    setCentralWidget( m_stack );

}

void MainWindow::initConfig() {
    Config qpeconfig( "qpe" );
    qpeconfig.setGroup("Time");
    m_ampm = qpeconfig.readBoolEntry( "AMPM", TRUE );
    m_onMonday = qpeconfig.readBoolEntry( "MONDAY" );
    
    Config config("DateBook");
    config.setGroup("Main");
    m_defaultViewIdx = config.readNumEntry("defaultview", 1);

/*    startTime = config.readNumEntry("startviewtime", 8);
    aPreset = config.readBoolEntry("alarmpreset");
    presetTime = config.readNumEntry("presettime");
    bJumpToCurTime = config.readBoolEntry("jumptocurtime");
    rowStyle = config.readNumEntry("rowstyle");
    weeklistviewconfig = config.readNumEntry("weeklistviewconfig",NORMAL);

    defaultLocation = config.readEntry("defaultLocation");
    QString tmpString = config.readEntry("defaultCategories");
    QStringList tmpStringList = QStringList::split(",",tmpString);
    defaultCategories.truncate(0);

    for( QStringList::Iterator i=tmpStringList.begin(); i!=tmpStringList.end(); i++) {
        defaultCategories.resize(defaultCategories.count()+1);
        defaultCategories[defaultCategories.count()-1]=(*i).toInt();
    }*/
}

void MainWindow::initViews() {
    m_views.append(new DayView(this, m_stack));
    m_views.append(new WeekView(this, m_stack));
    m_views.append(new WeekLstView(this, m_stack));
    m_views.append(new MonthView(this, m_stack));

    Config config("DateBook");
    config.setGroup("Main");
    m_viewsGroup = new QActionGroup( this );

    int idx = 1;
    for ( QListIterator<View> it(m_views); it.current(); ++it ) {
        QAction *a = new ViewSelectAction( it.current(), 0, m_viewsGroup, it.current()->type());
        if(idx == m_defaultViewIdx)
            a->setOn(TRUE);
        connect(a, SIGNAL( activated() ), this, SLOT( slotChangeView() ) );
        
        it.current()->loadConfig( &config );
        idx++;
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
    slotChangeView();
}

void MainWindow::slotGoToNow() {
    currentView()->showDay( QDate::currentDate() );
}

View* MainWindow::currentView() {
    return m_currView;
}

void MainWindow::slotFind() {

}

void MainWindow::slotConfigure() {
    DateBookSettings frmSettings( m_ampm, this );
/*X    frmSettings.setStartTime( startTime );
    frmSettings.setAlarmPreset( aPreset, presetTime );
    frmSettings.setJumpToCurTime( bJumpToCurTime );
    frmSettings.setRowStyle( rowStyle );
    frmSettings.comboDefaultView->setCurrentItem(m_defaultViewIdx-1);
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
        m_defaultViewIdx = frmSettings.comboDefaultView->currentItem()+1;
        defaultLocation = frmSettings.comboLocation->currentText();
        defaultCategories = frmSettings.comboCategory->currentCategories();

        saveSettings();
*/
        for ( QListIterator<View> it(m_views); it.current(); ++it ) {
            
        }    
    }
}

void MainWindow::slotClockChanged( bool ) {
    Config qpeconfig( "qpe" );
    qpeconfig.setGroup("Time");
    m_ampm = qpeconfig.readBoolEntry( "AMPM", TRUE );
    
    for ( QListIterator<View> it(m_views); it.current(); ++it ) {
        (*it)->clockChanged();
    }
}

void MainWindow::slotWeekChanged(bool ) {
    Config qpeconfig( "qpe" );
    qpeconfig.setGroup("Time");
    m_onMonday = qpeconfig.readBoolEntry( "MONDAY" );
    
    for ( QListIterator<View> it(m_views); it.current(); ++it ) {
        (*it)->dayChanged();
    }
}

void MainWindow::slotSystemReceive( const QCString& msg, const QByteArray& data ) {
    if ( msg == "timeChange(QString)" ) {
        currentView()->timeChanged();
    }
}

void MainWindow::slotAppMessage( const QCString& msg, const QByteArray& data ) {
    bool needShow = false;
    QDataStream stream( data, IO_ReadOnly );
    if (msg == "editEvent(int,QDate)") {
        int uid;
        QDate date;
        stream >> uid >> date;
        // Find the specific occurrence of this event
        // FIXME implement
        needShow = true;
    }
    else if (msg == "editEvent(int)") {
        int uid;
        stream >> uid;
        // Find the specific occurrence of this event
        edit(uid);
        needShow = true;
    }
    else if (msg == "viewDefault(QDate)") {
        QDate day;
        stream >> day;
        // FIXME implement
        needShow = true;
    }

    if (needShow )
        QPEApplication::setKeepRunning();
}

void MainWindow::slotItemNew() {
    if( editor()->newEvent( currentView()->date() ) ) {
        manager()->add( editor()->event() );
        currentView()->reschedule();
    }
}

void MainWindow::slotItemEdit() {
    const OPimOccurrence *occurrence = currentView()->currentItem();
    if(!occurrence)
        return;
    
    OPimEvent event = occurrence->toEvent();
    while ( editor()->edit( event ) ) {
        int result = 0;
        event = editor()->event();
        OPimRecurrence rec = event.recurrence();
        if( rec.type() != OPimRecurrence::NoRepeat ) {
            result = QMessageBox::warning( this, tr("Calendar"), tr( "This is a recurring event.\n\nDo you want to apply changes to\nall occurrences or just this one?"), tr("All"), tr("This one"), tr("Cancel") );
            if(result == 1) {
                // Now create a copy of the event just for this day
                event.assignUid();
                rec.setType( OPimRecurrence::NoRepeat );
                event.setRecurrence(rec);
                event.setStartDateTime( QDateTime(occurrence->date(), event.startDateTime().time()) );
                event.setEndDateTime( QDateTime(occurrence->date(), event.endDateTime().time()) );
                // Add an exception for the existing event
                OPimEvent dupEvent( occurrence->toEvent() );
                OPimRecurrence dupRec( dupEvent.recurrence() );
                dupRec.exceptions().append( occurrence->date() );
                dupEvent.setRecurrence(dupRec);
                // Link the two events
                event.setParent(dupEvent.uid());
                dupEvent.addChild(event.uid());
                // Write changes
                manager()->add(event);
                manager()->update( dupEvent );
            }
            else if(result == 2)
                continue;
        }

        if(result == 0) {
            manager()->update(event);
        }

        currentView()->reschedule();

        break;
    }
}

void MainWindow::slotItemDuplicate() {
    const OPimOccurrence *occurrence = currentView()->currentItem();
    if(!occurrence)
        return;
    
    OPimEvent event = occurrence->toEvent();
    if ( editor()->edit( event ) ) {
        OPimEvent dupEvent( editor()->event() );
        dupEvent.assignUid();
        manager()->add( dupEvent );
        currentView()->reschedule();
    }
}

void MainWindow::slotItemDelete() {
    const OPimOccurrence *occurrence = currentView()->currentItem();
    if(!occurrence)
        return;
    
    OPimEvent ev = occurrence->toEvent();
    QString strName = ev.description();

    if ( !QPEMessageBox::confirmDelete( this, tr( "Calendar" ),strName ) )
        return;

    int result = 0;
    if( ev.hasRecurrence() ) {
        result = QMessageBox::warning( this, tr("Calendar"), tr( "This is a recurring event.\n\nDo you want to delete all\noccurrences or just this one?"), tr("All"), tr("This one"), tr("Cancel") );
    }

    if(result == 0) {
        manager()->remove( ev.uid() );
        currentView()->reschedule();
    }
    else if(result == 1) {
        // Add an exception to the existing event
        OPimRecurrence dupRec( ev.recurrence() );
        dupRec.exceptions().append( occurrence->date() );
        ev.setRecurrence(dupRec);
        // Write changes
        manager()->update( ev );
        currentView()->reschedule();
    }
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
    QDate lastDate;
    if(m_currView)
        lastDate = m_currView->date();
    else
        lastDate = QDate::currentDate();

    View *selected = NULL;
    QObjectListIt itact( *(m_viewsGroup->children()) );
    QAction *a;
    while ( (a=(QAction *)itact.current()) ) {
        ++itact;
        if ( a->isOn() ) {
            for ( QListIterator<View> it(m_views); it.current(); ++it ) {
                if(it.current()->type() == a->name())
                    selected = it.current();
            }    
            
            break;
        }
    }
    m_currView = selected;
   
    if( m_currView->date() != lastDate ) {
        odebug << "*** calling showDay" << oendl;
        m_currView->showDay(lastDate);
    }
    else {
        odebug << "*** reschedule only" << oendl;
        m_currView->reschedule();
    }

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
    if(eve.isValidUid()) {  // not a holiday event
        QPopupMenu m;
        m_itemEditAction->addTo( &m );
        m_itemDuplicateAction->addTo( &m );
        m_itemDeleteAction->addTo( &m );
        if(Ir::supported()) {
            m_itemBeamAction->addTo( &m );
//X            if( eve.hasRecurrence() ) 
//X                m.insertItem( tr( "Beam this occurrence"), 5 );
        }
        int r = m.exec( pt );

        if ( r == 5 ) {
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
//X                emit beamMe( event );
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
                }
                else {
//X                        QDate dt = DateBookDay::findRealStart( ev.event().uid(), ev.date(), dateBook->db );
//X                        start.setDate( dt );
                }


                /*
                    * ok we know now the end date...
                    * else
                    *   get to know the offset btw the real start and real end
                    *   and then add it to the new start date...
                    */
                if ( ev.endTime() != QTime(23, 59, 59 ) ) {
                    end.setDate( ev.date() );
                }
                else{
                    int days = ev.toEvent().startDateTime().date().daysTo( ev.toEvent().endDateTime().date() );
                    end.setDate( start.date().addDays( days ) );
                }



                OPimEvent event( ev.toEvent() );
                event.setStartDateTime( start );
                event.setEndDateTime( end   );


//X                emit beamMe( event );
            }
        }
    }
}

void MainWindow::viewAdd( const QDateTime& start, const QDateTime& end ) {
    if( editor()->newEvent( start, end ) ) {
        manager()->add( editor()->event() );
        currentView()->reschedule();
    }
}

void MainWindow::viewAdd( const OPimEvent &ev ) {
    manager()->add(ev);
    currentView()->reschedule();
}

bool MainWindow::viewAP()const{
    return m_ampm;
}

bool MainWindow::viewStartMonday()const {
    return m_onMonday;
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
    }
}

Editor* MainWindow::editor() {
    if(!m_edit)
        m_edit = new DateEntryEditor(this, this);
    return m_edit;
}
