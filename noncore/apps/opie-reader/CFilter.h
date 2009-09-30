#ifndef __CFILTER_H
#define __CFILTER_H

#include "CExpander.h"
#include "CEncoding.h"

class CFilter_IFace : public CCharacterSource
{
 public:
    virtual linkType hyperlink(unsigned int n, unsigned int noff, QString& w, QString& nm) = 0;
    virtual void setparent(CCharacterSource* p) = 0;
    virtual ~CFilter_IFace() {};
    virtual void locate(unsigned int n) = 0;
    virtual bool findanchor(const QString& nm) = 0;
    virtual void saveposn(const QString& f, size_t posn) = 0;
    virtual void writeposn(const QString& f, size_t posn) = 0;
    virtual linkType forward(QString& f, size_t& loc) = 0;
    virtual linkType back(QString& f, size_t& loc) = 0;
    virtual bool hasnavigation() = 0;
    virtual int getwidth() = 0;
    virtual CCharacterSource* getparent() = 0;
    virtual unsigned long startSection() = 0;
};

class CFilter : public CFilter_IFace
{
 protected:
    CCharacterSource* parent;
 public:
  virtual QString getTableAsHtml(unsigned long loc)
    {
      qDebug("CFilter::getTableAsHtml()");
      return parent->getTableAsHtml(loc);
    }
    virtual linkType hyperlink(unsigned int n, unsigned int noff, QString& w, QString& nm)
	{
	    return parent->hyperlink(n,noff,w,nm);
	}
    CFilter() : parent(0) {}
    void setparent(CCharacterSource* p) { parent = p; }
    CCharacterSource* getparent() { return parent; }
    virtual ~CFilter() {};
    virtual void locate(unsigned int n)
      {
	parent->locate(n);
      }
   virtual bool findanchor(const QString& nm)
     {
       return parent->findanchor(nm);
     }
    virtual void saveposn(const QString& f, size_t posn) { parent->saveposn(f, posn); }
    virtual void writeposn(const QString& f, size_t posn) { parent->writeposn(f, posn); }
    virtual linkType forward(QString& f, size_t& loc) { return parent->forward(f, loc); }
    virtual linkType back(QString& f, size_t& loc) { return parent->back(f, loc); }
    virtual bool hasnavigation() { return parent->hasnavigation(); }
    virtual int getwidth() { return parent->getwidth(); }
    QImage* getPicture(unsigned long tgt) { return parent->getPicture(tgt); }
    QImage* getPicture(const QString& href) { return parent->getPicture(href); }
    bool getFile(const QString& href, const QString& nm) { return parent->getFile(href, nm); }
    virtual unsigned long startSection() { return parent->startSection(); }
};

class CFilterChain
{
    CExpander_Interface* expander;
    CEncoding* encoder;
    CFilter_IFace* first;
    CCharacterSource* front;
 public:
    CFilterChain(CEncoding* _e) : expander(0), encoder(_e), first(0), front(_e) {};
    ~CFilterChain()
	{
	    CCharacterSource* p = front;
	    while (p != encoder)
	    {
		CFilter_IFace* pnext = (CFilter_IFace*)p;
		p = ((CFilter_IFace*)p)->getparent();
		delete pnext;
	    }
	    delete encoder;
	}
    linkType hyperlink(unsigned int n, unsigned int noff, QString& wrd, QString& nm)
      {
	return front->hyperlink(n, noff, wrd, nm);
      }
    QString getTableAsHtml(unsigned long loc)
      {
	return front->getTableAsHtml(loc);
      }
    void locate(unsigned int n)
      {
	front->locate(n);
      }
    void getch(tchar& ch, CStyle& sty, unsigned long& pos)
	{
	    front->getch(ch, sty, pos);
	}
    /*
    void rawgetch(tchar& ch, CStyle& sty, unsigned long& pos)
      {
	encoder->getch(ch, sty, pos);
      }
    */
    void addfilter(CFilter_IFace* p)
	{
	    if (!first)
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
    void setsource(CExpander_Interface* p)
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
    bool findanchor(const QString& nm)
      {
	return front->findanchor(nm);
      }
    void saveposn(const QString& f, size_t posn) { front->saveposn(f, posn); }
    void writeposn(const QString& f, size_t posn) { front->writeposn(f, posn); }
    linkType forward(QString& f, size_t& loc) { return front->forward(f, loc); }
    linkType back(QString& f, size_t& loc) { return front->back(f, loc); }
    bool hasnavigation() { return front->hasnavigation(); }
    QString about() { return QString("Filter chain (c) Tim Wentford\n")+front->about(); }
};

class stripcr : public CFilter
{
 public:
    stripcr() {}
    ~stripcr() {}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos)
	{
	    do
	    {
		parent->getch(ch, sty, pos);
	    }
	    while (ch == 13);
	}
    QString about() { return QString("StripCR filter (c) Tim Wentford\n")+parent->about(); }
};

