#ifndef __CFILTER_H
#define __CFILTER_H

#include "CExpander.h"
#include "CEncoding.h"

class CFilter : public CCharacterSource
{
  friend class CFilterChain;
 protected:
  CCharacterSource* parent;
public:
  CFilter() : parent(NULL) {}
  void setparent(CCharacterSource* p) { parent = p; }
  virtual ~CFilter() {};
};

class vanilla : public CFilter
{
public:
  vanilla() {}
  virtual ~vanilla() {}
#ifdef _UNICODE
  virtual tchar getch()
#else
  virtual int getch()
#endif
  {
    return parent->getch();
  }
};

class CFilterChain
{
    CExpander* expander;
    CEncoding* encoder;
    CFilter* first;
    CCharacterSource* front;
 public:
    CFilterChain(CEncoding* _e) : encoder(_e), first(NULL), front(_e) {};
    ~CFilterChain()
	{
	    CCharacterSource* p = front;
	    while (p != encoder)
	    {
		CFilter* pnext = (CFilter*)p;
		p = ((CFilter*)p)->parent;
		delete pnext;
	    }
	    delete encoder;
	}
    int getch() { return front->getch(); }
    void addfilter(CFilter* p)
	{
	    if (first == NULL)
	    {
		front = first = p;
		p->setparent(encoder);
	    }
	    else
	    {
		p->setparent(front);
		front = p;
	    }
	}
    void setsource(CExpander* p)
	{
	    expander = p;
	    encoder->setparent(p);
	}
    void setencoder(CEncoding* p)
	{
	    delete encoder;
	    encoder = p;
	    first->setparent(p);
	    encoder->setparent(expander);
	}
};

class stripcr : public CFilter
{
public:
  stripcr() {}
  virtual ~stripcr() {}
#ifdef _UNICODE
  virtual tchar getch()
  {
    tchar ch;
    do
      {
	ch = parent->getch();
      }
    while (ch == 13);
    return ch;
  }
#else
  virtual int getch()
  {
    int ch;
    do
      {
	ch = parent->getch();
      }
    while (ch == 13);
    return ch;
  }
#endif
};

class dehyphen : public CFilter
{
    bool m_bCharWaiting;
    tchar m_nextChar;
 public:
    dehyphen() : m_bCharWaiting(false) {}
    virtual ~dehyphen() {}
    virtual tchar getch()
	{
	    if (m_bCharWaiting)
	    {
		m_bCharWaiting = false;
		return m_nextChar;
	    }
	    tchar ch = parent->getch();
	    if (ch != '-') return ch;
	    m_nextChar = parent->getch();
	    if (m_nextChar != 10)
	    {
		m_bCharWaiting = true;
		return '-';
	    }
	    return parent->getch();
	}
};

class striphtml : public CFilter
{
public:
  striphtml() {}
  virtual ~striphtml() {}
#ifdef _UNICODE
  virtual tchar getch()
  {
    tchar ch;
    ch = parent->getch();
    while (ch == '<')
    {
	while (ch != '>')
	{
	    ch = parent->getch();
	}
	ch = parent->getch();
    }
    if (ch == '&')
    {
	ch = parent->getch();
	if (ch == '#')
	{
	    int id = 0;
	    while ((ch = parent->getch()) != ';') id = 10*id+ch-'0';
	    ch = id;
	}
    }
    return ch;
  }
#else
  virtual int getch()
  {
    int ch;
    ch = parent->getch();
    while (ch == '<')
    {
	while (ch != '>')
	{
	    ch = parent->getch();
	}
	ch = parent->getch();
    }
    if (ch == '&')
    {
	ch = parent->getch();
	if (ch == '#')
	{
	    int id = 0;
	    while ((ch = parent->getch()) != ';') id = 10*id+ch-'0';
	    ch = id;
	}
    }
    return ch;
  }
#endif
};

class unindent : public CFilter
{
  tchar lc;
public:
  unindent() : lc(0) {}
  virtual ~unindent() {}
#ifdef _UNICODE
  virtual tchar getch()
  {
    tchar ch;
    if (lc == 10)
      {
	while ((ch = parent->getch()) == ' ');
      }
    else ch = parent->getch();
    lc = ch;
    return ch;
  }
#else
  virtual int getch()
  {
    int ch;
    if (lc == 10)
      {
	while ((ch = parent->getch()) == ' ');
      }
    else ch = parent->getch();
    lc = ch;
    return ch;
  }
#endif
};

#ifdef _UNICODE
class repara : public CFilter
{
  tchar tch;
public:
  repara() : tch(0) {}
  virtual ~repara() {}
  virtual tchar getch()
  {
    tchar ch = parent->getch();
    if (ch == 10)
      {
	if (tch == 10)
	  {
	    return ch;
	  }
	else
	  {
	    tch = ch;
	    return ' ';
	  }
      }
    tch = ch;
    return ch;
  }
};
#else
class repara : public CFilter
{
  int tch;
public:
  repara() : tch(0) {}
  virtual ~repara() {}
  virtual int getch()
  {
    int ch = parent->getch();
    if (ch == 10)
      {
	if (tch == 10)
	  {
	    return ch;
	  }
	else
	  {
	    tch = ch;
	    return ' ';
	  }
      }
    tch = ch;
    return ch;
  }
};
#endif

class indenter : public CFilter
{
  int amnt;
  int indent;
public:
  indenter(int _a=5) : amnt(_a), indent(0) {}
  virtual ~indenter() {}
#ifdef _UNICODE
  virtual tchar getch()
    {
      if (indent > 0)
	{
	  indent--;
	  return ' ';
	}
      tchar ch = parent->getch();
      if (ch == 10)
	{
	  indent = amnt;
	}
      return ch;
    }
#else
  virtual int getch()
    {
      if (indent > 0)
	{
	  indent--;
	  return ' ';
	}
      int ch = parent->getch();
      if (ch == 10)
	{
	  indent = amnt;
	}
      return ch;
    }
#endif
};

class dblspce : public CFilter
{
  bool lastlf;
public:
  dblspce() : lastlf(false) {}
  virtual ~dblspce() {}
#ifdef _UNICODE
  virtual tchar getch()
    {
      if (lastlf)
	{
	  lastlf = false;
	  return 10;
	}
      tchar ch = parent->getch();
      lastlf = (ch == 10);
      return ch;
    }
#else
  virtual int getch()
    {
      if (lastlf)
	{
	  lastlf = false;
	  return 10;
	}
      int ch = parent->getch();
      lastlf = (ch == 10);
      return ch;
    }
#endif
};

#endif
