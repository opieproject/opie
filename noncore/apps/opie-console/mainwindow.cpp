#include <assert.h>

#include <qaction.h>
#include <qmenubar.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>
#include <qfileinfo.h>

#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <qpe/filemanager.h>
#include <qpe/mimetype.h>

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
#include "emulation_handler.h"
#include "script.h"

static char * filesave_xpm[] = {
"16 16 78 1",
"   c None",
".  c #343434",
"+  c #A0A0A0",
"@  c #565656",
"#  c #9E9E9E",
"$  c #525252",
"%  c #929292",
"&  c #676767",
"*  c #848484",
"=  c #666666",
"-  c #D8D8D8",
";  c #FFFFFF",
">  c #DBDBDB",
",  c #636363",
"'  c #989898",
")  c #2D2D2D",
"!  c #909090",
"~  c #AEAEAE",
"{  c #EAEAEA",
"]  c #575757",
"^  c #585858",
"/  c #8A8A8A",
"(  c #828282",
"_  c #6F6F6F",
":  c #C9C9C9",
"<  c #050505",
"[  c #292929",
"}  c #777777",
"|  c #616161",
"1  c #3A3A3A",
"2  c #BEBEBE",
"3  c #2C2C2C",
"4  c #7C7C7C",
"5  c #F6F6F6",
"6  c #FCFCFC",
"7  c #6B6B6B",
"8  c #959595",
"9  c #4F4F4F",
"0  c #808080",
"a  c #767676",
"b  c #818181",
"c  c #B8B8B8",
"d  c #FBFBFB",
"e  c #F9F9F9",
"f  c #CCCCCC",
"g  c #030303",
"h  c #737373",
"i  c #7A7A7A",
"j  c #7E7E7E",
"k  c #6A6A6A",
"l  c #FAFAFA",
"m  c #505050",
"n  c #9D9D9D",
"o  c #333333",
"p  c #7B7B7B",
"q  c #787878",
"r  c #696969",
"s  c #494949",
"t  c #555555",
"u  c #949494",
"v  c #E6E6E6",
"w  c #424242",
"x  c #515151",
"y  c #535353",
"z  c #3E3E3E",
"A  c #D4D4D4",
"B  c #0C0C0C",
"C  c #353535",
"D  c #474747",
"E  c #ECECEC",
"F  c #919191",
"G  c #7D7D7D",
"H  c #000000",
"I  c #404040",
"J  c #858585",
"K  c #323232",
"L  c #D0D0D0",
"M  c #1C1C1C",
"    ...+        ",
"   @#$%&..+     ",
"   .*=-;;>,..+  ",
"  ')!~;;;;;;{]..",
"  ^/(-;;;;;;;_:<",
"  [}|;;;;;;;{12$",
" #34-55;;;;678$+",
" 90ab=c;dd;e1fg ",
" [ahij((kbl0mn$ ",
" op^q^^7r&]s/$+ ",
"@btu;vbwxy]zAB  ",
"CzDEvEv;;DssF$  ",
"G.H{E{E{IxsJ$+  ",
"  +...vEKxzLM   ",
"     +...z]n$   ",
"        +...    "};