class dehyphen : public CFilter
{
    bool m_bCharWaiting;
    tchar m_nextChar;
    CStyle m_nextSty;
 public:
    dehyphen() : m_bCharWaiting(false), m_nextChar(0) {}
    ~dehyphen() {}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos)
	{
	    if (m_bCharWaiting)
	    {
		m_bCharWaiting = false;
		ch = m_nextChar;
		sty = m_nextSty;
		return;
	    }
	    parent->getch(ch, sty, pos);
	    if (ch != '-') return;
	    parent->getch(m_nextChar, m_nextSty, pos);
	    if (m_nextChar != 10)
	    {
		m_bCharWaiting = true;
		ch = '-';
		return;
	    }
	    parent->getch(ch, sty, pos);
	}
    QString about() { return QString("Hyphenation filter (c) Tim Wentford\n")+parent->about(); }
};

template<class A, class B>class QMap;

const int m_cmaxdepth = 8;

class htmlmark
{
  QString file;
  size_t pos;
 public:
  htmlmark() : file(), pos(0) {}
  htmlmark(const QString& _f, size_t _p) : file(_f), pos(_p) {}
  QString filename() { return file; }
  size_t posn() { return pos; }
};

class unindent : public CFilter
{
    tchar lc;
 public:
    unindent() : lc(0) {}
    ~unindent() {}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos)
	{
	    if (lc == 10)
	    {
		do
		{
		    parent->getch(ch, sty, pos);
		}
		while (ch == ' ');
	    }
	    else parent->getch(ch, sty, pos);
	    lc = ch;
	    return;
	}
    QString about() { return QString("Unindent filter (c) Tim Wentford\n")+parent->about(); }
};

class CRegExpFilt;
class repara : public CFilter
{
    tchar tch;
    CRegExpFilt* flt;
 public:
    repara(const QString&);
    ~repara();
    void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    QString about() { return QString("Reparagraph filter (c) Tim Wentford\n")+parent->about(); }
};

class indenter : public CFilter
{
    int amnt;
    int indent;
    CStyle lsty;
 public:
    indenter(int _a=5) : amnt(_a), indent(0) {}
    ~indenter() {}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos)
	{
	    if (indent > 0)
	    {
		indent--;
		ch = ' ';
		sty = lsty;
		return;
	    }
	    parent->getch(ch, sty, pos);
	    if (ch == 10)
	    {
		indent = amnt;
		lsty = sty;
	    }
	    return;
	}
    QString about() { return QString("Indentation filter (c) Tim Wentford\n")+parent->about(); }
};

class dblspce : public CFilter
{
    bool lastlf;
    CStyle lsty;
 public:
    dblspce() : lastlf(false) {}
    ~dblspce() {}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos)
	{
	    if (lastlf)
	    {
		lastlf = false;
		ch = 10;
		sty = lsty;
		return;
	    }
	    parent->getch(ch, sty, pos);
	    if (lastlf = (ch == 10))
	    {
		lsty = sty;
	    }
	    return;
	}
    QString about() { return QString("Double space (c) Tim Wentford\n")+parent->about(); }
};

class textfmt : public CFilter
{
    CStyle currentstyle;
    tchar lastchar;
    bool uselast;
    void mygetch(tchar&, CStyle&, unsigned long& pos);
 public:
    textfmt() : lastchar(0), uselast(false) {}
    ~textfmt() {}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    QString about() { return QString("Text formatting filter (c) Tim Wentford\n")+parent->about(); }
};

class embolden : public CFilter
{
 public:
    embolden() {}
    ~embolden() {}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos)
	{
	    parent->getch(ch, sty, pos);
	    sty.setBold();
	}
    QString about() { return QString("Emboldening filter (c) Tim Wentford\n")+parent->about(); }
};

class remap : public CFilter
{
    tchar q[3];
    int offset;
    CStyle currentstyle;
 public:
    remap() : offset(0) { q[0] = 0; }
    ~remap() {}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    QString about() { return QString("Character remapping filter (c) Tim Wentford\n")+parent->about(); }
};

class PeanutFormatter : public CFilter
{
    CStyle currentstyle;
 public:
    ~PeanutFormatter() {}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    QString about() { return QString("PML filter (c) Tim Wentford\n")+parent->about(); }
};

class OnePara : public CFilter
{
    tchar m_lastchar;
 public:
    OnePara() : m_lastchar(0) {}
    ~OnePara() {}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    QString about() { return QString("Single space filter (c) Tim Wentford\n")+parent->about(); }
};

class DePluck : public CFilter
{
    tchar* nextpart;
    tchar m_buffer;
    int m_buffed;
    int m_current;
    bool m_debuff;
    CStyle m_laststyle;
 public:
    DePluck(tchar* t) : nextpart(t), m_buffer(0), m_buffed(0), m_current(0), m_debuff(false) {}
    ~DePluck() {}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    QString about() { return QString("Depluck filter (c) Tim Wentford\n")+parent->about(); }
};

class repalm : public CFilter
{
 public:
    ~repalm() {}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    QString about() { return QString("Repalm filter (c) Tim Wentford\n")+parent->about(); }
};

