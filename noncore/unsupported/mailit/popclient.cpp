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
#include "popclient.h"
#include "emailhandler.h"
//#define APOP_TEST

extern "C" {
#include "md5.h"
}

#include <qcstring.h>

PopClient::PopClient()
{
  
  socket = new QSocket(this, "popClient");
  connect(socket, SIGNAL(error(int)), this, SLOT(errorHandling(int)));
  connect(socket, SIGNAL(connected()), this, SLOT(connectionEstablished()));
  connect(socket, SIGNAL(readyRead()), this, SLOT(incomingData()));
  
  stream = new QTextStream(socket);
  
  receiving = FALSE;
  synchronize = FALSE;
  lastSync = 0;
  headerLimit = 0;
  preview = FALSE;
}

PopClient::~PopClient()
{
  delete socket;
  delete stream;
}

void PopClient::newConnection(const QString &target, int port)
{
  if (receiving) {
    qWarning("socket in use, connection refused");
    return;
  }
  
  status = Init;
  
  socket->connectToHost(target, port);
  receiving = TRUE;
  //selected = FALSE;
    
  emit updateStatus(tr("DNS lookup"));
}

void PopClient::setAccount(const QString &popUser, const QString &popPasswd)
{
  popUserName = popUser;
  popPassword = popPasswd;
}             

void PopClient::setSynchronize(int lastCount)
{
  synchronize = TRUE;
  lastSync = lastCount;
}

void PopClient::removeSynchronize()
{
  synchronize = FALSE;
  lastSync = 0;
}

void PopClient::headersOnly(bool headers, int limit)
{
  preview = headers;
  headerLimit = limit;
}

void PopClient::setSelectedMails(MailList *list)
{
  selected = TRUE;
  mailList = list;
}

void PopClient::connectionEstablished()
{
  emit updateStatus(tr("Connection established"));
}

void PopClient::errorHandling(int status)
{
  errorHandlingWithMsg( status, QString::null );
}
void PopClient::errorHandlingWithMsg(int status, const QString & Msg )
{
  emit updateStatus(tr("Error Occured"));
  emit errorOccurred(status, Msg);
  socket->close();
  receiving = FALSE;
}

