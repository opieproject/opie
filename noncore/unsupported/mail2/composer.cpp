#include <qmultilineedit.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qaction.h>
#include <qtimer.h>
#include <qlabel.h>
#include <qapplication.h>

#include <qpe/resource.h>
#include <qpe/config.h>

#include <opie/ofiledialog.h>

#include "mailstatusbar.h"
#include "addresspicker.h"
#include "listviewplus.h"
#include "smtphandler.h"
#include "composer.h"
#include "rename.h"

AttachViewItem::AttachViewItem(QListView *parent, Attachment &attachment)
	: QListViewItem(parent), _attachment(attachment)
{
	setPixmap(0, _attachment.docLnk().pixmap().isNull() ? Resource::loadPixmap("UnknownDocument-14") : _attachment.docLnk().pixmap());
	setText(0, _attachment.newName().isEmpty() ? _attachment.fileName() : _attachment.newName());
	setText(1, _attachment.description());
}

Composer::Composer(QWidget *parent, const char *name, WFlags fl, bool sendQueue)
	: ComposerBase(parent, name, fl), _inLoop(false)
{
	_sendQueued = sendQueue;
	status->setStopEnabled(false);
	to->setFocus();

	connect(sendmail, SIGNAL(activated()), SLOT(slotSendMail()));
	connect(queuemail, SIGNAL(activated()), SLOT(slotQueueMail()));
	connect(addressbook, SIGNAL(activated()), SLOT(slotOpenAddressPicker()));
	connect(addattach, SIGNAL(activated()), SLOT(slotAddAttach()));
	connect(delattach, SIGNAL(activated()), SLOT(slotDelAttach()));

	connect(from, SIGNAL(activated(int)), SLOT(slotFromChanged(int)));

	connect(attachPopup, SIGNAL(activated(int)), SLOT(slotPopupHandler(int)));

	QTimer::singleShot(0, this, SLOT(slotFillStuff()));
	QTimer::singleShot(0, this, SLOT(slotResizing()));

}

Composer::~Composer()
{
	hide();
}

void Composer::hide()
{
	QWidget::hide();

	if (_inLoop) {
		_inLoop = false;
		qApp->exit_loop();
	}
}

void Composer::exec()
{
	show();
	if (!_inLoop) {
		_inLoop = true;
		qApp->enter_loop();
	}
}

void Composer::setSendMail(SendMail &sendMail)
{
	to->setText(sendMail.to());
	cc->setText(sendMail.cc());
	bcc->setText(sendMail.bcc());
	subject->setText(sendMail.subject());
	message->setText(sendMail.message());
	_inReplyTo = sendMail.inReplyTo();

	QValueList<Attachment> attachments = sendMail.attachments();
	QValueList<Attachment>::Iterator it;
	for (it = attachments.begin(); it != attachments.end(); it++) {
		(void) new AttachViewItem(attachView, *it);
		if (attachView->isHidden()) attachView->show();
	}
}

void Composer::slotResizing()
{
	from->setMaximumWidth(width() - fromBox->width());
	from->resize(width() - fromBox->width(), y());
  if (_sendQueued) slotSendQueued();
}

void Composer::slotPopupHandler(int itemid)
{
	if (attachView->currentItem() == NULL) {
		QMessageBox::information(this, tr("Error"), tr("Please select an entry first."), tr("Ok"));
		return;
	}

	if (itemid == POPUP_ATTACH_RENAME) {
		QString tmp = Rename::rename(attachView->currentItem()->text(0), this);
		if (tmp != QString(0)) attachView->currentItem()->setText(0, tmp);
	} else if (itemid == POPUP_ATTACH_DESC) {
		QString tmp = Rename::getText(tr("Set Description"), tr("<div align=center>Description"), this);
		if (tmp != QString(0)) attachView->currentItem()->setText(1, tmp);
	} else if (itemid == POPUP_ATTACH_REMOVE) {
		attachView->takeItem(attachView->currentItem());
	}
}

