#include "addserviceimp.h"
#include <qpe/qlibrary.h>
#include <qlistview.h>
#include <qlist.h>

void AddServiceImp::addServices(QList<QString> list){
  list.setAutoDelete(true);
  
  for(uint i = 0; i < list.count(); i++){
    QString pluginFileName = "";
    QLibrary *lib = new QLibrary(pluginFileName);
    void *functionPointer = lib->resolve("info");
    if( !functionPointer ){
      qDebug(QString("AddServiceImp: File: %1 is not a plugin, but though was.").arg(pluginFileName).latin1());
      delete lib;
      break;
    }
  
    // Try to get an info.
    QString info = ((QString (*)()) functionPointer)();
    QListViewItem *newItem = new QListViewItem(registeredServicesList, info);
  }
}


// addserviceimp.cpp
