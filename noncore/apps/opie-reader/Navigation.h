#include <stdlib.h>

const size_t NAVIGATION_HISTORY_SIZE = 32;

class CNavigation
{
    size_t history[NAVIGATION_HISTORY_SIZE];
    size_t historystart, historyend, historycurrent;
 public:
    CNavigation() : historystart(0),historyend(0),historycurrent(0) {}
    void saveposn(size_t posn);
    bool forward(size_t& loc);
    bool back(size_t& loc);
    void setSaveData(unsigned char*& data, unsigned short& len, unsigned char* src, unsigned short srclen);
    void putSaveData(unsigned char*& src, unsigned short& srclen);
};
