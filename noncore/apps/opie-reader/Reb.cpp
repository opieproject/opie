#include <stdio.h>
#include <string.h>
#include <qimage.h>
#include "decompress.h"
#include "Reb.h"
#include "my_list.h"
#include "Bkmks.h"
#include "Model.h"

CReb::CReb()
:
  fin(NULL), m_indexpages(NULL), m_pagedetails(NULL),tagoffset(0),
  tags(NULL), paras(NULL), noparas(0), joins(NULL), nojoins(0)
{
}

CReb::~CReb()
{
  if (fin != NULL) fclose(fin);
  if (m_indexpages != NULL) delete [] m_indexpages;
  if (m_pagedetails != NULL) delete [] m_pagedetails;
  if (tags != NULL) delete [] tags;
  if (paras != NULL) delete [] paras;
  if (joins != NULL) delete [] joins;
}

unsigned int CReb::locate()
{
  return m_pagedetails[currentpage.pageno()].pagestart+currentpage.offset();
}

void CReb::locate(unsigned int n)
{
  UInt32 jl = 0,jh = nopages-1,jm = (jl+jh)/2;
  while (jh > jl+1)
    {
      if (m_pagedetails[jm].pagestart > n)
	{
	  jh = jm;
	}
      else
	{
	  jl = jm;
	}
      jm = (jl+jh)/2;
    }

  unsuspend();
  Page_detail rs = m_pagedetails[jl];
  UInt32 val = n - rs.pagestart;
  if (jl != currentpage.pageno()) readindex(jl);
  currentpage.setoffset(page2pos(jl), jl, ((rs.flags & 8) != 0), rs.len, val);
  if (noparas > 0)
    {
      UInt32 jl = 0,jh = noparas-1,jm = (jl+jh)/2;
      while (jh > jl+1)
	{
	  if (paras[jm].pos > val)
	    {
	      jh = jm;
	    }
	  else
	    {
	      jl = jm;
	    }
	  jm = (jl+jh)/2;
	}

      qDebug("TAGS:%s", (const char*)tags[paras[jl].tag]);
      tagstring = tags[paras[jl].tag]+"<br>"; // Add br to set extra space to 0
      tagoffset = 0;
    }
  unsigned long current = locate();
  if (m_currentstart > current || current > m_currentend)
    {
      start2endSection();
    }
  if (current != n) qDebug("ERROR:Ended up at %u", current);
}

bool CReb::getFile(const QString& href, const QString& nm)
{
  qDebug("File:%s, Name:%s", (const char*)href, (const char*)nm);
  QMap<QString, UInt32>::Iterator iter = m_index.find(href);
  if (iter != m_index.end())
    {
      qDebug("REB:BEFORE:%u", locate());
      startpage(iter.data());
      qDebug("REB:AFTER:%u", locate());
      return true;
    }
  else
    {
      return false;
    }
}

QImage* CReb::getPicture(const QString& ref)
{
  QMap<QString, UInt32>::Iterator iter = m_index.find(ref);
  if (iter != m_index.end())
    {
      unsuspend();
      Page_detail rs = m_pagedetails[iter.data()];
      char* imgbuffer = new char[rs.len];
      fseek(fin, page2pos(iter.data()), SEEK_SET);
      fread(imgbuffer, rs.len, 1, fin);
      QByteArray arr;
      arr.assign((const char*)imgbuffer, rs.len);
      QImage* qimage = new QImage(arr);
      return qimage;
    }
  else
    {
      return NULL;
    }
}

CList<Bkmk>* CReb::getbkmklist() { return NULL; }

void CReb::home()
{
  startpage(m_homepage);
}

