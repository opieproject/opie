#ifndef MAINWINDOWBASE_H
#define MAINWINDOWBASE_H

#include <qmainwindow.h>

class ServerConnection;
class QProgressBar;
class FolderWidget;
class MailTable;
class QToolBar;
class QAction;
class QLabel;

class MainWindowBase : public QMainWindow
{
	Q_OBJECT

public:
	MainWindowBase(QWidget *parent = 0, const char *name = 0, WFlags fl = 0);

protected slots:
	void slotFoldersToggled(bool toggled);

protected:
	ServerConnection *serverConnection;
	QProgressBar *statusProgress;
	FolderWidget *folderView;
	MailTable *mailView;
	QToolBar *toolbar;
	QAction *compose, *folders, *findmails, *configure, *stop;
	QLabel *statusLabel;

};

#endif
