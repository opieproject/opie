#include <stdio.h>
#include <string.h>
#include <qmessagebox.h>
#include "plucker.h"

#include "Aportis.h"

CPlucker::CPlucker() : expandedtextbuffer(NULL), compressedtextbuffer(NULL) { /*printf("constructing:%x\n",fin);*/ }


int CPlucker::openfile(const char *src)
{
    if (!Cpdb::openfile(src))
    {
  return -1;
    }

//printf("Okay %u\n", 4);

    if (memcmp(&head.type, "DataPlkr", 8) != 0) return -1;

//    qDebug("Cool - this IS plucker");

    textlength = 0;
    for (int recptr = 1; recptr < ntohs(head.recordList.numRecords); recptr++)
    {
  CPlucker_dataRecord thisHdr;
  gotorecordnumber(recptr);
  fread(&thisHdr, 1, sizeof(thisHdr), fin);
  if (thisHdr.type < 2) textlength += htons(thisHdr.size);
    }

    gotorecordnumber(0);
    fread(&hdr0, 1, sizeof(hdr0), fin);
//printf("Okay %u\n", 5);
    buffersize = 32*1024;
    compressedtextbuffer = new UInt8[buffersize];
    expandedtextbuffer = new UInt8[buffersize];

//    qDebug("Total number of records:%u", ntohs(head.recordList.numRecords));
    
    unsigned int nrecs = ntohs(hdr0.nRecords);
//    qDebug("Version %u, no. res %u", ntohs(hdr0.version), nrecs);
    for (unsigned int i = 0; i < 4*nrecs; i++)
    {
  UInt8 id;
  fread(&id, 1, sizeof(id), fin);
//  qDebug("%x", id);
    }
    home();
    return 0;
}

int CPlucker::bgetch()
{
    int ch = EOF;
    if (bufferpos >= buffercontent)
    {
  if (bufferrec >= ntohs(head.recordList.numRecords) - 1) return EOF;
//  qDebug("Passing through %u", currentpos);
  if (!expand(bufferrec+1)) return EOF;
  mystyle.unset();
    }

    if (bufferpos == m_nextPara)
    {
  UInt16 attr = m_ParaAttrs[m_nextParaIndex];
  m_nextParaIndex++;
  if (m_nextParaIndex == m_nParas)
  {
      m_nextPara = -1;
  }
  else
  {
      m_nextPara += m_ParaOffsets[m_nextParaIndex];
  }
//  qDebug("New paragraph");
  ch = 10;
    }
    else
    {
  currentpos++;
  ch = expandedtextbuffer[bufferpos++];
    }
    return ch;
}

