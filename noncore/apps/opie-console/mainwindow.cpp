
#include <qaction.h>
#include <qmenubar.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>

#include "metafactory.h"
#include "mainwindow.h"

MainWindow::MainWindow()
{
    qWarning("c'tor");
    m_factory = new MetaFactory();
    m_sessions.setAutoDelete( TRUE );
    m_curSession = 0l;

    initUI();

}
void MainWindow::initUI() {
    setToolBarsMovable( FALSE  );

    m_tool = new QToolBar( this );
    m_tool->setHorizontalStretchable( TRUE );

    m_bar = new QMenuBar( m_tool );
    m_console = new QPopupMenu( this );

    /*
     * new Action for new sessions
     */
    QAction* a = new QAction();
    a->setText( tr("New Connection") );
    a->addTo( m_console );
    connect(a, SIGNAL(activated() ),
            this, SLOT(slotNew() ) );

    a = new QAction();
    a->setText( tr("New from Session") );

    m_connect = new QAction();
    m_connect->setText( tr("Connect") );
    m_connect->addTo( m_console );
    connect(m_connect, SIGNAL(activated() ),
            this, SLOT(slotConnect() ) );

    m_bar->insertItem( tr("Connection"), m_console );

}
MainWindow::~MainWindow() {
    delete m_factory;
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
