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
#include <qfileinfo.h>
#include <stdlib.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qcstring.h>
#include "emailhandler.h"
#include <qpe/applnk.h>
#include <qpe/filemanager.h>

QCollection::Item EnclosureList::newItem(QCollection::Item d)
{
  return dupl( (Enclosure *) d);
}

Enclosure* EnclosureList::dupl(Enclosure *in)
{
  ac = new Enclosure(*in);
  return ac;
}

EmailHandler::EmailHandler()
{
  smtpClient = new SmtpClient();
  popClient = new PopClient();
  
  connect(smtpClient, SIGNAL(errorOccurred(int)), this,
      SIGNAL(smtpError(int)) );
  connect(smtpClient, SIGNAL(mailSent()), this, SIGNAL(mailSent()) );
  connect(smtpClient, SIGNAL(updateStatus(const QString &)), this,
      SIGNAL(updateSmtpStatus(const QString &)) );
  
  connect(popClient, SIGNAL(errorOccurred(int)), this,
      SIGNAL(popError(int)) );
  connect(popClient, SIGNAL(newMessage(const QString &, int, uint, bool)),
      this, SLOT(messageArrived(const QString &, int, uint, bool)) );
  connect(popClient, SIGNAL(updateStatus(const QString &)), this,
      SIGNAL(updatePopStatus(const QString &)) );
  connect(popClient, SIGNAL(mailTransfered(int)), this,
      SIGNAL(mailTransfered(int)) );


  //relaying size information
  connect(popClient, SIGNAL(currentMailSize(int)),
    this, SIGNAL(currentMailSize(int)) );
  connect(popClient, SIGNAL(downloadedSize(int)),
    this, SIGNAL(downloadedSize(int)) );
}

void EmailHandler::sendMail(QList<Email> *mailList)
{
  Email *currentMail;
  QString temp;
  QString userName = mailAccount.name;
  userName += " <" + mailAccount.emailAddress + ">";
  
  for (currentMail = mailList->first(); currentMail != 0;
      currentMail = mailList->next()) {
    
    if (encodeMime(currentMail) == 0) {
      smtpClient->addMail(userName, currentMail->subject,
        currentMail->recipients, currentMail->rawMail);
    } else {                    //error
      temp = tr("Could not locate all files in \nmail with subject: ") +
        currentMail->subject;
      temp += tr("\nMail has NOT been sent");
      QMessageBox::warning(qApp->activeWindow(), tr("Attachment error"), temp, tr("OK\n"));
      
    }
  }
  smtpClient->newConnection(mailAccount.smtpServer, 25);
}

void EmailHandler::setAccount(MailAccount account)
{
  mailAccount = account;
}

void EmailHandler::getMail()
{
  popClient->setAccount(mailAccount.popUserName, mailAccount.popPasswd);
  if (mailAccount.synchronize) {
    popClient->setSynchronize(mailAccount.lastServerMailCount);
  } else {
    popClient->removeSynchronize();
  }
  
  headers = FALSE;
  popClient->headersOnly(headers, 0);
  popClient->newConnection(mailAccount.popServer, 110);
}

void EmailHandler::getMailHeaders()
{
  popClient->setAccount(mailAccount.popUserName, mailAccount.popPasswd);
  if (mailAccount.synchronize) {
    popClient->setSynchronize(mailAccount.lastServerMailCount);
  } else {
    popClient->removeSynchronize();
  }
  
  headers = TRUE;
  popClient->headersOnly(headers, mailAccount.syncLimit);  //less than requested syncLimit, download all
  popClient->newConnection(mailAccount.popServer, 110);
}

void EmailHandler::getMailByList(MailList *mailList)
{
  if (mailList->count() == 0) { //should not occur though
    emit mailTransfered(0);
    return;
  }
  
  headers = FALSE;
  popClient->headersOnly(FALSE, 0);
  popClient->newConnection(mailAccount.popServer, 110);
  popClient->setSelectedMails(mailList);
}

