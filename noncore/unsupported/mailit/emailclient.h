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
#ifndef EMAILCLIENT_H
#define EMAILCLIENT_H

#include <qlist.h>
#include <qcstring.h>
#include <qmainwindow.h>

#include <qtoolbar.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qtabwidget.h>
#include <qaction.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qprogressbar.h>
#include <qstatusbar.h>
#include <qdir.h>
#include <stdlib.h>

#include "emailhandler.h"
#include "emaillistitem.h"
#include "textparser.h"
#include "editaccount.h"
#include "maillist.h"
#include "addresslist.h"

#include <qpe/config.h>

class AccountList : public QList<MailAccount>
{
public:
  Item newItem(Item d);
private:
  MailAccount* dupl(MailAccount *in);
  MailAccount *ac;
};

//class EmailClient : public EmailClientBase
class EmailClient : public QMainWindow
{
    Q_OBJECT

public:
  EmailClient( QWidget* parent, const char* name, WFlags fl = 0 );
  ~EmailClient();
  AddressList* getAdrListRef();

signals:
  void composeRequested();
  void viewEmail(QListView *, Email *);
  void mailUpdated(Email *);
  void newCaption(const QString &);

public slots:
  void compose();
  void cancel();
  void enqueMail(const Email &mail);
  void setMailAccount();
  void sendQuedMail();
  void mailSent();
  void getNewMail();
  void getAllNewMail();
  void smtpError(int code);
  void popError(int code);
  void inboxItemSelected();
  void outboxItemSelected();
  void mailArrived(const Email &mail, bool fromDisk);
  void allMailArrived(int);
  void saveMail(QString fileName, QListView *view);
  void selectAccount(int);
  void editAccount(int);
  void updateAccounts();
  void deleteAccount(int);
  void deleteMail(EmailListItem *mailItem, bool &inbox);
  void setTotalSize(int);
  void setMailSize(int);
  void setDownloadedSize(int);
  void moveMailFront(Email *mailPtr);

private:
  void init();
  void readMail();
  QString getPath(bool enclosurePath);
  void readSettings();
  void saveSettings();
    
private:
  Config *mailconf;
  int newAccountId, idCount, mailIdCount;
  int accountIdCount;
  AccountList accountList;
  AddressList *addressList;
  
  EditAccount *editAccountView;
  EmailListItem *item;
  EmailHandler *emailHandler;
  QList<Email> quedMessages;
  MailList mailDownloadList;
  bool sending, receiving, previewingMail, allAccounts;
  QString lineShift;
  MailAccount account, *currentAccount;
  
  QToolBar *bar;
  QProgressBar *progressBar;
  QStatusBar *statusBar;
  QLabel *status1Label, *status2Label;
  QAction *getMailButton;
  QAction *sendMailButton;
  QAction *composeButton;
  QAction *cancelButton;

  QMenuBar *mb;
  QPopupMenu *selectAccountMenu;
  QPopupMenu *editAccountMenu;
  QPopupMenu *deleteAccountMenu;

  QTabWidget* mailboxView;
  QListView* inboxView;
  QListView* outboxView;

  QGridLayout* grid_2;
  QGridLayout* grid_3;
};

#endif // EMAILCLIENT_H
