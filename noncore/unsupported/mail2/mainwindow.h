#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mainwindowbase.h"

#include <qstring.h> 

class IMAPHandler;
class IMAPResponseFETCH;

class MainWindow : public MainWindowBase
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, const char *name = 0, WFlags fl = 0);

protected slots:
	void slotCompose( const QString& name, const QString& email );
	void slotComposeNoParams();
 	void slotSendQueued();
	void slotSearch();
	void slotConfigure();
	void mailClicked(IMAPResponseFETCH mail, IMAPHandler *handler);
	void appMessage(const QCString &msg, const QByteArray &data);

};

#endif