int CReb::OpenFile(const char *src)
{
  m_binary = false;
  if (fin != NULL) fclose(fin);
  fin = fopen(src, "r");
  if (fin == NULL)
    {
      return -1;
    }
  UInt32 type;
  fseek(fin, 6, SEEK_SET);
  fread(&type, 1, sizeof(type), fin);
  qDebug("CREB:Okay %x", type);

  if (type == 0x4f56554e || type == 0x574d4954 || type == 0x576d6954)
    {
      struct stat _stat;
      stat(src, &_stat);
      file_length = _stat.st_size;
      fread(&m_blocksize, 1, sizeof(m_blocksize), fin);
      if (type == 0x574d4954 || type == 0x576d6954)
	{
	  if (type == 0x576d6954) m_binary = true;
	  qDebug("Blocksize(1) %x", m_blocksize);
	  unsigned char ct = (m_blocksize >> 24) & 0xff;
	  qDebug("Compress type:%x", ct);
	  switch (ct)
	    {
	    case 0:
	      m_decompress = UnZip;
	      break;
	    case 3:
	      m_decompress = getdecompressor("PluckerDecompress3");
	      break;
	    case 4:
	      m_decompress = getdecompressor("PluckerDecompress4");
	      break;
	    }
	  if (m_decompress == NULL) return -1;
	  m_blocksize = 1024*(m_blocksize & 0xffffff);
	}
      else
	{
	  m_blocksize = 4096;
	  m_decompress = UnZip;
	}
      qDebug("Blocksize %u", m_blocksize);
      currentpage.init(fin, m_blocksize, m_decompress);
      qDebug("Its a REB!!!!");
      fseek(fin, 0x18, SEEK_SET);
      fread(&toc, 1, sizeof(toc), fin);
      qDebug("Expect this to be 128 or 20:%x", toc);
      fread(&type, 1, sizeof(type), fin);
      qDebug("File length:%u", type);
      fseek(fin, toc, SEEK_SET);
      fread(&nopages, 1, sizeof(nopages), fin);
      // sanitize nopages
      // We will commit an integer overflow if nopages is larger then a certain
      // amount
      if (nopages > (((UInt32)(-1)) / (sizeof(UInt32) > sizeof(Page_detail) ? sizeof(UInt32) : sizeof(Page_detail))))
	 nopages = (((UInt32)(-1)) / (sizeof(UInt32) > sizeof(Page_detail) ? sizeof(UInt32) : sizeof(Page_detail))) - 1;

      m_indexpages = new UInt32[nopages];
      m_pagedetails = new Page_detail[nopages];
      qDebug("There are %u pages", nopages);
      UInt32 loc = 0;
      UInt32 homeguess = nopages-1;
      QString homeurl;
      for (int i = 0; i < nopages; ++i)
	{
	  char name[32];
	  UInt32 len, pos, flags;
	  fread(name, 1, 32, fin);
	  fread(&len, 1, 4, fin);
	  fread(&pos, 1, 4, fin);
	  fread(&flags, 1, 4, fin);
	  //qDebug("Page %u (%s) is %u bytes at %u (%u) of type %u", i, name, len, pos, loc, flags);
	  m_index[name] = i;
	  m_pagedetails[i] = Page_detail(loc, len, flags);

	  if (QString(name).find(".htm", 0, false) >= 0)
	    {
	      if (homeguess > i) homeguess = i;
	      if ((flags & 8) != 0)
		{
		  UInt32 lastpos = ftell(fin);
		  loc += pagelength(i);
		  fseek(fin, lastpos, SEEK_SET);
		}
	      else
		{
		  loc += len;
		}
	    }
	  if ((flags & 2) != 0)
	    {
	      UInt32 lastpos = ftell(fin);
	      RBPage* idx = new RBPage();
	      idx->init(fin, m_blocksize, m_decompress);
	      idx->startpage(page2pos(i), i, ((flags & 8) != 0), len);
	      int c = 0;
	      while (c != EOF)
		{
		  QString s("");
		  while (1)
		    {
		      c = idx->getch(this);
		      if (c == 10 || c == EOF) break;
		      s += c;
		    }
		  if (s.left(5) == "BODY=")
		    {
		      homeurl = s.right(s.length()-5);
		      qDebug("Home:%s", (const char*)homeurl);
		    }
		  else
		    {
		      qDebug("Info:%s", (const char*)s);
		    }
		}
	      delete idx;
	      fseek(fin, lastpos, SEEK_SET);
	    }
	}
      text_length = loc;
      qDebug("Looking for homepage");
      if (homeurl.isEmpty())
	{
	  m_homepage = homeguess;
	}
      else
	{
	  QMap<QString, UInt32>::Iterator iter = m_index.find(homeurl);
	  if (iter != m_index.end())
	    {
	      m_homepage = iter.data();
	    }
	  else
	    {
	      m_homepage = homeguess;
	    }
	}
      m_homepos = m_pagedetails[m_homepage].pagestart;
      qDebug("Finding indices");
      for (QMap<QString, UInt32>::Iterator iter = m_index.begin(); iter != m_index.end(); ++iter)
	{
	  QString href = iter.key();
	  if (href.find(".htm", 0, false) >= 0)
	    {
	      QString hind = href.left(href.find(".htm", 0, false))+".hidx";
	      //qDebug("Index is %s", (const char*)hind);
	      QMap<QString, UInt32>::Iterator iter2 = m_index.find(hind);
	      if (iter2 != m_index.end())
		{
		  m_indexpages[iter.data()] = iter2.data();
		}
	    }
	}
      qDebug("Going home");
      home();
      return 0;
    }
  else
    {
      char * tmp = (char*)(&type);
      for (int i = 0; i < 4; ++i) qDebug("%d:%c", i, tmp[i]);
      return -1;
    }
}

