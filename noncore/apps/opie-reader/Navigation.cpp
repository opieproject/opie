#include "Navigation.h"

#include <string.h>

void CNavigation::saveposn(size_t posn)
{
//    qDebug("Saved:%u [%u,%u,%u]", posn, historystart, historycurrent, historyend);
    historycurrent = historyend = (historycurrent+1)%NAVIGATION_HISTORY_SIZE;
    history[historycurrent] = posn;
    if (historystart == historyend) historystart = (historystart+1)%NAVIGATION_HISTORY_SIZE;
//    qDebug("Saved:%u [%u,%u,%u]", posn, historystart, historycurrent, historyend);
}

bool CNavigation::forward(size_t& loc)
{
    if (historycurrent != historyend)
    {
	historycurrent = (historycurrent + 1)%NAVIGATION_HISTORY_SIZE;
	loc = history[historycurrent];
//	qDebug("Forward:%u [%u,%u,%u]", loc, historystart, historycurrent, historyend);
	return true;
    }
    else
    {
	return false;
    }
}

bool CNavigation::back(size_t& loc)
{
    if (historyend != historystart)
    {
//	qDebug("Back:%u [%u,%u,%u]", loc, historystart, historycurrent, historyend);
	if (historycurrent == historyend && history[historycurrent] != loc)
	{
	    historyend = (historyend+1) % NAVIGATION_HISTORY_SIZE;
	    history[historyend] = loc;
	}
	else
	{
	    size_t sv = historycurrent;
	    historycurrent = (historycurrent + NAVIGATION_HISTORY_SIZE - 1) % NAVIGATION_HISTORY_SIZE;
	    if (historycurrent == historystart)
	    {
		historycurrent = sv;
		return false;
	    }
	}
	loc = history[historycurrent];
//	qDebug("Back:%u [%u,%u,%u]", loc, historystart, historycurrent, historyend);
	return true;
    }
    else
    {
	return false;
    }
}

#include <stdio.h>

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
    printf("<%u,%u,%u>\n", historystart, historyend, historycurrent);
    for (int i = historystart; i <= historyend; i++)
	printf("<%u> ", history[i]);
    printf("\n");
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
    printf("<%u,%u,%u>\n", historystart, historyend, historycurrent);
    for (int i = historystart; i <= historyend; i++)
	printf("<%u> ", history[i]);
    printf("\n");
}
