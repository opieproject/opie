/*
** $Id: zlistview.cpp,v 1.1 2003-07-22 19:23:19 zcarsten Exp $
*/

#include "zlistview.h"
#include <qlistview.h>
#include <qevent.h>
#include <stdio.h>
#include "zsafe.h"

ZListView::ZListView (ZSafe *zsafe, const char *id) : QListView(zsafe, id)
{
   zs = zsafe;
}

ZListView::~ZListView()
{
}

void ZListView::keyPressEvent ( QKeyEvent *e )
{
/*
   char buf[64];
   sprintf (buf, "key: %d\n", e->key());
   qWarning (buf);
*/

   switch (e->key())
   {
      case 32: // middle cursor key pressed
         zs->showInfo (zs->selectedItem);
         break;
      default:
         QListView::keyPressEvent( e );
   }

}

