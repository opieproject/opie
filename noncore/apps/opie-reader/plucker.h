#ifndef __plucker_h
#define __plucker_h

#include "CExpander.h"
#include <zlib.h>
#include "ztxt.h"
#include "pdb.h"
#include "CBuffer.h"

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
    size_t textlength;
    UInt16 uid;
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
    size_t bufferpos;
    UInt16 bufferrec;
    CPlucker_record0 hdr0;
    size_t currentpos;
    bool expand(int);
    void UnZip(size_t, UInt8*, UInt16);
    void UnDoc(size_t, UInt8*, UInt16);
    void expandimg(UInt16 tgt);
    void home();
    int bgetch();
 public:
    virtual void sizes(unsigned long& _file, unsigned long& _text)
	{
	    _file = file_length;
	    _text = textlength;
//ntohl(hdr0.size);
	}
    virtual bool hasrandomaccess() { return true; }
    virtual ~CPlucker()
	{
	    if (expandedtextbuffer != NULL) delete [] expandedtextbuffer;
	    if (compressedtextbuffer != NULL) delete [] compressedtextbuffer;
	}
    CPlucker();
    virtual int openfile(const char *src);
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
};

#endif
