/*
Opie-sh.  convinience app to allow you to use qdialogs in scripts (mainly shell scripts)
Copyright (C) 2002 Thomas Stephens

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
#include "fviewer.h"

FViewer::FViewer(QString filename, QString title, QWidget *parent=0, const char*name=0):QWidget(parent, name)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	textView = new QTextBrowser(this, "textview");
	layout->addWidget(textView);

	QString string;

	if(title.isNull())
	{
		setCaption(filename);
	}
	else
	{
		setCaption(title);
	}
	
	file = new QFile();

	if(!filename.isNull())
	{
		file->setName(filename);
		file->open(IO_ReadOnly);
	}
	else
	{
		file->open(IO_ReadOnly, 0);
	}
	
	stream =  new QTextStream(file);

	string = stream->read();
	textView->mimeSourceFactory()->setFilePath(QDir::currentDirPath()+"/");
	textView->setText(string, QDir::currentDirPath()+"/");
	printf("%s\n", QDir::currentDirPath().latin1());
	file->close();

}

