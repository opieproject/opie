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
#ifndef TABLISTVIEW_H
#define TABLISTVIEW_H

#include <qlistview.h>
#include <qcursor.h>
#include <qapplication.h>

class TabListView : public QListView {
  Q_OBJECT

signals:
  void moveItem(QListViewItem *item, QListViewItem *newFolder);

public:
  TabListView( QWidget* parent = 0, const char* name = 0) : QListView(parent, name){ currentSelectedItem = NULL;
  connect(this, SIGNAL(pressed ( QListViewItem *)), this, SLOT(itemPressed(QListViewItem *)));
  internalCursor.setShape(0);
  };

protected:
  void contentsMouseReleaseEvent(QMouseEvent* ){
	QListViewItem *newGroup = this->currentItem();
  // Make sure they are both real.
  if (currentSelectedItem == NULL  || newGroup == NULL)
    return;
  // Make sure they are not the same
  if(this->isSelected(currentSelectedItem) == true)
    return;

  // Ok we have two valid items.
  if(newGroup->parent())
    newGroup = newGroup->parent();

  // Just in case the parent was null
  if(newGroup == NULL)
    return;

  // If the new folder and buddies current parent are the same don't do anything.
  if (newGroup != currentSelectedItem->parent())
    moveItem(currentSelectedItem, newGroup);
  currentSelectedItem = NULL;
  qApp->restoreOverrideCursor();
};

private slots:
  void itemPressed(QListViewItem *item){
  if(item == NULL || !item->parent()){
    if(item == NULL)
      qDebug("Item is NULL");
    return;
  }

  currentSelectedItem = item;
  internalCursor.setShape(13);
  qApp->setOverrideCursor(internalCursor);
};


private:
  QListViewItem *currentSelectedItem;
  QCursor internalCursor;
};

#endif

