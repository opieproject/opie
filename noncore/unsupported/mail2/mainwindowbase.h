#ifndef MAINWINDOWBASE_H
#define MAINWINDOWBASE_H

#include <qmainwindow.h>

class ServerConnection;
class MailStatusBar;
class FolderWidget;
class QPEToolBar;
class QPEMenuBar;
class QPopupMenu;
class MailTable;
class QAction;

class MainWindowBase : public QMainWindow
{
	Q_OBJECT

public:
	MainWindowBase(QWidget *parent = 0, const char *name = 0, WFlags fl = 0);

protected slots:
	void slotFoldersToggled(bool toggled);

protected:
	ServerConnection *serverConnection;
	MailStatusBar *status;
	FolderWidget *folderView;
	QPEToolBar *toolbar;
 	QPEMenuBar *menubar;
	QPopupMenu *mailmenu, *servermenu;
	MailTable *mailView;
	QAction *compose, *sendQueue, *folders, *findmails, *configure;

};

#endif
