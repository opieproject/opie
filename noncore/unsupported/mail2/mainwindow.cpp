#include <qmessagebox.h>
#include <qaction.h>
#include <qpe/qcopenvelope_qws.h>

#include "mailstatusbar.h"
#include "folderwidget.h"
#include "mainwindow.h"
#include "configdiag.h"
#include "configfile.h"
#include "searchdiag.h"
#include "mailtable.h"
#include "composer.h"
#include "viewmail.h"
#include "mailfactory.h"

MainWindow::MainWindow(QWidget *parent, const char *name, WFlags fl)
	: MainWindowBase(parent, name, fl)
{
	status->setStopEnabled(false);

	connect(folderView, SIGNAL(status(const QString &)), status, SLOT(setStatusText(const QString &)));
	connect(folderView, SIGNAL(folderSelected(Folder)), mailView, SLOT(setFolder(Folder)));

	connect(mailView, SIGNAL(mailClicked(IMAPResponseFETCH, IMAPHandler *)), SLOT(mailClicked(IMAPResponseFETCH, IMAPHandler *)));
	connect(mailView, SIGNAL(status(const QString &)), status, SLOT(setStatusText(const QString &)));
	connect(mailView, SIGNAL(totalSteps(int)), status, SLOT(setProgressTotalSteps(int)));
	connect(mailView, SIGNAL(progress(int)), status, SLOT(setProgress(int)));
	connect(mailView, SIGNAL(resetProgress()), status, SLOT(resetProgress()));
	connect(mailView, SIGNAL(stopEnabled(bool)), status, SLOT(setStopEnabled(bool)));

	connect(status, SIGNAL(stop()), mailView, SLOT(stop()));

	connect(compose, SIGNAL(activated()), SLOT(slotComposeNoParams()));
	connect(sendQueue, SIGNAL(activated()), SLOT(slotSendQueued()));
	connect(findmails, SIGNAL(activated()), SLOT(slotSearch()));
	connect(configure, SIGNAL(activated()), SLOT(slotConfigure()));

	// Added by Stefan Eilers to allow starting by addressbook..
#if !defined(QT_NO_COP) 
	QCopChannel *addressChannel = new QCopChannel("QPE/Application/mail" , this );
	connect (addressChannel, SIGNAL( received(const QCString &, const QByteArray &)),
		 this, SLOT ( appMessage(const QCString &, const QByteArray &) ) );
#endif
}

// Added by Stefan Eilers to allow starting by addressbook..
void MainWindow::appMessage(const QCString &msg, const QByteArray &data)
{
	if (msg == "writeMail(QString,QString)") {
		QDataStream stream(data,IO_ReadOnly);
		QString name, email;
		stream >> name >> email;

		qWarning("opie-mail:: Should send mail to %s with address %s", name.latin1(), email.latin1() );

		slotCompose( name, email );
		
	}else{
		QString str_message = msg;
		qWarning("opie-mail:: Received unknown QCop-Message: %s", str_message.latin1() );
	}
}

void MainWindow::slotCompose( const QString& name, const QString& email )
{
	Composer composer(this, 0, true);

	// If there is a mailaddress given, create message..
	if ( ! name.isEmpty() ){
		qWarning("opie-mail:: Compose mail for %s with address %s", name.latin1(), email.latin1() );
		SendMail compMail;
		compMail.setTo( "\"" + name + "\"" + " " + "<"+ email + ">");
		composer.setSendMail( compMail );
	}
	composer.showMaximized();
	composer.exec();
}

void MainWindow::slotComposeNoParams()
{
	slotCompose( 0l, 0l);
}

void MainWindow::slotSendQueued()
{
	Composer composer(this, 0, true, true);
// 	composer.sendQueue();
	composer.showMaximized();
	composer.exec();
//	composer.close();
}

void MainWindow::slotSearch()
{
	SearchDiag searchDiag(this, 0, true);
	searchDiag.showMaximized();
	searchDiag.exec();
}

void MainWindow::slotConfigure()
{
	ConfigDiag configDiag(this, 0, true);
	configDiag.showMaximized();
	configDiag.exec();

	connect(&configDiag, SIGNAL(changed()), folderView, SLOT(update()));
}

void MainWindow::mailClicked(IMAPResponseFETCH mail, IMAPHandler *handler)
{
	ViewMail viewMail(mail, handler, this, 0, true);
	viewMail.showMaximized();
	viewMail.exec();
}