int CPlucker::getch()
{
    int ch = bgetch();
    while (ch == 0)
    {
  ch = bgetch();
//  qDebug("Function:%x", ch);
  switch (ch)
  {
      case 0x38:
    ch = 10;
    break;
      case 0x0a:
      case 0x0c:
      {
    unsigned long ln = 0;
    int skip = ch & 7;
    for (int i = 0; i < 2; i++)
    {
        int ch = bgetch();
        ln = (ln << 8) + ch;
//        qDebug("ch:%d, ln:%u", ch, ln);
    }
    if (skip == 2)
    {
        ln <<= 16;
    }
    else
    {
        for (int i = 0; i < 2; i++)
        {
      int ch = bgetch();
      ln = (ln << 8) + ch;
//      qDebug("ch:%d, ln:%u", ch, ln);
        }
    }
//    qDebug("ln:%u", ln);
    mystyle.setLink(true);
    mystyle.setData(ln);
    mystyle.setColour(255, 0, 0);
    ch = bgetch();
      }
      break;
      case 0x08:
    ch = bgetch();
    mystyle.setColour(0, 0, 0);
    mystyle.setLink(false);
    mystyle.setData(0);
    break;
      case 0x40:
    mystyle.setItalic();
    ch = bgetch();
    break;
      case 0x48:
    mystyle.unsetItalic();
    ch = bgetch();
    break;
      case 0x11:
      {
    ch = bgetch();
    qDebug("Font:%d",ch);
    switch (ch)
    {
        case 0:
      mystyle.unsetBold();
      mystyle.setFontSize(0);
      break;
        case 1:
      mystyle.setBold();
      mystyle.setFontSize(1);
      break;
        case 2:
      mystyle.setBold();
      mystyle.setFontSize(1);
      break;
        case 3:
//      mystyle.setBold();
      mystyle.setFontSize(1);
      break;
        case 4:
//      mystyle.setBold();
      mystyle.setFontSize(1);
      break;
        case 5:
      mystyle.setBold();
      mystyle.setFontSize(0);
      break;
        case 6:
      mystyle.setBold();
      mystyle.setFontSize(0);
      break;
        case 7:
      mystyle.setBold();
      mystyle.setFontSize(0);
      break;
        case 8: // should be fixed width
      mystyle.unsetBold();
      mystyle.setFontSize(0);
      break;
        default:
      mystyle.unsetBold();
      mystyle.setFontSize(0);
      break;
    }
    ch = bgetch();
      }
      break;
      case 0x29:
    ch = bgetch();
    switch (ch)
    {
        case 0:
      mystyle.setLeftJustify();
//      qDebug("left");
      break;
        case 1:
      mystyle.setRightJustify();
//      qDebug("right");
      break;
        case 2:
      mystyle.setCentreJustify();
//      qDebug("centre");
      break;
        case 3:
      mystyle.setFullJustify();
//      qDebug("full");
      break;

    }
    ch = bgetch();
    break;
      case 0x53:
      {
    int r = bgetch();
    int g = bgetch();
    int b = bgetch();
    mystyle.setColour(r,g,b);
    ch = bgetch();
      }
      break;
      case 0x1a:
/*
      {
    UInt16 ir = bgetch();
    ir = (ir << 8) + bgetch();
    expandimg(ir);
      }
      ch = bgetch();
      break;
*/
      case 0x33:
      case 0x22:
      case 0x5c:
      case 0x60:
      case 0x68:
      case 0x70:
      case 0x78:
      case 0x83:
      case 0x85:
      default:
    qDebug("Function:%x NOT IMPLEMENTED", ch);
      {
    int skip = ch & 7;
    for (int i = 0; i < skip; i++)
    {
        ch = bgetch();
        qDebug("Arg %d, %d", i, ch);
    }
    ch = bgetch();
      }
  }
    }
    return ch;
}

void CPlucker::getch(int& ch, CStyle& sty)
{
    ch = getch();
    sty = mystyle;
}

unsigned int CPlucker::locate()
{
    return currentpos;
/*
    UInt16 thisrec = 1;
    unsigned long locpos = 0;
    gotorecordnumber(thisrec);
    CPlucker_dataRecord thisHdr;
    while (thisrec < bufferrec)
    {
  fread(&thisHdr, 1, sizeof(thisHdr), fin);
  if (thisHdr.type < 2) locpos += htons(thisHdr.size);
  thisrec++;
  gotorecordnumber(thisrec);
    }
    return locpos+bufferpos;
*/
}

void CPlucker::locate(unsigned int n)
{
    UInt16 thisrec = 0;
    unsigned long locpos = 0;
    unsigned long bs = 0;
    CPlucker_dataRecord thisHdr;
    do
    {
        thisrec++;
  locpos += bs;
  gotorecordnumber(thisrec);
  fread(&thisHdr, 1, sizeof(thisHdr), fin);
  if (thisHdr.type < 2)
  {
      bs = htons(thisHdr.size);
  }
  else
  {
      bs = 0;
  }
    } while (locpos + bs < n);
    currentpos = locpos;
    expand(thisrec);
    while (currentpos < n && bufferpos < buffercontent) bgetch();
}

