#ifndef __CENCODING_H
#define __CENCODING_H

#include "CExpander.h"

class CEncoding : public CCharacterSource
{
  friend class CFilterChain;
 protected:
  CExpander* parent;
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

#endif
