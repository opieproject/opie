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
#include "smtpclient.h"
#include "emailhandler.h"

SmtpClient::SmtpClient()
{
  socket = new QSocket(this, "smtpClient");
  stream = new QTextStream(socket);
  mailList.setAutoDelete(TRUE);
  
  connect(socket, SIGNAL(error(int)), this, SLOT(errorHandling(int)));
  connect(socket, SIGNAL(connected()), this, SLOT(connectionEstablished()));
  connect(socket, SIGNAL(readyRead()), this, SLOT(incomingData()));
  
  sending = FALSE;
}

SmtpClient::~SmtpClient()
{
  delete socket;
  delete stream;
}

void SmtpClient::newConnection(const QString &target, int port)
{
  if (sending) {
    qWarning("socket in use, connection refused");
    return;
  }

  status = Init;
  sending = TRUE;
  socket->connectToHost(target, port);
  
  emit updateStatus(tr("DNS lookup"));
}

void SmtpClient::addMail(const QString &from, const QString &subject, const QStringList &to, const QString &body)
{
  RawEmail *mail = new RawEmail;
  
  mail->from = from;
  mail->subject = subject;
  mail->to = to;
  mail->body = body;
  
  mailList.append(mail);
}

void SmtpClient::connectionEstablished()
{
  emit updateStatus(tr("Connection established"));

}

void SmtpClient::errorHandling(int status)
{
  errorHandlingWithMsg( status, QString::null );
}

void SmtpClient::errorHandlingWithMsg(int status, const QString & EMsg )
{
  emit errorOccurred(status, EMsg );
  socket->close();
  mailList.clear();
  sending = FALSE;
}

void SmtpClient::incomingData()
{
  QString response;
  
  if (!socket->canReadLine())
    return;
  
  response = socket->readLine();
  switch(status) {
    case Init:  {
          if (response[0] == '2') {
            status = From;
            mailPtr = mailList.first();
            *stream << "HELO there\r\n";
          } else errorHandlingWithMsg(ErrUnknownResponse,response);
          break;
        } 
    case From:  {
          if (response[0] == '2') {
             qDebug(mailPtr->from);
            *stream << "MAIL FROM: <" << mailPtr->from << ">\r\n";
            status = Recv;
          } else errorHandlingWithMsg(ErrUnknownResponse, response );
          break;
        } 
    case Recv:  {
       if (response[0] == '2') {
            it = mailPtr->to.begin();
            if (it == NULL) {
              errorHandlingWithMsg(ErrUnknownResponse,response);
            }
            *stream << "RCPT TO: <" << *it << ">\r\n";
            status = MRcv;
          } else errorHandlingWithMsg(ErrUnknownResponse,response);
          break;
        } 
    case MRcv:  {
          if (response[0] == '2') {
            it++;
            if ( it != mailPtr->to.end() ) {
              *stream << "RCPT TO: <" << *it << ">\r\n";
              break;
            } else  {
              status = Data;
            }
          } else errorHandlingWithMsg(ErrUnknownResponse,response);
        } 
    case Data:  {
          if (response[0] == '2') {
            *stream << "DATA\r\n";
            status = Body;
            emit updateStatus(tr("Sending: ") + mailPtr->subject);
            
          } else errorHandlingWithMsg(ErrUnknownResponse,response);
          break;
        } 
    case Body:  {
          if (response[0] == '3') {
            *stream << mailPtr->body << "\r\n.\r\n";
            mailPtr = mailList.next();
            if (mailPtr != NULL) {
              status = From;
            } else {
              status = Quit;
            }
          } else errorHandlingWithMsg(ErrUnknownResponse,response);
          break;
        } 
    case Quit:  {
          if (response[0] == '2') {
            *stream << "QUIT\r\n";
            status = Done;
            QString temp;
            temp.setNum(mailList.count());
            emit updateStatus(tr("Sent ") + temp + tr(" messages"));
            emit mailSent();
            mailList.clear();
            sending = FALSE;
            socket->close();
          } else errorHandlingWithMsg(ErrUnknownResponse,response);
          break;
        }
  }
}
