/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>

#ifndef PACKAGELISTVIEW_H
#define PACKAGELISTVIEW_H

#include <qlistview.h>
#include <qaction.h>
#include <qdict.h>
#include <qstring.h>
#include <qtimer.h>
#include <qwidget.h>
#include <qpopupmenu.h>
#include "listviewitemoipkg.h"
#include "packagelist.h"
#include "debug.h"

class PackageListItem;
class OipkgPackage;
class PackageManagerSettings;
//class ListViewItemOipkg;

class PackageListView : public QListView
{
    Q_OBJECT

public:
	PackageListView(QWidget*, const char*, PackageManagerSettings*);
  void addList( QString, PackageList* );
//	~PackageListView();
  QTimer *popupTimer;
private:
  QDict<PackageList> PackageLists;
  QDict<ListViewItemOipkg> rootItems;
  PackageManagerSettings *settings;
//  OipkgPackage *activePackage;
//  PackageListItem *activePackageListItem;
  ListViewItemOipkg *activeItem;
  QPopupMenu *popupMenu;
  QPopupMenu *destsMenu;
public slots:
  void showPopup();
  void changePackageDest( int );
  void setCurrent( QListViewItem* );
  void stopTimer( QListViewItem* );
  void toggleProcess();
  void display();
};

#endif
