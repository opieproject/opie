#include <qmultilineedit.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qaction.h>
#include <qtimer.h>
#include <qlabel.h>

#include <qpe/resource.h>

#include "addresspicker.h"
#include "listviewplus.h"
#include "smtphandler.h"
#include "attachdiag.h"
#include "composer.h"
#include "rename.h"

AttachViewItem::AttachViewItem(QListView *parent, Attachment &attachment)
	: QListViewItem(parent), _attachment(attachment)
{
	setPixmap(0, _attachment.docLnk()->pixmap().isNull() ? Resource::loadPixmap("UnknownDocument-14") : _attachment.docLnk()->pixmap());
	setText(0, _attachment.newName().isEmpty() ? _attachment.fileName() : _attachment.newName());
	setText(1, _attachment.description());
}

Composer::Composer(QWidget *parent, const char *name, WFlags fl)
	: ComposerBase(parent, name, fl)
{
	abort->setEnabled(false);
	to->setFocus();

	connect(sendmail, SIGNAL(activated()), SLOT(slotSendMail()));
	connect(addressbook, SIGNAL(activated()), SLOT(slotOpenAddressPicker()));
	connect(addattach, SIGNAL(activated()), SLOT(slotAddAttach()));
	connect(delattach, SIGNAL(activated()), SLOT(slotDelAttach()));

	connect(from, SIGNAL(activated(int)), SLOT(slotFromChanged(int)));

	connect(attachPopup, SIGNAL(activated(int)), SLOT(slotPopupHandler(int)));

	QTimer::singleShot(0, this, SLOT(slotFillStuff()));
	QTimer::singleShot(0, this, SLOT(slotResizing()));
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

	abort->setEnabled(true);

	SmtpHandler *handler = new SmtpHandler(header, message, accountsLoaded[from->currentItem()], to->text());
	connect(handler, SIGNAL(finished()), SLOT(slotSendFinished()));
	connect(handler, SIGNAL(error(const QString &)), SLOT(slotSendError(const QString &)));
	connect(handler, SIGNAL(status(const QString &)), status, SLOT(setText(const QString &)));
}

void Composer::slotSendError(const QString &error)
{
	status->setText(tr("<font color=#ff0000>Error occoured during sending.</font>"));
	QMessageBox::warning(this, tr("Error"), tr("<p>%1</p").arg(error), tr("Ok"));
}

void Composer::slotSendFinished()
{
	QMessageBox::information(this, tr("Success"), tr("<p>The mail was sent successfully.</p>"), tr("Ok"));

	status->setText(QString(0));
	abort->setEnabled(false);
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
	DocLnk lnk = AttachDiag::getFile(this);
	if (lnk.name().isEmpty()) return;

	Attachment attachment;
	attachment.setFileName(lnk.file());
	attachment.setNewName(lnk.name());
	attachment.setDocLnk(&lnk);
	
	(void) new AttachViewItem(attachView, attachment);
}

void Composer::slotDelAttach()
{
	if (attachView->currentItem() == NULL) return;
	attachView->takeItem(attachView->currentItem());
}

