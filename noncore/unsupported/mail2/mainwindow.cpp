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
	Composer *composer = new Composer();
	composer->showMaximized();
	composer->show();
}

void MainWindow::slotSearch()
{
	SearchDiag *searchDiag = new SearchDiag(this, 0, true);
	searchDiag->showMaximized();
	searchDiag->show();
}

void MainWindow::slotConfigure()
{
	ConfigDiag *configDiag = new ConfigDiag(this, 0, true);
	configDiag->showMaximized();
	configDiag->show();

	connect(configDiag, SIGNAL(changed()), folderView, SLOT(update()));
}

void MainWindow::mailClicked(IMAPResponseFETCH mail, IMAPHandler *handler)
{
	ViewMail *viewMail = new ViewMail(mail, handler);
	viewMail->showMaximized();
	viewMail->show();
}
