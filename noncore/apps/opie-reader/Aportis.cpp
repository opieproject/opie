#include <stdio.h>
#include <string.h>
#include "Aportis.h"
#include "my_list.h"
#include "Bkmks.h"

Aportis::Aportis()
    : peanutfile(false)
    , html(false)
    , dwLen(0)
    , nRecs2(0)
    , dwTLen(0)
    , nRecs(0)
    , BlockSize(0)
    , dwRecLen(0)
    , mobiimagerec(0)
    , currentrec(0)
    , currentpos(0)
    , cbptr(0)
    , outptr(0)
    , bCompressed(0)
{
    memset(circbuf, 0, sizeof(circbuf));
}

void Aportis::dePeanut(int& ch)
{
    if (peanutfile && ch != EOF)
    {
	unsigned char c = ch;
	if (peanutfile) c ^= 0xa5;
	ch = c;
    }
}

CList<Bkmk>* Aportis::getbkmklist()
{
  if (bCompressed != 4)
    return 0;
  unsuspend();
  long cur = ftell(fin);
  if (cur == -1)
    return 0;

  CList<Bkmk>* t = new CList<Bkmk>;
  for (int i = 0; i < nRecs2; i++)
  {
    DWORD dwPos;
    fseek(fin, 0x56 + 8*i, SEEK_SET);
    fread(&dwPos, 4, 1, fin);
    dwPos = SwapLong(dwPos);
    fseek(fin,dwPos,SEEK_SET);
    unsigned char ch;
    fread(&ch,1,1,fin);
    if (ch != 241)
    {
      char name[17];
      name[16] = '\0';
      fseek(fin,dwPos,SEEK_SET);
      fread(name,1,16,fin);
      unsigned long lcn;
      fread(&lcn,sizeof(lcn),1,fin);
      lcn = SwapLong(lcn);
#ifdef _UNICODE
      tchar tname[17];
      memset(tname, 0, sizeof(tname));
      for (int i = 0; name[i] != 0; i++)
	    tname[i] = name[i];

      t->push_back(Bkmk(tname, NULL, lcn));
#else
      t->push_back(Bkmk(name,lcn));
#endif
    }
  }
  fseek(fin, cur, SEEK_SET);
  return t;
}

