#ifndef MICROKDE_KLISTVIEW_H
#define MICROKDE_KLISTVIEW_H

#include <qlistview.h>
#include <qpoint.h>
#include <qstring.h>

class KConfig;

class KListView : public QListView
{
    Q_OBJECT
  public:
    KListView( QWidget *parent=0, const char *name=0 )
      : QListView( parent, name ) {}

    void saveLayout( KConfig *, const QString & ) {}
    void restoreLayout( KConfig *, const QString & ) {}

  signals:
    void doubleClicked( QListViewItem *, QPoint, int );
};

#endif
