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
#ifndef TABMAINWINDOW_H
#define TABMAINWINDOW_H

#include "tabmanager.h"
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpe/resource.h>

class TabMainWindow : public QMainWindow {

public:
  TabMainWindow( QWidget* parent = 0, const char* name = 0) : QMainWindow(parent, name){
    tabManager = new TabManager(this, "TabManger");
    setCaption("Tab Manager");
    this->setCentralWidget(tabManager);
    QToolBar *newToolBar = new QToolBar("toolbar", this);

    // Insert all of the buttons.
    QToolButton *newFolder = new QToolButton(Resource::loadPixmap("fileopen"), "New Folder", 0, tabManager, SLOT(newFolder()), newToolBar);
    QToolButton *newFile = new QToolButton(Resource::loadPixmap("new"), "New Folder", 0, tabManager, SLOT(newApplication()), newToolBar);
    QToolButton *editItem = new QToolButton(Resource::loadPixmap("edit"), "Edit", 0, tabManager, SLOT(editCurrentItem()), newToolBar);
    QToolButton *deleteItem = new QToolButton(Resource::loadPixmap("reset"), "Deelete", 0, tabManager, SLOT(removeItem()), newToolBar);

  };

protected:
  TabManager *tabManager;

};

#endif

// tabmainwindow.h