int Aportis::OpenFile(const char *src)
{
  //  printf("In openfile\n");
  int ret = 0;
  html = false;
  mobiimagerec = 0;
  if (!Cpdb::openpdbfile(src)) return -1;

  if (head.creator == 0x64414552 //   'dAER'
      || head.type == 0x74584554) // 'tXET')
    {
    }
  else if (memcmp(&head.creator, "PPrs", 4) == 0 && memcmp(&head.type, "PNRd", 4) == 0)
    {
      peanutfile = true;
    }
  else if (memcmp(&head.creator, "MOBI", 4) == 0 && memcmp(&head.type, "BOOK", 4) == 0)
    {
      html = true;
      unsigned char vsn;
      fseek(fin, recordpos(0)+39, SEEK_SET);
      fread(&vsn, 1, sizeof(vsn), fin);
      qDebug("Mobi version:%x", vsn);
      if (vsn > 2)
	 {
	    fseek(fin, recordpos(0)+110, SEEK_SET);
	    fread(&mobiimagerec, 1, sizeof(mobiimagerec), fin);
	    mobiimagerec = ntohs(mobiimagerec)-1;
	 }
    }
  else
    {
      return -2;
    }

  nRecs2 = nRecs = SwapWord(head.recordList.numRecords) - 1;

  struct stat _stat;
  stat(src,&_stat);
  dwLen = _stat.st_size;

  //fseek(fin,0,SEEK_END);
  //dwLen = ftell(fin);

  if (peanutfile)
  {

      PeanutHeader hdr0;
      gotorecordnumber(0);
      fread(&hdr0, sizeof(hdr0), 1, fin);
//      qDebug("Version:%x", ntohs(hdr0.Version));
      if (hdr0.Version && 0x0200)
      {
	  bCompressed = 2;
      }
      else
      {
	  bCompressed = 1;
      }
      BlockSize = 4096;
      nRecs = SwapWord(hdr0.Records)-1;
      dwTLen = nRecs*BlockSize;
  }
  else
  {
      gotorecordnumber(0);
      tDocRecord0 hdr0;
      fread(&hdr0, sizeof(hdr0), 1, fin);
      bCompressed = SwapWord(hdr0.wVersion);
      if (bCompressed!=1 && bCompressed!=2 && bCompressed != 4) {
	  qDebug("ERROR:Unrecognised compression type in Aportis:%u", bCompressed);
	ret = bCompressed;
	  bCompressed = 2;
      }
      switch (bCompressed)
      {
	  case 4:
	  {
	      dwTLen = 0;
	      int i;
	      for (i = 0; i < nRecs; i++)
	      {
		  unsigned int bs = GetBS(i);
		  if (bs == 0) break;
		  else dwTLen += bs;
	      }
	      nRecs = i;
	      BlockSize = 0;
	  }
	  break;
	  case 1:
	  case 2:
	  default:
	      nRecs = SwapWord(hdr0.wNumRecs);
	      if (mobiimagerec == 0 || mobiimagerec > nRecs2) mobiimagerec = nRecs;
	      dwTLen = SwapLong(hdr0.dwStoryLen);
	      BlockSize = SwapWord(hdr0.wRecSize);
	      if (BlockSize == 0)
	      {
		  BlockSize = 4096;
		  printf("WARNING: Blocksize not set in source file\n");
	      }
      }
  }

  qDebug("Mobi image rec:%u", mobiimagerec);

  // this is the main record buffer
  // it knows how to stretch to accomodate the decompress
  currentrec = 0;
  cbptr = 0;
  outptr = 0;
  refreshbuffer();
  if (!html)
    {
      int c;
      char htmltag[] = "<HTML>";
      char *p = htmltag;
      while (1)
	{
	  c = getch();
	  char ch = *p++;
	  if (ch == 0)
	    {
	      html = true;
	      break;
	    }
	  if (c != ch)
	    {
	      html = false;
	      break;
	    }
	}
      currentrec = 0;
      cbptr = 0;
      outptr = 0;
      refreshbuffer();
    }
  /*
     for (int i = 0; i < nRecs2; i++)
     {
	qDebug("Record:%u - %u bytes at position %x", i, recordlength(i), recordpos(i));
     }
  */

  qDebug("Number of records:[%u,%u]", nRecs, nRecs2);
  return ret;
}

int Aportis::getch()
{
  if (bCompressed == 1)
    {
      if ((dwRecLen == 0) && !refreshbuffer()) return EOF;
      else
	{
          unsuspend();
	  int c = getc(fin);
	  dePeanut(c);
	  dwRecLen--;
	  currentpos++;
	  return c;
	}
    }
  if (outptr != cbptr)
    {
      currentpos++;
      return (circbuf[outptr = (outptr + 1) % 2048]);
    }
  if ((dwRecLen == 0) && !refreshbuffer()) return EOF;
  currentpos++;
  int c;

  unsuspend();
  // take a char from the input buffer
  c = getc(fin);
  dePeanut(c);
  dwRecLen--;
  // separate the char into zones: 0, 1...8, 9...0x7F, 0x80...0xBF, 0xC0...0xFF

  // codes 1...8 mean copy that many chars; for accented chars & binary
  if (c == 0)
    {
      circbuf[outptr = cbptr = (cbptr+1)%2048] = c;
      return c;
    }
  else if (c >= 0x09 && c <= 0x7F)
    {
      circbuf[outptr = cbptr = (cbptr+1)%2048] = c;
      return c;
    }
  else if (c >= 0x01 && c <= 0x08)
    {
      dwRecLen -= c;
      while(c--)
	{
	    int c = getc(fin);
	    dePeanut(c);
	    circbuf[cbptr = (cbptr+1)%2048] = c;
	}
      return circbuf[outptr = (outptr+1)%2048];
    }
  else if (c >= 0x80 && c <= 0xBF)
    {
      int m,n;
      c <<= 8;
      int c1 = getc(fin);
      dePeanut(c1);
      c += c1;
      dwRecLen--;
      m = (c & 0x3FFF) >> COUNT_BITS;
      n = c & ((1<<COUNT_BITS) - 1);
      n += 3;
      while (n--)
	{
	  cbptr = (cbptr+1)%2048;
	  circbuf[cbptr] = circbuf[(cbptr+2048-m)%2048];
	}
      return circbuf[outptr = (outptr+1)%2048];
    }
  else if (c >= 0xC0 && c <= 0xFF)
    {
      circbuf[cbptr = (cbptr+1)%2048] = ' ';
      circbuf[cbptr = (cbptr+1)%2048] = c^0x80;
      return circbuf[outptr = (outptr+1)%2048];
    }
  return 0;
}

