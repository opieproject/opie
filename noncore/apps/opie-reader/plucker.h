#ifndef __plucker_h
#define __plucker_h

#include "CExpander.h"
#include "zlib/zlib.h"
#include "ztxt.h"
#include "pdb.h"
#include "CBuffer.h"
#include "Navigation.h"
#include "my_list.h"

#ifdef LOCALPICTURES
class QScrollView;
class QWidget;
#endif

struct CPlucker_dataRecord
{
    UInt16 uid;
    UInt16 nParagraphs;
    UInt16 size;
    UInt8 type;
    UInt8 reserved;
};

struct CPlucker_record0
{
    UInt16 uid;
    UInt16 version;
    UInt16 nRecords;
};

struct CPluckerbkmk
{
    UInt32 offset;
    tchar title[MAX_BMRK_LENGTH];
};

const UInt32 CPLUCKER_ID = 0x5458547a;

class CPlucker : public CExpander, Cpdb
{
    unsigned short finduid(unsigned short);
    char* geturl(UInt16);
    void Expand(UInt16, UInt8, UInt8*, UInt16);
    CList<unsigned long> visited;
    bool m_lastIsBreak;
#ifdef LOCALPICTURES
    QScrollView* m_viewer;
    QWidget* m_picture;
#endif
    size_t textlength, m_lastBreak;
    UInt16 uid;
    UInt8 EOPPhase;
    int m_nextPara, m_nextParaIndex;
    CBufferFace<UInt16> m_ParaOffsets;
    CBufferFace<UInt16> m_ParaAttrs;
    UInt16 m_nParas;
    CStyle mystyle;
//    bool bInit;
    UInt32 buffersize;
    UInt32 buffercontent;
    UInt8* expandedtextbuffer;
    UInt8* compressedtextbuffer;
    char* urls;
    size_t urlsize;
    size_t bufferpos;
    UInt16 bufferrec;
    CPlucker_record0 hdr0;
    size_t currentpos;
    bool expand(int);
    void UnZip(size_t, UInt8*, UInt16);
    void UnDoc(size_t, UInt8*, UInt16);
#ifdef LOCALPICTURES
    void showimg(UInt16 tgt);
#endif
    QImage* getimg(UInt16 tgt);
    QPixmap* expandimg(UInt16 tgt, bool border=false);
    void home();
    int bgetch();
    CNavigation m_nav;
 public:
  virtual void suspend()
      {
	  CExpander::suspend(fin);
      }
  virtual void unsuspend()
      {
	  CExpander::unsuspend(fin);
      }
    virtual QPixmap* getPicture(unsigned long tgt);
    virtual void sizes(unsigned long& _file, unsigned long& _text);
    virtual bool hasrandomaccess() { return true; }
    virtual ~CPlucker();
    CPlucker();
    virtual int OpenFile(const char *src);
    virtual int getch();
    virtual void getch(int&, CStyle&);
    virtual unsigned int locate();
    virtual void locate(unsigned int n);
    virtual CList<Bkmk>* getbkmklist();
    virtual bool hyperlink(unsigned int n);
    virtual MarkupType PreferredMarkup()
	{
	    return cNONE;
	}
    void saveposn(size_t posn) { m_nav.saveposn(posn); }
    bool forward(size_t& loc) { return m_nav.forward(loc); }
    bool back(size_t& loc) { return m_nav.back(loc); }
    bool hasnavigation() { return true; }
    unsigned long startSection()
	{
	    return currentpos-bufferpos;
	}
    unsigned long endSection()
	{
	    return startSection()+buffercontent;
	}
    void setSaveData(unsigned char*& data, unsigned short& len, unsigned char* src, unsigned short srclen);
    void putSaveData(unsigned char*& src, unsigned short& srclen);
};

#endif
