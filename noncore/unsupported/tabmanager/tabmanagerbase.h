/****************************************************************************
** Form interface generated from reading ui file 'tabmanagerbase.ui'
**
** Created: Fri May 3 14:38:09 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef TABMANAGERBASE_H
#define TABMANAGERBASE_H

#include <qvariant.h>
#include <qwidget.h>
#include "tablistview.h"
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QListView;
class QListViewItem;

class TabManagerBase : public QWidget
{ 
    Q_OBJECT

public:
    TabManagerBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~TabManagerBase();

    TabListView* tabList;

protected:
    QVBoxLayout* TabManagerBaseLayout;
};

#endif // TABMANAGERBASE_H
