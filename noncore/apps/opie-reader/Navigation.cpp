#ifdef _WINDOWS
#include <string.h>
#endif
#include "Navigation.h"

void CNavigation::setSaveData(unsigned char*& data, unsigned short& len, unsigned char* src, unsigned short srclen)
{
    len = srclen+sizeof(size_t)*(3+NAVIGATION_HISTORY_SIZE);
    data = new unsigned char[len];
    unsigned char* p = data;
    memcpy(p, src, srclen);
    p += srclen;
    memcpy(p, &historystart, sizeof(size_t));
    p += sizeof(size_t);
    memcpy(p, &historyend, sizeof(size_t));
    p += sizeof(size_t);
    memcpy(p, &historycurrent, sizeof(size_t));
    p += sizeof(size_t);
    memcpy(p, history, sizeof(size_t)*NAVIGATION_HISTORY_SIZE);
}

void CNavigation::putSaveData(unsigned char*& src, unsigned short& srclen)
{
    if (srclen >= sizeof(size_t)*(3+NAVIGATION_HISTORY_SIZE))
    {
	unsigned char* p = src;
	memcpy(&historystart, p, sizeof(size_t));
	p += sizeof(size_t);
	memcpy(&historyend, p, sizeof(size_t));
	p += sizeof(size_t);
	memcpy(&historycurrent, p, sizeof(size_t));
	p += sizeof(size_t);
	memcpy(history, p, sizeof(size_t)*NAVIGATION_HISTORY_SIZE);
	src = p + sizeof(size_t)*NAVIGATION_HISTORY_SIZE;
	srclen -= sizeof(size_t)*(3+NAVIGATION_HISTORY_SIZE);
    }
/*
    printf("<%u,%u,%u>\n", historystart, historyend, historycurrent);
    for (int i = historystart; i <= historyend; i++)
	printf("<%u> ", history[i]);
    printf("\n");
*/
}
