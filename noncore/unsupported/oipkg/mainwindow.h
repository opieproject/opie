/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// Copyright (C) 2000 Trolltech AS.
// adadpted form qpe/qipkg
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qaction.h>
#include <qtimer.h>
#include <qpopupmenu.h>

#include "packagelist.h"
#include "packagelistremote.h"
#include "packagelistfeeds.h"
#include "packagelistdoclnk.h"
#include "pmipkg.h"
#include "pksettings.h"
#include "packagelistview.h"

class QComboBox;
class QPEToolBar;
class QLineEdit;
class PackageListItem;
class QCopChannel;
class QMessageBox;
class QCheckBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
  MainWindow( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
  ~MainWindow();
  void makeChannel();

	QCopChannel *channel;

protected slots:
  void runIpkg();
  void updateList();
  void displayList();
  void subSectionChanged();
  void sectionChanged();
  void showSettings(int);
  void showSettingsSetup();
  void showSettingsSrv();
  void showSettingsDst();

public slots:
  void sectionClose();
  void sectionShow(bool);
  void findClose();
  void findShow(bool);
  void searchClose();
  void searchShow(bool);
  void destClose();
  void destShow(bool);
  void filterList();
  void createLinks();
  void removeLinks();
	void receive (const QCString &, const QByteArray &);
	void setDocument (const QString &);
  void remotePackageQuery();

private:
  void makeMenu();
  void setSections();
  void setSubSections();
  bool updateIcon;

  PmIpkg* ipkg;
  PackageManagerSettings *settings;
  PackageListFeeds  *packageListServers;
  PackageListRemote *packageListSearch;
  PackageListDocLnk *packageListDocLnk;
  PackageListView *listViewPackages;
  QAction *runAction;
  QAction *updateAction;
  QAction *findAction;
  QPEToolBar *findBar;
  QLineEdit *findEdit;
  QAction *searchAction;
  QAction *searchCommit;
  QPEToolBar *searchBar;
  QLineEdit *searchEdit;
  QAction *sectionAction;
  QPEToolBar *sectionBar;
  QComboBox *section;
  QComboBox *subsection;
  QAction *destAction;
  QPEToolBar *destBar;
  QComboBox *destination;
  QCheckBox* CheckBoxLink;
};

#endif