bool CPlucker::hyperlink(unsigned int n)
{
    UInt16 tuid = (n >> 16);
    n &= 0xffff;
    UInt16 thisrec = 1;
    currentpos = 0;
    gotorecordnumber(thisrec);
    CPlucker_dataRecord thisHdr;
    while (1)
    {
  fread(&thisHdr, 1, sizeof(thisHdr), fin);
  if (tuid <= htons(thisHdr.uid)) break;
  if (thisHdr.type < 2) currentpos += htons(thisHdr.size);
//  qDebug("hyper-cp:%u", currentpos);
  thisrec++;
  gotorecordnumber(thisrec);
    }
    if (thisHdr.type > 1)
    {
  QMessageBox::information(NULL,
         QString("OpieReader"),
         QString("External links\nnot yet supported")
      );
  return false;
    }
    else
    {
  expand(thisrec);
  while (bufferpos < n && bufferpos < buffercontent) getch();
    }
    return true;
}

bool CPlucker::expand(int thisrec)
{
    mystyle.unset();
    size_t reclen = recordlength(thisrec);
    gotorecordnumber(thisrec);
    CPlucker_dataRecord thisHdr;
    while (1)
    {
  fread(&thisHdr, 1, sizeof(thisHdr), fin);
//  qDebug("This (%d) type is %d, uid is %u", thisrec, thisHdr.type, ntohs(thisHdr.uid));
  if (thisHdr.type < 2) break;
  qDebug("Skipping paragraph of type %d", thisHdr.type);
  if (++thisrec >= ntohs(head.recordList.numRecords) - 1) return false;
  reclen = recordlength(thisrec);
  gotorecordnumber(thisrec);
    }
    m_nParas = ntohs(thisHdr.nParagraphs);
//  qDebug("It has %u paragraphs and is %u bytes", htons(thisHdr.nParagraphs), htons(thisHdr.size));
    uid = ntohs(thisHdr.uid);
    for (int i = 0; i < m_nParas; i++)
    {
  UInt16 ubytes, attrs;
  fread(&ubytes, 1, sizeof(ubytes), fin);
  fread(&attrs, 1, sizeof(attrs), fin);
  m_ParaOffsets[i] = ntohs(ubytes);
  m_ParaAttrs[i] = ntohs(attrs);
//  qDebug("Bytes %u, Attr %x", ntohs(ubytes), attrs);
    }
    if (m_nParas > 0)
    {
  m_nextPara = m_ParaOffsets[0];
//  qDebug("First offset = %u", m_nextPara);
  m_nextParaIndex = 0;
    }
    else
    {
  m_nextPara = -1;
    }

    reclen -= sizeof(thisHdr)+4*m_nParas;

    buffercontent = htons(thisHdr.size);

    if (thisHdr.type == 0)
    {
  fread(expandedtextbuffer, reclen, sizeof(char), fin);
    }
    else
    {
  fread(compressedtextbuffer, reclen, sizeof(char), fin);
  switch (ntohs(hdr0.version))
  {
      case 2:
    UnZip(reclen, expandedtextbuffer, buffercontent);
    break;
      case 1:
    UnDoc(reclen, expandedtextbuffer, buffercontent);
    break;
  }
    }
    bufferpos = 0;
    bufferrec = thisrec;
//    qDebug("BC:%u, HS:%u", buffercontent, ntohs(thisHdr.size));
    return true;
}

void CPlucker::UnZip(size_t reclen, UInt8* tgtbuffer, UInt16 bsize)
{
    z_stream zstream;
    memset(&zstream,sizeof(zstream),0);
    zstream.next_in = compressedtextbuffer;
    zstream.next_out = tgtbuffer;
    zstream.avail_out = bsize;
    zstream.avail_in = reclen;

    int keylen = 0;

    zstream.zalloc = Z_NULL;
    zstream.zfree = Z_NULL;
    zstream.opaque = Z_NULL;
  
//  printf("Initialising\n");
  
    inflateInit(&zstream);
    int err = 0;
    do {
        if ( zstream.avail_in == 0 && 0 < keylen ) {
            zstream.next_in   = compressedtextbuffer + keylen;
            zstream.avail_in  = reclen - keylen;
            keylen      = 0;
        }
        zstream.next_out  = tgtbuffer;
        zstream.avail_out = bsize;

        err = inflate( &zstream, Z_SYNC_FLUSH );

    } while ( err == Z_OK );

    inflateEnd(&zstream);
}

