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

#include "httpcomm.h"

HttpComm::HttpComm(QSocket *newSocket, QTextBrowser *newBrowser):QObject()
{
	socket = newSocket;

	connect(socket, SIGNAL(hostFound()), this, SLOT(hostFound()) );
	connect(socket, SIGNAL(connected()), this, SLOT(connected()) );
	connect(socket, SIGNAL(readyRead()), this, SLOT(incoming()) );
	connect(socket, SIGNAL(connectionClosed()), this, SLOT(connectionClosed()) );

	headerRead=false;
	length = 0;
	bRead = 0;
	chunked=false;
	lengthset=false;

	browser=newBrowser;
}

void HttpComm::setUp(QString *newName)
{
	name = newName;
}

void HttpComm::setStuff(QString newHost, QString newPortS, QString newFile, QTextDrag *newText)
{
	host = newHost;
	portS = newPortS;
	file = newFile;
	text = newText;
}

void HttpComm::hostFound()
{
	printf("HttpComm::hostFound: host found\n");
}

void HttpComm::connected()
{
	QString request("GET " + file + " HTTP/1.1\r\nHost: " + host + ':' + portS + "\r\nConnection: close\r\n\r\n");
//	QString request("GET " + file + " HTTP/1.0\r\n\r\n");
	printf("HttpComm::data: bytes written: %d\n", socket->writeBlock(request.latin1(), request.length()) );
	printf("HttpComm::data: request sent:\n%s", request.latin1());

	headerRead=false;
	bRead=0;
	length = 0;
	header="";
	body="";
	chunked=false;
	lengthset=false;
}

void HttpComm::incoming()
{
	int ba=socket->bytesAvailable(), i=0, j=0, semi=0;
	char *tempString = new char [ba];
	bool nextChunk=false;
	bool done=false;
	socket->readBlock(tempString, ba);
	printf("HttpComm::incoming: ba: %d\n", ba);
	QString sclength;
	
	if(headerRead == false)
	{
		for(i=0; i<ba; i++)
		{
			if(tempString[i] != '\r')
			{
				if(tempString[i] == '\n' && header[header.length()-1] == '\n')
				{
					j=i;
					headerRead = true;
					parseHeader();
					goto body;
				}
				else
				{
					header+=tempString[i];
				}
			}
//			printf("%d %d\n", ba, i);
		}
	}
	else
	{
		body:
		printf("HttpComm::incoming: reading body\n");
		printf("HttpComm::incoming: j is: %d\n", j);
		if(!chunked)
		{
//make sure we didnt just leave that break above...
			if(j != 0)
			{
				for( ; j<ba ; j++)
				{
					body+=tempString[j];
					bRead++;
//				printf("bRead1: %d\n", bRead);
				}
			}
			else
			{
				body += tempString;
				bRead+=ba;
//				printf("bRead2: %d\n", bRead);
			}

			if(bRead >= length)
			{
				printf("HttpComm::incoming: finished reading body\n");
				processBody();
				socket->close();
			}
		}
		else
		{
			QString tempQString = tempString;
			//remove the http header, if one exists
			if(j != 0)
			{
				tempQString.remove(0, j+1);
				printf("HttpComm::incoming: removing http header.  Result: \n%s", tempQString.latin1());
			}
			while(!done)
			{
				switch(status)
				{
				//case 0=need to read chunk length
				case 0:
					j = tempQString.find('\n');
					sclength = tempQString;
					sclength.truncate(j);
					clength = sclength.toUInt(0, 16);
					printf("HttpComm::Incoming: chunk length: %d\n", clength);
					//end of data
					if(clength==0)
					{
						processBody();
						done=true;
					}
					//still more, but it hasnt been recieved yet
					if(ba <= j)
					{
						status=1;
						done=true;
						break;
					}
					//still more data waiting
					else
					{
						done=false;
						//remove the chunk length header
						tempQString.remove(0,j+1);
					}
					bRead=0;
//					break;
				//if there is more fall through to:
				//chunk length just read, still more in tempQstring
				case 1:
					//the current data extends beyond the end of the chunk
					if(bRead + tempQString.length() > clength)
					{
						QString newTQstring = tempQString;
						newTQstring.truncate(clength-bRead);
						bRead+=newTQstring.length();
						body+=newTQstring;
						printf("HttpComm::incoming: start new body piece 1: \n");
						printf("%s", newTQstring.latin1() );
						printf("HttpComm::incoming: end new body piece 1.\n");
						status=0;
						j=clength-bRead;
						done=false;
//						break;
					}
					//the chunk extends beyond the current data;
					else
					{
						body+=tempQString;
						bRead+=ba;
						printf("HttpComm::incoming: start new body piece 2: \n");
						printf("%s", tempQString.latin1() );
						printf("HttpComm::incoming: end new body piece 2.\n");
						done=true;
						status=2;
//						break;
					}
					break;
				//just got data in, continue reading chunk
				case 2:
					//the current data extends beyond the end of the chunk
					if(bRead + tempQString.length() > clength)
					{
						QString newTQstring = tempQString;
						newTQstring.truncate(clength-bRead);
						bRead+=newTQstring.length();
						body+=newTQstring;
						printf("HttpComm::incoming: start new body piece 3: \n");
						printf("%s", newTQstring.latin1() );
						printf("HttpComm::incoming: end new body piece 3.\n");
						status=0;
						j=clength-bRead;
						done=false;
//						break;
					}
					//the chunk extends beyond the current data;
					else
					{
						body+=tempQString;
						bRead+=ba;
						printf("HttpComm::incoming: start new body piece 4: \n");
						printf("%s", tempQString.latin1() );
						printf("HttpComm::incoming: end new body piece 4.\n");
						done=true;
						status=2;
//						break;
					}
					break;
				}
				printf("HttpComm::incoming: chunked encoding: bRead: %d\n", bRead);
			}
		}
	}
	delete tempString;
}

void HttpComm::connectionClosed()
{
	printf("HttpComm::connectionClosed: connection closed\n");
	processBody();
}

void HttpComm::parseHeader()
{
	QStringList headerLines, tempList;
	int i;

	printf("HttpComm::parseHeader: start header\n\n");
	printf("%s", header.latin1());
	printf("HttpComm::parseHeader: end header\n");

	headerLines = QStringList::split('\n', header);

	for(i=0; i<headerLines.count(); i++)
	{
		if(headerLines[i].startsWith("Content-Length:") )
		{
			tempList = QStringList::split(':', headerLines[i]);
			tempList[1].stripWhiteSpace();
			length = tempList[1].toUInt();
		}
		else if(headerLines[i].startsWith("Transfer-Encoding: chunked") )
		{
			printf("HttpComm::parseHeader: using chunked encoding\n");
			chunked = true;
			status=0;
		}
	}
	
	printf("HttpConn::parseHeader: content-length: %d\n", length);
}

void HttpComm::processBody()
{
//	printf("HttpComm::processBody: start body\n\n");
//	printf("%s", body.latin1());
//	printf("HttpComm::processBody: end body\n");

	int lastSlash = file.findRev('/');
	
	QString end = file;
	end.truncate(lastSlash-1);
	QString context("http://"+host+':'+portS+end);

	browser->setTextFormat(RichText);
	browser->setText(body, context);
	printf("%s\n", context.latin1() );
}
