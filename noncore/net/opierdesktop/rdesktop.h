/*
   rdesktop: A Remote Desktop Protocol client.
   Master include file
   Copyright (C) Matthew Chapman 1999-2002
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#define __LITTLE_ENDIAN 1234
//#define __BIG_ENDIAN    4321
//#define __BYTE_ORDER __BIG_ENDIAN

//#if __BYTE_ORDER == __LITTLE_ENDIAN
#define L_ENDIAN
#define NEED_ALIGN
//#error le
//#elif __BYTE_ORDER == __BIG_ENDIAN
//#define B_ENDIAN
//#error be
//#else
//#error Unknown endianness. Edit rdesktop.h.
//#endif


//#define B_ENDIAN
//#define B_ENDIAN_PREFERRED

#define VERSION "1.2.0"

#ifdef WITH_DEBUG
#define DEBUG(args)	printf args;
#else
#define DEBUG(args)
#endif

#ifdef WITH_DEBUG_KBD
#define DEBUG_KBD(args) printf args;
#else
#define DEBUG_KBD(args)
#endif

#define STRNCPY(dst,src,n) { strncpy(dst,src,n-1); dst[n-1] = 0; }

#include "constants.h"
#include "types.h"
#include "parse.h"

#ifndef MAKE_PROTO
#include "proto.h"
#endif
