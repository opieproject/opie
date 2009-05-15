/*
                             This file is part of the Opie Project
                             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
              =.             
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#define QTOPIA_INTERNAL_FD

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <qcopchannel_qws.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qaction.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qobjectlist.h>
#include <qpushbutton.h>

#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/ir.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qpe/qpemessagebox.h>
#include <qmessagebox.h>
#include <qpe/finddialog.h>
#include <opie2/oresource.h>
#include <opie2/opluginloader.h>
#include <opie2/opimnotifymanager.h>
#include <opie2/opimalarmdlg.h>
#include <opie2/odatebookaccess.h>
#include <opie2/odatebookaccessbackend_vcal.h>

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
    : OPimMainWindow( "Datebook", 0, 0, 0, 0, 0, WType_TopLevel | WStyle_ContextHelp ), m_descMan( "Descriptions" ),  m_locMan( "Locations" ) /* no tr */
{
    m_currView = NULL;
    m_initialDate = QDate::currentDate();
    m_inSearch = false;
    
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

    chan = new QCopChannel( "QPE/Datebook", this );
    connect( chan, SIGNAL( received(const QCString&,const QByteArray&) ),
             this, SLOT( slotAppMessage(const QCString&,const QByteArray&) ) );
    connect( qApp, SIGNAL( appMessage(const QCString&,const QByteArray&) ),
             this, SLOT( slotAppMessage(const QCString&,const QByteArray&) ) );
}

MainWindow::~MainWindow() {
    m_tempMan.save();
    m_locMan.save();
    m_descMan.save();

    manager()->save();
    saveConfig();
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
    // Need to make sure manager is loaded (in case called from QCop message)
    if (!manager()->isLoaded() )
        manager()->load();

    OPimEvent event = manager()->event( uid );
    if ( editor()->edit( event ) ) {
        event = editor()->event();
        manager()->update(event);
    }
}

void MainWindow::edit( int uid, const QDate &date ) {
    // Need to make sure manager is loaded (in case called from QCop message)
    if (!manager()->isLoaded() )
        manager()->load();

    OPimOccurrence::List occs = manager()->list( date, date );
    for (OPimOccurrence::List::ConstIterator it = occs.begin(); it != occs.end(); ++it ) {
        OPimOccurrence ito = (*it);
        if ( ito.toEvent().uid() == uid ) {
            edit( &ito );
            break;
        }
    }
}

void MainWindow::edit( const OPimOccurrence *occurrence ) {
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
                dupRec.addException( occurrence->date() );
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
            currentView()->reschedule();
        }
        break;
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

void MainWindow::showDefaultView() {
    int idx = 1;
    QObjectListIt itact( *(m_viewsGroup->children()) );
    QAction *a;
    while ( (a=(QAction *)itact.current()) ) {
        ++itact;
        if(idx == m_defaultViewIdx) {
            if( !a->isOn() ) {
                a->setOn(TRUE);
                slotChangeView();
            }
            break;
        }
        idx++;
    }
}

/*
 * init tool bars layout and so on
 */
