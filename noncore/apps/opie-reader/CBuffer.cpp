#include "CBuffer.h"

CBuffer& CBuffer::operator=(const tchar*sztmp)
{
  int i;
  for (i = 0; sztmp[i] != '\0'; i++) (*this)[i] = sztmp[i];
  (*this)[i] = '\0';
  return *this;
}

tchar& CBuffer::operator[](int i)
{
  if (i >= len)
    {
      tchar *newbuff = new tchar[i+1];
      memcpy(newbuff,buffer,sizeof(tchar)*len);
      delete [] buffer;
      buffer = newbuff;
      len = i+1;
    }
  return buffer[i];
}

size_t& CSizeBuffer::operator[](int i)
{
  if (i >= len)
    {
      size_t *newbuff = new size_t[i+1];
      memcpy(newbuff,buffer,sizeof(size_t)*len);
      delete [] buffer;
      buffer = newbuff;
      len = i+1;
    }
  return buffer[i];
}
