#ifndef __REB_H
#define __REB_H
#include <stdio.h>
#include <zlib.h>
#include <sys/stat.h>
#ifdef USEQPE
#include <qpe/global.h>
#endif
#include <qmap.h>

#include "CExpander.h"
#include <zlib.h>

#ifdef _WINDOWS
#include <winsock.h>
#endif

#include "mytypes.h"

/*
struct Reb_Segment
{
  UInt32 len, page, flags;
  Reb_Segment(UInt32 _len = 0, UInt32 _page = 0, UInt32 _flags = 0)
    :
    len(_len), page(_page), flags(_flags)
  {}
};
*/

class CReb;

class RBPage
{
  UInt32 filepos;
  UInt32 pageoffset;
  UInt32 nochunks, currentchunk, chunkpos, chunklen, chunkoffset;
  UInt32 m_pagelen, m_blocksize;
  bool m_Compressed;
  void readchunk();
  UInt32* chunklist;
  UInt8* chunk;
  FILE* fin;
  UInt32 m_pageno;
  void initpage(UInt32 pos, size_t _cp, bool _isCompressed, UInt32 _len);
  size_t (*m_decompress)(UInt8*, size_t, UInt8*, size_t);
 public:
  RBPage() : pageoffset(0), nochunks(0), currentchunk(0), chunkpos(0), chunkoffset(0), m_pagelen(0), m_Compressed(false), chunklist(NULL), chunk(NULL) {}
  ~RBPage()
    {
      if (chunk != NULL) delete [] chunk;
      if (chunklist != NULL) delete [] chunklist;
    }
  int getch(CReb*);
  unsigned short int getuint(CReb*);
  int getint(CReb*);
  void startpage(UInt32, UInt32, bool, UInt32);
  UInt32 pageno() { return m_pageno; }
  UInt32 offset() { return pageoffset+chunkoffset; }
  void setoffset(UInt32, size_t, bool, UInt32, UInt32);
  void init(FILE* _f, UInt32 _bs, size_t (*_decompress)(UInt8*, size_t, UInt8*, size_t))
    {
      fin = _f;
      m_blocksize = _bs;
      chunk = new UInt8[m_blocksize];
      m_decompress = _decompress;
    }
  UInt32 length() { return m_pagelen; }
  UInt32 m_startoff, m_endoff;
};

struct Page_detail
{
  UInt32 pagestart, len, flags;
  Page_detail(UInt32 _ps = 0, UInt32 _l = 0, UInt32 _f = 0)
    :
    pagestart(_ps), len(_l), flags(_f)
  {
  }
};

struct ParaRef
{
  int pos, tag;
  ParaRef(int _pos = 0, int _tag = 0) : pos(_pos), tag(_tag)
  {
    //qDebug("New PARAREF:%d, %d", pos, tag);
  }
};

class CReb : public CExpander
{
  //friend class RBPage;
  size_t (*m_decompress)(UInt8*, size_t, UInt8*, size_t);
  RBPage currentpage;
  UInt32 nopages, m_homepage, m_blocksize;

  QMap<QString, UInt32> m_index;

  size_t file_length, text_length;
  UInt32 toc;
  FILE* fin;
  void home();
  void startpage(UInt32);
  void startpage(UInt32, bool, UInt32);
  void readchunk();
  QString tagstring;
  UInt32 tagoffset;
  UInt32 pagelength(UInt32);
  QImage* getPicture(const QString& ref);
  UInt32 page2pos(UInt32);
  //UInt32 m_pagelen;
  UInt32* m_indexpages;
  Page_detail* m_pagedetails;
  //  void UnZip(size_t bsize);
  QMap<QString, int> names;
  QString* tags;
  ParaRef* paras;
  UInt32* joins;
  UInt32 nojoins;
  UInt32 noparas;
  bool m_binary;
  void readindex(UInt32);
public:
  bool findanchor(const QString& _info);
  QString about() { return QString("REB codec (c) Tim Wentford"); }
  bool getFile(const QString& href, const QString& nm);
  void start2endSection();
  void sizes(unsigned long& _file, unsigned long& _text)
    {
      _file = file_length;
      _text = text_length;
    }
  bool hasrandomaccess() { return true; }
  virtual ~CReb();
  CReb();
  int OpenFile(const char *src);
  int getch();
  unsigned int locate();
  void locate(unsigned int n);
  CList<Bkmk>* getbkmklist();
  MarkupType PreferredMarkup()
    {
      return cCHM;
    }
#ifdef USEQPE
    void suspend();
    void unsuspend();
#else
    void suspend() {}
    void unsuspend() {}
#endif
};

#endif