void Composer::slotSendMail()
{
	if (to->text().find(QRegExp(".*\\@.*\\..*")) == -1) {
		QMessageBox::information(this, tr("Error"), tr("<p>You have to specify a recipient.<br>(eg: foo@bar.org)</p>"), tr("Ok"));
		return;
	}

	SendMail smail;
	smail.setFrom(from->currentText());
	smail.setReplyTo(replyto->text());
	smail.setTo(to->text());
	smail.setCc(cc->text());
	smail.setBcc(bcc->text());
	smail.setSubject(subject->text());
	smail.setMessage(message->text());
	smail.setNeedsMime(attachView->childCount() == 0 ? false : true);
	smail.setAccount(accountsLoaded[from->currentItem()]);

	if (priority->currentItem() == POPUP_PRIO_LOW) {
		smail.setPriority("Low");	// No i18n on purpose
	} else if (priority->currentItem() == POPUP_PRIO_NORMAL) {
		smail.setPriority("Normal");	// No i18n on purpose
	} else if (priority->currentItem() == POPUP_PRIO_HIGH) {
		smail.setPriority("High");	// No i18n on purpose
	}

	QValueList<Attachment> attachments;
	QListViewItem *item;
	for (item = attachView->firstChild(); item != 0; item = item->itemBelow()) {
		attachments.append(((AttachViewItem *)item)->attachment());
	}

	smail.setAttachments(attachments);

	QString header, message;
	MailFactory::genMail(header, message, smail, this);
	if (header.isNull() || message.isNull()) return;	// Aborted.

	status->setStopEnabled(true);

	SmtpHandler *handler = new SmtpHandler(header, message, accountsLoaded[from->currentItem()], to->text());
	connect(handler, SIGNAL(finished()), SLOT(slotSendFinished()));
	connect(handler, SIGNAL(error(const QString &)), SLOT(slotSendError(const QString &)));
	connect(handler, SIGNAL(status(const QString &)), status, SLOT(setStatusText(const QString &)));
}

void Composer::slotSendQueued()
{
  int effSendCount = 0;
	qDebug("Sending queued messages");
	Config cfg( "mailqueue", Config::User );
	cfg.setGroup( "Settings" );
	_sendCount = 0;
	_sendError = 0;
	_toSend = cfg.readNumEntry( "count", 0 );
	
 	if (_toSend == 0) close();
	
 	qDebug("%i messages to send", _toSend);
  QString str;
	for (int i=1;i<=_toSend;i++)
	  {
     	qDebug("sending message %i",i);
	    cfg.setGroup( "Mail_" + QString::number(i) );
	    SendMail smail;
     	str = cfg.readEntry("from");
      qDebug("setFrom %s",str.latin1());
	    smail.setFrom( str );
     	str = cfg.readEntry("reply");
      qDebug("setReplyTo %s",str.latin1());
	    smail.setReplyTo( str );
	    QString toAdr = cfg.readEntry("to");
      qDebug("to %s",toAdr.latin1());
	    smail.setTo( toAdr ); //to->text());
     	str = cfg.readEntry("cc");
      qDebug("setCc %s",str.latin1());
	    smail.setCc( str ); //cc->text());
	    smail.setBcc( cfg.readEntry("bcc") ); //bcc->text());
     	str = cfg.readEntry("subject");
      qDebug("setSubject %s",str.latin1());
	    smail.setSubject( str ); //subject->text());
     	str = cfg.readEntryCrypt("message");
      qDebug("setMessage %s",str.latin1());
	    smail.setMessage( str ); //message->text());
	    smail.setNeedsMime( cfg.readBoolEntry("mime") ); //attachView->childCount() == 0 ? false : true);
	
     	qDebug("setting account [%i]",cfg.readNumEntry("account"));
     	Account accnt = accountsLoaded[ cfg.readNumEntry("account") ];
	    smail.setAccount( accnt ); //accountsLoaded[from->currentItem()]);


	    int prio = cfg.readNumEntry( "priority" );
     	qDebug("setting priority %i",prio);
	    if (prio == POPUP_PRIO_LOW) {
	      smail.setPriority("Low");	// No i18n on purpose
	    } else if (prio == POPUP_PRIO_NORMAL) {
	      smail.setPriority("Normal");	// No i18n on purpose
	    } else if (prio == POPUP_PRIO_HIGH) {
	      smail.setPriority("High");	// No i18n on purpose
	    }

	    QValueList<Attachment> attachments;
	    Attachment a;
	    QString an;

	    int ac = cfg.readNumEntry( "attachments", 0 );
			qDebug("%i Attachments",ac);
	    for (int j = 0; i < ac; ac++) {
	      an = "Attachment_" + QString::number( j );
       	qDebug(an.latin1());
	      a.setFileName(cfg.readEntry( an + "fileName" ));
	      a.setNewName(cfg.readEntry( an + "newName" ));
	      a.setDescription(cfg.readEntry( an + "description" ));
	      a.setDocLnk( DocLnk( cfg.readEntry( an + "docLnk" )) );
	      attachments.append( a ); 
	    }

	    smail.setAttachments(attachments);

     	qDebug("putting mail together");

	    QString header, message;
	    MailFactory::genMail(header, message, smail, this);
	    if (header.isNull() || message.isNull()) continue;//return;	// Aborted.

	   // abort->setEnabled(true);

     	qDebug("Sending to %s",toAdr.latin1());
	    SmtpHandler *handler = new SmtpHandler(header, message, accnt ,toAdr);
   	  effSendCount++;
    	connect(handler, SIGNAL(finished()), SLOT(slotSendQueuedFinished()));
  	  connect(handler, SIGNAL(error(const QString &)), SLOT(slotSendQueuedError(const QString &)));
	    connect(handler, SIGNAL(status(const QString &)), status, SLOT(setStatusText(const QString &)));

   	}
 	if (effSendCount < _toSend)
  {
   	_toSend = effSendCount;
		QMessageBox::information(this, tr("Error"), tr("<p>There was a problem sending some of the queued mails.</p>"), tr("Ok"));
  }
}