void CPlucker::UnDoc(size_t reclen, UInt8* tgtbuffer, UInt16 bsize)
{
//    UInt16      headerSize;
    UInt16      docSize;
    UInt16      i;
    UInt16      j;
    UInt16      k;

    UInt8 *inBuf = compressedtextbuffer;
    UInt8 *outBuf = tgtbuffer;

//    headerSize  = sizeof( Header ) + record->paragraphs * sizeof( Paragraph );
    docSize     = reclen;

    j               = 0;
    k               = 0;
    while ( j < docSize ) {
        i = 0;
        while ( i < bsize && j < docSize ) {
            UInt16 c;

            c = (UInt16) inBuf[ j++ ];
            if ( 0 < c && c < 9 ) {
                while ( 0 < c-- )
                    outBuf[ i++ ] = inBuf[ j++ ];
            }
            else if ( c < 0x80 )
                outBuf[ i++ ] = c;
            else if ( 0xc0 <= c ) {
                outBuf[ i++ ] = ' ';
                outBuf[ i++ ] = c ^ 0x80;
            }
            else {
                Int16 m;
                Int16 n;

                c <<= 8;
                c  += inBuf[ j++ ];

                m   = ( c & 0x3fff ) >> COUNT_BITS;
                n   = c & ( ( 1 << COUNT_BITS ) - 1 );
                n  += 2;

                do {
                    outBuf[ i ] = outBuf[ i - m ];
                    i++;
                } while ( 0 < n-- );
            }
        }
        k += bsize;
    }
}

void CPlucker::home()
{
    currentpos = 0;
    expand(1);
}

CList<Bkmk>* CPlucker::getbkmklist()
{
/*
    CPlucker_dataRecord thisHdr;

    for (int i = 1; i < ntohs(head.recordList.numRecords); i++)
    {
  gotorecordnumber(i);
  fread(&thisHdr, 1, sizeof(thisHdr), fin);
  if (thisHdr.type == 8)
  {
      UInt16 n;
      fread(&n, 1, sizeof(n), fin);
      n = ntohs(n);
      qDebug("Found %u bookmarks", n);
  }
  qDebug("Found:%d, %u", i , thisHdr.type);
    }
*/
    return NULL;
}

void CPlucker::expandimg(UInt16 tgt)
{
    qDebug("Image:%u", tgt);
    CPlucker_dataRecord thisHdr;
    size_t reclen;
    UInt16 thisrec = 0;
    do
    {
  thisrec++;
  reclen = recordlength(thisrec);
  gotorecordnumber(thisrec);
  qDebug("thisrec:%u.%u", ftell(fin),thisrec);
  fread(&thisHdr, 1, sizeof(thisHdr), fin);
    }
    while (ntohs(thisHdr.uid) != tgt);

    reclen -= sizeof(thisHdr);

    UInt16 imgsize = htons(thisHdr.size);
    UInt8* imgbuffer = new UInt8[imgsize];
    
    qDebug("type:%u", thisHdr.type);

    if (thisHdr.type == 2)
    {
  qDebug("Not compressed:%u.%u", ftell(fin),reclen);
  fread(imgbuffer, reclen, sizeof(char), fin);
  qDebug("Not compressed:%u.%u", ftell(fin),reclen);
    }
    else
    {
  qDebug("Compressed");
  fread(compressedtextbuffer, reclen, sizeof(char), fin);
  switch (ntohs(hdr0.version))
  {
      case 2:
    UnZip(reclen, imgbuffer, imgsize);
    break;
      case 1:
    UnDoc(reclen, imgbuffer, imgsize);
    break;
  }
    }
    FILE* imgfile = fopen("/home/tim/newreader/imagefile.tbmp", "w");
    if (imgfile != NULL)
    {
  fwrite(imgbuffer, 1, imgsize, imgfile);
  fclose(imgfile);
    }
    delete [] imgbuffer;
}