UInt32 CReb::page2pos(UInt32 page)
{
  fseek(fin, toc+40+44*page, SEEK_SET);
  UInt32 pos;
  fread(&pos, 1, 4, fin);
  return pos;
}

UInt32 CReb::pagelength(UInt32 pagenum)
{
  fseek(fin, toc+40+44*pagenum, SEEK_SET);
  UInt32 pos;
  fread(&pos, 1, 4, fin);
  fseek(fin, pos+4, SEEK_SET);
  UInt32 len;
  fread(&len, 1, sizeof(len), fin);
  return len;
}

void CReb::readindex(UInt32 cp)
{
  if (joins != NULL)
    {
      delete [] joins;
      joins = NULL;
    }
  if (tags != NULL)
    {
      delete [] tags;
      tags = NULL;
    }
  if (paras != NULL)
    {
      delete [] paras;
      paras = NULL;
    }
  noparas = 0;
  nojoins = 0;
  names.clear();

  UInt32 rspage = m_indexpages[cp];
  if (rspage != 0)
    {
      Page_detail rs = m_pagedetails[rspage];
      int count = 0;
      RBPage* idx = new RBPage();
      idx->init(fin, m_blocksize, m_decompress);
      idx->startpage(page2pos(rspage), rspage, ((rs.flags & 8) != 0), rs.len);
      int c = 0;
      int phase = 0;
      int i;
      if (m_binary)
	{
	  count = idx->getuint(this);
	  qDebug("tag count:%d", count);
	  tags = new QString[count];
	  for (int i = 0; i < count; ++i)
	    {
	      QString s;
	      while (1)
		{
		  c = idx->getch(this);
		  if (c == 0 || c == EOF) break;
		  s += c;
		}
	      unsigned short val = idx->getuint(this);
	      if (val != 0xffff)
		{
		  tags[i] = tags[val]+s;
		}
	      else
		{
		  tags[i] = s;
		}
	      //qDebug("tags[%d](%d) = %s", i, val, (const char*)tags[i]);
	    }
	  noparas = idx->getint(this);
	  qDebug("Para count %d", noparas);
	  paras = new ParaRef[noparas];
	  for (int i = 0; i < noparas; ++i)
	    {
	      paras[i] = ParaRef(idx->getint(this), idx->getuint(this));
	    }
	  count = idx->getint(this);
	  qDebug("Name count %d", count);
	  for (int i = 0; i < count; ++i)
	    {
	      QString s;
	      while (1)
		{
		  c = idx->getch(this);
		  if (c == 0 || c == EOF) break;
		  s += c;
		}
	      int val = idx->getint(this);
	      names[s.mid(1,s.length()-2)] = val;
	      qDebug("names[%s] = %d", (const char*)s, val);
	    }
	  count = idx->getint(this);
	  qDebug("Join count %d", count);
	  if (count > 0)
	    {
	      nojoins = count+2;
	      joins = new UInt32[count+2];
	      joins[0] = 0;
	      joins[count+1] = currentpage.length();
	      for (int i = 1; i < count+1; ++i)
		{
		  joins[i] = idx->getint(this);
		}
	    }
	}
      else
	{
	  while (c != EOF)
	    {
	      QString s("");
	      while (1)
		{
		  c = idx->getch(this);
		  if (c == 10 || c == EOF) break;
		  s += c;
		}
	      //qDebug("%s", (const char*)s);
	      if (count > 0)
		{
		  --count;
		  int sp = s.findRev(' ');
		  QString l = s.left(sp);
		  int val = s.right(s.length()-sp).toInt();
		  switch (phase)
		    {
		    case 4:
		      //qDebug("Join %d is at offset %d", i, val);
		      joins[i++] = val;
		      break;
		    case 3:
		      //qDebug("Name %s is at offset %d", (const char*)l.mid(1,l.length()-2), val+m_pagedetails[cp].pagestart);
		      names[l.mid(1,l.length()-2)] = val;
		      break;
		    case 1:
		      //qDebug("%s:%d [%d]", (const char*)l, val, i);
		      if (val >= 0)
			{
			  tags[i++] = tags[val]+l;
			}
		      else
			{
			  tags[i++] = l;
			}
		      //qDebug("TAG:%s", (const char*)tags[i-1]);
		      break;
		    case 2:
		      paras[i++] = ParaRef(QString(l).toInt(), val);
		      //qDebug("Para:%u - %u (%s)", QString(l).toInt(), val, (const char*)s);
		      break;
		    default:
		      qDebug("%s:%d", (const char*)l, val);
		      break;
		    }
		}
	      else
		{
		  QString key = "[tags ";
		  if (s.left(key.length()) == key)
		    {
		      phase = 1;
		      i = 0;
		      count = s.mid(key.length(),s.length()-key.length()-1).toInt();
		      qDebug("%s:%s:%d", (const char*)key, (const char*)s, count);
		      tags = new QString[count];
		    }
		  key = "[paragraphs ";
		  if (s.left(key.length()) == key)
		    {
		      phase = 2;
		      i = 0;
		      count = s.mid(key.length(),s.length()-key.length()-1).toInt();
		      qDebug("%s:%s:%d", (const char*)key, (const char*)s, count);
		      paras = new ParaRef[count];
		      noparas = count;
		    }
		  key = "[names ";
		  if (s.left(key.length()) == key)
		    {
		      phase = 3;
		      count = s.mid(key.length(),s.length()-key.length()-1).toInt();
		      qDebug("%s:%s:%d", (const char*)key, (const char*)s, count);
		    }
		  key = "[joins ";
		  if (s.left(key.length()) == key)
		    {
		      phase = 4;
		      count = s.mid(key.length(),s.length()-key.length()-1).toInt();
		      qDebug("%s:%s:%d", (const char*)key, (const char*)s, count);
		      nojoins = count+2;
		      i = 1;
		      joins = new UInt32[count+2];
		      joins[0] = 0;
		      joins[count+1] = currentpage.length();
		      qDebug("%s:%s:%d", (const char*)key, (const char*)s, count);
		    }
		  qDebug("ZC:%s", (const char*)s);
		}
	    }
	}
      //      for (int i = 0; i < nojoins; ++i) qDebug("JOINS:%u %u", i, joins[i]);
      delete idx;
    }
}