void Composer::slotQueueMail()
{
	if (to->text().find(QRegExp(".*\\@.*\\..*")) == -1) {
		QMessageBox::information(this, tr("Error"), tr("<p>You have to specify a recipient.<br>(eg: foo@bar.org)</p>"), tr("Ok"));
		return;
	}

 	Config cfg( "mailqueue", Config::User );

	cfg.setGroup( "Settings" );
	int count = cfg.readNumEntry( "count", 0 );
 	count++;
	cfg.writeEntry( "count", count );
 	qDebug("queueing mail %i",count);

	cfg.setGroup( "Mail_" + QString::number( count ));
	cfg.writeEntry( "from", from->currentText() );
	cfg.writeEntry( "reply", replyto->text());
	cfg.writeEntry( "to", to->text());
	cfg.writeEntry( "cc", cc->text());
	cfg.writeEntry( "bcc", bcc->text());
	cfg.writeEntry( "subject", subject->text());
	cfg.writeEntryCrypt( "message", message->text());
	cfg.writeEntry( "mime", attachView->childCount() == 0 );
	cfg.writeEntry( "account", from->currentItem());
	cfg.writeEntry( "priority", priority->currentItem() );
	cfg.writeEntry( "attachments", attachView->childCount() );

	Attachment a;
	QListViewItem *item;
	QString an;
	int i = 0;
	for (item = attachView->firstChild(); item != 0; item = item->itemBelow()) {
		a = ((AttachViewItem *)item)->attachment();
		an = "Attachment_" + QString::number( i++ );
		cfg.writeEntry( an + "fileName", a.fileName() );
		cfg.writeEntry( an + "newName", a.newName() );
		cfg.writeEntry( an + "description", a.description() );
		cfg.writeEntry( an + "docLnk", a.docLnk().file() );	
	}

	QMessageBox::information(this, tr("Success"), tr("<p>The mail was queued successfully.</p><p>The queue contains ")+QString::number(count)+tr(" mails.</p>"), tr("Ok"));

}

void Composer::slotSendError(const QString &error)
{
	status->setStatusText(tr("<font color=#ff0000>Error occoured during sending.</font>"));
	QMessageBox::warning(this, tr("Error"), tr("<p>%1</p").arg(error), tr("Ok"));
}