void PopClient::incomingData()
{
  QString response, temp, temp2, timeStamp;
  QString md5Source;
  int start, end;
//  char *md5Digest;
  char md5Digest[16];
//  if ( !socket->canReadLine() )
//    return;

  
  response = socket->readLine();
  
  switch(status) {
    //logging in
    case Init:  {
#ifdef APOP_TEST
       start = response.find('<',0);
       end = response.find('>', start);
       if( start >= 0 && end > start )
       {
          timeStamp = response.mid( start , end - start + 1);
          md5Source = timeStamp + popPassword;
          
          md5_buffer( (char const *)md5Source, md5Source.length(),&md5Digest[0]);

           for(int j =0;j < MD5_DIGEST_LENGTH ;j++)
           {
              printf("%x", md5Digest[j]);
           }
           printf("\n");          
//          qDebug(md5Digest);
          *stream << "APOP " <<  popUserName << " " << md5Digest << "\r\n";
          //    qDebug("%s", stream);
          status = Stat;
       }
       else
#endif
       {
          timeStamp = "";
          *stream << "USER " << popUserName << "\r\n";
          status = Pass;
       }
          
        break;
        }
    
    case Pass:  {
          *stream << "PASS " << popPassword << "\r\n";
    status = Stat;
    
          break;
        }
    //ask for number of messages
    case Stat:  {
          if (response[0] == '+') {
            *stream << "STAT" << "\r\n";
            status = Mcnt;      
          } else errorHandlingWithMsg(ErrLoginFailed, response);
            break;
          }
    //get count of messages, eg "+OK 4 900.." -> int 4
    case Mcnt:  {
          if (response[0] == '+') {
            temp = response.replace(0, 4, "");
            int x = temp.find(" ", 0);
            temp.truncate((uint) x);
            newMessages = temp.toInt();
            messageCount = 1;
            status = List;
              
            if (synchronize) {
              //messages deleted from server, reload all
                if (newMessages < lastSync)
                  lastSync = 0;
                messageCount = 1;
              }
              
              if (selected) {   
                int *ptr = mailList->first();
                if (ptr != 0) {
                  newMessages++;  //to ensure no early jumpout
                  messageCount = *ptr;
                } else newMessages = 0; 
        }

            } else errorHandlingWithMsg(ErrUnknownResponse, response);
          }
    //Read message number x, count upwards to messageCount
    case List:  {
          if (messageCount <= newMessages) {
            *stream << "LIST " << messageCount << "\r\n";
            status = Size;
            temp2.setNum(newMessages - lastSync);
            temp.setNum(messageCount - lastSync);
            if (!selected) {
              emit updateStatus(tr("Retrieving ") + temp + "/" + temp2);
            } else {
              //completing a previously closed transfer
             /* if ( (messageCount - lastSync) <= 0) {
                temp.setNum(messageCount);
                emit updateStatus(tr("Previous message ") + temp);
              } else {*/
                emit updateStatus(tr("Completing message ") + temp);
              //}
            }
            break;
          } else {
            emit updateStatus(tr("No new Messages"));
            status = Quit;
          }
        } 
    //get size of message, eg "500 characters in message.." -> int 500
    case Size:  {
          if (status != Quit) { //because of idiotic switch
            if (response[0] == '+') {
              temp = response.replace(0, 4, "");
              int x = temp.find(" ", 0);
              temp = temp.right(temp.length() - ((uint) x + 1) );
              mailSize = temp.toInt();
              emit currentMailSize(mailSize);
              
              status = Retr;
            } else {
              //qWarning(response);
              errorHandlingWithMsg(ErrUnknownResponse, response);
            }
          }
        } 
    //Read message number x, count upwards to messageCount
    case Retr:  {
          if (status != Quit) {
            if ((selected)||(mailSize <= headerLimit)) 
      {
              *stream << "RETR " << messageCount << "\r\n";
            } else {        //only header
       *stream << "TOP " << messageCount << " 0\r\n";
            }
            messageCount++;
            status = Ignore;
            break;
          }         } 
    case Ignore:  {
          if (status != Quit) { //because of idiotic switch
            if (response[0] == '+') {
              message = "";
              status = Read;
              if (!socket->canReadLine()) //sync. problems
                break;
              response = socket->readLine();
            } else errorHandlingWithMsg(ErrUnknownResponse, response);
          }
        }
    //add all incoming lines to body.  When size is reached, send
    //message, and go back to read new message
    case Read:  {
          if (status != Quit) { //because of idiotic switch
            message += response;
            while ( socket->canReadLine() ) {
              response = socket->readLine();
              message += response;
            }
            emit downloadedSize(message.length());
            int x = message.find("\r\n.\r\n",-5);
            if (x == -1) {
              break;
            } else {  //message reach entire size
        if ( (selected)||(mailSize <= headerLimit)) //mail size limit is not used if late download is active
        {
        emit newMessage(message, messageCount-1, mailSize, TRUE);
              } else {  //incomplete mail downloaded
    emit newMessage(message, messageCount-1, mailSize, FALSE);
              }
        
              if ((messageCount > newMessages)||(selected)) //last message ?
        {
                status = Quit;
                if (selected) {   //grab next from queue
                    newMessages--;
                    status = Quit;
                  }
        }
              else 
        {
                  *stream << "LIST " << messageCount << "\r\n";
                  status = Size;
                  temp2.setNum(newMessages - lastSync);
                  temp.setNum(messageCount - lastSync);
                  emit updateStatus(tr("Retrieving ") + temp + "/" + temp2);
                  
                  break;
              }
              }
            }
            if (status != Quit)
              break;
        }
    case Quit:  {
          *stream << "Quit\r\n";
          status = Done;
          int newM = newMessages - lastSync;
          if (newM > 0) {
            temp.setNum(newM);
            emit updateStatus(temp + tr(" new messages"));
          } else {
            emit updateStatus(tr("No new messages"));
          }
          
          socket->close();
          receiving = FALSE;
          emit mailTransfered(newM);
          break;
        }
  }

}
