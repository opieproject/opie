

#include <qtabwidget.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <qpe/storage.h>

#include "mediumwidget.h"
#include "mediumglobal.h"

#include "mainwindow.h"

using namespace MediumMountSetting;

/* TRANSLATOR MediumMountSetting::MainWindow */

MainWindow::MainWindow( QWidget *parent, const char *name, bool modal, WFlags )
  : QDialog( parent, name, modal, WStyle_ContextHelp  )

{
    setCaption ( tr( "Medium Mount Settings" ));

    m_lay = new QVBoxLayout( this );

    m_tab = new QTabWidget( this );

    m_lay->addWidget( m_tab );

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

void MainWindow::accept()
{
    m_global->writeConfig();

    MediumMountWidget *confWidget;
    for ( confWidget = m_mediums.first(); confWidget != 0;
              confWidget = m_mediums.next() ) {
        confWidget->writeConfig();
    }

    QDialog::accept();
}

void MainWindow::done ( int r )
{
	QDialog::done( r );
	close ();
}


void MainWindow::slotGlobalChanged(int )
{

}
void MainWindow::slotCheckingChanged(int )
{

}