void EmailHandler::messageArrived(const QString &message, int id, uint size, bool complete)
{
  Email mail;
  
  mail.rawMail = message;
  mail.serverId = id;
  mail.size = size;
  mail.downloaded = complete;
  
  emit mailArrived(mail, FALSE);
}

bool EmailHandler::parse(QString in, QString lineShift, Email *mail)
{
  QString temp, boundary;
  int pos;
  QString delimiter, header, body, mimeHeader, mimeBody;
  QString content, contentType, contentAttribute, id, encoding;
  QString fileName, storedName;
  int enclosureId = 0;
  
  mail->rawMail = in;
  mail->received = TRUE;
  mail->files.setAutoDelete(TRUE);
  
  temp = lineShift + "." + lineShift;
  
  if (in.right(temp.length()) != temp) {
    mail->rawMail += temp;
  }

  
  delimiter = lineShift + lineShift;  // "\n\n" or "\r\n\r\n"
  pos = in.find(delimiter, 0, FALSE);
  header = in.left(pos);
  body = in.right(in.length() - pos - delimiter.length());
  if ((body.at(body.length()-2) == '.') && (body.at(body.length()-3) == '\n'))
      body.truncate(body.length()-2);
  
  TextParser p(header, lineShift);
  
  if ((pos = p.find("FROM",':', 0, TRUE)) != -1) {
    pos++;
    if (p.separatorAt(pos) == ' ') {
      mail->from = p.getString(&pos, '<', false);
      mail->from = mail->from.stripWhiteSpace();
      if ( (mail->from.length() > 2) && (mail->from[0] == '"') ) {
        mail->from = mail->from.left(mail->from.length() - 1);
        mail->from = mail->from.right(mail->from.length() - 1);
      }
      pos++;
      mail->fromMail = p.getString(&pos, '>', false);
    } else {
      if ((p.separatorAt(pos) == '<')
          || (p.separatorAt(pos) == ' '))       //No name.. nasty
          pos++;
          pos++;
      mail->fromMail = p.getString(&pos, 'z', TRUE);
      if (mail->fromMail.at(mail->fromMail.length()-1) == '>')
      mail->fromMail.truncate(mail->fromMail.length() - 1);
      mail->from=mail->fromMail;
    }
  }
  
  //@@@ToDo: Rewrite the parser as To: stops at the first occurence- which is Delivered-To:
  if ((pos = p.find("TO",':', 0, TRUE)) != -1)
  {
    	pos++;
    	mail->recipients.append (p.getString(&pos, 'z', TRUE) );
  }
 
  //@@@ToDo: Rewrite the parser as To: stops at the first occurence- which is Delivered-To:
  if ((pos = p.find("CC",':', 0, TRUE)) != -1)
  {
    	pos++;
    	mail->carbonCopies.append (p.getString(&pos, 'z', TRUE) );
  }
 
  
  if ((pos = p.find("SUBJECT",':', 0, TRUE)) != -1) {
    pos++;
    mail->subject = p.getString(&pos, 'z', TRUE);
  }
  if ((pos = p.find("DATE",':', 0, TRUE)) != -1) {
    pos++;
    mail->date = p.getString(&pos, 'z', true);
  }
  
  
  
  if ((pos = p.find("MESSAGE",'-', 0, TRUE)) != -1) {
    pos++;
    if ( (p.wordAt(pos).upper() == "ID") &&
      (p.separatorAt(pos) == ':') ) {
      
      id = p.getString(&pos, 'z', TRUE);
      mail->id = id;
    }
  }
  
  pos = 0;
  while ( ((pos = p.find("MIME",'-', pos, TRUE)) != -1) ) {
    pos++;
    if ( (p.wordAt(pos).upper() == "VERSION") &&
      (p.separatorAt(pos) == ':') ) {
        pos++;
        if (p.getString(&pos, 'z', true) == "1.0") {
          mail->mimeType = 1;
        }
    }
  }
  
  if (mail->mimeType == 1) {
    boundary = "";
    if ((pos = p.find("BOUNDARY", '=', 0, TRUE)) != -1) {
      pos++;
      boundary = p.getString(&pos, 'z', true);
      if (boundary[0] == '"') {
        boundary = boundary.left(boundary.length() - 1); //strip "
        boundary = boundary.right(boundary.length() - 1); //strip "
      }
      boundary = "--" + boundary;   //create boundary field
    }
    
    if (boundary == "") {     //fooled by Mime-Version
      mail->body = body;
      mail->bodyPlain = body;
      return mail;
    }
    
    while (body.length() > 0) {
      pos = body.find(boundary, 0, FALSE);
      pos = body.find(delimiter, pos, FALSE);
      mimeHeader = body.left(pos);
      mimeBody = body.right(body.length() - pos - delimiter.length());
      TextParser bp(mimeHeader, lineShift);
    
      contentType = "";
      contentAttribute = "";
      fileName = "";
      if ((pos = bp.find("CONTENT",'-', 0, TRUE)) != -1) {
        pos++;
        if ( (bp.wordAt(pos).upper() == "TYPE") &&
          (bp.separatorAt(pos) == ':') ) {
          contentType = bp.nextWord().upper();
          if (bp.nextSeparator() == '/')
            contentAttribute = bp.nextWord().upper();
          content = contentType + "/" + contentAttribute;
        }
        if ((pos = bp.find("ENCODING",':', 0, TRUE)) != -1) {
          pos++;
          encoding = bp.getString(&pos, 'z', TRUE);
        }
        
        if ( (pos = bp.find("FILENAME",'=', 0, TRUE)) != -1) {
          pos++;
          fileName = bp.getString(&pos, 'z', TRUE);
          fileName = fileName.right(fileName.length() - 1);
          fileName = fileName.left(fileName.length() - 1);
        }

      }
      pos = mimeBody.find(boundary, 0, FALSE);
      if (pos == -1)            //should not occur, malformed mail
        pos = mimeBody.length();
      body = mimeBody.right(mimeBody.length() - pos);
      mimeBody = mimeBody.left(pos);
        
      if (fileName != "") { //attatchments of some type, audio, image etc.
        
        Enclosure e;
        e.id = enclosureId;
        e.originalName = fileName;
        e.contentType = contentType;
        e.contentAttribute = contentAttribute;
        e.encoding = encoding;
        e.body = mimeBody;
        e.saved = FALSE;
        mail->addEnclosure(&e);
        enclosureId++;
        
      } else if (contentType == "TEXT") {
        if (contentAttribute == "PLAIN") {
          mail->body = mimeBody;
          mail->bodyPlain = mimeBody;
        }
        if (contentAttribute == "HTML") {
          mail->body = mimeBody;
        }
      }
    }
  } else {
    mail->bodyPlain = body;
    mail->body = body;
  }
  return TRUE;
}

