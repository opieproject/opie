#ifndef __CSOURCE_H
#define __CSOURCE_H

class CInfo
{
 public:
  virtual unsigned long size() = 0;
};

class CSource : public CInfo
{
 public:
  virtual int get() = 0;
};

class CSink : public CInfo
{
 public:
  virtual void put(unsigned char c) = 0;
};

class CMemSource : public CSource
{
  unsigned long m_total_size;
  unsigned long m_current;
  unsigned char* m_buffer;
 public:
  CMemSource(unsigned char* _buffer, unsigned long _size)
    :
    m_total_size(_size),
    m_current(0),
    m_buffer(_buffer)
    {}
  unsigned long size() { return m_current; }
  int get()
    {
      return ((m_current < m_total_size) ? m_buffer[m_current++] : -1);
    }
};

class CMemSink : public CSink
{
  unsigned long m_total_size;
  unsigned long m_current;
  unsigned char* m_buffer;
 public:
  CMemSink(unsigned char* _buffer, unsigned long _size)
    :
    m_total_size(_size),
    m_current(0),
    m_buffer(_buffer)
    {}
  unsigned long size() { return m_current; }
  void put(unsigned char c)
    {
      if (m_current < m_total_size)
	{
	  m_buffer[m_current++] = c;
	} 
    }
};
#endif
