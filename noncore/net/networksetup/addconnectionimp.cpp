#include "addconnectionimp.h"
#include <qlistview.h>
#include <qlist.h>
#include <qlabel.h>
#include <qheader.h>

/**
 * Constructor
 */ 
AddConnectionImp::AddConnectionImp(QWidget *parent=0, const char *name=0, WFlags f=0):AddConnection(parent, name, f){
  connect(registeredServicesList, SIGNAL(selectionChanged()), this, SLOT(changed()));
  registeredServicesList->header()->hide();
};

/**
 * The current item changed, update the discription.
 */ 
void AddConnectionImp::changed(){
  QListViewItem *item = registeredServicesList->currentItem();
  if(item){
    help->setText(list[item->text(0)]);
  }
}

/**
 * Save a copy of newList for the discriptions and append them all to the view
 * @param newList the new list of possible interfaces
 */ 
void AddConnectionImp::addConnections(QMap<QString, QString> newList){
  list = newList;
  QMap<QString, QString>::Iterator it;
  for( it = list.begin(); it != list.end(); ++it )
    QListViewItem *item = new QListViewItem(registeredServicesList, it.key());
  registeredServicesList->setCurrentItem(registeredServicesList->firstChild());
}

// addserviceimp.cpp

