#include <qtextbrowser.h>
#include <qmessagebox.h>
#include <qaction.h>
#include <qapplication.h>

//#include "mailfactory.h"
//#include "composer.h"
#include "viewmail.h"

AttachItem::AttachItem(QListView *parent, AttachItemStore &attachItemStore)
	: QListViewItem(parent), _attachItemStore(attachItemStore)
{
	setText(0, _attachItemStore.mimeType());
	setText(1, _attachItemStore.fileName());
	setText(2, _attachItemStore.description());
}

AttachItem::AttachItem(QListViewItem *parent, AttachItemStore &attachItemStore)
	: QListViewItem(parent), _attachItemStore(attachItemStore)
{
	setText(0, _attachItemStore.mimeType());
	setText(1, _attachItemStore.fileName());
	setText(2, _attachItemStore.description());
}

void ViewMail::setMailInfo( const QString & from, const QString & to, const QString & subject, const QString & cc, const QString & bcc, const QString & bodytext ) {

m_mail[0] = from;
m_mail[1] = to;
m_mail[2] = subject;
m_mail[3] = cc;
m_mail[4] = bcc;
m_mail[5] = bodytext;

setText();

}


ViewMail::ViewMail( QWidget *parent, const char *name, WFlags fl)
	: ViewMailBase(parent, name, fl), _inLoop(false)
{
	_gotBody = false;

	connect(reply, SIGNAL(activated()), SLOT(slotReply()));
	connect(forward, SIGNAL(activated()), SLOT(slotForward()));

	attachments->setEnabled(_gotBody);

//	_handler->iUid("FETCH", QString("%1 (BODY[1])").arg(mail.uid()));
//	connect(_handler, SIGNAL(gotResponse(IMAPResponse &)), SLOT(slotIMAPUid(IMAPResponse &)));
}

void ViewMail::setText()
{

 setCaption( caption().arg( m_mail[0] ) );

     _mailHtml = tr(
		"<html><body>"
		"<div align=center><b>%1</b></div>"
		"<b>From:</b> %2<br>"
		"<b>To:</b> %3<br>"
		"%4"
		"%5"
		"<b>Date:</b> %6<hr>"
		"<font face=fixed>%7</font>")
		.arg( deHtml( m_mail[2] ) )
		.arg( deHtml( m_mail[0] ) )
		.arg( deHtml( m_mail[1] ) )
		.arg( tr("<b>Cc:</b> %1<br>").arg( deHtml( m_mail[3] ) ) )
		.arg( tr("<b>Bcc:</b> %1<br>").arg( deHtml( m_mail[4] ) ) )
		.arg( tr("(no date)" ) )
		.arg("%1");
	browser->setText( QString(_mailHtml) + deHtml( m_mail[5] ) );
}



ViewMail::~ViewMail()
{
	hide();
}

void ViewMail::hide()
{
	QWidget::hide();

	if (_inLoop) {
		_inLoop = false;
		qApp->exit_loop();
	}
}

void ViewMail::exec()
{
	show();

	if (!_inLoop) {
		_inLoop = true;
		qApp->enter_loop();
	}
}

QString ViewMail::deHtml(const QString &string)
{
	QString string_ = string;
	string_.replace(QRegExp("&"), "&amp;");
	string_.replace(QRegExp("<"), "&lt;");
	string_.replace(QRegExp(">"), "&gt;");
	string_.replace(QRegExp("\\n"), "<br>");
	return string_;
}

void ViewMail::slotReply()
{
	if (!_gotBody) {
		QMessageBox::information(this, tr("Error"), tr("<p>The mail body is not yet downloaded, so you cannot reply yet."), tr("Ok"));
		return;
	}

	QString rtext;
//	rtext += QString("* %1 wrote on %2:\n")		// no i18n on purpose
//		.arg(_mail.envelope().from()[0].toString())
//		.arg(_mail.envelope().mailDate());

//	QString text = _mail.bodyPart(1).data();
//	QStringList lines = QStringList::split(QRegExp("\\n"), text);
	QStringList::Iterator it;
//	for (it = lines.begin(); it != lines.end(); it++) {
//		rtext += "> " + *it + "\n";
//	}
	rtext += "\n";

	QString prefix;
//	if (_mail.envelope().subject().find(QRegExp("^Re: *$")) != -1) prefix = "";
//	else prefix = "Re: ";				// no i18n on purpose

//	SendMail sendMail;
//	sendMail.setTo(_mail.envelope().from()[0].toString());
//	sendMail.setSubject(prefix + _mail.envelope().subject());
//	sendMail.setInReplyTo(_mail.envelope().messageId());
//	sendMail.setMessage(rtext);

//	Composer composer(this, 0, true);
//	composer.setSendMail(sendMail);
//	composer.showMaximized();
//	composer.exec();
}

void ViewMail::slotForward()
{
	if (!_gotBody) {
		QMessageBox::information(this, tr("Error"), tr("<p>The mail body is not yet downloaded, so you cannot forward yet."), tr("Ok"));
		return;
	}

	QString ftext;
/*	ftext += QString("\n----- Forwarded message from %1 -----\n\n")
		.arg(_mail.envelope().from()[0].toString());
	if (!_mail.envelope().mailDate().isNull())
		ftext += QString("Date: %1\n")
			.arg(_mail.envelope().mailDate());
	if (!_mail.envelope().from()[0].toString().isNull())
		ftext += QString("From: %1\n")
			.arg(_mail.envelope().from()[0].toString());
	if (!_mail.envelope().to().toString().isNull())
		ftext += QString("To: %1\n")
			.arg(_mail.envelope().to().toString());
	if (!_mail.envelope().cc().toString().isNull())
		ftext += QString("Cc: %1\n")
			.arg(_mail.envelope().cc().toString());
	if (!_mail.envelope().bcc().toString().isNull())
		ftext += QString("Bcc: %1\n")
			.arg(_mail.envelope().bcc().toString());
	if (!_mail.envelope().subject().isNull())
		ftext += QString("Subject: %1\n")
			.arg(_mail.envelope().subject());

	ftext += QString("\n%1\n")
		.arg(_mail.bodyPart(1).data());

	ftext += QString("----- End forwarded message -----\n");
*/
/*
	SendMail sendMail;
	sendMail.setSubject("Fwd: " + _mail.envelope().subject());
	sendMail.setMessage(ftext);

	Composer composer(this, 0, true);
	composer.setSendMail(sendMail);
	composer.showMaximized();
	composer.exec();
*/
}

/*
void ViewMail::slotIMAPUid(IMAPResponse &response)
{
	disconnect(_handler, SIGNAL(gotResponse(IMAPResponse &)), this, SLOT(slotIMAPUid(IMAPResponse &)));

	if (response.statusResponse().status() == IMAPResponseEnums::OK) {
		QValueList<IMAPResponseBodyPart> bodyParts;
		bodyParts.append(response.FETCH()[0].bodyPart(0));
		_mail.setBodyParts(bodyParts);

		browser->setText(QString(_mailHtml).arg(deHtml(response.FETCH()[0].bodyPart(0).data())));

//		fillList(response.FETCH()[0].bodyStructure());

		_gotBody = true;
	} else {
		QMessageBox::warning(this, tr("Error"), tr("<p>I was unable to retrieve the mail from the server. You can try again later or give up.</p>"), tr("Ok"));
	}
}
*/
