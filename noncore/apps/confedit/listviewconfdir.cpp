/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
  // (c) 2002 Patrick S. Vogtp <tille@handhelds.org>

#include "listviewconfdir.h"

#include <qmessagebox.h>

#include "listviewitemconffile.h"

ListViewConfDir::ListViewConfDir(QString settingsPath, QWidget *parent, const char *name )
  : QListView(parent,name), confDir(settingsPath)
{

	setRootIsDecorated( true );
	addColumn(tr("Files"));

	if (!confDir.isReadable())
 		QMessageBox::critical(this,tr("Could not open"),tr("The directory ")+settingsPath+tr(" could not be opened."),1,0);
	readConfFiles();

 	connect( this, SIGNAL(expanded(QListViewItem*)), SLOT(expand(QListViewItem*)));
}


ListViewConfDir::~ListViewConfDir()
{
}

void ListViewConfDir::readConfFiles()
{

	confDir.setFilter( QDir::Files | QDir::NoSymLinks );
  confDir.setSorting( QDir::Name );
  confDir.setNameFilter("*.conf");
  const QFileInfoList *list = confDir.entryInfoList();
  QFileInfoListIterator it( *list );
  QFileInfo *fi;

  ListViewItemConfFile *fileEntry;

	while ( (fi=it.current()) )
 	{
    qDebug( "opening: >%s<", fi->fileName().data() );
  	fileEntry = new ListViewItemConfFile( fi, this );
    QListViewItem *dummy = new QListViewItem(fileEntry, "dummy");
  	++it;
  }
}

void ListViewConfDir::expand(QListViewItem *item)
{
 	((ListViewItemConf*)item)->expand();
}

