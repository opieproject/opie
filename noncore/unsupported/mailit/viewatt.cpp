/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#include <qpe/resource.h>
#include "viewatt.h"
#include <qwhatsthis.h>
#include <qpe/applnk.h>
#include <qpe/mimetype.h>

ViewAtt::ViewAtt(QWidget *parent, const char *name, WFlags f)
	: QMainWindow(parent, name, f)
{
	setCaption(tr("Exploring attatchments"));

	setToolBarsMovable( FALSE );
	bar = new QToolBar(this);
	installButton = new QAction( tr( "Install" ), Resource::loadPixmap( "exec" ), QString::null, CTRL + Key_C, this, 0 );
	connect(installButton, SIGNAL(activated()), this, SLOT(install()) );
	installButton->setWhatsThis(tr("Click here to install the attachment to your Documents"));

	listView = new QListView(this, "AttView");
	listView->addColumn( tr("Attatchment") );
	listView->addColumn( tr("Type") );
	listView->addColumn( tr("Installed") );
	setCentralWidget(listView);
	QWhatsThis::add(listView,QWidget::tr("This is an overview about all attachments in the mail"));
}

void ViewAtt::update(Email *mailIn, bool inbox)
{
	QListViewItem *item;
	Enclosure *ePtr;



	listView->clear();
	if (inbox) {
		bar->clear();
		installButton->addTo( bar );
		bar->show();
	} else {
		bar->hide();
	}

	mail = mailIn;
	for ( ePtr=mail->files.first(); ePtr != 0; ePtr=mail->files.next() ) {

		QString isInstalled = tr("No");
		if (ePtr->installed)
			isInstalled = tr("Yes");
		item = new QListViewItem(listView, ePtr->originalName, ePtr->contentType, isInstalled);

		const QString& mtypeDef=(const QString&) ePtr->contentType+"/"+ePtr->contentAttribute;

		MimeType mt(mtypeDef);

		item->setPixmap(0, mt.pixmap());

		/*
		if (ePtr->contentType == "TEXT") {
			actions = new QAction( tr("View"), Resource::loadPixmap("TextEditor"), QString::null, CTRL + Key_C, this, 0);
			actions->addTo(bar);
				}
		if (ePtr->contentType == "AUDIO") {
			actions = new QAction( tr("Play"), Resource::loadPixmap("SoundPlayer"), QString::null, CTRL + Key_C, this, 0);
			actions->addTo(bar);
			item->setPixmap(0, Resource::loadPixmap("play"));
		}
		if (ePtr->contentType == "IMAGE") {
			actions = new QAction( tr("Show"), Resource::loadPixmap("pixmap"), QString::null, CTRL + Key_C, this, 0);
			actions->addTo(bar);
			item->setPixmap(0, Resource::loadPixmap("pixmap"));
		}*/
	}
}

void ViewAtt::install()
{
	Enclosure *ePtr, *selPtr;
	QListViewItem *item;
	QString filename;
	DocLnk d;

	item = listView->selectedItem();
	if (item != NULL) {
		filename  = item->text(0);
		selPtr = NULL;
		for ( ePtr=mail->files.first(); ePtr != 0; ePtr=mail->files.next() ) {
			if (ePtr->originalName == filename)
				selPtr = ePtr;
		}

		if (selPtr == NULL) {
			qWarning("Internal error, file is not installed to documents");
			return;
		}

		d.setName(selPtr->originalName);
		d.setFile(selPtr->path + selPtr->name);
		d.setType(selPtr->contentType + "/" + selPtr->contentAttribute);
		d.writeLink();
		selPtr->installed = TRUE;
		item->setText(2, tr("Yes"));
	}
}
