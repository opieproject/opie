#include <qmessagebox.h>
#include <qtextstream.h>
#include <qpopupmenu.h>
#include <qheader.h>
#include <qfile.h>
#include <qdir.h> 

#include <qpe/resource.h>

#include <stdlib.h>

#include "folderwidget.h"
#include "imaphandler.h"
#include "imapbase.h"
#include "rename.h"

FolderWidgetItem::FolderWidgetItem(Folder &folder, QListView *parent)
	: QListViewItem(parent), _folder(folder)
{
	setPixmap(0, QPixmap(Resource::loadPixmap("mail/inbox")));
	setText(0, _folder.topFolder().account().user() + " (" + _folder.topFolder().account().imapServer() + ")");
	setOpen(true);
}

FolderWidgetItem::FolderWidgetItem(Folder &folder, FolderWidgetItem *parent)
	: QListViewItem(parent), _folder(folder)
{
	if (_folder.noCache()) {
		setText(0, QObject::tr("<Foldertree not known.>"));
	} else {
		if (folder.fullName().upper() == "INBOX") {
			setPixmap(0, QPixmap(Resource::loadPixmap("mail/inbox")));
			setText(0, QObject::tr("Inbox"));
		} else {
			setPixmap(0, QPixmap(Resource::loadPixmap("mail/folder")));
			setText(0, folder.fullName());
		}
		setOpen(true);
	}
}

FolderWidget::FolderWidget(QWidget *parent, const char *name, WFlags fl)
	: ListViewPlus(parent, name, fl)
{
	header()->hide();
	addColumn("");
	setSorting(-1);

	QPopupMenu *menu = new QPopupMenu();
	menu->insertItem(tr("Rename"), MENU_RENAME);
	menu->insertItem(tr("Delete"), MENU_DELETE);
	menu->insertItem(tr("Move"), MENU_MOVE);
	menu->insertItem(tr("Copy"), MENU_COPY);
	menu->insertSeparator();
	menu->insertItem(tr("Create folder"), MENU_CREATE);
	menu->insertSeparator();
	menu->insertItem(tr("Rescan folder list"), MENU_RESCAN);
	setPopup(menu);

	getAccounts();

	connect(menu, SIGNAL(activated(int)), SLOT(slotMenuActivated(int)));
	connect(this, SIGNAL(clicked(QListViewItem *)), SLOT(slotItemClicked(QListViewItem *)));
}

FolderWidget::~FolderWidget()
{
	// TODO: Save folder tree.
}

void FolderWidget::update()
{
	getAccounts();
}

void FolderWidget::getAccounts()
{
	clear();

	QValueList<Account> accounts = ConfigFile::getAccounts();
	QValueList<Account>::Iterator it;
	for (it = accounts.begin(); it != accounts.end(); it++) {
		FolderWidgetItem *item = addAccount(*it);
		QFile f((QString) getenv("HOME") + "/Applications/mail/foldercache/foldercache-" + (*it).accountName());
		if (!f.open(IO_ReadOnly)) {
			Folder folder;
			folder.setNoCache(true);
			addFolder(folder, item);
		} else {
			QTextStream t(&f);
			while (!t.atEnd()) {
				QString separator = t.readLine();
				QString fullname = t.readLine();
				Folder folder;
				folder.setSeparator(separator);
				folder.setFullName(fullname);
				folder.setTopFolder(item->folder().topFolder());
				addFolder(folder, item);
			}
			f.close();
		}
	}
}

FolderWidgetItem *FolderWidget::addAccount(Account &account)
{
	TopFolder tf;
	tf.setAccount(account);

	// XXX This has to change!!! The folderwidget may not create an
	// XXX IMAPHandler!!!! Do this in IMAPHandler!
	tf.setIMAPHandler(new IMAPHandler(account));

	Folder folder;
	folder.setTopFolder(tf);

	connect(tf.handler(), SIGNAL(IMAPLookingUpHost()), SLOT(slotIMAPLookingUpHost()));
	connect(tf.handler(), SIGNAL(IMAPHostFound()), SLOT(slotIMAPHostFound()));
	connect(tf.handler(), SIGNAL(IMAPConnected()), SLOT(slotIMAPConnected()));
	connect(tf.handler(), SIGNAL(IMAPDisconnected()), SLOT(slotIMAPDisconnected()));
	connect(tf.handler(), SIGNAL(IMAPError(int)), SLOT(slotIMAPError(int)));

	return new FolderWidgetItem(folder, this);
}