bool EmailHandler::getEnclosure(Enclosure *ePtr)
{
  QFile f(ePtr->path + ePtr->name);
  char src[4];
  char *destPtr;
  QByteArray buffer;
  uint bufCount, pos, decodedCount, size, x;
  
  if (! f.open(IO_WriteOnly) ) {
    qWarning("could not save: " + ePtr->path + ePtr->name);
    return FALSE;
  }
  
  if (ePtr->encoding.upper() == "BASE64") {
    size = (ePtr->body.length() * 3 / 4); //approximate size (always above)
    buffer.resize(size);
    bufCount = 0;
    pos = 0;
    destPtr = buffer.data();
    
    while (pos < ePtr->body.length()) {
      decodedCount = 4;
      x = 0;
      while ( (x < 4) && (pos < ePtr->body.length()) ) {
        src[x] = ePtr->body[pos].latin1();
        pos++;
        if (src[x] == '\r' || src[x] == '\n' || src[x] == ' ')
          x--;
        x++;
      }
      if (x > 1) {
        decodedCount = parse64base(src, destPtr);
        destPtr += decodedCount;
        bufCount += decodedCount;
      }
    }
    
    buffer.resize(bufCount);  //set correct length of file
    f.writeBlock(buffer);
  } else {
    QTextStream t(&f);
    t << ePtr->body;
  }
  return TRUE;
}

