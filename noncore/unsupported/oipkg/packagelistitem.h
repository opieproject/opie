/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
#ifndef PK_LISTITEM_H
#define PK_LISTITEM_H

#include "package.h"

#include <qstring.h>
#include <qlistview.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qdict.h>

#include "pksettings.h"

class PackageListItem
: public QCheckListItem
{
public:
  PackageListItem(QListViewItem*, Package*, PackageManagerSettings*);
  PackageListItem(QListView*, Package*, PackageManagerSettings*);
  void paintCell( QPainter*, const QColorGroup&, int, int, int );
  void paintFocus( QPainter*, const QColorGroup&, const QRect&  );
  QPixmap statePixmap() const;
  QString key( int, bool ) const;
  Package* getPackage() { return package; } ;
  QString getName() { return package->name(); } ;
  bool isInstalled(){ return package->installed(); };
  virtual void setOn ( bool );
  void displayDetails();

private:
  void init(Package*, PackageManagerSettings*);
	QCheckListItem *nameItem;
	QCheckListItem *destItem;
	QCheckListItem *linkItem;
	QCheckListItem *statusItem;
  PackageManagerSettings *settings;
  Package *package;
};


#endif
