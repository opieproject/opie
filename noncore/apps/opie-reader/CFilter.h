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
    void getch(tchar& ch, CStyle& sty)
	{
	    front->getch(ch, sty);
	}
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
    virtual void getch(tchar& ch, CStyle& sty)
	{
	    do
	    {
		parent->getch(ch, sty);
	    }
	    while (ch == 13);
	}
};

class dehyphen : public CFilter
{
    bool m_bCharWaiting;
    tchar m_nextChar;
    CStyle m_nextSty;
 public:
    dehyphen() : m_bCharWaiting(false) {}
    virtual ~dehyphen() {}
    virtual void getch(tchar& ch, CStyle& sty)
	{
	    if (m_bCharWaiting)
	    {
		m_bCharWaiting = false;
		ch = m_nextChar;
		sty = m_nextSty;
		return;
	    }
	    parent->getch(ch, sty);
	    if (ch != '-') return;
	    parent->getch(m_nextChar, m_nextSty);
	    if (m_nextChar != 10)
	    {
		m_bCharWaiting = true;
		ch = '-';
		return;
	    }
	    parent->getch(ch, sty);
	}
};

class striphtml : public CFilter
{
    CStyle currentstyle;
    unsigned short skip_ws();
    unsigned short skip_ws_end();
    unsigned short parse_m();
    void mygetch(tchar& ch, CStyle& sty);
 public:
    striphtml() {}
    virtual ~striphtml() {}
    virtual void getch(tchar& ch, CStyle& sty);
};

class unindent : public CFilter
{
    tchar lc;
 public:
    unindent() : lc(0) {}
    virtual ~unindent() {}
    virtual void getch(tchar& ch, CStyle& sty)
	{
	    if (lc == 10)
	    {
		do
		{
		    parent->getch(ch, sty);
		}
		while (ch == ' ');
	    }
	    else parent->getch(ch, sty);
	    lc = ch;
	    return;
	}
};

class repara : public CFilter
{
    tchar tch;
 public:
    repara() : tch(0) {}
    virtual ~repara() {}
    virtual void getch(tchar& ch, CStyle& sty)
	{
	    parent->getch(ch, sty);
	    if (ch == 10)
	    {
		if (tch == 10)
		{
		    return;
		}
		else
		{
		    tch = ch;
		    ch = ' ';
		    return;
		}
	    }
	    tch = ch;
	    return;
	}
};

class indenter : public CFilter
{
    int amnt;
    int indent;
    CStyle lsty;
 public:
    indenter(int _a=5) : amnt(_a), indent(0) {}
    virtual ~indenter() {}
    virtual void getch(tchar& ch, CStyle& sty)
	{
	    if (indent > 0)
	    {
		indent--;
		ch = ' ';
		sty = lsty;
		return;
	    }
	    parent->getch(ch, sty);
	    if (ch == 10)
	    {
		indent = amnt;
		lsty = sty;
	    }
	    return;
	}
};

class dblspce : public CFilter
{
    bool lastlf;
    CStyle lsty;
 public:
    dblspce() : lastlf(false) {}
    virtual ~dblspce() {}
    virtual void getch(tchar& ch, CStyle& sty)
	{
	    if (lastlf)
	    {
		lastlf = false;
		ch = 10;
		sty = lsty;
		return;
	    }
	    parent->getch(ch, sty);
	    if (lastlf = (ch == 10))
	    {
		lsty = sty;
	    }
	    return;
	}
};

class textfmt : public CFilter
{
    CStyle currentstyle;
    tchar lastchar;
    bool uselast;
    void mygetch(tchar&, CStyle&);
 public:
    textfmt() : lastchar(0), uselast(false) {}
    virtual ~textfmt() {}
    virtual void getch(tchar& ch, CStyle& sty);
};

class embolden : public CFilter
{
 public:
    embolden() {}
    virtual ~embolden() {}
    virtual void getch(tchar& ch, CStyle& sty)
	{
	    parent->getch(ch, sty);
	    sty.setBold();
	}
};

class remap : public CFilter
{
    tchar q[3];
    int offset;
    CStyle currentstyle;
 public:
    remap() : offset(0) { q[0] = 0; }
    virtual ~remap() {}
    virtual void getch(tchar& ch, CStyle& sty);
};

class PeanutFormatter : public CFilter
{
    CStyle currentstyle;
 public:
    virtual ~PeanutFormatter() {}
    virtual void getch(tchar& ch, CStyle& sty);
};
#endif
