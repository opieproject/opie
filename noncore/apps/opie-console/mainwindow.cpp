#include <assert.h>



#include <qaction.h>
#include <qmenubar.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qmessagebox.h>

#include <qpe/resource.h>
#include <opie/ofiledialog.h>


#include "keytrans.h"
#include "profileeditordialog.h"
#include "configdialog.h"
#include "default.h"
#include "metafactory.h"
#include "profile.h"
#include "profilemanager.h"
#include "mainwindow.h"
#include "tabwidget.h"
#include "transferdialog.h"
#include "function_keyboard.h"
#include "script.h"

MainWindow::MainWindow() {
    KeyTrans::loadAll();
    for (int i = 0; i < KeyTrans::count(); i++ ) {
        KeyTrans* s = KeyTrans::find(i );
        assert( s );
    }
    m_factory = new MetaFactory();
    Default def(m_factory);
    m_sessions.setAutoDelete( TRUE );
    m_curSession = 0;
    m_manager = new ProfileManager( m_factory );
    m_manager->load();

    initUI();
    populateProfiles();
}
void MainWindow::initUI() {
    setToolBarsMovable( FALSE  );

    /* tool bar for the menu */
    m_tool = new QToolBar( this );
    m_tool->setHorizontalStretchable( TRUE );

    m_bar = new QMenuBar( m_tool );
    m_console = new QPopupMenu( this );
    m_scripts = new QPopupMenu( this );
    m_sessionsPop= new QPopupMenu( this );
    m_settings = new QPopupMenu( this );

    /* add a toolbar for icons */
    m_icons = new QToolBar(this);

    /*
     * new Action for new sessions
     */
    QAction* a = new QAction(tr("New Connection"),
                             Resource::loadPixmap( "new" ),
                             QString::null, 0, this, 0);
    a->addTo( m_console );
    a->addTo( m_icons );
    connect(a, SIGNAL(activated() ),
            this, SLOT(slotNew() ) );

    /*
     * connect action
     */
    m_connect = new QAction();
    m_connect->setText( tr("Connect") );
    m_connect->addTo( m_console );
    connect(m_connect, SIGNAL(activated() ),
            this, SLOT(slotConnect() ) );

    /*
     * disconnect action
     */
    m_disconnect = new QAction();
    m_disconnect->setText( tr("Disconnect") );
    m_disconnect->addTo( m_console );
    connect(m_disconnect, SIGNAL(activated() ),
            this, SLOT(slotDisconnect() ) );

    m_transfer = new QAction();
    m_transfer->setText( tr("Transfer file...") );
    m_transfer->addTo( m_console );
    connect(m_transfer, SIGNAL(activated() ),
            this, SLOT(slotTransfer() ) );

    /*
     * terminate action
     */
    m_terminate = new QAction();
    m_terminate->setText( tr("Terminate") );
    m_terminate->addTo( m_console );
    connect(m_terminate, SIGNAL(activated() ),
            this, SLOT(slotTerminate() ) );

    a = new QAction();
    a->setText( tr("Close Window") );
    a->addTo( m_console );
    connect(a, SIGNAL(activated() ),
            this, SLOT(slotClose() ) );

    /*
     * the settings action
     */
    m_setProfiles = new QAction(tr("Configure Profiles"),
                             Resource::loadPixmap( "SettingsIcon" ),
                             QString::null, 0, this, 0);
    m_setProfiles->addTo( m_settings );
    m_setProfiles->addTo( m_icons );
    connect( m_setProfiles, SIGNAL(activated() ),
             this, SLOT(slotConfigure() ) );

    /*
     * script actions
     */
    m_recordScript = new QAction(tr("Record Script"), QString::null, 0, this, 0);
    m_recordScript->addTo(m_scripts);
    connect(m_recordScript, SIGNAL(activated()), this, SLOT(slotRecordScript()));

    m_saveScript = new QAction(tr("Save Script"), QString::null, 0, this, 0);
    m_saveScript->addTo(m_scripts);
    connect(m_saveScript, SIGNAL(activated()), this, SLOT(slotSaveScript()));

    m_runScript = new QAction(tr("Run Script"), QString::null, 0, this, 0);
    m_runScript->addTo(m_scripts);
    connect(m_runScript, SIGNAL(activated()), this, SLOT(slotRunScript()));

    /*
     * action that open/closes the keyboard
     */
    m_openKeys = new QAction (tr("Open Keyboard..."),
                             Resource::loadPixmap( "down" ),
                             QString::null, 0, this, 0);

    m_openKeys->setToggleAction(true);

    connect (m_openKeys, SIGNAL(toggled(bool)),
             this, SLOT(slotOpenKeb(bool)));
    m_openKeys->addTo(m_icons);


    /* insert the submenu */
    m_console->insertItem(tr("New from Profile"), m_sessionsPop,
                          -1, 0);

    /* insert the connection menu */
    m_bar->insertItem( tr("Connection"), m_console );

    /* the scripts menu */
    m_bar->insertItem( tr("Scripts"), m_scripts );

    /* the settings menu */
    m_bar->insertItem( tr("Settings"), m_settings );

    /* and the keyboard */
    m_keyBar = new QToolBar(this);
    addToolBar( m_keyBar,  "Keyboard", QMainWindow::Top, TRUE );
    m_keyBar->setHorizontalStretchable( TRUE );
    m_keyBar->hide();

    m_kb = new FunctionKeyboard(m_keyBar);



    m_connect->setEnabled( false );
    m_disconnect->setEnabled( false );
    m_terminate->setEnabled( false );
    m_transfer->setEnabled( false );
    m_recordScript->setEnabled( false );
    m_saveScript->setEnabled( false );
    m_runScript->setEnabled( false );

    /*
     * connect to the menu activation
     */
    connect( m_sessionsPop, SIGNAL(activated( int ) ),
             this, SLOT(slotProfile( int ) ) );

    m_consoleWindow = new TabWidget( this, "blah");
    connect(m_consoleWindow, SIGNAL(activated(Session*) ),
            this, SLOT(slotSessionChanged(Session*) ) );
    setCentralWidget( m_consoleWindow );

}

