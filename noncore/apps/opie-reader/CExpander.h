#ifndef __CExpander_h
#define __CExpander_h

#ifndef _WINDOWS
#include <unistd.h>
#endif
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <qmessagebox.h>
#include "config.h"
#include "StyleConsts.h"
#include "Markups.h"
#include "names.h"
#include "linktype.h"

class QImage;
class Bkmk;
class QString;

template<class T>
class CList;

class CCharacterSource
{
 public:
    virtual void getch(tchar&, CStyle&, unsigned long&) = 0;
    virtual linkType hyperlink(unsigned int n, unsigned int noff, QString&, QString& nm) = 0;
    virtual void locate(unsigned int n) = 0;
    virtual bool findanchor(const QString& nm) = 0;
    virtual void saveposn(const QString& f, size_t posn) = 0;
    virtual void writeposn(const QString& f, size_t posn) = 0;
    virtual linkType forward(QString& f, size_t& loc) = 0;
    virtual linkType back(QString& f, size_t& loc) = 0;
    virtual bool hasnavigation() = 0;
    virtual int getwidth() = 0;
    virtual QImage* getPicture(unsigned long) = 0;
    virtual QImage* getPicture(const QString& href) = 0;
    virtual bool getFile(const QString& href) = 0;
    virtual QString about() = 0;
};

class CExpander_Interface
{
 public:
#ifdef USEQPE
	 virtual void suspend() = 0;
    virtual void unsuspend() = 0;
#endif
    virtual QString about() = 0;
    virtual size_t getHome() = 0;
    //CExpander_Interface();
    //virtual ~CExpander_Interface();
    virtual int openfile(const char *src) = 0;
    //virtual int OpenFile(const char *src) = 0;
    virtual unsigned int locate() = 0;
    virtual void locate(unsigned int n) = 0;
    virtual bool hasrandomaccess() = 0;
    virtual void sizes(unsigned long& file, unsigned long& text) = 0;
    virtual CList<Bkmk>* getbkmklist() { return NULL; }
    virtual void getch(tchar& ch, CStyle& sty, unsigned long& pos) = 0;
    virtual int getch() = 0;
    virtual linkType hyperlink(unsigned int n, unsigned int noff, QString& wrd, QString& nm) = 0;
    virtual MarkupType PreferredMarkup() = 0;
    virtual void saveposn(size_t posn) {}
    virtual void writeposn(size_t posn) {}
    virtual linkType forward(size_t& loc) { return eNone; }
    virtual linkType back(size_t& loc) { return eNone; }
    virtual bool hasnavigation() { return false; }
    virtual unsigned long startSection() = 0;
    virtual unsigned long endSection() = 0;
    virtual void start2endSection() = 0;
    virtual QImage* getPicture(unsigned long tgt) { return NULL; }
    virtual void setContinuous(bool _b) = 0;
#ifdef USEQPE
    virtual void suspend(FILE*& fin) = 0;
    virtual void unsuspend(FILE*& fin) = 0;
#endif
    virtual void setSaveData(unsigned char*& data, unsigned short& len, unsigned char* src, unsigned short srclen) = 0;
    virtual void putSaveData(unsigned char*& src, unsigned short& srclen) = 0;
    virtual void setwidth(int w) = 0;
    //    bool findstring(const QString& info);
    //    int getpara(CBuffer& buff);
    virtual int getwidth() = 0;
    virtual QImage* getPicture(const QString& href) = 0;
    virtual bool getFile(const QString& href) = 0;
};

class CExpander : public CExpander_Interface
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
    size_t getHome();
    CExpander();
    virtual ~CExpander();
    int openfile(const char *src);
    virtual int OpenFile(const char *src) = 0;
    virtual unsigned int locate() = 0;
    virtual void locate(unsigned int n) = 0;
    virtual bool hasrandomaccess() = 0;
    virtual void sizes(unsigned long& file, unsigned long& text) = 0;
    virtual CList<Bkmk>* getbkmklist() { return NULL; }
    virtual void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    virtual int getch() = 0;
    virtual linkType hyperlink(unsigned int n, unsigned int noff, QString& wrd, QString& nm);
    virtual MarkupType PreferredMarkup() = 0;
    virtual void saveposn(size_t posn) {}
    virtual void writeposn(size_t posn) {}
    virtual linkType forward(size_t& loc) { return eNone; }
    virtual linkType back(size_t& loc) { return eNone; }
    virtual bool hasnavigation() { return false; }
    unsigned long startSection();
    unsigned long endSection();
    virtual void start2endSection();
    virtual QImage* getPicture(unsigned long tgt) { return NULL; }
    void setContinuous(bool _b) { m_continuous = _b; }
#ifdef USEQPE
    virtual void suspend(FILE*& fin);
    virtual void unsuspend(FILE*& fin);
#endif
    virtual void setSaveData(unsigned char*& data, unsigned short& len, unsigned char* src, unsigned short srclen);
    virtual void putSaveData(unsigned char*& src, unsigned short& srclen);
    void setwidth(int w)
      {
	m_scrWidth = w;
      }
    int getwidth()
      {
	return m_scrWidth;
      }
    //    bool findstring(const QString& info);
    //    int getpara(CBuffer& buff);
    virtual QImage* getPicture(const QString& href) { return NULL; }
    virtual bool getFile(const QString& href) { return false; }
};
#endif
