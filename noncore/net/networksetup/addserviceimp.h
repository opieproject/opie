#ifndef ADDSERVICEIMP_H 
#define ADDSERVICEIMP_H

#include "addservice.h"
#include <qmap.h>
#include <qlist.h>

class QListViewItem;

class AddServiceImp : public AddService {

Q_OBJECT
	
public:
  AddServiceImp(QWidget *parent=0, const char *name=0, WFlags f=0):AddService(parent, name, f){};
  void addServices(QList<QString> list);
      
private:
  QMap<QListViewItem*, QString> pluginInfo; 

};

#endif

// addserviceimp.h

