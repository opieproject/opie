#ifdef _WINDOWS
#include <string.h>
#endif
#include "Navigation.h"
//#include <stdio.h>

/*
    void saveposn(size_t posn)
    save/push position to history buffer for future use of back() function
*/
void CNavigation::saveposn(size_t posn)
{
    //printf("saving position %u, depth %u\n",posn,historycurrent);
    history[historycurrent] = posn;
    historycurrent=(historycurrent+1)%NAVIGATION_HISTORY_SIZE;
    if (historycurrent==historystart)
        // circular buffer full, forget oldest record
        historystart=(historystart+1)%NAVIGATION_HISTORY_SIZE;
    // no forward possible after saveposn
    historyend = historycurrent;
}

/*
    void writeposn(size_t posn)
    overwrite current (unused) position
    useful for saving current position before using back button
*/
void CNavigation::writeposn(size_t posn)
{
    //printf("witing position %u, depth %u\n",posn,historycurrent);
    history[historycurrent] = posn;
}

/*
    bool back(size_t& posn)
    go back in history
    restore last position saved with saveposn() and return true
    return false if there is nothing saved in history
*/
bool CNavigation::back(size_t& posn)
{
    if (historycurrent!=historystart) {
        // buffer is not empty
        if (historycurrent==0)
            historycurrent=NAVIGATION_HISTORY_SIZE-1;
        else
            historycurrent--;
        posn=history[historycurrent];
        //printf("back(): going back to %u depth %u\n",posn,historycurrent);
        return true;

    } else {
        // circular buffer empty
        //printf("back(): empty history\n");
        return false;
    }
}

/*
    bool forward(size_t& posn)
    go forward in history, if possible
    undo calling of back()
*/
bool CNavigation::forward(size_t& posn)
{
    if (historycurrent!=historyend) {
        // [historycurrent] = current position
        // [historycurrent+1] = position we need
        historycurrent=(historycurrent+1)%NAVIGATION_HISTORY_SIZE;
        posn = history[historycurrent];
        //printf("forward(): going to position %d\n",posn);
        return true;
    } else {
        //printf("forward(): there is no future :)\n");
        return false;
    }
}

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
/*
    printf("<%u,%u,%u>\n", historystart, historyend, historycurrent);
    for (int i = historystart; i <= historyend; i++)
	printf("<%u> ", history[i]);
    printf("\n");
*/
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
