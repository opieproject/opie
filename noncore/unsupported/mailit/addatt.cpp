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
#include <qlayout.h>
#include <qhbox.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qpe/resource.h>
#include "addatt.h"

FileItem::FileItem(QListView *parent, DocLnk* dl)
		: QListViewItem(parent)
{
	/*file = fileInfo;
	type = fileType;*/
	
	doclnk=dl;

	setText(0, doclnk->name());
	
/*	if (fileType == "Picture") {
		setPixmap(0, Resource::loadPixmap("pixmap"));
	} else if (fileType == "Document") {
		setPixmap(0, Resource::loadPixmap("txt"));	
	} else if (fileType == "Sound") {
		setPixmap(0, Resource::loadPixmap("play"));
	} else if (fileType == "Movie") {
		setPixmap(0, Resource::loadPixmap("MPEGPlayer"));
	} else if (fileType == "File") {
		setPixmap(0, Resource::loadPixmap("exec"));
	}*/
}

FileItem::~FileItem()
{
	if (doclnk!=NULL) delete doclnk;
	doclnk=NULL;
}

AddAtt::AddAtt(QWidget *parent, const char *name, WFlags f)
	: QDialog(parent, name, f)
{
	setCaption(tr("Adding attachments") );

	QGridLayout *top = new QGridLayout(this, 1,1 );
	
	QHBox *buttons=new QHBox(this);
	/*fileCategoryButton = new QPushButton(this);*/
	attachButton = new QPushButton(tr("attach..."), buttons);
	removeButton = new QPushButton(tr("Remove"), buttons);

	/*fileCategories = new QPopupMenu(fileCategoryButton);
	fileCategoryButton->setPopup(fileCategories);
	fileCategories->insertItem("Document");
	fileCategories->insertItem("Picture");
	fileCategories->insertItem("Sound");
	fileCategories->insertItem("Movie");
	fileCategories->insertItem("File");

	fileCategoryButton->setText("Document");
	top->addWidget(fileCategoryButton, 0, 0);*/
	

	top->addWidget(buttons,1,0);
	//buttons->addWidget(attachButton,0,0);
	//buttons->addWidget(removeButton,0,1);

	//connect(fileCategories, SIGNAL(activated(int)), this,
    	//	SLOT(fileCategorySelected(int)) );*/
	connect(attachButton, SIGNAL(clicked()), this,
		SLOT(addattachment()) );
	connect(removeButton, SIGNAL(clicked()), this,
		SLOT(removeattachment()) );
	
	/*listView = new QListView(this, "AttView");
	listView->addColumn("Documents");*
	connect(listView, SIGNAL(doubleClicked(QListViewItem*)), this,
		SLOT(addattachment()) );*/
	
	
	attView = new QListView(this, "Selected");
	attView->addColumn(tr("Attached"));
	attView->addColumn(tr("File type"));
	connect(attView, SIGNAL(doubleClicked(QListViewItem*)), this,
		SLOT(removeattachment()) );

	//top->addWidget(ofs, 0,0);
	top->addWidget(attView, 0,0);
	
	clear();
	

}

void AddAtt::clear()
{
	attView->clear();
	//getFiles();
	modified = FALSE;
}

/*void AddAtt::fileCategorySelected(int id)
{
	fileCategoryButton->setText(fileCategories->text(id));
	getFiles();
}*/

void AddAtt::addattachment()
{	// ### FIXME wrong use -zecke
	OFileDialog ofs("Attachments",this,0,0,"/root/Documents");
	
	ofs.showMaximized();
	
	if (ofs.exec()==QDialog::Accepted)
	{
		DocLnk* dl=new DocLnk(ofs.selectedDocument());
		FileItem* fi=new FileItem(attView,dl);
		fi->setPixmap(0,dl->pixmap());
		fi->setText(1,dl->type());
		attView->insertItem(fi);
		modified = TRUE;	
	}
}

void AddAtt::removeattachment()
{
	if (attView->selectedItem() != NULL) 
	{
		attView->takeItem(attView->selectedItem());
	}
	modified = TRUE;
}

void AddAtt::reject()
{
	if (modified) {
		attView->clear();
		modified = FALSE;
	}
}

void AddAtt::accept()
{
	modified = FALSE;
	hide();
}

void AddAtt::getFiles()
{
	QString path, selected;
	
	/*listView->clear();
	
	selected = fileCategoryButton->text();
	if (selected == "Picture") {
		path = "../pics/";
	} else if (selected == "Document") {
		path = ""			;					//sub-dirs not decided
	} else if (selected == "Sound") {
		path = "../sounds/";					//sub-dirs not decided
	} else if (selected == "Movie") {
		path = "";								//sub-dirs not decided
	} else if (selected == "File") {
		path = "";								//sub-dirs not decided
	}

	dir = new QDir(path);
	dir->setFilter(QDir::Files);
	const QFileInfoList *dirInfoList = dir->entryInfoList();

	QFileInfoListIterator it(*dirInfoList);      // create list iterator

	while ( (fi=it.current()) ) {           // for each file...
		item = new FileItem(lis+ütView, *fi, selected);
		++it;                               // goto next list element
	}*/
}

QStringList AddAtt::returnattachedFiles()
{
	QFileInfo info;
	QStringList list;
	
	item = (FileItem *) attView->firstChild();
	
	
	while (item != NULL) {
		DocLnk* dl=item->getDocLnk();
		list+=dl->file();
		/*info = item->getFileInfo();
		list += info.filePath();*/
		item = (FileItem *) item->nextSibling();
	}
	return list;
}

QStringList AddAtt::returnFileTypes()
{
	QStringList list;
		
	item = (FileItem *) attView->firstChild();
			
	while (item != NULL) {
		list += item->getDocLnk()->type();
		item = (FileItem *) item->nextSibling();
	}
	return list;
}
