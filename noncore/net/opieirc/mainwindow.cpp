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
}

void MainWindow::tabSelected(QWidget *) {
}

void MainWindow::closeTab() {
    /* Does not directly close the tab but triggers an action
       which at some point will close the tab using a callback */
    IRCTab *tab = (IRCTab *)m_tabWidget->currentPage();
    if (tab) {
        tab->remove();
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
    IRCServerList list(this, "ServerList", TRUE);
    if (list.exec() == QDialog::Accepted && list.hasServer()) {
        IRCServerTab *serverTab = new IRCServerTab(list.server(), this, m_tabWidget);
        addTab(serverTab);
        serverTab->doConnect();
    }
}
