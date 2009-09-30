#ifndef __BuffDoc_h
#define __BuffDoc_h

#include "CDrawBuffer.h"
#include "CFilter.h"
#include "CExpander.h"
#include <qfontmetrics.h>
#include <qmessagebox.h>

class BuffDoc
{
    CDrawBuffer lastword;
    CSizeBuffer lastsizes, allsizes;
    size_t laststartline;
    bool lastispara;
    CExpander_Interface* exp;
    CFilterChain* filt;
    int m_hyphenthreshold;
 public:
    QString about();
    bool findanchor(const QString& nm) { return filt->findanchor(nm); }
    void setHyphenThreshold(int _v) { m_hyphenthreshold = _v; }
    void setSaveData(unsigned char*& data, unsigned short& len, unsigned char* src, unsigned short srclen)
	{
	    if (!exp)
	    {
		data = 0;
		len = 0;
	    }
	    else
		exp->setSaveData(data, len, src, srclen);
	}
    void putSaveData(unsigned char*& src, unsigned short& srclen)
	{ if (exp) exp->putSaveData(src, srclen); }
#ifdef USEQPE
    void suspend() { if (exp) exp->suspend(); }
#else
    void suspend() {}
#endif
    ~BuffDoc()
	{
	    delete filt;
	    delete exp;
	}
    BuffDoc() : laststartline(0), lastispara(false), exp(0), filt(0) { lastword.empty(); }
    bool empty() { return !exp; }
    void setfilter(CFilterChain* _f)
	{
	    if (filt) delete filt;

	    filt = _f;
	    filt->setsource(exp);
	}
    CList<Bkmk>* getbkmklist() { return exp->getbkmklist(); }
    bool hasrandomaccess() { return !exp ? false : exp->hasrandomaccess(); }
    bool iseol() { return (lastword[0] == '\0'); }
    int openfile(QWidget* _parent, const char *src);
    tchar getch()
	{
	    tchar ch = UEOF;
	    CStyle sty;
	    if (exp)
	    {
                unsigned long dummy;
		filt->getch(ch, sty, dummy);
	    }
	    return ch;
	}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos)
	{
	    if (exp)
		filt->getch(ch, sty, pos);
	    else
		ch = UEOF;
	}
    void setwidth(int w) { if (exp) exp->setwidth(w); }
    QImage* getPicture(unsigned long tgt) { return !exp ? 0 : exp->getPicture(tgt); }
    unsigned int startSection() { return !exp ? 0 : exp->startSection(); }
    unsigned int endSection() { return !exp ? 0 : exp->endSection(); }
    void resetPos();
    unsigned int locate() { return !exp ? 0 : laststartline; }
    unsigned int explocate() { return !exp ? 0 : exp->locate(); }
    void setContinuous(bool _b) { if (exp) exp->setContinuous(_b); }
    MarkupType PreferredMarkup() { return !exp ? cTEXT : exp->PreferredMarkup(); }
    linkType hyperlink(unsigned int n, unsigned int noff, QString& wrd, QString& nm);
    size_t getHome() { return (exp ? exp->getHome() : 0); }
    void locate(unsigned int n);
    bool getline(CDrawBuffer* buff, int w, unsigned short _lborder, unsigned short _rborder, bool hyphenate, int availht);
    bool getline(CDrawBuffer* buff, int w, int cw, unsigned short _lborder, unsigned short _rborder, int availht);
    void sizes(unsigned long& fs, unsigned long& ts)
      {
	if (exp)
	    exp->sizes(fs,ts);
	else
	    fs = ts = 0;
      }
    int getpara(CBuffer& buff)
	{
	    tchar ch;
	    int i = 0;
	    while ((ch = getch()) != 10 && ch != UEOF) buff[i++] = ch;

	    buff[i] = '\0';
	    if (i == 0 && ch == UEOF) i = -1;

	    laststartline = exp->locate();
	    return i;
	}
    int getsentence(CBuffer& buff);
    void saveposn(const QString& f, size_t posn) { filt->saveposn(f, posn); }
    void writeposn(const QString& f, size_t posn) { filt->writeposn(f, posn); }
    linkType forward(QString& f, size_t& loc) { return filt->forward(f, loc); }
    linkType back(QString& f, size_t& loc) { return filt->back(f, loc); }
    bool hasnavigation() { return !exp ? false : filt->hasnavigation(); }
    bool getFile(const QString& href, QString& nm)
      {
	return !exp ? false : exp->getFile(href, nm);
      }
    QString getTableAsHtml(unsigned long loc)
      {
	return !exp ? filt->getTableAsHtml(loc) : QString("");
      }
};

#endif
