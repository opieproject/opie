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
//	socket = new QSocket;
	text = new QTextDrag;
	browser=newBrowser;
//	comm = new HttpComm(socket, browser);
	image = new QImageDrag;
}

const QMimeSource * HttpFactory::data(const QString &abs_name) const
{
	printf("HttpFactory::data: using absolute data func\n");

	int port=80, addrEnd, portSep;
	QString host, file, portS, name, tempString;
	bool done=false, isText=true;
	
//	comm->setUp((QString *)&abs_name);

	name = abs_name;
//	name = name.lower();
	name = name.stripWhiteSpace();
	
//	printf("%s\n", name.latin1());

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

//	printf("%s %s %d\n", host.latin1(), file.latin1(), port);
	
	if(port == 80)
	{
		portS="80";
	}
	
//	if(file.find(".png", file.length()-4) != -1 || file.find(".gif", file.length()-4) != -1 || file.find(".jpg", file.length()-4) != -1)
//	{
//		isImage=true;
//	}

//	comm->setStuff(host, portS, file, text, image, isImage);

//	socket->connectToHost(host, port);

	int con, bytesSent;
	struct sockaddr_in serverAddr;
	struct hostent * serverInfo = gethostbyname( host.latin1() );
	
	if( serverInfo == NULL )
	{
		QMessageBox *mb = new QMessageBox(QObject::tr("Error!"),
			QObject::tr("couldnt find ip address"),
			QMessageBox::NoIcon,
			QMessageBox::Ok,
			QMessageBox::NoButton,
			QMessageBox::NoButton);
		mb->exec();
		perror("HttpFactory::data:");
		return 0;
	}
	
	QByteArray data;
//	printf( "HttpFactory::data: %s\n", inet_ntoa(*((struct in_addr *)serverInfo->h_addr )) );
	
	QString request("GET " + file + " HTTP/1.1\r\nHost: " + host + ':' + portS + "\r\nConnection: close\r\n\r\n");
	 
	con = socket( AF_INET, SOCK_STREAM, 0 );
	if( con == -1 )
	{
		QMessageBox *mb = new QMessageBox(QObject::tr("Error!"),
			QObject::tr("couldnt create socket"),
			QMessageBox::NoIcon,
			QMessageBox::Ok,
			QMessageBox::NoButton,
			QMessageBox::NoButton);
		mb->exec();
		perror("HttpFactory::data:");
		return 0;
	}
	
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( port );
	serverAddr.sin_addr.s_addr = inet_addr( inet_ntoa(*((struct in_addr *)serverInfo->h_addr )) );
	memset( &(serverAddr.sin_zero), '\0', 8 );
	
	if(::connect( con, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) == -1 )
	{
		QMessageBox *mb = new QMessageBox(QObject::tr("Error!"),
			QObject::tr("couldnt connect to socket"),
			QMessageBox::NoIcon,
			QMessageBox::Ok,
			QMessageBox::NoButton,
			QMessageBox::NoButton);
		mb->exec();
		perror("HttpFactory::data:");
		return 0;
	}
	
	
	bytesSent = send( con, request.latin1(), request.length(), 0);
//	printf("HttpFactory::data: bytes written: %d out of: %d\n", bytesSent, request.length() );
//	printf("HttpFactory::data: request sent:\n%s", request.latin1());
	
	data = this->processResponse( con, isText );
	
	::close( con );
	
	if(isText)
	{
		text->setText( QString( data ) );
		return text;
	}
	else
	{
		image->setImage( QImage( data ) );
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

const QByteArray HttpFactory::processResponse( int sockfd, bool &isText ) const
{
	QByteArray inputBin( 1 );
	QByteArray conClosedErr( 27 );
	char conClosedErrMsg[] = "Connection to server closed";
	QString currentLine;
	bool done=false, chunked=false;
	int dataLength = 0;
	
	for( int i = 0; i < 27; i++)
	{
		conClosedErr[i] = conClosedErrMsg[i];
	}
	
	while( !done )
	{
		recv( sockfd, inputBin.data(), inputBin.size(), 0 );
		currentLine += *(inputBin.data());
		
		if( *(inputBin.data()) == '\n' )
		{
			if( currentLine.isEmpty() || currentLine.startsWith( "\r") )
			{
				printf( "HttpFactory::processResponse: end of header\n" );
				if( chunked )
				{
					return recieveChunked( sockfd );
				} else {
					return recieveNormal( sockfd, dataLength );
				}
				done = true;
			}
			
			if( currentLine.contains( "Transfer-Encoding: chunked", false) >= 1 )
			{
				chunked = true;
//				printf( "HttpFactory::processResponse: chunked encoding\n" );
			}
			
			if( currentLine.contains( "Content-Type: text", false ) >= 1 )
			{
				isText = true;
//				printf( "HttpFactory::processResponse: content type text\n" );
				if( currentLine.contains( "html", false ) >= 1)
				{
					browser->setTextFormat(Qt::RichText);
//					printf( "HttpFactory::processResponse: content type html\n" );
				}
			}
			
			if( currentLine.contains( "Content-Type: image", false ) >= 1 )
			{
				isText = false;
//				printf( "HttpFactory::processResponse: content type image\n" );
			}
			
			if( currentLine.contains( "Content-Length", false ) >= 1 )
			{
				currentLine.remove( 0, 16 );
				dataLength = currentLine.toInt();
//				printf( "HttpFactory::processResponse: content length: %d\n", dataLength );
			}
			
			if( currentLine.contains( "404", false ) >= 1 )
			{
//				printf( "HttpFactory::processResponse: 404 error\n" );
				return 0;
			}
			
			currentLine = "";
//			printf("HttpFactory::processResponse: reseting currentLine: %s\n", currentLine.latin1() );
		}
	}
}

const QByteArray HttpFactory::recieveNormal( int sockfd, int dataLen ) const
{
//	printf( "HttpFactory::recieveNormal: recieving w/out chunked encoding\n" );
	
	QByteArray data( dataLen );
	QByteArray temp( dataLen );
	int recieved, i;
	
	recieved = recv( sockfd, temp.data(), temp.size(), 0 );
//	printf( "HttpFactory::recieveNormal: found some data: %s\n", (char *)temp.data() );
	for( i = 0; i < recieved; i++ )
	{
		data[i] = temp[i];
	}
	dataLen -= recieved;
	while( dataLen > 0 )
	{
		recieved = recv( sockfd, temp.data(), temp.size(), 0 );
		dataLen -= recieved;
//		printf( "HttpFactory::recieveNormal: found some more data: %s\n", (char *)temp.data() );
		for( int j = 0; j < recieved; j++ )
		{
			data[i] = temp[j];
			i++;
		}
		temp.fill('\0');
	}
	
//	printf( "HttpFactory::recieveNormal: end of data\n" );
	return data;
}

const QByteArray HttpFactory::recieveChunked( int sockfd ) const
{
//	printf( "HttpFactory::recieveChunked: recieving data with chunked encoding\n" );
	
	QByteArray data;
	QByteArray temp( 1 );
	int recieved, i = 0, cSize = 0;
	QString cSizeS;
	
//	printf( "HttpFactory::recieveChunked: temp.size(): %d\n", temp.size() );
	recv( sockfd, temp.data(), temp.size(), 0 );
	while( *(temp.data()) != '\n' && *(temp.data()) != ';' )
	{
//		printf( "HttpFactory::recieveChunked: temp.size(): %d\n", temp.size() );
//		printf( "HttpFactory::recieveChunked: temp.data(): %c\n", temp[0] );
		cSizeS += temp[0];
		recv( sockfd, temp.data(), temp.size(), 0 );
	}
	
//	printf( "HttpFactory::recieveChunked: cSizeS: %s\n", cSizeS.latin1() );
	cSize = cSizeS.toInt( 0, 16 );
//	printf( "HttpFactory::recieveChunked: first chunk of size: %d\n", cSize );
	
	if( *(temp.data()) == ';' )
	{
		while( *(temp.data()) != '\n' )
		{
			recv( sockfd, temp.data(), temp.size(), 0 );
		}
	}
	
	temp.fill( '\0', cSize );
	data.fill( '\0', cSize );
	
	while( cSize > 0 )
	{
		while( cSize > 0 )
		{
			recieved = recv( sockfd, temp.data(), temp.size(), 0 );
			cSize -= recieved;
			for( int j = 0; j < recieved; j++ )
			{
				data[i] = temp[j];
				i++;
			}
			temp.fill('\0', cSize);
		}
		
//		printf( "HttpFactory::recieveChunked: current data:\n%s", data.data() );
		
		temp.fill('\0', 1);
		cSizeS = "";
		cSize = 0;
		
		recv( sockfd, temp.data(), temp.size(), 0 );
		if( *(temp.data()) == '\r' )
		{
			recv( sockfd, temp.data(), temp.size(), 0 );
		}
		recv( sockfd, temp.data(), temp.size(), 0 );
		while( *(temp.data()) != '\n' && *(temp.data()) != ';' )
		{
//			printf( "HttpFactory::recieveChunked: temp.size(): %d\n", temp.size() );
//			printf( "HttpFactory::recieveChunked: temp.data(): %d\n", temp[0] );
			cSizeS += temp[0];
			recv( sockfd, temp.data(), temp.size(), 0 );
		}
	
//		printf( "HttpFactory::recieveChunked: cSizeS: %s\n", cSizeS.latin1() );
		cSize = cSizeS.toInt( 0, 16 );
//		printf( "HttpFactory::recieveChunked: next chunk of size: %d\n", cSize );
	
		if( *(temp.data()) == ';' )
		{
			while( *(temp.data()) != '\n' )
			{
				recv( sockfd, temp.data(), temp.size(), 0 );
			}
		}
	
		temp.fill( '\0', cSize );
		data.resize( data.size() + cSize );
	}
	
//	printf( "HttpFactory::recieveChunked: end of data\n" );
	return data;
}
