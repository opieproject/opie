/***************************************************************************
                          mem.h  -  description
                             -------------------
    begin                : Mon Aug 26 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdio.h>
#include <fstream>
#include <list>
using namespace std;

#define __MEMFILE_C
#include "global.h"

#ifdef _DEBUG

void __cdecl *operator new( unsigned int size, const char *file, int line )
{
      void *ptr = (void *)malloc(size);
      AddTrack((long)ptr, size, file, line);
      return(ptr);
}

void operator delete(void *p)
{
      RemoveTrack((long)p);
      free(p);
}

#endif


typedef struct {
      long	address;
      long	size;
      char	file[64];
      long	line;
} ALLOC_INFO;

typedef list<ALLOC_INFO*> AllocList;

AllocList allocList;



void AddTrack(long addr,  long asize,  const char *fname, long lnum)
{
      ALLOC_INFO *info;


      info = (ALLOC_INFO *)malloc(sizeof( ALLOC_INFO ));
      info->address = addr;
      strncpy(info->file, fname, 63);
      info->line = lnum;
      info->size = asize;
      allocList.insert(allocList.begin(), info);
};

void RemoveTrack(long addr)
{
      AllocList::iterator i;

      bool found = false;
      for(i = allocList.begin(); i != allocList.end(); i++)
      {
	      if((*i)->address == addr)
	      {
		      allocList.remove((*i));
		      found = true;
		      break;
	      }
      }
}

void DumpUnfreed()
{
      AllocList::iterator i;
      long totalSize = 0;
      char buf[1024];


//      if(!allocList)
//	      return;

      for(i = allocList.begin(); i != allocList.end(); i++) {
	      sprintf(buf, "%-15s:  LINE %ld,  ADDRESS %ld %ld unfreed",
		      (*i)->file, (*i)->line, (*i)->address, (*i)->size);
	      cout <<buf << endl;
	      totalSize += (*i)->size;
      }
      sprintf(buf, "-----------------------------------------------------------\n");
	      cout <<buf << endl;
      sprintf(buf, "Total Unfreed: %ld bytes\n", totalSize);
	      cout <<buf << endl;
};
