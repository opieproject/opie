#ifndef __CBUFFER_H
#define __CBUFFER_H

#include <stdlib.h>
#include <string.h>
#include "config.h"

class CBuffer
{
  size_t len;
  tchar *buffer;
  CBuffer(const CBuffer&);
 public:
  CBuffer& operator=(const tchar*sztmp);
#ifdef _UNICODE
  size_t length() { return ustrlen(buffer); }
#else
  size_t length() { return strlen(buffer); }
#endif
  tchar* data() { return buffer; }
  CBuffer(size_t n = 16) : len(n)
    {
      buffer = new tchar[len];
      buffer[0] = '\0';
    }
  ~CBuffer() { delete [] buffer; }
  tchar& operator[](int i);
};

class CSizeBuffer
{
  size_t len;
  size_t *buffer;
  CSizeBuffer(const CSizeBuffer&);
 public:
  size_t* data() { return buffer; }
  CSizeBuffer(size_t n = 16) : len(n)
    {
      buffer = new size_t[len];
    }
  ~CSizeBuffer() { delete [] buffer; }
  size_t& operator[](int i);
};

#endif