ProfileManager* MainWindow::manager() {
    return m_manager;
}
TabWidget* MainWindow::tabWidget() {
    return m_consoleWindow;
}
void MainWindow::populateProfiles() {
    m_sessionsPop->clear();
    Profile::ValueList list = manager()->all();
    for (Profile::ValueList::Iterator it = list.begin(); it != list.end(); ++it ) {
        m_sessionsPop->insertItem( (*it).name() );
    }

}
MainWindow::~MainWindow() {
    delete m_factory;
    manager()->save();
}

MetaFactory* MainWindow::factory() {
    return m_factory;
}

Session* MainWindow::currentSession() {
    return m_curSession;
}

QList<Session> MainWindow::sessions() {
    return m_sessions;
}

void MainWindow::slotNew() {
    qWarning("New Connection");
    ProfileEditorDialog dlg(factory() );
    dlg.showMaximized();
    int ret = dlg.exec();

    if ( ret == QDialog::Accepted ) {
        create( dlg.profile() );
    }
}

void MainWindow::slotRecordScript() {
/*    if (currentSession()) {
        currentSession()->emulationLayer()->startRecording();
    }
    */
}

void MainWindow::slotSaveScript() {
/*    if (currentSession() && currentSession()->emulationLayer()->isRecording()) {
        MimeTypes types;
        QStringList script;
        script << "text/plain";
        types.insert("Script", script);
        QString filename = OFileDialog::getSaveFileName(2, "/", QString::null, types);
        if (!filename.isEmpty()) {
            currentSession()->emulationLayer()->script()->saveTo(filename);
            currentSession()->emulationLayer()->clearScript();
        }
    }
    */
}

void MainWindow::slotRunScript() {
/*
    if (currentSession()) {
        MimeTypes types;
        QStringList script;
        script << "text/plain";
        types.insert("Script", script);
        QString filename = OFileDialog::getOpenFileName(2, "/", QString::null, types);
        if (!filename.isEmpty()) {
            Script script(DocLnk(filename).file());
            currentSession()->emulationLayer()->runScript(&script);
        }
    }
    */
}

void MainWindow::slotConnect() {
    if ( currentSession() ) {
        bool ret = currentSession()->layer()->open();
		if(!ret) QMessageBox::warning(currentSession()->widgetStack(),
			QObject::tr("Failed"),
			QObject::tr("Connecting failed for this session."));
	}
}

void MainWindow::slotDisconnect() {
    if ( currentSession() )
        currentSession()->layer()->close();
}

void MainWindow::slotTerminate() {
    if ( currentSession() )
        currentSession()->layer()->close();

    slotClose();
    /* FIXME move to the next session */
}

void MainWindow::slotConfigure() {
    qWarning("configure");
    ConfigDialog conf( manager()->all(), factory() );
    conf.showMaximized();

    int ret = conf.exec();

    if ( QDialog::Accepted == ret ) {
        qWarning("conf %d", conf.list().count() );
        manager()->setProfiles( conf.list() );
        manager()->save();
        populateProfiles();
    }
}
/*
 * we will remove
 * this window from the tabwidget
 * remove it from the list
 * delete it
 * and set the currentSession()
 */
void MainWindow::slotClose() {
    qWarning("close");
    if (!currentSession() )
        return;

    tabWidget()->remove( currentSession() );
    /*it's autodelete */
    m_sessions.remove( m_curSession );
    m_curSession = m_sessions.first();
    tabWidget()->setCurrent( m_curSession );
}

/*
 * We will get the name
 * Then the profile
 * and then we will make a profile
 */
void MainWindow::slotProfile( int id) {
    Profile prof = manager()->profile( m_sessionsPop->text( id)  );
    create( prof );
}
void MainWindow::create( const Profile& prof ) {
    Session *ses = manager()->fromProfile( prof, tabWidget() );

    if((!ses) || (!ses->layer()) || (!ses->widgetStack()))
	{
		QMessageBox::warning(this,
			QObject::tr("Session failed"),
			QObject::tr("Cannot open session: Not all components were found."));
		//if(ses) delete ses;
		return;
	}

    m_sessions.append( ses );
    tabWidget()->add( ses );
    m_curSession = ses;

    // dicide if its a local term ( then no connction and no tranfer)
    m_connect->setEnabled( true );
    m_disconnect->setEnabled( true );
    m_terminate->setEnabled( true );
    m_transfer->setEnabled( true );
    m_recordScript->setEnabled( true );
    m_saveScript->setEnabled( true );
    m_runScript->setEnabled( true );


}

void MainWindow::slotTransfer()
{
    //   if ( currentSession() ) {
	TransferDialog dlg(this);
	dlg.showMaximized();
	dlg.exec();
         // }
}


void MainWindow::slotOpenKeb(bool state) {

    if (state) m_keyBar->show();
    else m_keyBar->hide();

}
void MainWindow::slotSessionChanged( Session* ses ) {
    if ( ses ) {
        qWarning("changing %s", ses->name().latin1() );
        m_curSession = ses;
    }
}

void MainWindow::setOn() {

/*
    m_connect
     m_disconnect
     m_terminate
     m_transfer
     m_recordScript
     m_saveScript
     m_runScript
*/


}
