#ifndef __BuffDoc_h
#define __BuffDoc_h

#include "ZText.h"
#include "Aportis.h"
#include "ztxt.h"
#include "ppm_expander.h"
#include "CDrawBuffer.h"
#include "CFilter.h"
#include <qfontmetrics.h>
#include <qmessagebox.h>

class BuffDoc
{
    CDrawBuffer lastword;
    CSizeBuffer lastsizes, allsizes;
    size_t laststartline;
    bool lastispara;
    CExpander* exp;
    CFilterChain* filt;
 public:
    void setSaveData(unsigned char*& data, unsigned short& len, unsigned char* src, unsigned short srclen)
	{
	    if (exp == NULL)
	    {
		data = NULL;
		len = 0;
	    }
	    else
	    {
		exp->setSaveData(data, len, src, srclen);
	    }
	}
    void putSaveData(unsigned char*& src, unsigned short& srclen)
	{
	    if (exp != NULL)
	    {
		exp->putSaveData(src, srclen);
	    }
	}
    void suspend() { if (exp != NULL) exp->suspend(); }
    void unsuspend() { if (exp != NULL) exp->unsuspend(); }
    ~BuffDoc()
	{
	    delete filt;
	    delete exp;
	}
    BuffDoc()
	{
	    exp = NULL;
	    filt = NULL;
	    lastword.empty();
	    //    qDebug("Buffdoc created");
	}
    bool empty() { return (exp == NULL); }
    void setfilter(CFilterChain* _f)
	{
	    if (filt != NULL) delete filt;
	    filt = _f;
	    filt->setsource(exp);
	}
    CList<Bkmk>* getbkmklist() { return exp->getbkmklist(); }
    bool hasrandomaccess() { return (exp == NULL) ? false : exp->hasrandomaccess(); }
    bool iseol() { return (lastword[0] == '\0'); }
    int openfile(QWidget* _parent, const char *src);
    tchar getch()
	{
	    tchar ch = UEOF;
	    CStyle sty;
	    if (exp != NULL)
	    {
		filt->getch(ch, sty);
	    }
	    return ch;
	}
    void getch(tchar& ch, CStyle& sty)
	{
	    if (exp != NULL)
	    {
		filt->getch(ch, sty);
	    }
	    else
		ch = UEOF;
	}
    QPixmap* getPicture(unsigned long tgt) { return (exp == NULL) ? NULL : exp->getPicture(tgt); }
    unsigned int startSection() { return (exp == NULL) ? 0 : exp->startSection(); }
    unsigned int endSection() { return (exp == NULL) ? 0 : exp->endSection(); }
    unsigned int locate() { return (exp == NULL) ? 0 : laststartline; }
    unsigned int explocate() { return (exp == NULL) ? 0 : exp->locate(); }
    void setContinuous(bool _b) { if (exp != NULL) exp->setContinuous(_b); }
    MarkupType PreferredMarkup() { return (exp == NULL) ? cTEXT : exp->PreferredMarkup(); }
    bool hyperlink(unsigned int n);
    size_t getHome() { return ((exp != NULL) ? exp->getHome() : 0); }
    void locate(unsigned int n);
    bool getline(CDrawBuffer* buff, int w);
    bool getline(CDrawBuffer* buff, int w, int cw);
    void sizes(unsigned long& fs, unsigned long& ts) { exp->sizes(fs,ts); }
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
    void saveposn(size_t posn) { exp->saveposn(posn); }
    bool forward(size_t& loc) { return exp->forward(loc); }
    bool back(size_t& loc) { return exp->back(loc); }
    bool hasnavigation() { return exp->hasnavigation(); }
};

#endif
