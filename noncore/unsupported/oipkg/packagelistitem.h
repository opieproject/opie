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


#include <qstring.h>
//#include <qlistview.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qdict.h>

#include "listviewitemoipkg.h"
#include "package.h"
#include "pksettings.h"

class QPopupMenu;

class PackageListItem : public ListViewItemOipkg
{
public:
  PackageListItem(ListViewItemOipkg *parent, QString name, Type ittype);
  PackageListItem(
		  ListViewItemOipkg *parent, 
		  OipkgPackage *pack, 
		  PackageManagerSettings *s);
  PackageListItem(QListView*, OipkgPackage*, PackageManagerSettings*);
  ~PackageListItem();
  void paintCell( QPainter*, const QColorGroup&, int, int, int );
  void paintFocus( QPainter*, const QColorGroup&, const QRect&  );
  QPixmap statePixmap() const;
  QString key( int, bool ) const;
  OipkgPackage* getPackage() { return package; } ;
  QString getName() { return package->name(); } ;
  bool isInstalled(){ return package->installed(); };
  virtual void setOn ( bool );
  void displayDetails();

private:
  QPopupMenu *popupMenu;
  QPopupMenu *destsMenu;
  void init(OipkgPackage*, PackageManagerSettings*);
  virtual QPopupMenu* getPopupMenu();
  static QPopupMenu *_destsMenu;
	QCheckListItem *nameItem;
	QCheckListItem *destItem;
	QCheckListItem *linkItem;
	QCheckListItem *statusItem;
  PackageManagerSettings *settings;
  OipkgPackage *package;
protected slots:
 // void toggleProcess();
  virtual void menuAction( int );
};


#endif