MainWindow::MainWindow(QWidget *parent, const char *name, WFlags) : QMainWindow(parent, name, WStyle_ContextHelp)  {
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
    m_scriptsData.setAutoDelete(TRUE);

    initUI();
    populateProfiles();
    populateScripts();
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
    m_scriptsPop = new QPopupMenu( this );

    /* add a toolbar for icons */
    m_icons = new QToolBar(this);

    /*
     * the settings action
     */
    m_setProfiles = new QAction(tr("Configure Profiles"),
                             Resource::loadPixmap( "SettingsIcon" ),
                             QString::null, 0, this, 0);
    m_setProfiles->addTo( m_console );
    connect( m_setProfiles, SIGNAL(activated() ),
             this, SLOT(slotConfigure() ) );

    m_console->insertSeparator();
    /*
     * new Action for new sessions
     */
    QAction* newCon = new QAction(tr("New Connection"),
                             Resource::loadPixmap( "new" ),
                             QString::null, 0, this, 0);
    newCon->addTo( m_console );
    connect( newCon, SIGNAL(activated() ),
            this, SLOT(slotNew() ) );

    m_console->insertSeparator();

    QAction *saveCon = new QAction(tr("Save Connection"),
                    QPixmap( ( const char** ) filesave_xpm ) , QString::null,
                    0, this, 0 );
    saveCon->addTo( m_console );
    connect( saveCon, SIGNAL(activated() ),
            this, SLOT(slotSaveSession() ) );
    m_console->insertSeparator();

    /*
     * connect action
     */
    m_connect = new QAction( tr("Connect"), Resource::loadPixmap("console/connected"),
                             QString::null, 0, this, 0 );
    m_connect->addTo( m_console );
    connect(m_connect, SIGNAL(activated() ),
            this, SLOT(slotConnect() ) );

    /*
     * disconnect action
     */
    m_disconnect = new QAction( tr("Disconnect"), Resource::loadPixmap("console/notconnected"),
                                QString::null, 0, this, 0 );
    m_disconnect->addTo( m_console );
    connect(m_disconnect, SIGNAL(activated() ),
            this, SLOT(slotDisconnect() ) );

    m_console->insertSeparator();

    m_transfer = new QAction( tr("Transfer file..."), Resource::loadPixmap("pass") , QString::null,
                    0, this, 0  );
    m_transfer->addTo( m_console );
    connect(m_transfer, SIGNAL(activated() ),
            this, SLOT(slotTransfer() ) );


    /*
     * fullscreen
     */
    m_isFullscreen = false;

    m_fullscreen = new QAction( tr("Full screen"), Resource::loadPixmap( "fullscreen" )
                           , QString::null, 0, this, 0);
    m_fullscreen->addTo( m_console );
    connect( m_fullscreen, SIGNAL( activated() ),
             this,  SLOT( slotFullscreen() ) );

    m_console->insertSeparator();
    /*
     * terminate action
     */
    m_terminate = new QAction();
    m_terminate->setText( tr("Terminate") );
    m_terminate->addTo( m_console );
    connect(m_terminate, SIGNAL(activated() ),
            this, SLOT(slotTerminate() ) );

    m_closewindow = new QAction();
    m_closewindow->setText( tr("Close Window") );
    m_closewindow->addTo( m_console );
    connect( m_closewindow, SIGNAL(activated() ),
            this, SLOT(slotClose() ) );


    /*
     * script actions
     */
    m_runScript_id = m_scripts->insertItem(tr("Run Script"), m_scriptsPop, -1, 0);
    connect(m_scriptsPop, SIGNAL(activated(int)), this, SLOT(slotRunScript(int)));

    m_recordScript = new QAction(tr("Record Script"), QString::null, 0, this, 0);
    m_recordScript->addTo(m_scripts);
    connect(m_recordScript, SIGNAL(activated()), this, SLOT(slotRecordScript()));

    m_saveScript = new QAction(tr("Save Script"), QString::null, 0, this, 0);
    m_saveScript->addTo(m_scripts);
    connect(m_saveScript, SIGNAL(activated()), this, SLOT(slotSaveScript()));


    /*
     * action that open/closes the keyboard
     */
    m_openKeys = new QAction (tr("Open Keyboard..."),
                             Resource::loadPixmap( "console/keys/keyboard_icon" ),
                             QString::null, 0, this, 0);
    m_openKeys->setToggleAction(true);
    connect (m_openKeys, SIGNAL(toggled(bool)), this, SLOT(slotOpenKeb(bool)));

    /* insert the submenu */
    m_console->insertItem(tr("New from Profile"), m_sessionsPop,
                          -1, 0);

    /* insert the connection menu */
    m_bar->insertItem( tr("Connection"), m_console );

    /* the scripts menu */
    m_bar->insertItem( tr("Scripts"), m_scripts );

    /* and the keyboard */
    m_keyBar = new QToolBar(this);
    addToolBar( m_keyBar,  "Keyboard", QMainWindow::Top, TRUE );
    m_keyBar->setHorizontalStretchable( TRUE );
    m_keyBar->hide();

    m_kb = new FunctionKeyboard(m_keyBar);
    connect(m_kb, SIGNAL(keyPressed(FKey, ushort, ushort, bool)),
            this, SLOT(slotKeyReceived(FKey, ushort, ushort, bool)));


    QAction *a = new QAction(tr("Copy"),
                    Resource::loadPixmap("copy"), QString::null,
                    0, this, 0 );
    //a->addTo( m_icons );
    connect( a, SIGNAL(activated() ),
             this, SLOT(slotCopy() ) );

    QAction *paste = new QAction(tr("Paste"),
                    Resource::loadPixmap("paste"), QString::null,
                    0, this, 0 );
    connect( paste, SIGNAL(activated() ),
             this, SLOT(slotPaste() ) );


    newCon->addTo( m_icons );
    m_setProfiles->addTo( m_icons );
    paste->addTo( m_icons );
    m_openKeys->addTo(m_icons);
    m_fullscreen->addTo( m_icons );

    m_connect->setEnabled( false );
    m_disconnect->setEnabled( false );
    m_terminate->setEnabled( false );
    m_transfer->setEnabled( false );
    m_scripts->setItemEnabled(m_runScript_id, false);
    m_recordScript->setEnabled( false );
    m_saveScript->setEnabled( false );
    m_fullscreen->setEnabled( false );
    m_closewindow->setEnabled( false );

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

void MainWindow::populateScripts() {
    m_scriptsPop->clear();
    m_scriptsData.clear();
    DocLnkSet files(QPEApplication::documentDir(), "text/plain");
    QListIterator<DocLnk> dit(files.children());
    for (; dit.current(); ++dit) {
        if (*dit && (*dit)->name().length()>0) {
            QFileInfo info((*dit)->file());
            if (info.extension(false) == "script") {
                m_scriptsData.append(new DocLnk(**dit));
                m_scriptsPop->insertItem((*dit)->name());
            }
        }
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
    ProfileEditorDialog dlg(factory() );
    dlg.showMaximized();
    dlg.setCaption( tr("New Connection") );
    int ret = dlg.exec();

    if ( ret == QDialog::Accepted ) {
        create( dlg.profile() );
    }
}

void MainWindow::slotRecordScript() {
    if (currentSession()) {
        currentSession()->emulationHandler()->startRecording();
        m_saveScript->setEnabled(true);
        m_recordScript->setEnabled(false);
    }
}

void MainWindow::slotSaveScript() {
    if (currentSession() && currentSession()->emulationHandler()->isRecording()) {
        QMap<QString, QStringList> map;
        QStringList text;
        text << "text/plain";
        map.insert(tr("Script"), text );
        QString filename = OFileDialog::getSaveFileName(2, QPEApplication::documentDir(), QString::null, map);
        if (!filename.isEmpty()) {
            QFileInfo info(filename);
            if (info.extension(FALSE) != "script")
                filename += ".script";
            DocLnk nf;
            nf.setType("text/plain");
            nf.setFile(filename);
            nf.setName(info.fileName());
            FileManager fm;
            fm.saveFile(nf, currentSession()->emulationHandler()->script()->script());
            currentSession()->emulationHandler()->clearScript();
            m_saveScript->setEnabled(false);
            m_recordScript->setEnabled(true);
            populateScripts();
        }
    }
}

void MainWindow::slotRunScript(int id) {
    if (currentSession()) {
        int index = m_scriptsPop->indexOf(id);
        DocLnk *lnk = m_scriptsData.at(index);
        QString filePath = lnk->file();
        Script script(filePath);
        currentSession()->emulationHandler()->runScript(&script);
    }
}

void MainWindow::slotConnect() {
    if ( currentSession() ) {
        bool ret = currentSession()->layer()->open();
        if(!ret) QMessageBox::warning(currentSession()->widgetStack(),
                                      QObject::tr("Failed"),
                                      QObject::tr("Connecting failed for this session."));
        else {
            m_connect->setEnabled( false );
            m_disconnect->setEnabled( true );

            // if it does not support file transfer, disable the menu entry
            if ( ( m_curSession->layer() )->supports()[1] == 0 ) {
                m_transfer->setEnabled( false );
            } else {
                m_transfer->setEnabled( true );
            }

            m_recordScript->setEnabled( true );
            m_scripts->setItemEnabled(m_runScript_id, true);
        }
    }
}

void MainWindow::slotDisconnect() {
    if ( currentSession() ) {
        currentSession()->layer()->close();
        m_connect->setEnabled( true );
        m_disconnect->setEnabled( false );
        m_transfer->setEnabled( false );
        m_recordScript->setEnabled( false);
        m_saveScript->setEnabled( false );
        m_scripts->setItemEnabled(m_runScript_id, false);
    }
}

void MainWindow::slotTerminate() {
    if ( currentSession() )
        currentSession()->layer()->close();

    slotClose();
    /* FIXME move to the next session */
}

void MainWindow::slotConfigure() {
    ConfigDialog conf( manager()->all(), factory() );
    conf.showMaximized();

    int ret = conf.exec();

    if ( QDialog::Accepted == ret ) {
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
    if (!currentSession() )
        return;

    Session* ses = currentSession();
    qWarning("removing! currentSession %s", currentSession()->name().latin1() );
	/* set to NULL to be safe, if its needed slotSessionChanged resets it automatically */
	m_curSession = NULL;
    tabWidget()->remove( /*currentSession()*/ses );
    /*it's autodelete */
    m_sessions.remove( ses );
    qWarning("after remove!!");

    if (!currentSession() ) {
        m_connect->setEnabled( false );
        m_disconnect->setEnabled( false );
        m_terminate->setEnabled( false );
        m_transfer->setEnabled( false );
        m_recordScript->setEnabled( false );
        m_saveScript->setEnabled( false );
        m_scripts->setItemEnabled(m_runScript_id, false);
        m_fullscreen->setEnabled( false );
        m_closewindow->setEnabled( false );
    }

    m_kb->loadDefaults();
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
	if(m_curSession)
		if(m_curSession->transferDialog()) m_curSession->transferDialog()->hide();

    Session *ses = manager()->fromProfile( prof, tabWidget() );

    if((!ses) || (!ses->layer()) || (!ses->widgetStack()))
	{
		QMessageBox::warning(this,
			QObject::tr("Session failed"),
			QObject::tr("<qt>Cannot open session: Not all components were found.</qt>"));
		//if(ses) delete ses;
		return;
	}

    m_sessions.append( ses );
    tabWidget()->add( ses );
    tabWidget()->repaint();
    m_curSession = ses;

    // dicide if its a local term ( then no connction and no tranfer), maybe make a wrapper method out of it
    m_connect->setEnabled( true );
    m_disconnect->setEnabled( false );
    m_terminate->setEnabled( true );
    m_fullscreen->setEnabled( true );
    m_closewindow->setEnabled( true );
    m_transfer->setEnabled( false );
    m_recordScript->setEnabled( false );
    m_saveScript->setEnabled( false );
    m_scripts->setItemEnabled(m_runScript_id, false);

    // is io_layer wants direct connection, then autoconnect
    //if ( ( m_curSession->layer() )->supports()[0] == 1 ) {
    if (prof.autoConnect()) {
        slotConnect();
    }


    QWidget *w = currentSession()->widget();
    if(w) w->setFocus();

    m_kb->load(currentSession()->profile());
}

void MainWindow::slotTransfer()
{
    if ( currentSession() ) {
	Session *mysession = currentSession();
	TransferDialog dlg(/*mysession->widgetStack()*/this, this);
	mysession->setTransferDialog(&dlg);
	//dlg.reparent(mysession->widgetStack(), QPoint(0, 0));
	//dlg.showMaximized();
	currentSession()->widgetStack()->addWidget(&dlg, -1);
	dlg.show();
	//dlg.exec();
	while(dlg.isRunning()) qApp->processEvents();
	mysession->setTransferDialog(0l);
    }
}


void MainWindow::slotOpenKeb(bool state) {

    if (state) m_keyBar->show();
    else m_keyBar->hide();

}


void MainWindow::slotOpenButtons( bool state ) {

    if ( state ) {
        m_buttonBar->show();
    } else {
        m_buttonBar->hide();
    }
}



void MainWindow::slotSessionChanged( Session* ses ) {
    qWarning("changed!");

	if(m_curSession)
		if(m_curSession->transferDialog()) m_curSession->transferDialog()->hide();
	if(ses)
		if(ses->transferDialog()) ses->transferDialog()->show();

    if ( ses ) {
        m_curSession = ses;
        qDebug(QString("is connected : %1").arg(  m_curSession->layer()->isConnected() ) );
        if ( m_curSession->layer()->isConnected() ) {
            m_connect->setEnabled( false );
            m_disconnect->setEnabled( true );
            m_recordScript->setEnabled(!m_curSession->emulationHandler()->isRecording());
            m_saveScript->setEnabled(m_curSession->emulationHandler()->isRecording());
            m_scripts->setItemEnabled(m_runScript_id, true);
        } else {
            m_connect->setEnabled( true );
            m_disconnect->setEnabled( false );
            m_recordScript->setEnabled( false );
            m_saveScript->setEnabled( false );
            m_scripts->setItemEnabled(m_runScript_id, false);
        }

        if ( ( m_curSession->layer() )->supports()[1] == 0 ) {
            m_transfer->setEnabled( false );
        } else {
            m_transfer->setEnabled( true );
        }

        QWidget *w = m_curSession->widget();
        if(w) w->setFocus();

        m_kb->load(currentSession()->profile());
    }
}

void MainWindow::slotFullscreen() {



    if ( m_isFullscreen ) {
        ( m_curSession->widgetStack() )->reparent(  savedParentFullscreen, 0, QPoint(0,0), true );
        ( m_curSession->widgetStack() )->resize( savedParentFullscreen->width(), savedParentFullscreen->height() );
        ( m_curSession->emulationHandler() )->cornerButton()->hide();
        disconnect( ( m_curSession->emulationHandler() )->cornerButton(), SIGNAL( pressed() ), this, SLOT( slotFullscreen() ) );

    } else {
        savedParentFullscreen = ( m_curSession->widgetStack() )->parentWidget();
        ( m_curSession->widgetStack() )->setFrameStyle( QFrame::NoFrame );
        ( m_curSession->widgetStack() )->reparent( 0, WStyle_Tool | WStyle_Customize | WStyle_StaysOnTop
                                                   , QPoint(0,0), false );
        ( m_curSession->widgetStack() )->resize( qApp->desktop()->width(), qApp->desktop()->height() );
        ( m_curSession->widgetStack() )->setFocus();
        ( m_curSession->widgetStack() )->show();

        ( ( m_curSession->emulationHandler() )->cornerButton() )->show();

        connect( ( m_curSession->emulationHandler() )->cornerButton(), SIGNAL( pressed() ), this, SLOT( slotFullscreen() ) );
    }

    m_isFullscreen = !m_isFullscreen;
}


void MainWindow::slotKeyReceived(FKey k, ushort, ushort, bool pressed) {

    if ( m_curSession ) {

        QEvent::Type state;

        if (pressed) state = QEvent::KeyPress;
        else state = QEvent::KeyRelease;

        QKeyEvent ke(state, k.qcode, k.unicode, 0, QString(QChar(k.unicode)));

        // is this the best way to do this? cant figure out any other way to work
        QApplication::sendEvent((QObject *)m_curSession->widget(), &ke);
        ke.ignore();
    }
}
void MainWindow::slotCopy() {
    if (!currentSession() ) return;
    currentSession()->emulationHandler()->copy();
}
void MainWindow::slotPaste() {
    if (!currentSession() ) return;
    currentSession()->emulationHandler()->paste();
}

/*
 * Save the session
 */

void MainWindow::slotSaveSession() {
    if (!currentSession() ) {
      QMessageBox::information(this, tr("Save Connection"),
                                          tr("<qt>There is no Connection.</qt>"), 1 );
      return;
    }
    manager()->add( currentSession()->profile() );
    manager()->save();
    populateProfiles();
}
