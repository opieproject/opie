#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

extern int debugLevel;

#define HACK

#define NEWLAYOUT

#define pvDebug(I, S) \
if ( debugLevel < 3 ) \
{ \
	if ( I <= debugLevel ) qDebug(S);\
}else{\
	if ( I <= debugLevel ) \
		qDebug("#%s:%i: %s \t\t(Level: %i)",__FILE__,__LINE__,QString(S).latin1(),I);\
}

#endif