/*
** $Id: zlistview.h,v 1.1 2003-07-22 19:23:17 zcarsten Exp $
*/

#ifndef _ZLIST_VIEW_H_
#define _ZLIST_VIEW_H_

#include <qwidget.h>
#include <qlistview.h>
#include <qaccel.h>

class QListView;
class ZSafe;
class QKeyEvent;

class ZListView : public QListView
{
    Q_OBJECT

  protected:
    ZSafe *zs;
    
  public:
    ZListView(ZSafe *, const char *);
    ~ZListView();

  public slots:

  protected:  
    virtual void keyPressEvent ( QKeyEvent * e );
};

#endif//_ZLIST_VIEW_H_
