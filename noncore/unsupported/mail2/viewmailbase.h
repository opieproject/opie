#ifndef VIEWMAILBASE_H
#define VIEWMAILBASE_H

#include <qmainwindow.h>

class QAction;
class OpenDiag;
class QListView;
class QPEToolBar;
class QTextBrowser;

class ViewMailBase : public QMainWindow
{
	Q_OBJECT

public:
	ViewMailBase(QWidget *parent = 0, const char *name = 0, WFlags fl = 0);

protected:
	QAction *reply, *forward, *attachbutton, *deleteMail;
	QListView *attachments;
	QPEToolBar *toolbar;
	QTextBrowser *browser;
	OpenDiag *openDiag;

protected slots:
	void slotChangeAttachview(bool state);

};

#endif

