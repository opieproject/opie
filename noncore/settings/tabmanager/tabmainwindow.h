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

