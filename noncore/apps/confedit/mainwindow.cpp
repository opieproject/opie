/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// (c) 2002 Patrick S. Vogt <tille@handhelds.org>


#include "mainwindow.h"

#include <qpe/qpemenubar.h>
#include <qpe/qpemessagebox.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/qpetoolbar.h>
#include <qaction.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qtoolbutton.h>
#include <qstring.h>
#include <qlabel.h>
#include <qfile.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlineedit.h>

#include "listviewconfdir.h"
#include "listviewitemconf.h"
#include "listviewitemconfigentry.h"


MainWindow::MainWindow( QWidget *parent, const char *name, WFlags f ) :
  QMainWindow( parent, name, f ), _currentItem(0), _fileItem(0)
{	
  setCaption( tr("Conf File Editor") );

  popupTimer = new QTimer(this);
  popupMenu = new QPopupMenu(this);

  QWidget *mainWidget = new  QWidget(this);
  setCentralWidget( mainWidget);

  QGridLayout *mainLayout = new QGridLayout( mainWidget );
	mainLayout->setSpacing( 3 );
  mainLayout->setMargin( 3 );


  settingList = new ListViewConfDir( "/root/Settings/", this, "settingslist");
  mainLayout->addWidget( settingList, 0, 0 );

  editor = new EditWidget(this);
  mainLayout->addWidget( editor, 1, 0 );

  connect (settingList, SIGNAL( currentChanged(QListViewItem*) ),
						this, SLOT(setCurrent(QListViewItem*)));

	  connect( popupTimer, SIGNAL(timeout()),
  			this, SLOT(showPopup()) );
		connect( this, SIGNAL( clicked( QListViewItem* ) ),
			   this, SLOT( stopTimer( QListViewItem* ) ) );	

  	connect( editor->LineEditGroup, SIGNAL( textChanged(const QString&) ),
           SLOT( groupChanged(const QString&) ) );
  	connect( editor->LineEditKey, SIGNAL( textChanged(const QString&) ),
           SLOT( keyChanged(const QString&) ) );
  	connect( editor->LineEditValue, SIGNAL( textChanged(const QString&) ),
           SLOT( valueChanged(const QString&) ) );
  makeMenu();
}

void MainWindow::makeMenu()
{


}

MainWindow::~MainWindow()
{
}



void MainWindow::setCurrent(QListViewItem *item)
{
	if (!item) return;
 	ListViewItemConf *i = (ListViewItemConf*) item;
  if (!i) return;
  if (i->getType() == ListViewItemConf::File)
  {
  	qDebug("start timer");
	  popupTimer->start( 750, true );
  	editor->hide();
   	updateGeometry();
    _currentItem=0;
    _fileItem = (ListViewItemConfFile*)item;
   	return;
  }else editor->show();
  _fileItem = 0;
 	_currentItem = (ListViewItemConfigEntry*)item;
  if (!_currentItem) return;
  QString file  = _currentItem->getFile();
  QString group = _currentItem->getGroup();
  QString key   = _currentItem->getKey();
  QString val   = _currentItem->getValue();
  editor->TextFileName->setText(file);
  editor->LineEditGroup->setText(group);
  if (!key.isEmpty())
  {
   	editor->isKey(true);
    editor->LineEditKey->setText(key);
    editor->LineEditValue->setText(val);
  }else{
    editor->isKey(false);
  }
  updateGeometry();
}


void MainWindow::groupChanged(const QString &g)
{
	if (!_currentItem) return;
	_currentItem->setGroup(g);
}

void MainWindow::keyChanged(const QString &k)
{
	if (!_currentItem) return;
	_currentItem->keyChanged(k);
}

void MainWindow::valueChanged(const QString &v)
{
	if (!_currentItem) return;
	_currentItem->valueChanged(v);
}


void MainWindow::stopTimer( QListViewItem* )
{
	qDebug("stopTimer");
	popupTimer->stop();
}

void MainWindow::saveConfFile()
{
	if (!_fileItem) return;	
 	_fileItem->save();
}

void MainWindow::showPopup()
{
	qDebug("showPopup");
 	if (!_fileItem) return;
  popupMenu->clear();
 		
 	QAction *popupAction;
  popupAction = new QAction( tr("Save"),QString::null,  0, this, 0 );
	popupAction->addTo( popupMenu );
  connect( popupAction, SIGNAL( activated() ),
	   	this , SLOT( saveConfFile() ) );


  popupMenu->popup( QCursor::pos() );
}