void Composer::slotSendQueuedError(const QString &error)
{
	_sendError++;
	qDebug("error send mail %i",_sendCount);
	status->setStatusText(tr("<font color=#ff0000>Error occoured during sending.</font>"));
	QMessageBox::warning(this, tr("Error"), tr("<p>%1</p").arg(error), tr("Ok"));
}

void Composer::slotSendFinished()
{
	QMessageBox::information(this, tr("Success"), tr("<p>The mail was sent successfully.</p>"), tr("Ok"));

	status->setStatusText(QString(0));
	status->setStopEnabled(false);
}

void Composer::slotSendQueuedFinished()
{
	
	_sendCount++;
 	qDebug("finished send mail %i of %i (error %i)",_sendCount,_toSend,_sendError);
	if (_sendCount < _toSend) return;
  if (_sendError == _toSend) close();
	QMessageBox::information(this, tr("Success"), tr("<p>The queued mails ")+QString::number(_toSend-_sendError)+tr(" of ")+QString::number(_toSend)+(" were sent successfully.</p>"), tr("Ok"));
	Config cfg( "mailqueue", Config::User );
	cfg.setGroup( "Settings" );
	cfg.writeEntry( "count", 0 );
  for (int i=1;i<=_sendCount;i++)
	  {
	    cfg.setGroup( "Mail_" + QString::number(i) );
     	qDebug("remove mail %i", i);
      cfg.clearGroup();
      cfg.removeEntry( "Mail_" + QString::number(i) );
   	}
 	close();
}

void Composer::slotFillStuff()
{
	QValueList<Account> accounts = ConfigFile::getAccounts();
	int i = 0;

	QValueList<Account>::Iterator it;
	for (it = accounts.begin(); it != accounts.end(); it++) {
		if (!(*it).email().isEmpty() && !(*it).smtpServer().isEmpty() && !(*it).smtpPort().isEmpty()) {
			if (!(*it).realName().isEmpty())
				from->insertItem((*it).realName() + " <" + (*it).email() + ">", i);
			else
				from->insertItem((*it).email());

			accountsLoaded.append(*it);
			i++;
		}
	}
}

void Composer::slotFromChanged(int id)
{
	Account account = accountsLoaded[id];

	if (account.defaultCc()) cc->setText(account.cc());
	if (account.defaultBcc()) bcc->setText(account.bcc());
	if (account.defaultReplyTo()) replyto->setText(account.replyTo());
	if (!account.signature().isEmpty())
		message->setText(message->text() + "\n\n-- \n" + account.signature());
}

void Composer::slotOpenAddressPicker()
{
	if (!to->isHidden() && cc->isHidden() && bcc->isHidden()) {
		if (to->text().isEmpty()) {
			to->setText(AddressPicker::getNames());
		} else {
			to->setText(to->text() + ", " + AddressPicker::getNames());
		}
	} else if (to->isHidden() && !cc->isHidden() && bcc->isHidden()) {
		if (cc->text().isEmpty()) {
			cc->setText(AddressPicker::getNames());
		} else {
			cc->setText(cc->text() + ", " + AddressPicker::getNames());
		}
	} else if (to->isHidden() && cc->isHidden() && !bcc->isHidden()) {
		if (bcc->text().isEmpty()) {
			bcc->setText(AddressPicker::getNames());
		} else {
			bcc->setText(bcc->text() + ", " + AddressPicker::getNames());
		}
	}
}

void Composer::slotAddAttach()
{
  DocLnk lnk = OFileDialog::getOpenFileName( 1,"/");
//	DocLnk lnk = AttachDiag::getFile(this);
	if (lnk.name().isEmpty()) return;

	Attachment attachment;
	attachment.setFileName(lnk.file());
	attachment.setNewName(lnk.name());
	attachment.setDocLnk(lnk);
	
	(void) new AttachViewItem(attachView, attachment);
}

void Composer::slotDelAttach()
{
	if (attachView->currentItem() == NULL) return;
	attachView->takeItem(attachView->currentItem());
}
