#ifndef __CExpander_h
#define __CExpander_h

#include "config.h"
#include "StyleConsts.h"
#include "Markups.h"

class Bkmk;

template<class T>
class CList;

class CCharacterSource
{
 public:
    virtual void getch(tchar&, CStyle&) = 0;
};

class CExpander
{
 public:
    CExpander() {};
    virtual ~CExpander() {};
    virtual int openfile(const char *src) = 0;
    virtual unsigned int locate() = 0;
    virtual void locate(unsigned int n) = 0;
    virtual bool hasrandomaccess() = 0;
    virtual void sizes(unsigned long& file, unsigned long& text) = 0;
    virtual CList<Bkmk>* getbkmklist() { return NULL; }
    virtual void getch(int& ch, CStyle& sty)
	{
	    ch = getch();
	    sty.unset();
	}
    virtual int getch() = 0;
    virtual bool hyperlink(unsigned int n)
	{
	    locate(n);
	    return true;
	}
    virtual MarkupType PreferredMarkup() = 0;
};
#endif
