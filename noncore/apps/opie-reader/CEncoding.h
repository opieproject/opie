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
    tchar getch();
};

class CUcs16be : public CEncoding
{
public:
    tchar getch();
};

class CUcs16le : public CEncoding
{
public:
    tchar getch();
};

class Ccp1252 : public CEncoding
{
public:
    virtual tchar getch();
};

class CPalm : public Ccp1252
{
public:
    tchar getch();
};

class CAscii : public CEncoding
{
public:
    tchar getch();
};

#endif