bool CReb::findanchor(const QString& _info)
{
  QMap<QString, int>::Iterator iter = names.find(_info);
  if (iter != names.end())
    {
      locate(iter.data()+m_pagedetails[currentpage.pageno()].pagestart);
      return true;
    }
  return false;
}

#ifdef USEQPE
void CReb::suspend()
{
  CExpander::suspend(fin);
}
void CReb::unsuspend()
{
  CExpander::unsuspend(fin);
}
#endif

#ifndef __STATIC
extern "C"
{
  CExpander* newcodec() { return new CReb; }
}
#endif

void CReb::startpage(UInt32 pgno)
{
  Page_detail rs = m_pagedetails[pgno];
  unsuspend();
  readindex(pgno);
  currentpage.startpage(page2pos(pgno), pgno, ((rs.flags & 8) != 0), rs.len);
}

void CReb::startpage(UInt32 _cp, bool _isCompressed, UInt32 _len)
{
  unsuspend();
  readindex(_cp);
  currentpage.startpage(page2pos(_cp), _cp, _isCompressed, _len);
}

void RBPage::initpage(UInt32 pos, size_t _cp, bool _isCompressed, UInt32 _len)
{
  filepos = pos;
  m_pageno = _cp;
  m_Compressed = _isCompressed;
  m_pagelen = _len;
  currentchunk = 0;
  pageoffset = 0;

  if (chunklist != NULL) delete [] chunklist;

  fseek(fin, filepos, SEEK_SET);
  if (m_Compressed)
    {
      fread(&nochunks, 1, sizeof(nochunks), fin);
      if (nochunks > (UInt32)(-1) / sizeof(UInt32))
	  nochunks = ((UInt32)(-1) / sizeof(UInt32)) - 1;
      fread(&m_pagelen, 1, sizeof(m_pagelen), fin);
      chunklist = new UInt32[nochunks];
      fread(chunklist, nochunks, 4, fin);
    }
  else
    {
      chunklist = NULL;
      nochunks = (_len+m_blocksize-1)/m_blocksize;
    }
  m_startoff = 0;
  m_endoff = m_pagelen;
  chunkpos = ftell(fin);
  qDebug("Compressed:%u Expanded:%u", _len, m_pagelen);
}