int EmailHandler::parse64base(char *src, char *bufOut) {
  
  char c, z;
  char li[4];
  int processed;

  //conversion table withouth table...
  for (int x = 0; x < 4; x++) {
    c = src[x];
  
    if ( (int) c >= 'A' && (int) c <= 'Z')
      li[x] = (int) c - (int) 'A';
    if ( (int) c >= 'a' && (int) c <= 'z')
      li[x] = (int) c - (int) 'a' + 26;
    if ( (int) c >= '0' && (int) c <= '9')
      li[x] = (int) c - (int) '0' + 52;
    if (c == '+')
      li[x] = 62;
    if (c == '/')
      li[x] = 63;
  }
  
  processed = 1;
  bufOut[0] = (char) li[0] & (32+16+8+4+2+1); //mask out top 2 bits
  bufOut[0] <<= 2;
  z = li[1] >> 4;
  bufOut[0] = bufOut[0] | z;    //first byte retrived
    
  if (src[2] != '=') {
    bufOut[1] = (char) li[1] & (8+4+2+1); //mask out top 4 bits
    bufOut[1] <<= 4;
    z = li[2] >> 2;
    bufOut[1] = bufOut[1] | z;    //second byte retrived
    processed++;

    if (src[3] != '=') {
      bufOut[2] = (char) li[2] & (2+1); //mask out top 6 bits
      bufOut[2] <<= 6;
      z = li[3];
      bufOut[2] = bufOut[2] | z;  //third byte retrieved
      processed++;
    }
  }
  return processed;
}

int EmailHandler::encodeMime(Email *mail) 
{
  
  QString fileName, fileType, contentType, newBody, boundary;
  Enclosure *ePtr;
  
  QString userName = mailAccount.name;
  if (userName.length()>0)	//only embrace it if there is a user name
  	userName += " <" + mailAccount.emailAddress + ">";
  
  //add standard headers
  newBody = "From: " + userName + "\r\nTo: ";
        for (QStringList::Iterator it = mail->recipients.begin(); it != mail->recipients.end(); ++it ) {
    newBody += *it + " ";
  }
  
   newBody += "\r\nCC: ";
   
   for (QStringList::Iterator it = mail->carbonCopies.begin(); it != mail->carbonCopies.end(); ++it ) {
    newBody += *it + " ";
  }
  
  newBody += "\r\nSubject: " + mail->subject + "\r\n";
  
  if (mail->files.count() == 0) {         //just a simple mail
    newBody += "\r\n" + mail->body;
    mail->rawMail = newBody;
    return 0;
  }

  //Build mime encoded mail
  boundary = "-----4345=next_bound=0495----";
  
  newBody += "Mime-Version: 1.0\r\n";
  newBody += "Content-Type: multipart/mixed; boundary=\"" +
        boundary + "\"\r\n\r\n";
  
  newBody += "This is a multipart message in Mime 1.0 format\r\n\r\n";
  newBody += "--" + boundary + "\r\nContent-Type: text/plain\r\n\r\n";
  newBody += mail->body;
  
  for ( ePtr=mail->files.first(); ePtr != 0; ePtr=mail->files.next() ) {
    fileName = ePtr->originalName;
    fileType = ePtr->contentType;
    QFileInfo fi(fileName);
    
    // This specification of contentType is temporary
    contentType = "";
    if (fileType == "Picture") {
      contentType = "image/x-image";
    } else if (fileType == "Document") {
      contentType = "text/plain";
    } else if (fileType == "Sound") {
      contentType = "audio/x-wav";
    } else if (fileType == "Movie") {
      contentType = "video/mpeg";
    } else {
      contentType = "application/octet-stream";
    }
    
    newBody += "\r\n\r\n--" + boundary + "\r\n";
    newBody += "Content-Type: " + contentType + "; name=\"" +
          fi.fileName() + "\"\r\n";
    newBody += "Content-Transfer-Encoding: base64\r\n";
    newBody += "Content-Disposition: inline; filename=\"" +
          fi.fileName() + "\"\r\n\r\n";
    
    if (encodeFile(fileName, &newBody) == -1)   //file not found?
      return -1;
  }
  
  newBody += "\r\n\r\n--" + boundary + "--";
  mail->rawMail = newBody;
  
  return 0;
}

