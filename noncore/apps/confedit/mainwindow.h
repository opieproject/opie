/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// (c) 2002 Patrick S. Vogt <tille@handhelds.org>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qaction.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include "editwidget.h"

class QPEToolBar;
class ListViewItemConfFile;
class ListViewConfDir;


class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
  MainWindow( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
  ~MainWindow();


public slots:
  void setCurrent(QListViewItem*);
  void groupChanged(const QString&);
  void keyChanged(const QString&);
  void valueChanged(const QString&);
  void showPopup();
  void stopTimer( QListViewItem* );
  void saveConfFile();
  void revertConfFile();

private:
	ListViewConfDir *settingList;
 	EditWidget *editor;
  ListViewItemConfigEntry *_currentItem;
  ListViewItemConfFile *_fileItem;
  QTimer *popupTimer;
  QPopupMenu *popupMenuFile;
  QPopupMenu *popupMenuEntry;
	QAction *popupActionSave;
	QAction *popupActionRevert;
	QAction *popupActionDelete;
	QAction *popupActionNew;
//	QAction *popupAction;
//	QAction *popupAction;

  void makeMenu();
};

#endif
