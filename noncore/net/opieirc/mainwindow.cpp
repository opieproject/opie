#include <qpe/qpemenubar.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpopupmenu.h>

#include "mainwindow.h"
#include "ircservertab.h"
#include "ircserverlist.h"

MainWindow::MainWindow(QWidget *parent, const char *name, WFlags f) : QMainWindow(parent, name, f) {
    setCaption(tr("IRC Client"));
    m_tabWidget = new QTabWidget(this);
    connect(m_tabWidget, SIGNAL(currentChanged(QWidget *)), this, SLOT(tabSelected(QWidget *)));
    setCentralWidget(m_tabWidget);
    setToolBarsMovable(FALSE);
    QPEMenuBar *menuBar = new QPEMenuBar(this);
    QPopupMenu *irc = new QPopupMenu(this);
    menuBar->insertItem(tr("IRC"), irc);
    QAction *a = new QAction(tr("New connection"), Resource::loadPixmap("pass"), QString::null, 0, this, 0);
    connect(a, SIGNAL(activated()), this, SLOT(newConnection()));
    a->addTo(irc);
    
    m_joinAction = new QAction(tr("Join channel"), Resource::loadPixmap("forward"), QString::null, 0, this, 0);
    m_joinAction->setEnabled(FALSE);
    connect(m_joinAction, SIGNAL(activated()), this, SLOT(join()));
    m_joinAction->addTo(irc);
}

void MainWindow::tabSelected(QWidget *) {
    m_joinAction->setEnabled(TRUE);
}

void MainWindow::closeTab() {
    /* Does not directly close the tab but triggers an action
       which at some point will close the tab using a callback */
    IRCTab *tab = (IRCTab *)m_tabWidget->currentPage();
    if (tab) {
        tab->remove();
    }
}

void MainWindow::join() {
    IRCTab *tab = (IRCTab *)m_tabWidget->currentPage();
    if (tab) {
        tab->session()->join("#opie.de");
    }
}

void MainWindow::addTab(IRCTab *tab) {
    m_tabWidget->addTab(tab, tab->title());
    m_tabWidget->showPage(tab);
    tabSelected(tab);
}

void MainWindow::killTab(IRCTab *tab) {
    m_tabWidget->removePage(tab);
    /* there might be nicer ways to do this .. */
    delete tab;
}

void MainWindow::newConnection() {
    IRCServer *server = new IRCServer();
    server->setHostname("irc.openprojects.net");
    server->setPort(6667);
    server->setDescription("OpenProjects");
    server->setNick("opie-irc");
    server->setUsername("opie-irc");
    server->setRealname("opie-irc");
    IRCServerTab *serverTab = new IRCServerTab(server, this, m_tabWidget);
    addTab(serverTab);
    serverTab->doConnect();

    /*
     * Serverlist : not functional yet
    IRCServerList *list = new IRCServerList(this, "ServerList", TRUE);
    list->exec();
    delete list;
    */
}
