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
#include <qpe/qpeapplication.h>
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

#include <stdlib.h>

MainWindow::MainWindow( QWidget *parent, const char *name, WFlags f ) :
  QDialog( parent, name, f ), _currentItem(0), _fileItem(0)
{	
  setCaption( tr("Conf File Editor") );

//	setBaseSize(  qApp->globalStrut() );
  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));//, sizePolicy().hasHeightForWidth() ) );

  mainLayout = new QVBoxLayout( this );
	mainLayout->setSpacing( 0 );
  mainLayout->setMargin( 0 );


  qDebug("creating settingList");
  settingList = new ListViewConfDir( QString(getenv("HOME")) + "/Settings", this, "settingslist");
  settingList->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));//, sizePolicy().hasHeightForWidth() ) );
  mainLayout->addWidget( settingList, 0);

  qDebug("creating editor");
  editor = new EditWidget(this);
  editor->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum));//, sizePolicy().hasHeightForWidth() ) );
  mainLayout->addWidget( editor, 1 );
  editor->layoutType( ListViewItemConf::File );

  makeMenu();

  connect(settingList, SIGNAL( pressed(QListViewItem*) ),
						this, SLOT(setCurrent(QListViewItem*)));
	connect( settingList, SIGNAL( clicked( QListViewItem* ) ),
					   this, SLOT( stopTimer( QListViewItem* ) ) );	

 	connect( editor->LineEditGroup, SIGNAL( textChanged(const QString&) ),
	           SLOT( groupChanged(const QString&) ) );
	connect( editor->LineEditKey, SIGNAL( textChanged(const QString&) ),
	           SLOT( keyChanged(const QString&) ) );
 	connect( editor->LineEditValue, SIGNAL( textChanged(const QString&) ),
	           SLOT( valueChanged(const QString&) ) );

 	setCurrent(0);
  editor->layoutType(EditWidget::File);
}

void MainWindow::makeMenu()
{
  popupTimer = new QTimer(this);
  popupMenuFile = new QPopupMenu(this);
  popupMenuEntry = new QPopupMenu(this);

	popupActionSave = new QAction( tr("Save"),QString::null,  0, this, 0 );
	popupActionSave->addTo( popupMenuFile );
 // popupActionSave->addTo( popupMenuEntry );
  connect( popupActionSave, SIGNAL( activated() ),
		  	 	this , SLOT( saveConfFile() ) );

	popupActionRevert = new QAction( tr("Revert"),QString::null,  0, this, 0 );
	popupActionRevert->addTo( popupMenuFile );
  popupActionRevert->addTo( popupMenuEntry );
  connect( popupActionRevert, SIGNAL( activated() ),
		  	 	this , SLOT( revertConfFile() ) );

  popupActionDelete = new QAction( tr("Delete"),QString::null,  0, this, 0 );
	popupActionDelete->addTo( popupMenuFile );
  popupActionDelete->addTo( popupMenuEntry );
  connect( popupActionDelete, SIGNAL( activated() ),
		  	 	this , SLOT( removeConfFile() ) );

  connect( popupTimer, SIGNAL(timeout()),
		  			this, SLOT(showPopup()) );
}

MainWindow::~MainWindow()
{
}



void MainWindow::setCurrent(QListViewItem *item)
{
//	qDebug("MainWindow::setCurrent");
	if (!item) return;
 	_item = (ListViewItemConf*) item;
  if (!_item) return;
  popupTimer->start( 750, true );
  if (_item->getType() == ListViewItemConf::File)
  {
	  editor->layoutType(EditWidget::File);
    _currentItem=0;
    _fileItem = (ListViewItemConfFile*)item;
   	return;
  }
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
   	editor->layoutType(EditWidget::Entry);
    editor->LineEditKey->setText(key);
    editor->LineEditValue->setText(val);
  }else{
    editor->layoutType(EditWidget::Group);
  }
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
	popupTimer->stop();
}

void MainWindow::saveConfFile()
{
	if (!_fileItem) return;	
 	_fileItem->save();
}

void MainWindow::revertConfFile()
{
	if (!_item) return;	
 	_item->revert();
}

void MainWindow::removeConfFile()
{
	if (!_item) return;	
 	_item->remove();
}

void MainWindow::showPopup()
{
qDebug("showPopup");
 	if (!_item) return;
 	popupActionRevert->setEnabled(_item->revertable());
	popupActionSave->setEnabled(_item->isChanged());
 	if (_fileItem)
  {
 		popupActionSave->setEnabled(_fileItem->isChanged());
    popupMenuFile->popup( QCursor::pos() );
  }else if(_currentItem)
  {
    popupMenuEntry->popup( QCursor::pos() );
  }
}