unsigned int Aportis::GetBS(unsigned int bn)
{
  DWORD dwPos;
  WORD fs;
unsuspend();
  fseek(fin, 0x56 + 8*bn, SEEK_SET);
  fread(&dwPos, 4, 1, fin);
  dwPos = SwapLong(dwPos);
  fseek(fin,dwPos,SEEK_SET);

//  gotorecordnumber(bn+1);
  unsigned char ch;
  fread(&ch,1,1,fin);
  if (ch == 241)
    {
      fread(&fs,sizeof(fs),1,fin);
      fs = SwapWord(fs);
    }
  else
    fs = 0;
  return fs;
}

unsigned int Aportis::locate()
{
  if (bCompressed == 4)
  {
    unsuspend();
    long cur = ftell(fin);
    if (cur == -1)
      return 0;

    unsigned int clen = 0;
    for (unsigned int i = 0; i < currentrec-1; i++)
    {
      unsigned int bs = GetBS(i);
      if (bs == 0)
	break;

      clen += bs;
    }
    fseek(fin,cur,SEEK_SET);
    return clen+currentpos;
  }
  else
    return (currentrec-1)*BlockSize+currentpos;
}

void Aportis::locate(unsigned int n)
{
  unsigned int offset;
  //  currentrec = (n >> OFFBITS);
  switch (bCompressed)
    {
    case 4:
      {
	DWORD clen = 0;
	offset = n;
	unsigned int i;
	for (i = 0; i < nRecs; i++)
	  {
	    unsigned int bs = GetBS(i);
	    if (bs == 0) break;
	    clen += bs;
	    if (clen > n) break;
	    offset = n - clen;
	  }
	currentrec = i;
      }
      break;
    case 1:
    case 2:
    default:
      currentrec = n / BlockSize;
      offset = n % BlockSize;
    }

  outptr = cbptr;
  refreshbuffer();
  while (currentpos < offset && getch() != EOF);
}

bool Aportis::refreshbuffer()
{
  if (currentrec < nRecs)
    {
	dwRecLen = recordlength(currentrec+1);
	gotorecordnumber(currentrec+1);
      if (bCompressed == 4)
	{
	  unsigned char t[3];
	  unsuspend();
	  fread(t,1,3,fin);
	  if (t[0] != 241)
	    {
	      printf("You shouldn't be here!\n");
	      return false;
	    }
	  dwRecLen -= 3;
	}
      /*
	int n = fread(t.buf, 1, dwRecLen, fin);
	t.len = n;
	//		if(bCompressed)
	t.Decompress();

	t.buf[t.Len()] = '\0';
      */
      currentpos = 0;
      currentrec++;
      return true;
    }
  else {
    return false;
  }
}

#include <qimage.h>

QImage* Aportis::getPicture(unsigned long tgt)
{
  unsuspend();
  unsigned short tgtrec = tgt+mobiimagerec;
  if (tgtrec > nRecs2)
    return 0;

  long cur = ftell(fin);
  if (cur == -1)
    return 0;

  unsigned short reclen = recordlength(tgtrec);
  gotorecordnumber(tgtrec);
  UInt8* imgbuffer = new UInt8[reclen];
  fread(imgbuffer, 1, reclen, fin);
  QByteArray arr;
  arr.assign((const char*)imgbuffer, reclen);

  QImage* qimage = new QImage(arr);
  fseek(fin, cur, SEEK_SET);

  return qimage;
}

#ifndef __STATIC
extern "C"
{
  CExpander* newcodec() { return new Aportis; }
}
#endif
