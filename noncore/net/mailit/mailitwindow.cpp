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
#include "mailitwindow.h"

MailItWindow::MailItWindow(QWidget *parent, const char *name, WFlags fl)
  : QMainWindow(parent, name, fl)
{
  currentCaption = "Mailit";
  setCaption(tr(currentCaption));
  views = new QWidgetStack(this);
  setCentralWidget(views);
  
 emailClient = new EmailClient(views, "client");
  writeMail = new WriteMail(views, "writing");
  readMail = new ReadMail(views, "reading");

  views->raiseWidget(emailClient);
                        
  connect(emailClient, SIGNAL(composeRequested()),
    this, SLOT(compose()) );
  connect(emailClient, SIGNAL(viewEmail(QListView *, Email *)), this,
      SLOT(viewMail(QListView *, Email *)) );
  connect(emailClient, SIGNAL(mailUpdated(Email *)), this,
      SLOT(updateMailView(Email *)) );

  connect(writeMail, SIGNAL(cancelMail()), this, SLOT(showEmailClient()) );
  connect(writeMail, SIGNAL(sendMailRequested(const Email &)), this,
      SLOT(showEmailClient()) );
  connect(writeMail, SIGNAL(sendMailRequested(const Email &)), emailClient,
      SLOT(enqueMail(const Email &)) );
  
  connect(readMail, SIGNAL(cancelView()), this, SLOT(showEmailClient()) );
  connect(readMail, SIGNAL(replyRequested(Email &, bool&)), this,
    SLOT(composeReply(Email &, bool&)) );
  connect(readMail, SIGNAL(forwardRequested(Email &)), this,
    SLOT(composeForward(Email &)) );

  connect(readMail, SIGNAL(removeItem(EmailListItem *, bool &)), emailClient,
    SLOT(deleteMail(EmailListItem *, bool &)) );
  connect(readMail, SIGNAL(viewingMail(Email *)), emailClient,
    SLOT(moveMailFront(Email *)) );
  
  connect(emailClient, SIGNAL(newCaption(const QString &)),
    this, SLOT(updateCaption(const QString &)) );
  viewingMail = FALSE;
  
}

MailItWindow::~MailItWindow()
{
}

void MailItWindow::closeEvent(QCloseEvent *e)
{
  if (views->visibleWidget() == emailClient) {
    e->accept();
  } else {
    showEmailClient();
  }
}

void MailItWindow::compose()
{
  viewingMail = FALSE;
  emailClient->hide();
  readMail->hide();
  views->raiseWidget(writeMail);
  writeMail->setAddressList(emailClient->getAdrListRef());
  setCaption( tr( "Write mail"  ) );
}

void MailItWindow::composeReply(Email &mail, bool& replyAll)
{
  compose();
  writeMail->reply(mail,replyAll) ;
}

void MailItWindow::composeForward(Email &mail)
{
  compose();
  writeMail->forward(mail) ;
}


void MailItWindow::showEmailClient()
{
  viewingMail = FALSE;
  writeMail->hide();
  readMail->hide();
  views->raiseWidget(emailClient);
  setCaption( tr(currentCaption) );
}

void MailItWindow::viewMail(QListView *view, Email *mail)
{
  viewingMail = TRUE;
  emailClient->hide();
  readMail->update(view, mail);
  views->raiseWidget(readMail);
  setCaption( tr( "Examine mail"  ) );
}

void MailItWindow::updateMailView(Email *mail)
{
  if (viewingMail) {
    readMail->mailUpdated(mail);
  }
}

void MailItWindow::updateCaption(const QString &newCaption)
{
  currentCaption = newCaption;
  setCaption(tr(currentCaption));
}

void MailItWindow::setDocument(const QString &_address)
{
    // strip leading 'mailto:'
    QString address = _address;
    if (address.startsWith("mailto:"))
        address = address.mid(6);
 
    compose();
    writeMail->setRecipient(address);
}

/*void MailItWindow::reply(Email& mail)
{
	qDebug("####EmailClient: 0 reached");
	composeReply(mail,(bool&)FALSE);
}

void MailItWindow::replyAll(Email& mail)
{
	qDebug("####EmailClient: 1 reached");
	composeReply(mail,(bool&)TRUE);
}

void MailItWindow::forward(Email& mail)
{
	qDebug("####EmailClient: 2 reached");
}

void MailItWindow::remove(Email&)
{
	qDebug("####EmailClient: 3 reached");
	//emit removeItem(eli,(bool&)TRUE);
} */