/*
Opie-uBrowser.  a very small web browser, using on QTextBrowser for html display/parsing
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

#include "httpfactory.h"

HttpFactory::HttpFactory(QTextBrowser *newBrowser):QMimeSourceFactory()
{
	socket = new QSocket;
	text = new QTextDrag;
	browser=newBrowser;
	comm = new HttpComm(socket, browser);
	image = new QImageDrag;
}

const QMimeSource * HttpFactory::data(const QString &abs_name) const
{
	printf("HttpFactory::data: using absolute data func\n");

	int port=80, addrEnd, portSep;
	QString host, file, portS, name, tempString;
	bool done=false, isImage=false;
	
	comm->setUp((QString *)&abs_name);

	name = abs_name;
//	name = name.lower();
	name = name.stripWhiteSpace();
	
	printf("%s\n", name.latin1());

	if(name.startsWith("http://"))
	{
		name = name.remove(0, 7);
	}
	else
	{
		name.prepend(browser->context());
		name = name.remove(0, 7);
	}

	addrEnd = name.find('/');
	if(addrEnd == -1)
	{
		name += '/';
		addrEnd = name.length()-1;
	}

	host = name;
	file = name;

	host.truncate(addrEnd);
	file.remove(0, addrEnd);
	
	portSep = host.find(':');
	if(portSep != -1)
	{
		portS=host;
		host.truncate(portSep);
		portS.remove(0, portSep+1);
		port = portS.toInt();
	}

	printf("%s %s %d\n", host.latin1(), file.latin1(), port);
	
	if(port == 80)
	{
		portS="80";
	}
	
	if(file.find(".png", file.length()-4) != -1 || file.find(".gif", file.length()-4) != -1 || file.find(".jpg", file.length()-4) != -1)
	{
		isImage=true;
	}

	comm->setStuff(host, portS, file, text, image, isImage);

	socket->connectToHost(host, port);
	
	if(!image)
	{
		text->setText("");
		return text;
	}
	else
	{
		return image;
	}
}

const QMimeSource * HttpFactory::data(const QString &abs_or_rel_name, const QString & context) const
{
	printf("HttpFactory::data: using relative data func\n");

	if(abs_or_rel_name.startsWith(context))
	{
		return data(abs_or_rel_name);
	}
	else
	{
		return data(context + abs_or_rel_name);
	}
	
	return 0;
}
