#ifndef __UTILS_H
#define __UTILS_H

#include <opie2/odebug.h>

#define Log(x)  VLog x
extern void VLog( char * Format, ... );
extern void LogClose( void );
extern QString removeSpaces( const QString & X );

#endif
