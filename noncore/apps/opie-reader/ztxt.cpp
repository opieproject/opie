#include <stdio.h>
#include <string.h>
#include "ztxt.h"
#include "my_list.h"
#include "Bkmks.h"

ztxt::ztxt() : bInit(false), expandedtextbuffer(NULL), compressedtextbuffer(NULL) { /*printf("constructing:%x\n",fin);*/ }


int ztxt::openfile(const char *src)
{
    if (!Cpdb::openfile(src))
    {
	return -1;
    }

//printf("Okay %u\n", 4);

    if (head.type != ZTXT_ID) return -1;

    gotorecordnumber(0);
    fread(&hdr0, 1, sizeof(hdr0), fin);
//printf("Okay %u\n", 5);
    buffersize = ntohl(hdr0.size);
    compressedtextbuffer = new UInt8[buffersize];
    expandedtextbuffer = new UInt8[buffersize];
//printf("Okay %u\n", 6);

    home();
//printf("Okay %u\n", 7);

//  printf("Returning 0\n");

    return 0;
}

int ztxt::getch()
{
    if (bufferpos >= buffercontent)
    {
	size_t reclen = recordlength(++bufferrec);
	if (reclen == 0) return -1;
	gotorecordnumber(bufferrec);
	fread(compressedtextbuffer, reclen, sizeof(char), fin);

	zstream.next_in = compressedtextbuffer;
	zstream.next_out = expandedtextbuffer;
	zstream.avail_out = buffersize;
	zstream.avail_in = reclen;

	int ret = inflate(&zstream, Z_SYNC_FLUSH);
	buffercontent = buffersize - zstream.avail_out;
	bufferpos = 0;

    }
    currentpos++;
    return expandedtextbuffer[bufferpos++];
}

unsigned int ztxt::locate()
{
    return currentpos;
}

void ztxt::locate(unsigned int n)
{

    if (hasrandomaccess())
    {
	bufferrec = n / ntohs(hdr0.recordSize) + 1;
	if (bufferrec == 1)
	{
	    inflateEnd(&zstream);
	}
	size_t reclen = recordlength(bufferrec);
	if (reclen == 0) return;
	gotorecordnumber(bufferrec);
	fread(compressedtextbuffer, reclen, sizeof(char), fin);

	zstream.next_in = compressedtextbuffer;
	zstream.next_out = expandedtextbuffer;
	zstream.avail_out = buffersize;
	zstream.avail_in = reclen;

	if (bufferrec == 1)
	{
	    zstream.zalloc = Z_NULL;
	    zstream.zfree = Z_NULL;
	    zstream.opaque = Z_NULL;
	    
	    inflateInit(&zstream);
	}


	int ret = inflate(&zstream, Z_SYNC_FLUSH);
	buffercontent = buffersize - zstream.avail_out;
	bufferpos = 0;
	currentpos = n - n % ntohs(hdr0.recordSize);
	while (currentpos < n) getch();
    }
    else
    {
	home();
	while (currentpos < n && getch() != EOF);
    }
}

void ztxt::home()
{
    if (bInit)
    {
	inflateEnd(&zstream);
    }
    bInit = true;
    size_t reclen = recordlength(1);
    gotorecordnumber(1);
    fread(compressedtextbuffer, reclen, sizeof(char), fin);

    zstream.next_in = compressedtextbuffer;
    zstream.next_out = expandedtextbuffer;
    zstream.avail_out = buffersize;
    zstream.avail_in = reclen;

    zstream.zalloc = Z_NULL;
    zstream.zfree = Z_NULL;
    zstream.opaque = Z_NULL;
  
//  printf("Initialising\n");
  
    inflateInit(&zstream);

    int ret = inflate(&zstream, Z_SYNC_FLUSH);
//  printf("Inflate : %d\n", ret);
    bufferpos = 0;
    bufferrec = 1;
    currentpos = 0;
    buffercontent = buffersize - zstream.avail_out;
//	printf("buffercontent:%u\n", buffercontent);
}

CList<Bkmk>* ztxt::getbkmklist()
{
  UInt16 recno = ntohs(hdr0.bookmarkRecord);

//  printf("Bookmarks - record %d contains %d\n", recno, ntohs(hdr0.numBookmarks));

  if (recno == 0) return NULL;

  CList<Bkmk>* t = new CList<Bkmk>;
  size_t cur = ftell(fin);
  gotorecordnumber(recno);
  for (int i = 0; i < ntohs(hdr0.numBookmarks); i++)
  {
      zTXTbkmk bkmk;
      if (fread(&bkmk, sizeof(bkmk), 1, fin) != 1) break;
//      printf("Bookmark number:%d:%.20s\n", i, bkmk.title);
      t->push_back(Bkmk(bkmk.title, NULL, ntohl(bkmk.offset)));
  }
  fseek(fin, cur, SEEK_SET);
  return t;
}
