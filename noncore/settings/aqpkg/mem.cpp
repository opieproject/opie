/*
                             This file is part of the OPIE Project
                             
               =.            Copyright (c)  2002 Andy Qua <andy.qua@blueyonder.co.uk>
             .=l.                                Dan Williams <drw@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <stdio.h>
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

      for(i = allocList.begin(); i != allocList.end(); i++) {
	      sprintf(buf, "%-15s:  LINE %ld,  ADDRESS %ld %ld unfreed",
		      (*i)->file, (*i)->line, (*i)->address, (*i)->size);
	      totalSize += (*i)->size;
      }
      sprintf(buf, "-----------------------------------------------------------\n");
      sprintf(buf, "Total Unfreed: %ld bytes\n", totalSize);
};
