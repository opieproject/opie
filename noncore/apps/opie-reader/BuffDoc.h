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
    unsigned int locate() { return (exp == NULL) ? 0 : laststartline; }
    unsigned int explocate() { return (exp == NULL) ? 0 : exp->locate(); }
    MarkupType PreferredMarkup() { return (exp == NULL) ? cTEXT : exp->PreferredMarkup(); }
    bool hyperlink(unsigned int n);
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
};

#endif
