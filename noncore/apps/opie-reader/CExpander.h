#ifndef __CExpander_h
#define __CExpander_h

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <qmessagebox.h>
#include "config.h"
#include "StyleConsts.h"
#include "Markups.h"
#include "name.h"

class QPixmap;
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
 protected:
    size_t m_homepos;
    bool m_continuous;
    char* fname;
    bool bSuspended;
    size_t suspos;
    time_t sustime;
 public:
    virtual void suspend() = 0;
    virtual void unsuspend() = 0;
    size_t getHome() { return m_homepos; }
    CExpander() : m_homepos(0), fname(NULL) {};
    virtual ~CExpander() { if (fname != NULL) delete [] fname; };
    int openfile(const char *src)
	{
	    bSuspended = false;
	    fname = strdup(src);
	    return OpenFile(src);
	}
    virtual int OpenFile(const char *src) = 0;
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
    virtual void saveposn(size_t posn) {}
    virtual bool forward(size_t& loc) {}
    virtual bool back(size_t& loc) {}
    virtual bool hasnavigation() { return false; }
    virtual unsigned long startSection()
	{
	    return 0;
	}
    virtual unsigned long endSection()
	{
	    unsigned long file, text;
	    sizes(file, text);
	    return text;
	}
    virtual QPixmap* getPicture(unsigned long tgt) { return NULL; }
    void setContinuous(bool _b) { m_continuous = _b; }

    virtual void suspend(FILE*& fin)
      {
	  bSuspended = true;
	  suspos = ftell(fin);
	  fclose(fin);
	  fin = NULL;
	  sustime = time(NULL);
      }
    virtual void unsuspend(FILE*& fin)
      {
	  if (bSuspended)
	  {
	      bSuspended = false;
	      int delay = time(NULL) - sustime;
	      if (delay < 10) sleep(10-delay);
	      fin = fopen(fname, "rb");
	      for (int i = 0; fin == NULL && i < 5; i++)
	      {
		  sleep(5);
		  fin = fopen(fname, "rb");
	      }
	      if (fin == NULL)
	      {
		  QMessageBox::warning(NULL, PROGNAME, "Couldn't reopen file");
		  exit(0);
	      }
	      suspos = fseek(fin, suspos, SEEK_SET);
	  }
      }
    virtual void setSaveData(unsigned char*& data, unsigned short& len, unsigned char* src, unsigned short srclen)
	{
	    len = srclen;
	    data = new unsigned char[len];
	    memcpy(data, src, len);
	}
    virtual void putSaveData(unsigned char*& src, unsigned short& srclen)
	{
	    if (srclen != 0)
	    {
		qDebug("Don't know what to do with non-zero save data");
	    }
	}
};
#endif
