#ifndef __CBUFFER_H
#define __CBUFFER_H

#include <stdlib.h>
#include <string.h>
#include "config.h"


class CBufferBase
{
 protected:
  size_t len;
  unsigned char *buffer;
  size_t membersize;
  CBufferBase(const CBufferBase&);
 public:
  CBufferBase& assign(const void* sztmp, size_t ms);
  void* data() { return buffer; }
  CBufferBase(size_t ms, size_t n = 16);
  ~CBufferBase() { delete [] buffer; }
  void* operator[](int i);
  size_t bstrlen(unsigned char* _buffer = NULL);
  size_t totallen() { return len; }
};

template<class T>
class CBufferFace
{
    CBufferBase m_buffer;
 protected:
  CBufferFace(const CBufferFace&);
 public:
  CBufferFace& operator=(const T* sztmp)
      {
	  m_buffer.assign(sztmp, m_buffer.bstrlen(sztmp));
	  return *this;
      }
  void assign(const T* sztmp, size_t n)
      {
	  m_buffer.assign(sztmp, n);
      }
  size_t length() { return m_buffer.bstrlen(); }
  size_t totallen() { return m_buffer.totallen(); }
  T* data() { return (T*)m_buffer.data(); }
  CBufferFace(size_t n = 16) : m_buffer(sizeof(T), n) {}
  T& operator[](int i)
      {
	  return *((T*)m_buffer[i]);
      }
};

typedef CBufferFace<tchar> CBuffer;
typedef CBufferFace<size_t> CSizeBuffer;

#endif
