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

#ifndef __GLOBAL_H
#define __GLOBAL_H

//extern QString VERSION_TEXT;

// Uncomment the below line to run on a Linux box rather than a Zaurus
// box this allows you to change where root is, and where to load config files from
// #define X86

// Sets up location of ipkg.conf and root directory
#ifdef QWS

#ifndef X86

// Running QT/Embedded on an arm processor
#define IPKG_CONF "/etc/ipkg.conf"
#define ROOT "/"
#define IPKG_DIR "/usr/lib/ipkg/"

#else

// Running QT/Embedded on a X86 linux box
#define IPKG_CONF "/home/andy/projects/aqpkg/data/ipkg.conf"
#define ROOT "/home/andy/projects/aqpkg/data/root"
#define IPKG_DIR "/home/andy/projects/aqpkg/data/"

#endif

#else

// Running QT on a X86 linux box
#define IPKG_CONF "/home/andy/projects/aqpkg/data/ipkg.conf"
#define ROOT "/home/andy/projects/aqpkg/data/root"
#define IPKG_DIR "/home/andy/projects/aqpkg/data/"

#endif


// Uncomment the below line to turn on memory checking
//#define _DEBUG 1

#ifndef __MEMFILE_C
#ifdef _DEBUG
void * operator new(unsigned int size,const char *file, int line);
void operator delete(void *p);
#endif

#ifdef _DEBUG
#define DEBUG_NEW new(__FILE__, __LINE__)
//#define DEBUG_NEW new
#else
#define DEBUG_NEW new
#endif

#define new DEBUG_NEW
#endif


void AddTrack(long addr,  long asize,  const char *fname, long lnum);
void RemoveTrack(long addr);
void DumpUnfreed();

#endif
