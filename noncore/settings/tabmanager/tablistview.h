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

