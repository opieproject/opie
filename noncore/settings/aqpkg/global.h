/***************************************************************************
                          global.h  -  description
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

#ifndef __GLOBAL_H
#define __GLOBAL_H

#define VERSION_TEXT "AQPkg Version 0.3"

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
#define IPKG_CONF "/home/andy/projects/aqpkg/aqpkg/data/ipkg.conf"
#define ROOT "/home/andy/projects/aqpkg/aqpkg/data/root"
#define IPKG_DIR "/home/andy/projects/aqpkg/aqpkg/data/"

#endif

#else

// Running QT on a X86 linux box
#define IPKG_CONF "/home/andy/projects/aqpkg/aqpkg/data/ipkg.conf"
#define ROOT "/home/andy/projects/aqpkg/aqpkg/data/root"
#define IPKG_DIR "/home/andy/projects/aqpkg/aqpkg/data/"

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
