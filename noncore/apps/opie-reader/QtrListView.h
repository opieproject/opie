#ifndef __QTRLISTVIEW_H
#define __QTRLISTVIEW_H

#include <qlistview.h>

class QtrListView : public QListView
{
    Q_OBJECT

    virtual void keyPressEvent(QKeyEvent* e);
 public:
    QtrListView(QWidget* parent, char* name) : QListView(parent, name) {};

 signals:
    void OnOKButton(QListViewItem*);
    void OnCentreButton(QListViewItem*);
    void OnCancelButton();
};

#endif
