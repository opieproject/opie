#ifndef ADDCONNECTIONIMP_H 
#define ADDCONNECTIONIMP_H

#include "addconnection.h"
#include <qmap.h>
#include <qlist.h>

class QListViewItem;

class AddConnectionImp : public AddConnection {

Q_OBJECT
	
public:
  AddConnectionImp(QWidget *parent=0, const char *name=0, WFlags f=0);
  void addConnections(QMap<QString, QString> newList);
   
private slots:
  void changed();

private:
  QMap<QString, QString> list;

};

#endif

// addconectionimp.h

