/*
** $Id: zlistview.cpp,v 1.2 2004-03-01 20:04:34 chicken Exp $
*/

#include "zlistview.h"
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

