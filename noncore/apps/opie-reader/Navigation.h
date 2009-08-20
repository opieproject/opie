#ifndef __NAVIGATION_H
#define __NAVIGATION_H

#include <string.h>
#include <stdlib.h>

const size_t NAVIGATION_HISTORY_SIZE = 32;

template<class T>
class CNavigation_base
{
protected:
    T history[NAVIGATION_HISTORY_SIZE];
    size_t historystart, historyend, historycurrent;
 public:
    CNavigation_base() : historystart(0),historyend(0),historycurrent(0) {}
    void saveposn(T posn);
    void writeposn(T posn);
    bool forward(T& loc);
    bool back(T& loc);
};

class CNavigation : public CNavigation_base<size_t>
{
 public:
    void setSaveData(unsigned char*& data, unsigned short& len, unsigned char* src, unsigned short srclen);
    void putSaveData(unsigned char*& src, unsigned short& srclen);
};

template<class T>
inline void CNavigation_base<T>::saveposn(T posn)
{
    history[historycurrent] = posn;
    historycurrent=(historycurrent+1)%NAVIGATION_HISTORY_SIZE;
    if (historycurrent==historystart)
      {
        historystart=(historystart+1)%NAVIGATION_HISTORY_SIZE;
      }
    historyend = historycurrent;
}

template<class T>
inline void CNavigation_base<T>::writeposn(T posn)
{
    history[historycurrent] = posn;
}

template<class T>
inline bool CNavigation_base<T>::back(T& posn)
{
  if (historycurrent!=historystart)
    {
      // buffer is not empty
      if (historycurrent==0)
	{
	  historycurrent=NAVIGATION_HISTORY_SIZE-1;
	}
      else
	{
	  historycurrent--;
	}
      posn=history[historycurrent];
      return true;
    }
  else
    {
      // circular buffer empty
      return false;
    }
}

template<class T>
inline bool CNavigation_base<T>::forward(T& posn)
{
  if (historycurrent!=historyend)
    {
      historycurrent=(historycurrent+1)%NAVIGATION_HISTORY_SIZE;
      posn = history[historycurrent];
      return true;
    }
  else
    {
      return false;
    }
}

#endif
