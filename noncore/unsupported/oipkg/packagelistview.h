/***************************************************************************
                          packagelistview.h  -  description
                             -------------------
    begin                : Sat Apr 27 2002
    copyright            : (C) 2002 by tille
    email                : tille@handhelds.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PACKAGELISTVIEW_H
#define PACKAGELISTVIEW_H

#include <qlistview.h>
#include <qaction.h>
#include <qtimer.h>
#include <qwidget.h>
#include <qpopupmenu.h>
#include "debug.h"

class PackageListItem;
class Package;
class PackageManagerSettings;

class PackageListView : public QListView
{
    Q_OBJECT

public:
	PackageListView(QWidget*, const char*, PackageManagerSettings*);
//	~PackageListView();
  QTimer *popupTimer;
private:
	PackageManagerSettings *settings;
  Package *activePackage;
  PackageListItem *activePackageListItem;
  QPopupMenu *popupMenu;
  QPopupMenu *destsMenu;
public slots:
  void showPopup();
  void changePackageDest( int );
  void setCurrent( QListViewItem* );
  void stopTimer( QListViewItem* );
  void toggleProcess();
};

#endif