class FullJust : public CFilter
{
 public:
    void getch(tchar& ch, CStyle& sty, unsigned long& pos)
	{
	    parent->getch(ch, sty, pos);
	    if (sty.getJustify() == m_AlignLeft) sty.setFullJustify();
	}
    QString about() { return QString("Full justification filter (c) Tim Wentford\n")+parent->about(); }
};
/*
class AddSpace : public CFilter
{
    unsigned char m_espc;
 public:
    AddSpace(unsigned char s) : m_espc(s) {}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos)
	{
	    parent->getch(ch, sty, pos);
	    sty.setExtraSpace(m_espc);
	}
};
*/

class QTReader;

class HighlightFilter : public CFilter
{
  QTReader* pReader;
  unsigned long lastpos, nextpos;
  unsigned char red, green, blue;
  CList<Bkmk>* bkmks;
 public:
  HighlightFilter(QTReader*);
  void getch(tchar& ch, CStyle& sty, unsigned long& pos);
  void refresh(unsigned long);
  QString about() { return QString("High-lighting filter (c) Tim Wentford\n")+parent->about(); }
};

#ifndef __STATIC
#include <dlfcn.h>

class ExternFilter : public CFilter_IFace
{
  CFilter* filt;
  void *handle;
 public:
  linkType hyperlink(unsigned int n, unsigned int noff, QString& w, QString& nm)
    {
      return filt->hyperlink(n, noff, w, nm);
    }
  QString getTableAsHtml(unsigned long loc)
    {
      qDebug("ExternFilter::getTableAsHtml()");
      return filt->getTableAsHtml(loc);
    }
  void setparent(CCharacterSource* p) { filt->setparent(p); }
  ExternFilter(const QString& nm, const QString& optional);
  ~ExternFilter()
    {
      if (filt) delete filt;
      if (handle) dlclose(handle);
    }
  void locate(unsigned int n) { filt->locate(n); }
  bool findanchor(const QString& nm)
    {
      return filt->findanchor(nm);
    }
  void saveposn(const QString& f, size_t posn) { filt->saveposn(f, posn); }
  void writeposn(const QString& f, size_t posn) { filt->writeposn(f, posn); }
  linkType forward(QString& f, size_t& loc) { return filt->forward(f, loc); }
  linkType back(QString& f, size_t& loc) { return filt->back(f, loc); }
  bool hasnavigation() { return filt->hasnavigation(); }
  int getwidth() { return filt->getwidth(); }
  CCharacterSource* getparent() { return filt->getparent(); }
  void getch(tchar& c, CStyle& s, unsigned long& l) { filt->getch(c, s, l); }
  QImage* getPicture(unsigned long tgt) { return filt->getPicture(tgt); }
  CFilter* filter() { return filt; }
  QImage* getPicture(const QString& href) { return filt->getPicture(href); }
  bool getFile(const QString& href, const QString& nm) { return filt->getFile(href, nm); }
  QString about() { return QString("Filter plug-in (c) Tim Wentford\n")+filt->about(); }
  unsigned long startSection() { return filt->startSection(); }
};
#endif

class kern : public CFilter
{
    tchar lastchar;
    bool uselast;
    CStyle laststy;
 public:
    kern() : lastchar(0), uselast(false) {}
    ~kern() {}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    QString about() { return QString("Kerning filter (c) Tim Wentford\n")+parent->about(); }
};

class makeInverse : public CFilter
{
 public:
  void getch(tchar& ch, CStyle& sty, unsigned long& pos);
  QString about() { return QString("Colourmap inversion filter (c) Tim Wentford\n")+parent->about(); }
};
/*
class makeNegative : public CFilter
{
 public:
  void getch(tchar& ch, CStyle& sty, unsigned long& pos);
};
*/
class setbg : public CFilter
{
  int m_r, m_g, m_b;
 public:
  setbg(int _r, int _g, int _b) : m_r(_r), m_g(_g), m_b(_b) {}
  void getch(tchar& ch, CStyle& sty, unsigned long& pos);
  QString about() { return QString("Background colour filter (c) Tim Wentford\n")+parent->about(); }
};

class setfg : public CFilter
{
  int m_r, m_g, m_b;
 public:
  setfg(int _r, int _g, int _b) : m_r(_r), m_g(_g), m_b(_b) {}
  void getch(tchar& ch, CStyle& sty, unsigned long& pos);
  QString about() { return QString("Foreground colour filter (c) Tim Wentford\n")+parent->about(); }
};

class tableLink : public CFilter
{
  QString text;
  int offset;
  int m_r, m_g, m_b;
 public:
  tableLink() : text( "See Table" ), offset(-1)
    {
    }
  void getch(tchar& ch, CStyle& sty, unsigned long& pos);
  QString about() { return QString("Table link filter (c) Tim Wentford\n")+parent->about(); }
};

class underlineLink : public CFilter
{
  bool isLink;
 public:
    underlineLink() : isLink(false) {}
    ~underlineLink() {}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    QString about() { return QString("Link underlining filter (c) Tim Wentford\n")+parent->about(); }
};

#endif
