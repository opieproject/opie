#ifndef __CExpander_h
#define __CExpander_h

#include "useqpe.h"
#include "config.h"
#include "StyleConsts.h"
#include "Markups.h"
#include "names.h"
#include "linktype.h"

/* OPIE */
#include <opie2/odebug.h>

/* QT */
#include <qmessagebox.h>

/* STD */
#ifndef _WINDOWS
#include <unistd.h>
#endif
#include <stdio.h>
#include <time.h>

class QImage;
class Bkmk;

template<class T>
class CList;

class CCharacterSource
{
 public:
    virtual void getch(tchar&, CStyle&) = 0;
    virtual linkType hyperlink(unsigned int n, QString&) = 0;
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
    int m_scrWidth;
    unsigned long m_currentstart, m_currentend;
 public:
#ifdef USEQPE
     virtual void suspend() = 0;
    virtual void unsuspend() = 0;
#endif
    size_t getHome() { return m_homepos; }
    CExpander() : m_homepos(0), fname(NULL), m_scrWidth(240), m_currentstart(1), m_currentend(0) {};
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
    virtual void getch(tchar& ch, CStyle& sty)
    {
        int ich = getch();
        ch = (ich == EOF) ? UEOF : ich;
        sty.unset();
    }
    virtual int getch() = 0;
    virtual linkType hyperlink(unsigned int n, QString& wrd)
    {
        locate(n);
        return eLink;
    }
    virtual MarkupType PreferredMarkup() = 0;
    virtual void saveposn(size_t posn) {}
    virtual void writeposn(size_t posn) {}
    virtual bool forward(size_t& loc) { return false; }
    virtual bool back(size_t& loc) { return false; }
    virtual bool hasnavigation() { return false; }
    unsigned long startSection()
    {
        unsigned long current = locate();
        if (m_currentstart > current || current > m_currentend)
        {
        start2endSection();
        }
        return m_currentstart;
    }
    unsigned long endSection()
    {
        unsigned long current = locate();
        if (m_currentstart > current || current > m_currentend)
        {
        start2endSection();
        }
        return m_currentend;
    }
    virtual void start2endSection()
    {
        m_currentstart = 0;
        unsigned long file;
        sizes(file, m_currentend);
    }
    virtual QImage* getPicture(unsigned long tgt) { return NULL; }
    void setContinuous(bool _b) { m_continuous = _b; }
#ifdef USEQPE
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
#endif
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
        odebug << "Don't know what to do with non-zero save data" << oendl;
        }
    }
    void setwidth(int w) { m_scrWidth = w; }
};
#endif
