#include <qmessagebox.h>
#include <qtextstream.h>
#include <qfile.h>

#include <qpe/mimetype.h>

#include "miscfunctions.h"
#include "mailfactory.h"
#include "defines.h"

MailFactory::MailFactory(SendMail &smail, QWidget *parent)
	: QObject(), _smail(smail), _parent(parent)
{
	_abort = false;
	Account account = _smail.account();

	if (!_smail.from().isEmpty())
		_header += "From: " + _smail.from() + "\n";
	if (!_smail.replyTo().isEmpty())
		_header += "Reply-To: " + _smail.replyTo() + "\n";
	if (!_smail.to().isEmpty())
		_header += "To: " + _smail.to() + "\n";
	if (!_smail.cc().isEmpty())
		_header += "Cc: " + _smail.cc() + "\n";
	if (!_smail.bcc().isEmpty())
		_header += "Bcc: " + _smail.bcc() + "\n";
	if (!_smail.subject().isEmpty())
		_header += "Subject: " + _smail.subject() + "\n";
	if (!_smail.priority().isEmpty() || (_smail.priority() != "Normal"))
		_header += "Priority: " + _smail.priority() + "\n";
	_header += "Date: " + MiscFunctions::rfcDate() + "\n";
	if (!_smail.account().org().isEmpty())
		_header += "Organization: " + _smail.account().org() + "\n";
	if (_smail.needsMime())
		_header += "Mime-Version: 1.0\n";
	_header += "Message-Id: <" + MiscFunctions::uniqueString() + "." + account.email() + ">\n";
	if (!_smail.inReplyTo().isEmpty())
		_header += "In-Reply-To: " + _smail.inReplyTo() + "\n";
	if (!QString((QString) USERAGENT).isEmpty())
		_header += (QString) "User-Agent: " + USERAGENT + "\n";

	if (!_smail.needsMime()) {
//		if (_smail.encrypt() && !_smail.sign()) {
//			openPgpEncrypt(_smail.message(), _header, _body);
//		} else if (!_smail.encrypt() && _smail.sign()) {
//			openPgpSign(_smail.message(), _header, _body);
//		} else if (_smail.encrypt() && _smail.sign()) {
//			openPgpSignEncrypt(_smail.message(), _header, _body);
//		} else  {
			_body += _smail.message();
//		}
	} else {
		QString boundary = MiscFunctions::uniqueString();

		_header += "Content-Type: multipart/mixed; boundary=\"" + boundary + "\"\n";

		_body += "This is a multi-part message in MIME format.\n\n";
		_body += "--" + boundary + "\n";

//		if (_smail.encrypt() && !_smail.sign()) {
//			QString header, body;
//			openPgpEncrypt(_smail.message(), header, body);
//			_body += header + "\n" + body + "\n";
//		} else if (!_smail.encrypt() && _smail.sign()) {
//			QString header, body;
//			openPgpSign(_smail.message(), header, body);
//			_body += header + "\n" + body + "\n";
//		} else if (_smail.encrypt() && _smail.sign()) {
//			QString header, body;
//			openPgpSignEncrypt(_smail.message(), header, body);
//			_body += header + "\n" + body + "\n";
//		} else {

			// TODO: Do proper charset handling!
			_body += "Content-Type: text/plain; charset=\"iso-8859-1\"\n";
			_body += "Content-Transfer-Encoding: 8bit\n\n";
			_body += _smail.message() + "\n";
//		}

		QValueList<Attachment> attachments = _smail.attachments();
		QValueList<Attachment>::Iterator it;
		for (it = attachments.begin(); it != attachments.end(); it++) {
			QFile f((*it).fileName());
			if (f.open(IO_ReadOnly)) {
				QTextStream t(&f);
				QString file;
				while (!t.atEnd()) file += t.readLine() + "\n";
				f.close();
				QString mimetype = (new MimeType(*(*it).docLnk()))->id(); 

				_body += "\n--" + boundary + "\n";
				_body += "Content-Type: " + mimetype + "; name=\"" + (*it).newName() + "\"\n";

				// TODO: Decide which content transfer encoding is best. B64 for binary, QP for text.
				_body += "Content-Transfer-Encoding: base64\n";

				_body += "Content-Disposition: attachment; filename=\"" + (*it).newName() + "\"\n";
				if (!(*it).description().isEmpty()) 
					_body += "Content-Description: " + (*it).description() + "\n";

				_body += "\n" + MiscFunctions::encodeBase64(file) + "\n";
			} else {
				int ret = QMessageBox::critical(_parent, tr("Error"), tr("<p>Couldn't attach file '%1'. Continue anyway or abort?</p>").arg((*it).fileName()), tr("Continue"), tr("Abort"));
				if (ret != 0) {
					_abort = true;
					break;
				}
			}
		}
		_body += "\n--" + boundary + "--";
	}

	if (_abort) {
		_body = QString(0);
		_header = QString(0);
	}
}

// Unfinished GPG code.
/*
void MailFactory::openPgpEncrypt(const QString &text, QString &header, QString &body)
{
	QString boundary = MiscFunctions::uniqueString();

	header += "Content-Type: multipart/encrypted; boundary=\"" + boundary + "\"; protocol=\"application/pgp-encrypted\"\n";

	body += "--" + boundary + "\n";
	body += "Content-Type: application/pgp-encrypted\n\n";
	body += "Version: 1\n\n";
	body += "--" + boundary + "\n";
	body += "Content-Type: application/octet-stream\n\n";
	body += GpgHandling::encrypt(_smail.gpgReceivers(), text);
	body += "\n--" + boundary + "--\n";
}

void MailFactory::openPgpSign(const QString &text, QString &header, QString &body)
{
	QString boundary = MiscFunctions::uniqueString();

	header += "Content-Type: multipart/signed; boundary=\"" + boundary + "\"; protocol=\"application/pgp-signature\"\n";

	body += "--" + boundary + "\n";

	QString temp;
	temp += "Content-Type: text/plain; charset=\"iso-8859-1\"\n";
	temp += "Content-Transfer-Encoding: quoted-printable\n\n";
	temp += MiscFunctions::encodeQPrintable(text) + "\n";
	body += temp;

	temp.replace(QRegExp("\n"), "\r\n");
	QString signature = GpgHandling::sign(temp, _parent);

	body += "\n--" + boundary + "\n";
	body += "Content-Type: application/pgp-signature\n\n";
	body += signature + "\n";
	body += "\n--" + boundary + "--\n";
}

void MailFactory::openPgpSignEncrypt(const QString &text, QString &header, QString &message)
{
	QString header_, message_;
	openPgpSign(text, header_, message_);
	openPgpEncrypt(header_ + "\n" + message_, header, message);
}
*/
void MailFactory::genMail(QString &header, QString &message, SendMail &smail, QWidget *parent)
{
	MailFactory factory(smail, parent);

	header = factory._header;
	message = factory._body;
}