void MainWindow::initUI() {
    setToolBarsMovable( false );

    m_toolBar = new QToolBar( this );
    m_toolBar->setHorizontalStretchable( TRUE );
    
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

    if ( Ir::supported() ) {
        m_itemBeamOccurrenceAction = new QAction( tr( "Beam this occurrence" ),
                                    QString::null, 0, this, 0 );
        connect( m_itemBeamOccurrenceAction, SIGNAL(activated()), this, SLOT(slotItemBeamOccurrence()) );
        m_itemBeamOccurrenceAction->setWhatsThis( tr( "Transmit the specific occurrence of the selected recurring item." ) );
    }
    
/*X
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

    m_startTime = config.readNumEntry("startviewtime", 8);
    m_alarmPreset = config.readBoolEntry("alarmpreset");
    m_alarmPresetTime = config.readNumEntry("presettime");

    m_defaultLocation = config.readEntry("defaultLocation");
    QString tmpString = config.readEntry("defaultCategories");
    QStringList tmpStringList = QStringList::split(",",tmpString);
    m_defaultCategories.truncate(0);

    for( QStringList::Iterator i=tmpStringList.begin(); i!=tmpStringList.end(); i++) {
        m_defaultCategories.resize(m_defaultCategories.count()+1);
        m_defaultCategories[m_defaultCategories.count()-1]=(*i).toInt();
    }
}

void MainWindow::saveConfig() {
    Config config("DateBook");
    config.setGroup("Main");

    config.writeEntry("defaultview", m_defaultViewIdx);
    config.writeEntry("startviewtime", m_startTime);
    config.writeEntry("alarmpreset", m_alarmPreset);
    config.writeEntry("presettime", m_alarmPresetTime);

    config.writeEntry("defaultLocation",m_defaultLocation);
    QStringList tmpStringList;
    for( uint i=0; i<m_defaultCategories.count(); i++) {
        tmpStringList << QString::number(m_defaultCategories[i]);
    }
    config.writeEntry("defaultCategories",tmpStringList.join(","));
    
    for ( QListIterator<View> it(m_views); it.current(); ++it ) {
        it.current()->saveConfig( &config );
    }
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

    if(!m_descMan.load()) {
        // Load default descriptions
        m_descMan.add( tr("Meeting") );
        m_descMan.add( tr("Lunch") );
        m_descMan.add( tr("Dinner") );
        m_descMan.add( tr("Travel") );
    }

    if(!m_locMan.load()) {
        // Load default locations
        m_locMan.add( tr("Office") );
        m_locMan.add( tr("Home") );
    }
    
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
    // move it to the day view...
    showDayView();
    FindDialog frmFind( "Calendar", this ); // no tr needed
    frmFind.setUseDate( true );
    frmFind.setDate( currentView()->date() );
    QObject::connect( &frmFind,
                      SIGNAL(signalFindClicked(const QString&,const QDate&,bool,bool,int)),
              this,
              SLOT(slotDoFind(const QString&,const QDate&,bool,bool,int)) );
    QObject::connect( this,
              SIGNAL(signalFindNotFound()),
              &frmFind,
              SLOT(slotNotFound()) );
    QObject::connect( this,
              SIGNAL(signalFindWrapAround()),
              &frmFind,
              SLOT(slotWrapAround()) );
    frmFind.move(0,0);
    frmFind.exec();
    m_inSearch = false;
}

void MainWindow::slotDoFind( const QString& txt, const QDate &dt,
               bool caseSensitive, bool /*backwards*/,
               int category )
{
    // if true at the end we will start at the begin again and afterwards
    // we will emit string not found
    static bool wrapAround = true;
    static QDateTime startdt;

    if ( !m_inSearch ) {
        startdt = QDateTime( dt, QTime(0,0,0) );
        m_inSearch = true;
    }
    
    OPimEvent ev = manager()->find( txt, caseSensitive, startdt );
    if ( ev.uid() != 0 ) {
//X        dayView->setStartViewTime( dtEnd.time().hour() );
        currentView()->showDay( startdt.date() );
        startdt = startdt.addDays(1);
    }
    else {
        if ( wrapAround ) {
            emit signalFindWrapAround();
            startdt = QDateTime();
        } 
        else
            emit signalFindNotFound();
        wrapAround = !wrapAround;
    }
}