FolderWidgetItem *FolderWidget::addFolder(Folder &folder, FolderWidgetItem *folderWidgetItem)
{
	return new FolderWidgetItem(folder, folderWidgetItem);
}

void FolderWidget::slotMenuActivated(int itemid)
{
	if (currentItem() == NULL) {
		QMessageBox::information(this, tr("Error"), tr("<p>Please select an item first.</p>"), tr("Ok"));
		return;
	}

	if (itemid == MENU_RENAME) {
		if (((FolderWidgetItem *)currentItem())->folder().fullName().isEmpty()) return;

		Folder folder = ((FolderWidgetItem *)currentItem())->folder();
		QString newName = Rename::rename(folder.fullName(), this); 
		if (newName.isNull()) return;

		folder.topFolder().handler()->iRename(folder.fullName(), newName);
		connect(folder.topFolder().handler(), SIGNAL(gotResponse(IMAPResponse &)), SLOT(slotIMAPRename(IMAPResponse &)));
	} else if (itemid == MENU_DELETE) {
		if (((FolderWidgetItem *)currentItem())->folder().fullName().isEmpty()) return;

		Folder folder = ((FolderWidgetItem *)currentItem())->folder();

		int ret = QMessageBox::information(this, tr("Question"), tr("<p>Do you really want to delete <pre>%1</pre>?").arg(folder.fullName()), tr("Yes"), tr("No"));
		if (ret == 1) return;

		_createFolder = folder;

		folder.topFolder().handler()->iDelete(folder.fullName());
		connect(folder.topFolder().handler(), SIGNAL(gotResponse(IMAPResponse &)), SLOT(slotIMAPDelete(IMAPResponse &)));
	} else if (itemid == MENU_MOVE) {

	} else if (itemid == MENU_COPY) {

	} else if (itemid == MENU_CREATE) {
		Folder folder = (((FolderWidgetItem *)currentItem())->folder());
		_createFolder = folder;

		QString folderName = Rename::getText(tr("Foldername"), tr("<p>Please enter the name of the new folder.</p>"), this);
		if (folderName.isNull()) return;

		folder.topFolder().handler()->iCreate(folder.fullName() + folder.separator() + folderName);
		connect(folder.topFolder().handler(), SIGNAL(gotResponse(IMAPResponse &)), SLOT(slotIMAPCreate(IMAPResponse &)));
	} else if (itemid == MENU_RESCAN) {
		Folder folder = (((FolderWidgetItem *)currentItem())->folder());
		_rescanAccount = folder.topFolder().account();

		folder.topFolder().handler()->iList("", "*");
		connect(folder.topFolder().handler(), SIGNAL(gotResponse(IMAPResponse &)), SLOT(slotIMAPList(IMAPResponse &)));
	}
}

void FolderWidget::slotItemClicked(QListViewItem *item)
{
	if (item == NULL) return;
	Folder folder = ((FolderWidgetItem *)item)->folder();
	if (folder.fullName().isEmpty()) return;

	emit folderSelected(folder);
}

void FolderWidget::slotIMAPLookingUpHost()
{
	emit status(tr("Looking up host..."));
	emit connecting();
}

void FolderWidget::slotIMAPHostFound()
{
	emit status(tr("Host found."));
}

void FolderWidget::slotIMAPConnected()
{
	emit status(tr("Connected to host."));
	emit connected();
}

void FolderWidget::slotIMAPError(int error)
{
	if (error == IMAPBase::IMAPErrConnectionRefused) {
		QMessageBox::warning(this, tr("Error"), tr("<p>The IMAP connection was refused.</p>"), tr("Ok"));
	} else if (error == IMAPBase::IMAPErrHostNotFound) {
		QMessageBox::warning(this, tr("Error"), tr("<p>The host was not found.</p>"), tr("Ok"));
	} else if (error == IMAPBase::IMAPErrSocketRead) {
		QMessageBox::warning(this, tr("Error"), tr("<p>There was an error while reading from the socket.</p>"), tr("Ok"));
	} else if (error == IMAPBase::IMAPErrLoginFailed) {
		QMessageBox::warning(this, tr("Error"), tr("<p>Login failed. Check your password/username.</p>"), tr("Ok"));
	} else {
		QMessageBox::warning(this, tr("Error"), tr("<p>An unknown error was encountered.</p>"), tr("Ok"));
	}
}

