/*
** $Id: zlistview.cpp,v 1.3 2004-05-31 15:26:51 ar Exp $
*/

#include "zlistview.h"
#include "zsafe.h"

/* OPIE */
//#include <opie2/odebug.h>

/* STD */
#include <stdio.h>

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
   owarn << "key: " << e->key() << oendl;
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