int EmailHandler::encodeFile(QString fileName, QString *toBody)
{
  char *fileData;
  char *dataPtr;
  QString temp;
  uint dataSize, count;
  QFile f(fileName);
  
  if (! f.open(IO_ReadOnly) ) {
    qWarning("could not open file: " + fileName);
    return -1;
  }
  QTextStream s(&f);
  dataSize = f.size();
  fileData = (char *) malloc(dataSize + 3);
  s.readRawBytes(fileData, dataSize);
  
  temp = "";
  dataPtr = fileData;
  count  = 0;
  while (dataSize > 0) {
    if (dataSize < 3) {
      encode64base(dataPtr, &temp, dataSize);
      dataSize = 0;
    } else {
      encode64base(dataPtr, &temp, 3);
      dataSize -= 3;
      dataPtr += 3;
      count += 4;
    }
    if (count > 72) {
      count = 0;
      temp += "\r\n";
    }
  }
  toBody->append(temp);
  
  delete(fileData);
  f.close();
  return 0;
}

void EmailHandler::encode64base(char *src, QString *dest, int len)
{
  QString temp;
  uchar c;
  uchar bufOut[4];
  
  bufOut[0] = src[0];
  bufOut[0] >>= 2;                  //Done byte 0
  
  bufOut[1] = src[0];
  bufOut[1] = bufOut[1] & (1 + 2);            //mask out top 6 bits
  bufOut[1] <<= 4;                  //copy up 4 places
  if (len > 1) {
    c = src[1];
  } else {
    c = 0;
  }
  
  c = c & (16 + 32 + 64 + 128);
  c >>= 4;
  bufOut[1] = bufOut[1] | c;              //Done byte 1
  
  bufOut[2] = src[1];
  bufOut[2] = bufOut[2] & (1 + 2 + 4 + 8);
  bufOut[2] <<= 2;
  if (len > 2) {
    c = src[2];
  } else {
    c = 0;
  }
  c >>= 6;
  bufOut[2] = bufOut[2] | c;
  
  bufOut[3] = src[2];
  bufOut[3] = bufOut[3] & (1 + 2 + 4 + 8 + 16 + 32);
  
  if (len == 1) {
    bufOut[2] = 64;
    bufOut[3] = 64;
  }
  if (len == 2) {
    bufOut[3] = 64;
  }
  for (int x = 0; x < 4; x++) {
    if (bufOut[x] <= 25)
      bufOut[x] += (uint) 'A';
    else if (bufOut[x] >= 26 && bufOut[x] <= 51)
      bufOut[x] += (uint) 'a' - 26;
    else if (bufOut[x] >= 52 && bufOut[x] <= 61)
      bufOut[x] += (uint) '0' - 52;
    else if (bufOut[x] == 62)
      bufOut[x] = '+';
    else if (bufOut[x] == 63)
      bufOut[x] = '/';
    else if (bufOut[x] == 64)
      bufOut[x] = '=';

    dest->append(bufOut[x]);
  }
}

void EmailHandler::cancel()
{
  popClient->errorHandling(ErrCancel);
  smtpClient->errorHandling(ErrCancel);
}
