/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#include "maillist.h"

void MailList::clear()
{
  sortedList.setAutoDelete(TRUE);
  sortedList.clear();
  currentPos = 0;
}

int MailList::count()
{
  return sortedList.count();
}

int* MailList::first()
{
  dList *mPtr;
  
  if (sortedList.count() == 0)
    return NULL;
    
  mPtr = sortedList.at(0);
  currentPos = 1;
  return &(mPtr->serverId);
}

int* MailList::next()
{
  dList *mPtr;
  
  if ( (currentPos) >= sortedList.count())
    return NULL;
  
  mPtr = sortedList.at(currentPos);
  currentPos++;
  return &(mPtr->serverId);
}

void MailList::sizeInsert(int serverId, uint size)
{
  dList *tempPtr;
  int x;
  
  dList *newEntry = new dList;
  newEntry->serverId = serverId;
  newEntry->size = size;
  
  for (tempPtr = sortedList.first(); tempPtr != NULL; tempPtr = sortedList.next() ) {
    if (newEntry->size < tempPtr->size) {
      x = sortedList.at();
      sortedList.insert(x, newEntry);
      return;
    }
  }
  sortedList.append(newEntry);
}

void MailList::moveFront(int serverId, uint/* size*/)
{
  dList *currentPtr;
  uint tempPos;
  QString temp;
  
  tempPos = currentPos;
  if ( tempPos >= sortedList.count() )
    return;
  currentPtr = sortedList.at(tempPos);
  while ( ((tempPos+1) < sortedList.count()) && ( currentPtr->serverId != serverId) ) {
    tempPos++;
    currentPtr = sortedList.at(tempPos);
  }
  
  if ( (currentPtr != NULL) && (currentPtr->serverId == serverId) ) {
    temp.setNum(currentPtr->serverId);
    qWarning("moved to front, message: " + temp);
    
    dList *itemPtr = sortedList.take(tempPos);
    sortedList.insert(currentPos, itemPtr);
  }

}

//only works if mail is not already in download
bool MailList::remove(int serverId, uint /*size*/)
{
  dList *currentPtr;
  uint tempPos;
  QString temp;
  
  tempPos = currentPos;
  if ( tempPos >=sortedList.count() )
    return FALSE;
  currentPtr = sortedList.at(tempPos);
  while ( ((tempPos + 1) < sortedList.count()) && ( currentPtr->serverId != serverId) ) {
    tempPos++;
    currentPtr = sortedList.at(tempPos);
  }
  
  if ( (currentPtr != NULL) && (currentPtr->serverId == serverId) ) {
    temp.setNum(currentPtr->serverId);
    qWarning("deleted message: " + temp);
    sortedList.remove(tempPos);
    
    return TRUE;
  }
  return FALSE;
}

void MailList::insert(int /*pos*/, int /*serverId*/, uint/* size*/)
{
//  sortedList.insert(pos, mPtr);
}
