#include <qprogressbar.h>
#include <qmessagebox.h>
#include <qaction.h>
#include <qlabel.h>

#include "folderwidget.h"
#include "mainwindow.h"
#include "configdiag.h"
#include "configfile.h"
#include "searchdiag.h"
#include "mailtable.h"
#include "composer.h"
#include "viewmail.h"

MainWindow::MainWindow(QWidget *parent, const char *name, WFlags fl)
	: MainWindowBase(parent, name, fl)
{
	stop->setEnabled(false);

	connect(folderView, SIGNAL(status(const QString &)), statusLabel, SLOT(setText(const QString &)));
	connect(folderView, SIGNAL(folderSelected(Folder)), mailView, SLOT(setFolder(Folder)));

	connect(mailView, SIGNAL(mailClicked(IMAPResponseFETCH, IMAPHandler *)), SLOT(mailClicked(IMAPResponseFETCH, IMAPHandler *)));
	connect(mailView, SIGNAL(status(const QString &)), statusLabel, SLOT(setText(const QString &)));
	connect(mailView, SIGNAL(totalSteps(int)), statusProgress, SLOT(setTotalSteps(int)));
	connect(mailView, SIGNAL(progress(int)), statusProgress, SLOT(setProgress(int)));
	connect(mailView, SIGNAL(resetProgress()), statusProgress, SLOT(reset()));
	connect(mailView, SIGNAL(stopEnabled(bool)), stop, SLOT(setEnabled(bool)));

	connect(stop, SIGNAL(activated()), mailView, SLOT(stop()));

	connect(compose, SIGNAL(activated()), SLOT(slotCompose()));
	connect(findmails, SIGNAL(activated()), SLOT(slotSearch()));
	connect(configure, SIGNAL(activated()), SLOT(slotConfigure()));
}

void MainWindow::slotCompose()
{
	Composer composer(this, 0, true);
	composer.showMaximized();
	composer.exec();
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
