#include "qcheckbook.h"

#include <qmenubar.h>
#include <qstatusbar.h>
#include <qpopupmenu.h>
#include <qapplication.h>
#include <qfile.h>
#include <qdir.h>

QCheckBook::QCheckBook()
  : QMainWindow(),
  m_view(),
  m_view2(),
  m_view3()
{
  initCheck = false;
  initMM = false;
  setCaption("Checking");
  statusBar()->hide();
  menuBar()->hide();

  bar = new QToolBar(this);
  bar->setHorizontalStretchable( TRUE );

  addToolBar(bar);

  Config config("qcheckbook");
  config.setGroup("Global");
  QString lastCheck = config.readEntry("LastCheckBook", QString(""));

  QString checkdirname = QDir::homeDirPath();
  checkdirname.append("/.checkbooks/");
  checkdirname.append(lastCheck);
  QFile f(checkdirname);


  if (lastCheck.isEmpty() == false && lastCheck != "" && f.exists() == true)
  {
    newCheck(lastCheck);
  } else {
    initMainMenus();
  }

  setToolBarsMovable( FALSE );
}

void QCheckBook::newCheck(const QString &filename)
{
  if (filename.isEmpty() == false)
  {
    initCheck = true;
    if (m_view != 0)
    {
      m_view=0;
      delete m_view;
    }
    m_view = new QCheckView(this, filename);
    m_view->hide();
    connect(m_view, SIGNAL(reload(const QString &)), this, SLOT(newCheck(const QString &)));
    if (initMM == true)
    {
      delete nb1;
    }
    bar->clear();
    mbar = new QMenuBar(bar);
    mbar->setMargin(0);

    QPixmap newIcon = Resource::loadPixmap( "new" );
    nb2 = new QToolButton( newIcon, "New", QString::null, m_view, SLOT(newClicked()), bar, "new item" );
    QPixmap pixmap = Resource::loadPixmap( "pixmap" );
    m_filename = filename;
    nb3 = new QToolButton( pixmap, "Graph", QString::null, this, SLOT(newGraph()), bar, "new graph" );

    QPixmap closeIcon = Resource::loadPixmap( "close" );
    nb4 = new QToolButton( closeIcon, "Close", QString::null, this, SLOT(initMainMenus()), bar, "close graph" );

    popup = new QPopupMenu(m_view);
    popup->insertItem("&New Entry", m_view, SLOT(newClicked()));
    popup->insertItem("&Graph Checkbook", this, SLOT(newGraph()));
    popup->insertItem("&Close Checkbook", this, SLOT(initMainMenus()));
    popup->insertItem("&Exit", this, SLOT(close()));
    mbar->insertItem("&File", popup);

    setCentralWidget(m_view);
    m_view->show();

    Config config("qcheckbook");
    config.setGroup("Global");
    config.writeEntry("LastCheckBook", filename);
    initMM = false;
  }
}

void QCheckBook::deleteCheck(const QString &filename)
{
    qDebug("Deleting "+filename);
}

void QCheckBook::close()
{
  QApplication::exit();
}

void QCheckBook::newGraph()
{
  if (m_filename.isEmpty() == false)
  {
    m_view2 = new QCheckGraph(m_filename);
    m_view2->showMaximized();
  }
}

void QCheckBook::initMainMenus()
{
  Config config("qcheckbook");
  config.setGroup("Global");
  config.writeEntry("LastCheckBook", QString(""));
  initMM = true;
  m_filename = "";
  if (m_view3 != 0)
  {
    delete m_view3;
  }
  m_view3 = new QCheckMainMenu(this);
  m_view3->hide();

  if (initCheck == true)
  {
    delete nb2;
    delete nb3;
    delete nb4;
  }

  bar->clear();

  mbar = new QMenuBar(bar);
  mbar->setMargin(0);

  QPixmap newIcon = Resource::loadPixmap( "new" );
  nb1 = new QToolButton( newIcon, "New", QString::null, m_view3, SLOT(newClicked()), bar, "new book" );

  popup = new QPopupMenu();
  popup->insertItem("&New", m_view3, SLOT(newClicked()));
  popup->insertItem("&Delete", m_view3, SLOT(deleteClicked()));
  popup->insertItem("&Exit", this, SLOT(close()));
  mbar->insertItem("&File", popup);

  setCentralWidget(m_view3);
  m_view3->show();

  connect(m_view3, SIGNAL(itemSelected(const QString &)), this, SLOT(newCheck(const QString &)));
  initCheck = false;
}
