#ifndef __BuffDoc_h
#define __BuffDoc_h

#include "CBuffer.h"
#include "ZText.h"
#include "Aportis.h"
#include "ztxt.h"
#include "ppm_expander.h"
#include "CFilter.h"
#include <qfontmetrics.h>
#include <qmessagebox.h>

class BuffDoc
{
    CBuffer lastword;
    CSizeBuffer lastsizes, allsizes;
    size_t laststartline;
    bool lastispara;
    CExpander* exp;
    CFilterChain* filt;
    QFontMetrics* m_fm;
 public:
    ~BuffDoc()
	{
	    delete filt;
	    delete exp;
	}
    BuffDoc() : m_fm(NULL)
	{
	    exp = NULL;
	    filt = NULL;
	    //    qDebug("Buffdoc created");
	}
    bool empty() { return (exp == NULL); }
    void setfm(QFontMetrics* fm)
	{
	    m_fm = fm;
	    //    qDebug("Buffdoc:setfm");
	}
    void setfilter(CFilterChain* _f)
	{
	    if (filt != NULL) delete filt;
	    filt = _f;
	    filt->setsource(exp);
	}
    CList<Bkmk>* getbkmklist() { return exp->getbkmklist(); }
    bool hasrandomaccess() { return (exp == NULL) ? false : exp->hasrandomaccess(); }
    bool iseol() { return (lastword[0] == '\0'); }
    int openfile(QWidget* _parent, const char *src)
	{
	    //    qDebug("BuffDoc:Openfile:%s", src);
	    //    qDebug("Trying aportis %x",exp);
	    if (exp != NULL) delete exp;
	    lastword[0] = '\0';
	    lastsizes[0] = laststartline = 0;
	    lastispara = false;
	    /*
	      exp = new Text;
	      int ret = exp->openfile(src);
	    */

	    exp = new Aportis;
	    //    qDebug("Calling openfile");
	    int ret = exp->openfile(src);
	    //    qDebug("Called openfile");
	    if (ret == -1)
	    {
		//		qDebug("problem opening source file:%s",src);
		delete exp;
		exp = NULL;
		return ret;
	    }
	    if (ret == -2)
	    {

		delete exp;
//		qDebug("Trying ztxt");
		exp = new ztxt;
		ret = exp->openfile(src);
	    }
#ifndef SMALL
	    if (ret != 0)
	    {
		delete exp;
//		qDebug("Trying ppms");
		exp = new ppm_expander;
		ret = exp->openfile(src);
	    }
	    
	    if (ret != 0)
	    {
		delete exp;
		exp = new Text;
//		qDebug("Trying text");
		ret = exp->openfile(src);
	    }
#else
	    if (ret != 0)
	    {
		delete exp;
	        exp = new Text;
		ret = exp->openfile(src);
	    }
#endif
	    if (ret != 0)
	    {
		delete exp;
		QMessageBox::information(_parent, "QTReader", "Unknown file compression type","Try another file");
		return ret;
	    }
	    //        qDebug("Doing final open:%x:%x",exp,filt);

	    lastword[0] = '\0';
	    lastsizes[0] = laststartline = 0;
	    lastispara = false;
	    exp->locate(0);
	    filt->setsource(exp);
	    //        qDebug("BuffDoc:file opened");
	    return 0;
	}
    int getch() { return (exp == NULL) ? UEOF : filt->getch(); }
    unsigned int locate() { return (exp == NULL) ? 0 : laststartline; }
    void locate(unsigned int n);
    bool getline(CBuffer* buff, int w);
    bool getline(CBuffer* buff, int w, int cw);
    void sizes(unsigned long& fs, unsigned long& ts) { exp->sizes(fs,ts); }
    int getpara(CBuffer& buff)
	{
	    int ch, i = 0;
	    while ((ch = getch()) != 10 && ch != UEOF) buff[i++] = ch;
	    buff[i] = '\0';
	    if (i == 0 && ch == UEOF) i = -1;
	    laststartline = exp->locate();
	    return i;
	}
};

#endif