void FolderWidget::slotIMAPDisconnected()
{
	emit status(tr("Disconnected."));
	emit disconnected();
}

void FolderWidget::slotIMAPLogin(IMAPResponse &response)
{
	disconnect(response.imapHandler(), SIGNAL(gotResponse(IMAPResponse &)), this, SLOT(slotIMAPLogin(IMAPResponse &)));

	if (response.statusResponse().status() == IMAPResponseEnums::OK) {
		emit status(tr("Login successfull!"));
	} else {
		QMessageBox::warning(this, tr("Error"), tr("<p>Login failed. Go away.</p>"), tr("Ok"));
	}
}

void FolderWidget::slotIMAPRename(IMAPResponse &response)
{
	disconnect(response.imapHandler(), SIGNAL(gotResponse(IMAPResponse &)), this, SLOT(slotIMAPRename(IMAPResponse &)));

	if (response.statusResponse().status() == IMAPResponseEnums::OK) {
		emit status(tr("Renaming successfull!"));
	} else {
		QMessageBox::warning(this, tr("Error"), tr("<p>Renaming failed. (Server said: %1)</p>").arg(response.statusResponse().comment()), tr("Ok"));
	}
}

void FolderWidget::slotIMAPDelete(IMAPResponse &response)
{
	disconnect(response.imapHandler(), SIGNAL(gotResponse(IMAPResponse &)), this, SLOT(slotIMAPDelete(IMAPResponse &)));

	if (response.statusResponse().status() == IMAPResponseEnums::OK) {
		emit status(tr("Deletion successfull!"));

		_rescanAccount = _createFolder.topFolder().account();

		_createFolder.topFolder().handler()->iList(".", "*");
		connect(_createFolder.topFolder().handler(), SIGNAL(gotResponse(IMAPResponse &)), SLOT(slotIMAPList(IMAPResponse &)));
	} else {
		QMessageBox::warning(this, tr("Error"), tr("<p>Delete failed. (Server said: %1)</p>").arg(response.statusResponse().comment()), tr("Ok"));
	}
}

void FolderWidget::slotIMAPCreate(IMAPResponse &response)
{
	disconnect(response.imapHandler(), SIGNAL(gotResponse(IMAPResponse &)), this, SLOT(slotIMAPCreate(IMAPResponse &)));

	if (response.statusResponse().status() == IMAPResponseEnums::OK) {
		emit status(tr("Folder created. Rescanning..."));

		_rescanAccount = _createFolder.topFolder().account();

		_createFolder.topFolder().handler()->iList(".", "*");
		connect(_createFolder.topFolder().handler(), SIGNAL(gotResponse(IMAPResponse &)), SLOT(slotIMAPList(IMAPResponse &)));
	} else {
		QMessageBox::warning(this, tr("Error"), tr("<p>The folder could not be created. (Server said: %1)</p>").arg(response.statusResponse().comment()), tr("Ok"));
	}
}

void FolderWidget::slotIMAPList(IMAPResponse &response)
{
	disconnect(response.imapHandler(), SIGNAL(gotResponse(IMAPResponse &)), this, SLOT(slotIMAPList(IMAPResponse &)));

	if (response.statusResponse().status() == IMAPResponseEnums::OK) {
		QDir d((QString) getenv("HOME") + "/Applications/mail/foldercache");
		if (!d.exists()) {
			system("mkdir -p $HOME/Applications/mail/foldercache");
			qWarning("Created $HOME/Applications/mail/foldercache.");
		}
		QFile f((QString) getenv("HOME") + "/Applications/mail/foldercache/foldercache-" + _rescanAccount.accountName());
		if (!f.open(IO_WriteOnly)) {
			QMessageBox::critical(this, tr("Error"), tr("<p>Couldn't open folder cache file for writing!</p>"), tr("Ok"));
			return;
		} 
		QTextStream t(&f);

		QValueList<IMAPResponseLIST>::Iterator it;
		QValueList<IMAPResponseLIST> lists = response.LIST();
		for (it = lists.begin(); it != lists.end(); it++) {
			t << (*it).folderSeparator() << "\n";
			t << (*it).folder() << "\n";
		}

		f.close();

		emit status(tr("Got folder list."));
		getAccounts();
	} else {
		QMessageBox::warning(this, tr("Error"), tr("<p>Couldn't retrieve the folder list. (Server said: %1)</p>").arg(response.statusResponse().comment()), tr("Ok"));
	}
}