void RBPage::startpage(UInt32 pos, UInt32 _cp, bool _isCompressed, UInt32 _len)
{
  initpage(pos, _cp, _isCompressed, _len);
  readchunk();
}

int CReb::getch()
{
  if (tagoffset < tagstring.length())
    return tagstring[tagoffset++].unicode();
  else
    return currentpage.getch(this);
}

int RBPage::getch(CReb* parent)
{
  if (chunkoffset >= chunklen)
    {
      if (++currentchunk >= nochunks)
	{
	  --currentchunk;
	  return EOF;
	}
      pageoffset += chunklen;
      parent->unsuspend();
      readchunk();
    }
  if (offset() == m_endoff) return EOF;
  return chunk[chunkoffset++];
}

unsigned short int RBPage::getuint(CReb* parent)
{
  unsigned short int ret = 0;
  char *buffer = (char*)(&ret);
  for (int i = 0; i < 2; ++i)
    {
      int ch = getch(parent);
      if (ch == EOF) return 0;
      buffer[i] = ch;
    }
  return ret;
}

int RBPage::getint(CReb* parent)
{
  int ret = 0;
  char *buffer = (char*)(&ret);
  for (int i = 0; i < 4; ++i)
    {
      int ch = getch(parent);
      if (ch == EOF) return 0;
      buffer[i] = ch;
    }
  return ret;
}

void RBPage::readchunk()
{
  if (m_Compressed)
    {
      chunkoffset = 0;
      fseek(fin, chunkpos, SEEK_SET);
      UInt8* inbuf = new UInt8[chunklist[currentchunk]];
      fread(inbuf, 1, chunklist[currentchunk], fin);
      chunklen = (*m_decompress)(inbuf, chunklist[currentchunk], chunk, m_blocksize);
      delete [] inbuf;
      chunkpos = ftell(fin);
    }
  else
    {
      chunkoffset = 0;
      chunklen = m_blocksize;
      if (m_blocksize*(currentchunk+1) > m_pagelen)
	{
	  chunklen = m_pagelen - currentchunk*m_blocksize;
	}
      fseek(fin, chunkpos, SEEK_SET);
      chunklen = fread(chunk, 1, chunklen, fin);
      chunkpos = ftell(fin);
    }
}

void RBPage::setoffset(UInt32 pos, size_t _cp, bool _isCompressed, UInt32 _len, UInt32 _offset)
{
  if (m_pageno != _cp)
    initpage(pos, _cp, _isCompressed, _len);
  else
    {
      if (m_Compressed)
	chunkpos = filepos + sizeof(nochunks) + sizeof(m_pagelen) + 4*nochunks;
      else
	chunkpos = filepos;
    }

  currentchunk = _offset/m_blocksize;
  pageoffset = m_blocksize*currentchunk;
  if (m_Compressed)
    {
      for (int i = 0; i < currentchunk; ++i)
	chunkpos += chunklist[i];
    }
  else
    chunkpos += pageoffset;

  readchunk();
  chunkoffset = _offset - pageoffset;
}

void CReb::start2endSection()
{
  if (m_pagedetails)
    {
      if (nojoins > 0)
	{
	  UInt32 jl = 0,jh = nojoins-1,jm = (jl+jh)/2;
	  while (jh > jl+1)
	    {
	      if (joins[jm] > currentpage.offset())
		jh = jm;
	      else
		jl = jm;

	      jm = (jl+jh)/2;
	    }

	  currentpage.m_startoff = joins[jl];
	  currentpage.m_endoff = joins[jl+1]-1;
	}
      m_currentstart = m_pagedetails[currentpage.pageno()].pagestart+currentpage.m_startoff;
      m_currentend = m_pagedetails[currentpage.pageno()].pagestart+currentpage.m_endoff;
    }
  else
    m_currentstart = m_currentend = 0;

  qDebug("s2e:[%u, %u, %u]", m_currentstart, m_pagedetails ? locate() : 0, m_currentend);
}
