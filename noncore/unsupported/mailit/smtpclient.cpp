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

void SmtpClient::newConnection(QString target, int port)
{
  if (sending) {
    qWarning("socket in use, connection refused");
    return;
  }

  status = Init;
  sending = TRUE;
  socket->connectToHost(target, port);
  
  emit updateStatus("DNS lookup");
}

void SmtpClient::addMail(QString from, QString subject, QStringList to, QString body)
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
  emit updateStatus("Connection established");

}

void SmtpClient::errorHandling(int status)
{
  emit errorOccurred(status);
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
          } else errorHandling(ErrUnknownResponse);
          break;
        } 
    case From:  {
          if (response[0] == '2') {
            *stream << "MAIL FROM: <" << mailPtr->from << ">\r\n";
            status = Recv;
          } else errorHandling(ErrUnknownResponse);
          break;
        } 
    case Recv:  {
          if (response[0] == '2') {
            it = mailPtr->to.begin();
            if (it == NULL)
              errorHandling(ErrUnknownResponse);
            *stream << "RCPT TO: <" << *it << ">\r\n";
            status = MRcv;
          } else errorHandling(ErrUnknownResponse);
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
          } else errorHandling(ErrUnknownResponse);
        } 
    case Data:  {
          if (response[0] == '2') {
            *stream << "DATA\r\n";
            status = Body;
            emit updateStatus("Sending: " + mailPtr->subject);
          } else errorHandling(ErrUnknownResponse);
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
          } else errorHandling(ErrUnknownResponse);
          break;
        } 
    case Quit:  {
          if (response[0] == '2') {
            *stream << "QUIT\r\n";
            status = Done;
            QString temp;
            temp.setNum(mailList.count());
            emit updateStatus("Sent " + temp + " messages");
            emit mailSent();
            mailList.clear();
            sending = FALSE;
            socket->close();
          } else errorHandling(ErrUnknownResponse);
          break;
        }
  }
}
