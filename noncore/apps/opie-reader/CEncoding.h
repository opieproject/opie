#ifndef __CENCODING_H
#define __CENCODING_H

#include "CExpander.h"

#define MAX_ENCODING 6

class CEncoding : public CCharacterSource
{
  friend class CFilterChain;
 protected:
  CExpander* parent;
  linkType hyperlink(unsigned int n, QString& t) { return parent->hyperlink(n,t); }
public:
  CEncoding() : parent(NULL) {}
  void setparent(CExpander* p) { parent = p; }
  virtual ~CEncoding() {};
};

class CUtf8 : public CEncoding
{
public:
    void getch(tchar& ch, CStyle& sty);
};

class CUcs16be : public CEncoding
{
public:
    void getch(tchar& ch, CStyle& sty);
};

class CUcs16le : public CEncoding
{
public:
    void getch(tchar& ch, CStyle& sty);
};

class Ccp1252 : public CEncoding
{
public:
    void getch(tchar& ch, CStyle& sty);
};

class CPalm : public Ccp1252
{
public:
    void getch(tchar& ch, CStyle& sty);
};

class CAscii : public CEncoding
{
public:
    void getch(tchar& ch, CStyle& sty);
};

#include "CEncoding_tables.h"

class CGeneral8Bit : public CEncoding
{
    int m_index;
 public:
    CGeneral8Bit(int _i) : m_index(_i)
    {
//      odebug << "8Bit: " << _i << oendl;
//      odebug << unicodetable::iterator(_i)->mime << oendl;
    }
    void getch(tchar& ch, CStyle& sty)
    {
        parent->getch(ch, sty);
        ch = unicodetable::unicodevalue(m_index, ch);
    }
};

#endif
