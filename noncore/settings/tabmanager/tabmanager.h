/*

               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Benjamin Meyer <meyerb@sharpsec.com>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/
#ifndef TABMANAGER_H
#define TABMANAGER_H

#include "tabmanagerbase.h"
#include <qmap.h>
#include <qevent.h>

class QListViewItem;

class TabManager : public TabManagerBase {
  Q_OBJECT

public:
  TabManager( QWidget* parent = 0, const char* name = 0);
  ~TabManager();
  bool changed;

public slots:
  void newFolder();
  void newApplication();
  void removeItem();
  void editCurrentItem();

private slots:
  void moveApplication(QListViewItem *, QListViewItem *);
  void editItem(QListViewItem * item);

protected:
  void rescanFolder(QString directory,  QListViewItem* parent=NULL);
  QMap<QListViewItem *, QString> itemList;
  bool findInstalledApplication(QString desktopFile, QString &installedAppFile);
  void swapInstalledLocation( QString installedAppFile, QString desktopFile, QString newLocation );

};

#endif

// tabmanager.h

