#ifndef FOLDERWIDGET_H
#define FOLDERWIDGET_H

#include "imapresponse.h"
#include "listviewplus.h"
#include "configfile.h"

class IMAPHandler;

class TopFolder
{
public:
	TopFolder() 
	{ 
		_valid = true; 
	}

	bool isValid() { return _valid; }
	void setAccount(Account account) { _account = account; }
	Account account() { return _account; }
	void setIMAPHandler(IMAPHandler *handler) { _handler = handler; }
	IMAPHandler *handler() { return _handler; }

private:
	bool _valid;
	Account _account;
	IMAPHandler *_handler;

};

class Folder
{
public:
	Folder() 
	{ 
		_valid = true; 
		_noCache = false;
	}

	bool isValid() { return _valid; }
	void setNoCache(bool noCache) { _noCache = noCache; }
	bool noCache() { return _noCache; }
	void setFullName(QString fullName) { _fullName = fullName; }
	QString fullName() { return _fullName; }
	void setSeparator(QString separator) { _separator = separator; }
	QString separator() { return _separator; }
	void setTopFolder(TopFolder topFolder) { _topFolder = topFolder; }
	TopFolder topFolder() { return _topFolder; }

private:
	bool _valid, _noCache;
	QString _fullName, _separator;
	TopFolder _topFolder;

};

class FolderWidgetItem : public QListViewItem
{
public:
	FolderWidgetItem(Folder &folder, QListView *parent);
	FolderWidgetItem(Folder &folder, FolderWidgetItem *parent);

	Folder folder() { return _folder; }

private:
	Folder _folder;

};

class FolderWidget : public ListViewPlus
{
	Q_OBJECT

public:
	FolderWidget(QWidget *parent = 0, const char *name = 0, WFlags fl = 0);
	~FolderWidget();

public slots:
	void update();

signals:
	void folderSelected(Folder folder);
	void status(const QString &text);
	void connecting();
	void connected();
	void disconnected();

protected:
	void getAccounts();
	FolderWidgetItem *addFolder(Folder &folder, FolderWidgetItem *folderWidgetItem);
	FolderWidgetItem *addAccount(Account &account);

	static const int MENU_RENAME = 0;
	static const int MENU_DELETE = 1;
	static const int MENU_MOVE = 2;
	static const int MENU_COPY = 3;
	static const int MENU_CREATE = 4;
	static const int MENU_RESCAN = 5;

protected slots:
	void slotMenuActivated(int itemid);
	void slotItemClicked(QListViewItem *item);

	void slotIMAPLookingUpHost();
	void slotIMAPHostFound();
	void slotIMAPConnected();
	void slotIMAPError(int error);
	void slotIMAPDisconnected();

	void slotIMAPLogin(IMAPResponse &response);
	void slotIMAPRename(IMAPResponse &response);
	void slotIMAPDelete(IMAPResponse &response);
	void slotIMAPCreate(IMAPResponse &response);
	void slotIMAPList(IMAPResponse &response);

private:
	Account _rescanAccount;
	Folder _createFolder;

};

#endif

