#ifndef __CONFIG_H
#define __CONFIG_H

#define _UNICODE

#ifdef _UNICODE
#include <limits.h>

#define UTF8

typedef unsigned short tchar;
const tchar UEOF = USHRT_MAX;

#else
typedef char tchar;
const int UEOF = -1;
#endif

#include "ustring.h"

#endif