void MainWindow::slotConfigure() {
    DateBookSettings frmSettings( m_ampm, this );
    frmSettings.setStartTime( m_startTime );
    frmSettings.setAlarmPreset( m_alarmPreset, m_alarmPresetTime );
    frmSettings.setPluginList( manager()->holiday()->pluginManager(), manager()->holiday()->pluginLoader() );
    frmSettings.setViews( &m_views );
    frmSettings.setManagers( descriptionManager(), locationManager() );
    frmSettings.comboDefaultView->setCurrentItem(m_defaultViewIdx-1);
    
    bool found=false;
    for (int i=0; i<(frmSettings.comboLocation->count()); i++) {
        if ( frmSettings.comboLocation->text(i) == m_defaultLocation ) {
            frmSettings.comboLocation->setCurrentItem(i);
            found=true;
            break;
        }
    }
    if(!found) {
        frmSettings.comboLocation->insertItem(m_defaultLocation);
        frmSettings.comboLocation->setCurrentItem(frmSettings.comboLocation->count()-1);
    }
    frmSettings.comboCategory->setCategories(m_defaultCategories,"Calendar", tr("Calendar"));

    if ( QPEApplication::execDialog( &frmSettings ) ) {
        frmSettings.savePlugins();
        manager()->holiday()->pluginManager()->save();
        manager()->holiday()->reloadPlugins();

        frmSettings.saveViews();
        
        m_alarmPreset = frmSettings.alarmPreset();
        m_alarmPresetTime = frmSettings.presetTime();
        m_startTime = frmSettings.startTime();
        m_defaultViewIdx = frmSettings.comboDefaultView->currentItem()+1;
        m_defaultLocation = frmSettings.comboLocation->currentText();
        m_defaultCategories = frmSettings.comboCategory->currentCategories();

        setDescriptionManager( frmSettings.descriptionManager() );
        setLocationManager( frmSettings.locationManager() );

        saveConfig();
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
    if ( msg == "nextView()" ) {
        needShow = true;
        if ( qApp->activeWindow() )
            nextView();
    }
    else if (msg == "editEvent(int,QDate)") {
        int uid;
        QDate date;
        stream >> uid >> date;
        // Edit the specific occurrence of this event
        edit( uid, date );
        needShow = true;
    }
    else if (msg == "editEvent(int)") {
        int uid;
        stream >> uid;
        edit(uid);
        needShow = true;
    }
    else if (msg == "viewDefault(QDate)") {
        QDate day;
        stream >> day;
        if(currentView()) {
            // Already running
            showDefaultView();
            currentView()->showDay(day);
        }
        else
            m_initialDate = day;  // still starting
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

    edit( occurrence );
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
        dupRec.addException( occurrence->date() );
        ev.setRecurrence(dupRec);
        // Write changes
        manager()->update( ev );
        currentView()->reschedule();
    }
}

void MainWindow::slotItemBeam() {
    const OPimOccurrence *ev = currentView()->currentItem();
    if(!ev)
        return;
    
    beamEvent( ev->toEvent() );
}

void MainWindow::slotItemBeamOccurrence() {
    const OPimOccurrence *ev = currentView()->currentItem();
    if(!ev)
        return;

    // create an OPimEvent from the OPimOccurrence and beam it...
    
    // Start with the easy stuff. If start and end date is the same we can just use
    // the values of the occurrence
    // If it is a multi-day event we need to find the real start and end date...
    if ( ev->toEvent().startDateTime().date() == ev->toEvent().endDateTime().date() ) {
        OPimEvent event( ev->toEvent() );

        QDateTime dt( ev->date(), ev->startTime() );
        event.setStartDateTime( dt );

        dt.setTime( ev->endTime() );
        event.setEndDateTime( dt );
        beamEvent( event );
    }
    else {
        // at least the the Times are right now
        QDateTime start( ev->toEvent().startDateTime() );
        QDateTime end  ( ev->toEvent().endDateTime() );

        // ok we know the start date or we need to find it
        if ( ev->startTime() != QTime( 0, 0, 0 ) ) {
            start.setDate( ev->date() );
        }
        else {
            QDate dt = manager()->findRealStart( *ev );
            start.setDate( dt );
        }

        if ( ev->endTime() != QTime(23, 59, 59 ) ) {
            // ok we know now the end date
            end.setDate( ev->date() );
        }
        else {
            // get the offset between the real start and real end
            // and then add it to the new start date...
            int days = ev->toEvent().startDateTime().date().daysTo( ev->toEvent().endDateTime().date() );
            end.setDate( start.date().addDays( days ) );
        }

        OPimEvent event( ev->toEvent() );
        event.setStartDateTime( start );
        event.setEndDateTime( end );

        beamEvent( event );
    }
}

static const char * beamfile = "/tmp/obex/event.vcs";

void MainWindow::beamEvent( const OPimEvent &e )
{
    odebug << "trying to beam" << oendl;
    unlink( beamfile ); // delete if exists
    mkdir("/tmp/obex/", 0755);
    ODateBookAccessBackend_VCal* cal = new ODateBookAccessBackend_VCal( "calendar", QString::fromLatin1(beamfile) );
    ODateBookAccess acc( cal );
    acc.load();
    acc.add( e );
    acc.save();
    Ir* ir = new Ir(this );
    connect(ir, SIGNAL(done(Ir*) ),
            this, SLOT(beamDone(Ir*) ) );
    ir->send(beamfile, e.description(), "text/x-vCalendar" );
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
        lastDate = m_initialDate;

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

void MainWindow::nextView() {
    QObjectListIt itact( *(m_viewsGroup->children()) );
    QAction *a;
    bool next = false;
    while ( (a=(QAction *)itact.current()) ) {
        ++itact;
        if ( a->isOn() )
            next = true;
        else if(next) {
            a->setOn(true);
            next = false;
        }
    }

    if(next) {
        // Loop back around
        ((QAction *)(m_viewsGroup->children()->getFirst()))->setOn(true);
    }
    slotChangeView();
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
        if( Ir::supported() && m_itemBeamAction ) {
            m_itemBeamAction->addTo( &m );
            if( eve.hasRecurrence() ) 
                m_itemBeamOccurrenceAction->addTo( &m );
        }
        m.exec( pt );
    }
}

void MainWindow::viewAdd( const QDateTime& start, const QDateTime& end, const QString &desc ) {
    if( editor()->newEvent( start, end, desc ) ) {
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

QString MainWindow::defaultLocation()const {
    return m_defaultLocation;
}

QArray<int> MainWindow::defaultCategories()const {
    return m_defaultCategories;
}

bool MainWindow::alarmPreset()const {
    return m_alarmPreset;
}

int MainWindow::alarmPresetTime()const {
    return m_alarmPresetTime;
}

bool MainWindow::doAlarm( const QDateTime &when, int uid ) {
    bool needshow = false;
    
    // check to make it's okay to continue,
    // this is the case that the time was set ahead, and
    // we are forced given a stale alarm...
    QDateTime current = QDateTime::currentDateTime();
    if ( current.time().hour() != when.time().hour() && current.time().minute() != when.time().minute() )
        return false;

    if(!manager()->isLoaded())
        manager()->load();

    OPimEvent event = manager()->event( uid );
    if ( !event.isEmpty() ) {
        QString msg;
        bool found = FALSE;
        bool bSound = FALSE;
        
        OPimAlarm alarm = event.notifiers().alarmAtDateTime( when, found );
        if ( found ) {
            QDateTime recurDateTime( alarm.occurrenceDateTime() );
            if( recurDateTime.isNull() ) {
                if( ! BookManager::nextOccurrence( event, when, recurDateTime ) ) {
                    // This should never happen, but just in case it does...
                    recurDateTime = event.startDateTime();
                }
            }
            
            msg += "<CENTER><B>" + event.description() + "</B>"
                + "<BR>" + event.location()
                + event.note() + "</CENTER>";

            if ( alarm.sound() != OPimAlarm::Silent ) {
                startAlarm();
                bSound = TRUE;
            }
            
            OPimAlarmDlg dlg( recurDateTime, tr("Calendar Alarm"), msg, 5, 0, m_ampm, TRUE, FALSE, this, TRUE );
            connect( &dlg, SIGNAL(viewItem(int)), this, SLOT(edit(int)) );
            QPEApplication::execDialog( &dlg );
            
            if ( bSound )
                killAlarm();

            event.notifiers().remove(alarm);
            if( dlg.response() == OPimAlarmDlg::Snooze ) {
                event.notifiers().add( OPimAlarm( alarm.sound(), dlg.snoozeDateTime(), 0, uid, recurDateTime ) );
            }
            manager()->update( event );
            
            if( dlg.response() == OPimAlarmDlg::View ) {
                edit( uid );
                needshow = TRUE;
            }
        }
        else 
            owarn << "Started for alarm at " << when << " (uid=" << uid << ") that does not exist!" << oendl;
    }
    
    return needshow;
}
