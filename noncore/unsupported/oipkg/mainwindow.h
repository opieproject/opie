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

#include "package.h"
#include "packagelist.h"
#include "packagelistremote.h"
#include "packagelistlocal.h"
#include "packagelistdoclnk.h"
#include "pmipkg.h"
#include "pksettings.h"
#include "packagelistview.h"

class QComboBox;
class QToolBar;
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
  void makeChannel();
  void setSections();
  void setSubSections();
  bool updateIcon;

  PmIpkg* ipkg;
  PackageManagerSettings *settings;
  PackageListLocal  packageListServers;
  PackageListRemote packageListSearch;
  PackageListDocLnk packageListDocLnk;
  PackageListView *listViewPackages;
  QAction *runAction;
  QAction *updateAction;
  QAction *findAction;
  QToolBar *findBar;
  QLineEdit *findEdit;
  QAction *searchAction;
  QAction *searchCommit;
  QToolBar *searchBar;
  QLineEdit *searchEdit;
  QAction *sectionAction;
  QToolBar *sectionBar;
  QComboBox *section;
  QComboBox *subsection;
  QAction *destAction;
  QToolBar *destBar;
  QComboBox *destination;
  QCheckBox* CheckBoxLink;
};

#endif
