

#include <qtabwidget.h>
#include <qlayout.h>

#include <qpe/storage.h>

#include "mediumwidget.h"
#include "mediumglobal.h"

#include "mainwindow.h"

using namespace MediumMountSetting;


MainWindow::MainWindow( QWidget *parent, const char *name )
  : QMainWindow( parent, name )

{
  //  m_lay = new QVBoxLayout( this );
  m_tab = new QTabWidget( this );
  setCentralWidget( m_tab );
  init();
}

MainWindow::~MainWindow()
{

}
void MainWindow::init()
{
  m_global = new MediumGlobalWidget( m_tab, "test drive" );
  m_tab->addTab( m_global, tr("Global") );

  StorageInfo storage;
  const QList<FileSystem> &fs = storage.fileSystems();
  QListIterator<FileSystem> it( fs );
  MediumMountWidget *wid;
  for( ; it.current(); ++it ){
    if( (*it)->isRemovable() ){
      wid = new MediumMountWidget((*it)->path(), QPixmap(), m_tab );
      m_mediums.append( wid );
      m_tab->addTab( wid, (*it)->name()  );
    }
  }
}

void MainWindow::slotGlobalChanged(int )
{

}
void MainWindow::slotCheckingChanged(int )
{

}
