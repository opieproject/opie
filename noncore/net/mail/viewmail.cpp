#include <qtextbrowser.h>
#include <qmessagebox.h>
#include <qaction.h>
#include <qapplication.h>

#include "settings.h"
#include "composemail.h"
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


void ViewMail::setBody( RecBody body ) {

m_mail[2] = body.Bodytext();

}

void ViewMail::setMail( RecMail mail ) {

m_mail[0] = mail.getFrom();
m_mail[1] = mail.getSubject();
m_mail[3] = mail.getDate();
m_mail[4] = mail.Msgid();

m_mail2[0] = mail.To();
m_mail2[1] = mail.CC();
m_mail2[2] = mail.Bcc();

setText();
}



ViewMail::ViewMail( QWidget *parent, const char *name, WFlags fl)
	: ViewMailBase(parent, name, fl), _inLoop(false)
{
	m_gotBody = false;

	connect(reply, SIGNAL(activated()), SLOT(slotReply()));
	connect(forward, SIGNAL(activated()), SLOT(slotForward()));

	attachments->setEnabled(m_gotBody);
}

void ViewMail::setText()
{

  QString toString;
  QString ccString;
  QString bccString;

  for ( QStringList::Iterator it = ( m_mail2[0] ).begin(); it != ( m_mail2[0] ).end(); ++it ) {
                  toString += (*it);
  }
  for ( QStringList::Iterator it = ( m_mail2[1] ).begin(); it != ( m_mail2[1] ).end(); ++it ) {
                  ccString += (*it);
  }
  for ( QStringList::Iterator it = ( m_mail2[2] ).begin(); it != ( m_mail2[2] ).end(); ++it ) {
                  bccString += (*it);
  }

 setCaption( caption().arg( m_mail[0] ) );

     m_mailHtml = "<html><body>"
                  "<table width=\"100%\" border=\"0\"><tr bgcolor=\"#FFDD76\"><td>"
                  "<div align=left><b>" + deHtml( m_mail[1] ) + "</b></div>"
                  "</td></tr><tr bgcolor=\"#EEEEE6\"><td>"
               	  "<b>" + tr( "From" ) + ": </b><font color=#6C86C0>" + deHtml( m_mail[0] ) + "</font><br>"
		  "<b>" + tr(  "To" ) + ": </b><font color=#6C86C0>" + deHtml( toString ) + "</font><br><b>" +
		  tr( "Cc" ) + ": </b>" + deHtml( ccString ) + "<br>"
            	  "<b>" + tr( "Date" ) + ": </b> " +  m_mail[3] +
                  "</td></tr></table><font face=fixed>";

	browser->setText( QString( m_mailHtml) + deHtml( m_mail[2] ) + "</font></html>" );
        // remove later in favor of a real handling
	m_gotBody = true;
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
	if (!m_gotBody) {
		QMessageBox::information(this, tr("Error"), tr("<p>The mail body is not yet downloaded, so you cannot reply yet."), tr("Ok"));
		return;
	}

	QString rtext;
	rtext += QString("* %1 wrote on %2:\n")		// no i18n on purpose
		.arg(  m_mail[1] )
		.arg( m_mail[3] );

	QString text = m_mail[2];
	QStringList lines = QStringList::split(QRegExp("\\n"), text);
        QStringList::Iterator it;
	for (it = lines.begin(); it != lines.end(); it++) {
		rtext += "> " + *it + "\n";
	}
	rtext += "\n";

	QString prefix;
	if ( m_mail[1].find(QRegExp("^Re: *$")) != -1) prefix = "";
	else prefix = "Re: ";				// no i18n on purpose

        Settings *settings = new Settings();
        ComposeMail composer( settings ,this, 0, true);
        composer.setTo( m_mail[0] );
        composer.setSubject( "Re: " + m_mail[1] );
	composer.setMessage( rtext );
	composer.showMaximized();
	composer.exec();

}

void ViewMail::slotForward()
{
	if (!m_gotBody) {
		QMessageBox::information(this, tr("Error"), tr("<p>The mail body is not yet downloaded, so you cannot forward yet."), tr("Ok"));
		return;
	}

	QString ftext;
	ftext += QString("\n----- Forwarded message from %1 -----\n\n")
		.arg( m_mail[0] );
	if (!m_mail[3].isNull())
	 	ftext += QString("Date: %1\n")
		 	.arg( m_mail[3] );
	if (!m_mail[0].isNull())
	 	ftext += QString("From: %1\n")
		 	.arg( m_mail[0] );
	if (!m_mail[1].isNull())
	 	ftext += QString("Subject: %1\n")
		 	.arg( m_mail[1] );

	ftext += QString("\n%1\n")
	 	.arg( m_mail[2]);

	ftext += QString("----- End forwarded message -----\n");

        Settings *settings = new Settings();
        ComposeMail composer( settings ,this, 0, true);
        composer.setSubject( "Fwd: " + m_mail[1] );
	composer.setMessage( ftext );
	composer.showMaximized();
	composer.exec();
}

