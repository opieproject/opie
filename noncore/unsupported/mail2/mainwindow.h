#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mainwindowbase.h"

class IMAPHandler;
class IMAPResponseFETCH;

class MainWindow : public MainWindowBase
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, const char *name = 0, WFlags fl = 0);

protected slots:
	void slotCompose();
	void slotSearch();
	void slotConfigure();
	void mailClicked(IMAPResponseFETCH mail, IMAPHandler *handler);

};

#endif
