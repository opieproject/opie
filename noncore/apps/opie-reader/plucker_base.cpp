#include "useqpe.h"
#include <stdio.h>
#include <string.h>
#ifdef USEQPE
#include <qpe/qcopenvelope_qws.h>
#endif /* USEQPE */
#ifdef LOCALPICTURES
#include <qscrollview.h>
#endif
#ifdef USEQPE
#include <qpe/global.h>
#endif /* USEQPE */
#ifndef USEQPE
#include <qapplication.h>
#else /* USEQPE */
#include <qpe/qpeapplication.h>
#endif /* USEQPE */

#include "plucker_base.h"
#include "Aportis.h"
#include "Palm2QImage.h"

CPlucker_base::CPlucker_base() :
#ifdef LOCALPICTURES
    m_viewer(NULL),
    m_picture(NULL),
#endif
    expandedtextbuffer(NULL),
    compressedtextbuffer(NULL)
//,    urls(NULL)
 { /*printf("constructing:%x\n",fin);*/ }


void CPlucker_base::Expand(UInt32 reclen, UInt8 type, UInt8* buffer, UInt32 buffersize)
{
    if (type%2 == 0)
    {
	fread(buffer, reclen, sizeof(char), fin);
    }
    else
    {
	UInt8* readbuffer = NULL;
	if (reclen > compressedbuffersize)
	{
	    readbuffer = new UInt8[reclen];
	}
	else
	{
	    readbuffer = compressedtextbuffer;
	}
	if (readbuffer != NULL)
	{
	    fread(readbuffer, reclen, sizeof(char), fin);
	    switch (ntohs(hdr0.version))
	    {
		case 2:
		    UnZip(readbuffer, reclen, buffer, buffersize);
		    break;
		case 1:
		    UnDoc(readbuffer, reclen, buffer, buffersize);
		    break;
	    }
	    if (reclen > compressedbuffersize)
	    {
		delete [] readbuffer;
	    }
	}
    }
}

void CPlucker_base::sizes(unsigned long& _file, unsigned long& _text)
{
    _file = file_length;
    if (textlength == 0)
    {
	for (int recptr = 1; recptr < ntohs(head.recordList.numRecords); recptr++)
	{
	    gotorecordnumber(recptr);
	    UInt16 thishdr_uid, thishdr_nParagraphs;
	    UInt32 thishdr_size;
	    UInt8 thishdr_type, thishdr_reserved;
	    GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
	    if (thishdr_type < 2) textlength += thishdr_size;
	}
    }
    _text = textlength;
//ntohl(hdr0.size);
}

char* CPlucker_base::geturl(UInt16 tgt)
{
    char * pRet = NULL;
    gotorecordnumber(0);
    fread(&hdr0, 1, 6, fin);
    unsigned int nrecs = ntohs(hdr0.nRecords);
    //qDebug("Version %u, no. recs %u", ntohs(hdr0.version), nrecs);
    UInt16 urlid = 0;
    bool urlsfound = false;
    char* urls = NULL;
    size_t urlsize = 0;
    for (unsigned int i = 0; i < nrecs; i++)
    {
	UInt16 id, name;
	fread(&name, 1, sizeof(name), fin);
	fread(&id, 1, sizeof(id), fin);
	//qDebug("N:%d, I:%d", ntohs(name), ntohs(id));
	if (ntohs(name) == 2)
	{
	    urlsfound = true;
	    urlid = id;
	    //qDebug("Found url index:%d", ntohs(urlid));
	}
//	//qDebug("%x", id);
    }
    if (urlsfound)
    {
	unsigned short recptr = finduid(ntohs(urlid));
	if (recptr != 0)
	{
	    gotorecordnumber(recptr);
	    UInt16 thishdr_uid, thishdr_nParagraphs;
	    UInt32 thishdr_size;
	    UInt8 thishdr_type, thishdr_reserved;
	    GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
	    UInt16 urlctr = 0;
	    while (1)
	    {
		UInt16 tctr;
		fread(&tctr, 1, sizeof(tctr), fin);
		fread(&urlid, 1, sizeof(urlid), fin);
		tctr = ntohs(tctr);
		//qDebug("tgt:%u urlctr:%u tctr:%u", tgt, urlctr, tctr);
		if (tctr >= tgt)
		{
		    break;
		}
		urlctr = tctr;
	    }
	    //qDebug("urls are in %d", ntohs(urlid));
	    recptr = finduid(ntohs(urlid));
	    if (recptr != 0)
	    {
		UInt32 reclen = recordlength(recptr) - HeaderSize();
		gotorecordnumber(recptr);
		GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
		//qDebug("Found urls:%x",thishdr_type);
		urlsize = thishdr_size;
		urls = new char[urlsize];
		Expand(reclen, thishdr_type, (UInt8*)urls, urlsize);
		char* ptr = urls;
		int rn = urlctr+1;
		while (ptr - urls < urlsize)
		{
		    if (rn == tgt)
			{
			    //qDebug("URL:%s", ptr);
			    int len = strlen(ptr)+1;
			    pRet = new char[len];
			    memcpy(pRet, ptr, len);
			    break;
			}
		    ptr += strlen(ptr)+1;
		    rn++;
		}
		delete [] urls;
	    }
	}
    }
    else
    {
	QMessageBox::information(NULL,
				 QString(PROGNAME),
				 QString("No external links\nin this pluck")
	    );
    }
    return pRet;
}

CPlucker_base::~CPlucker_base()
{
    if (expandedtextbuffer != NULL) delete [] expandedtextbuffer;
    if (compressedtextbuffer != NULL) delete [] compressedtextbuffer;
#ifdef LOCALPICTURES
    if (m_viewer != NULL) delete m_viewer;
#endif
}

int CPlucker_base::getch() { return getch(false); }

void CPlucker_base::getch(tchar& ch, CStyle& sty)
{
    ch = getch(false);
    sty = mystyle;
}

unsigned int CPlucker_base::locate()
{
    return currentpos;
/*
    UInt16 thisrec = 1;
    unsigned long locpos = 0;
    gotorecordnumber(thisrec);
    UInt16 thishdr_uid, thishdr_nParagraphs;
    UInt32 thishdr_size;
    UInt8 thishdr_type, thishdr_reserved;
    while (thisrec < bufferrec)
    {
        GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
	if (thishdr_type < 2) locpos += thishdr_size;
	thisrec++;
	gotorecordnumber(thisrec);
    }
    return locpos+bufferpos;
*/
}

void CPlucker_base::locate(unsigned int n)
{

//    clock_t start = clock();
    UInt32 textlength = currentpos - bufferpos;
    UInt16 recptr = bufferrec;
    if (n < textlength/2)
    {
	textlength = 0;
	UInt16 thishdr_uid, thishdr_nParagraphs;
	UInt32 thishdr_size = buffercontent;
	UInt8 thishdr_type, thishdr_reserved;
	for (recptr = 1; recptr < ntohs(head.recordList.numRecords); recptr++)
	{
	    gotorecordnumber(recptr);
	    GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
	    if (thishdr_type < 2)
	    {
		textlength += thishdr_size;
		if (textlength > n)
		{
		    textlength -= thishdr_size;
		    break;
		}
	    }
	}
    }
    else if (n < textlength)
    {
	UInt16 thishdr_uid, thishdr_nParagraphs;
	UInt32 thishdr_size;
	UInt8 thishdr_type, thishdr_reserved;
	while (n < textlength && recptr > 1)
	{
	    recptr--;
	    gotorecordnumber(recptr);
	    //qDebug("recptr:%u", recptr);
	    GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
	    if (thishdr_type < 2)
	    {
		textlength -= thishdr_size;
	    }
	}
    }
    else
    {
	UInt16 thishdr_uid, thishdr_nParagraphs;
	UInt32 thishdr_size = buffercontent;
	UInt8 thishdr_type, thishdr_reserved;
	while (n > textlength + thishdr_size && recptr < ntohs(head.recordList.numRecords)-1)
	{
	    textlength += thishdr_size;
	    recptr++;
	    gotorecordnumber(recptr);
	    GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
	    if (!(thishdr_type < 2))
	    {
		thishdr_size = 0;
	    }
	}
    }
//    qDebug("Time(1): %u", clock()-start);
/*
    expand(recptr);
    mystyle.unset();
    bufferpos = n-textlength;
    currentpos = n;
    while (bufferpos >= m_nextPara && m_nextPara >= 0)
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
    }

    return;
*/
//    start = clock();

    UInt16 thisrec = 0;
    unsigned long locpos = 0;
    unsigned long bs = 0;
    UInt16 thishdr_uid, thishdr_nParagraphs;
    UInt32 thishdr_size;
    UInt8 thishdr_type, thishdr_reserved;
    do
    {
       	thisrec++;
	locpos += bs;
	gotorecordnumber(thisrec);
	GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
	if (thishdr_type < 2)
	{
	    bs = thishdr_size;
	}
	else
	{
	    bs = 0;
	}
    } while (locpos + bs <= n);

//    qDebug("Time(2): %u", clock()-start);
    if (recptr != thisrec)
    {
	qDebug("Disaster:recptr:%u thisrec:%u", recptr, thisrec);
	UInt16 thishdr_uid, thishdr_nParagraphs;
	UInt32 thishdr_size = buffercontent;
	UInt8 thishdr_type, thishdr_reserved;
	for (recptr = 1; recptr < ntohs(head.recordList.numRecords); recptr++)
	{
	    gotorecordnumber(recptr);
	    GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
//	    qDebug("UID:%u Paras:%u Size:%u Type:%u Reserved:%u", thishdr_uid, thishdr_nParagraphs, thishdr_size, (unsigned int)thishdr_type, (unsigned int)thishdr_reserved);
	}
//	QApplication::exit ( 100 );
    }

    currentpos = locpos;
    expand(thisrec);
    while (currentpos < n && bufferpos < buffercontent) getch_base(true);

/*  // This is faster but the alignment attribute doesn't get set 8^(
    bufferpos = n-locpos;
    currentpos = n;
    while (bufferpos >= m_nextPara && m_nextPara >= 0)
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
    }
*/
}

bool CPlucker_base::expand(int thisrec)
{
    mystyle.unset();
    size_t reclen = recordlength(thisrec);
    gotorecordnumber(thisrec);
    UInt16 thishdr_uid, thishdr_nParagraphs;
    UInt32 thishdr_size;
    UInt8 thishdr_type, thishdr_reserved;
    while (1)
    {
	GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
	//qDebug("This (%d) type is %d, uid is %u", thisrec, thishdr_type, thishdr_uid);
	if (thishdr_type < 2) break;
	//qDebug("Skipping paragraph of type %d", thishdr_type);
	if (++thisrec >= ntohs(head.recordList.numRecords) - 1) return false;
	reclen = recordlength(thisrec);
	gotorecordnumber(thisrec);
    }
    m_nParas = thishdr_nParagraphs;
    m_bufferisreserved = (thishdr_reserved != 0);
    //qDebug("It has %u paragraphs and is %u bytes", thishdr_nParagraphs, thishdr_size);
    uid = thishdr_uid;
//    gotorecordnumber(thisrec);
//    fread(expandedtextbuffer,1,10,fin);
    for (int i = 0; i < m_nParas; i++)
    {
	UInt16 ubytes, attrs;
	fread(&ubytes, 1, sizeof(ubytes), fin);
	fread(&attrs, 1, sizeof(attrs), fin);
	m_ParaOffsets[i] = ntohs(ubytes);
	m_ParaAttrs[i] = ntohs(attrs);
//	//qDebug("Bytes %u, Attr %x", ntohs(ubytes), ntohs(attrs));
    }
    if (m_nParas > 0)
    {
	m_nextPara = m_ParaOffsets[0];
	//qDebug("First offset = %u", m_nextPara);
	m_nextParaIndex = 0;
    }
    else
    {
	m_nextPara = -1;
    }

    reclen -= HeaderSize()+4*m_nParas;

    buffercontent = thishdr_size;

    if (thishdr_size > buffersize)
    {
	delete [] expandedtextbuffer;
	buffersize = thishdr_size;
	expandedtextbuffer = new UInt8[buffersize];
    }

    Expand(reclen, thishdr_type, expandedtextbuffer, buffercontent);
    bufferpos = 0;
    bufferrec = thisrec;
    //qDebug("BC:%u, HS:%u", buffercontent, thishdr_size);
    return true;
}

void CPlucker_base::UnZip(UInt8* compressedbuffer, size_t reclen, UInt8* tgtbuffer, size_t bsize)
{
    z_stream zstream;
    memset(&zstream,sizeof(zstream),0);
    zstream.next_in = compressedbuffer;
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
            zstream.next_in   = compressedbuffer + keylen;
            zstream.avail_in  = reclen - keylen;
            keylen      = 0;
        }
        zstream.next_out  = tgtbuffer;
        zstream.avail_out = bsize;

        err = inflate( &zstream, Z_SYNC_FLUSH );

//	//qDebug("err:%d - %u", err, zstream.avail_in);

    } while ( err == Z_OK );

    inflateEnd(&zstream);
}

void CPlucker_base::UnDoc(UInt8* compressedbuffer, size_t reclen, UInt8* tgtbuffer, size_t bsize)
{
//    UInt16      headerSize;
    UInt16      docSize;
    UInt16      i;
    UInt16      j;
    UInt16      k;

    UInt8 *inBuf = compressedbuffer;
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

void CPlucker_base::home()
{
    currentpos = 0;
    expand(1);
}

CList<Bkmk>* CPlucker_base::getbkmklist()
{
/*
  UInt16 thishdr_uid, thishdr_nParagraphs;
  UInt32 thishdr_size;
  UInt8 thishdr_type, thishdr_reserved;

  for (int i = 1; i < ntohs(head.recordList.numRecords); i++)
  {
	gotorecordnumber(i);
	GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
	if (thishdr_type == 8)
	{
	    UInt16 n;
	    fread(&n, 1, sizeof(n), fin);
	    n = ntohs(n);
	    //qDebug("Found %u bookmarks", n);
	}
	//qDebug("Found:%d, %u", i , thishdr_type);
    }
*/
    return NULL;
}


QImage* CPlucker_base::expandimg(UInt16 tgt, bool border)
{
    QImage* qimage = getimg(tgt);
    QImage* ret;
    if (qimage == NULL) return NULL;
    if (border)
    {
	QPixmap* image = new QPixmap(0,0);
	image->convertFromImage(*qimage);
	delete qimage;
	QPixmap* pret = new QPixmap(image->width()+4, image->height()+4);
	pret->fill(Qt::red);
	bitBlt(pret, 2, 2, image, 0, 0, -1, -1);//, Qt::RasterOp::CopyROP);
	delete image;
	ret = new QImage(pret->convertToImage());
    }
    else
    {
	ret = qimage;
    }
    return ret;
}

#ifdef _BUFFERPICS
#include <qmap.h>
#endif

QImage* CPlucker_base::getPicture(unsigned long tgt)
{
#ifdef _BUFFERPICS
    static QMap<unsigned long, QPixmap> pix;
    QMap<unsigned long, QPixmap>::Iterator t = pix.find(tgt);
    if (t == pix.end())
    {
	pix[tgt] = *expandimg(tgt);
	return &pix[tgt];
    }
    else
	return &(t.data());
#else
    return expandimg(tgt >> 16);
#endif
}

#ifdef LOCALPICTURES
#include <unistd.h>
#include <qpe/global.h>
void CPlucker_base::showimg(UInt16 tgt)
{
    //qDebug("Crassssssh!");
    QPixmap* qimage = expandimg(tgt);
    m_picture->setFixedSize(qimage->size());
    m_picture->setBackgroundPixmap(*qimage);
    delete qimage;
    m_viewer->show();

/*
    char tmp[] = "uqtreader.XXXXXX";
    QImage* qimage = getimg(tgt);
    QPixmap* image = new QPixmap(0,0);
//    //qDebug("New image");
    image->convertFromImage(*qimage);
    delete qimage;
    char tmpfile[sizeof(tmp)+1];
    strcpy(tmpfile,tmp);
    int f = mkstemp(tmpfile);
    close(f);
    //qDebug("TMPFILE:%s", tmpfile);
    if (image->save(tmpfile,"PNG"))
    {
	QCopEnvelope e("QPE/Application/showimg", "setDocument(QString)");
	e << QString(tmpfile);
    }
    Global::statusMessage("Opening image");
    sleep(5);
    delete image;
    unlink(tmpfile);
*/
}

#endif

unsigned short CPlucker_base::finduid(unsigned short urlid)
{
//    //qDebug("Finding %u", urlid);
    unsigned short jmin = 1, jmax = ntohs(head.recordList.numRecords);
    unsigned short jmid = (jmin+jmax) >> 1;
    while (jmax - jmin > 1)
    {
	gotorecordnumber(jmid);
	UInt16 thishdr_uid, thishdr_nParagraphs;
	UInt32 thishdr_size;
	UInt8 thishdr_type, thishdr_reserved;
	GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
	unsigned short luid = thishdr_uid;
//	//qDebug("%u %u %u : %u", jmin, jmid, jmax, urlid);
	if (luid == urlid)
	{
	    return jmid;
	}
	if (luid < urlid)
	{
	    jmin = jmid;
	}
	else
	{
	    jmax = jmid;
	}
	jmid = (jmin+jmax) >> 1;
    }
    gotorecordnumber(jmin);
    UInt16 thishdr_uid, thishdr_nParagraphs;
    UInt32 thishdr_size;
    UInt8 thishdr_type, thishdr_reserved;
    GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
    unsigned short luid = thishdr_uid;
    //qDebug("jmin at end:%u,%u", jmin, luid);
    if (luid == urlid)
    {
	return jmin;
    }
    gotorecordnumber(jmax);
    GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
    luid = thishdr_uid;
    //qDebug("jmax at end:%u,%u", jmax, luid);
    if (luid == urlid)
    {
	return jmax;
    }
    //qDebug("Couldn't find %u", urlid);
    return 0; // Not found!
}


void CPlucker_base::setSaveData(unsigned char*& data, unsigned short& len, unsigned char* src, unsigned short srclen)
{
    unsigned short sz = 0;
    for (CList<unsigned long>::iterator it = visited.begin(); it != visited.end(); it++)
    {
	sz++;
    }
    size_t newlen = srclen+sizeof(sz)+sz*sizeof(unsigned long);
    unsigned char* newdata = new unsigned char[newlen];
    unsigned char* pdata = newdata;
    memcpy(newdata, src, srclen);
    newdata += srclen;
    memcpy(newdata, &sz, sizeof(sz));
    newdata += sizeof(sz);
#ifdef _WINDOWS
    for (it = visited.begin(); it != visited.end(); it++)
#else
    for (CList<unsigned long>::iterator it = visited.begin(); it != visited.end(); it++)
#endif
    {
	unsigned long t = *it;
//	qDebug("[%u]", t);
	memcpy(newdata, &t, sizeof(t));
	newdata += sizeof(t);
    }
    m_nav.setSaveData(data, len, pdata, newlen);
    delete [] pdata;
}

void CPlucker_base::putSaveData(unsigned char*& src, unsigned short& srclen)
{
    unsigned short sz;
    if (srclen >= sizeof(sz))
    {
	memcpy(&sz, src, sizeof(sz));
	src += sizeof(sz);
	srclen -= sizeof(sz);
    }
    for (int i = 0; i < sz; i++)
    {
	unsigned long t;
	if (srclen >= sizeof(t))
	{
	    memcpy(&t, src, sizeof(t));
//	    qDebug("[%u]", t);
	    visited.push_front(t);
	    src += sizeof(t);
	    srclen -= sizeof(t);
	}
	else
	{
	    QMessageBox::warning(NULL, PROGNAME, "File data mismatch\nMight fix itself");
	    break;
	}
    }
    m_nav.putSaveData(src, srclen);
}

int CPlucker_base::OpenFile(const char *src)
{
    m_lastBreak = 0;
    if (!Cpdb::openfile(src))
    {
	return -1;
    }

    if (!CorrectDecoder()) return -1;

    gotorecordnumber(0);
    fread(&hdr0, 1, 6, fin);
    setbuffersize();
    compressedtextbuffer = new UInt8[compressedbuffersize];
    expandedtextbuffer = new UInt8[buffersize];

    //qDebug("Total number of records:%u", ntohs(head.recordList.numRecords));
    
    unsigned int nrecs = ntohs(hdr0.nRecords);
    //qDebug("Version %u, no. recs %u", ntohs(hdr0.version), nrecs);
    UInt16 homerecid = 1;
    for (unsigned int i = 0; i < nrecs; i++)
    {
	UInt16 id, name;
	fread(&name, 1, sizeof(name), fin);
	fread(&id, 1, sizeof(id), fin);
	//qDebug("N:%d, I:%d", ntohs(name), ntohs(id));
	if (ntohs(name) == 0) homerecid = ntohs(id);
    }

    textlength = 0;
    for (int recptr = 1; recptr < ntohs(head.recordList.numRecords); recptr++)
    {
	gotorecordnumber(recptr);
	UInt16 thishdr_uid, thishdr_nParagraphs;
	UInt32 thishdr_size;
	UInt8 thishdr_type, thishdr_reserved;
	GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
	if (thishdr_uid == homerecid)
	{
	    m_homepos = textlength;
	    break;
	}
	if (thishdr_type < 2) textlength += thishdr_size;
    }
    textlength = 0;
    home();
#ifdef LOCALPICTURES
    if (m_viewer == NULL)
    {
	m_viewer = new QScrollView(NULL);
	m_picture = new QWidget(m_viewer->viewport());
	m_viewer->addChild(m_picture);
    }
#endif
    return 0;

}

QImage* CPlucker_base::getimg(UInt16 tgt)
{
    size_t reclen;
    UInt16 thisrec = finduid(tgt);
    reclen = recordlength(thisrec);
    gotorecordnumber(thisrec);
    UInt16 thishdr_uid, thishdr_nParagraphs;
    UInt32 thishdr_size;
    UInt8 thishdr_type, thishdr_reserved;
    GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
    reclen -= HeaderSize();

    UInt32 imgsize = thishdr_size;
    UInt8* imgbuffer = new UInt8[imgsize];
    
    Expand(reclen, thishdr_type, imgbuffer, imgsize);

    return imagefromdata(imgbuffer, imgsize);
}

linkType CPlucker_base::hyperlink(unsigned int n, QString& wrd)
{
    visited.push_front(n);
    UInt16 tuid = (n >> 16);
    n &= 0xffff;
//    //qDebug("Hyper:<%u,%u>", tuid, n);
    UInt16 thisrec = 1;
    currentpos = 0;
    gotorecordnumber(thisrec);
    UInt16 thishdr_uid, thishdr_nParagraphs;
    UInt32 thishdr_size;
    UInt8 thishdr_type, thishdr_reserved;
    while (1)
    {
	GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
	if (tuid == thishdr_uid) break;
	if (thishdr_type < 2) currentpos += thishdr_size;
//	//qDebug("hyper-cp:%u", currentpos);
	thisrec++;
	if (thisrec >= ntohs(head.recordList.numRecords))
	{
	    char *turl = geturl(tuid);
	    if (turl == NULL)
	    {
		QMessageBox::information(NULL,
					 QString(PROGNAME),
					 QString("Couldn't find link")
		    );
	    }
	    else
	    {
		wrd = turl;
#ifdef USEQPE
		if (wrd.length() > 10)
		{
		    Global::statusMessage(wrd.left(8) + "..");
		}
		else
		{
		    Global::statusMessage(wrd);
		}
#else
#endif /* USEQPE */
		//qDebug("Link:%s", (const char*)wrd);
//		setlink(fn, wrd);
		delete [] turl;
	    }
	    return eNone;
	}
	gotorecordnumber(thisrec);
    }
    if (thishdr_type > 1)
    {
	if (thishdr_type == 4)
	{
	    QMessageBox::information(NULL,
				     QString(PROGNAME),
				     QString("Mailto links\nnot yet supported (2)"));
	}
	else
	{
	    if (thishdr_type > 3)
	    {
		QMessageBox::information(NULL,
					 QString(PROGNAME),
					 QString("External links\nnot yet supported (2)")
		    );
		return eNone;
	    }
	    else
	    {
#ifdef LOCALPICTURES
		showimg(tuid);
#else
		return ePicture;
#endif
	    }
	}
	return eNone;
    }
/*
    if (thishdr_type == 2 || thishdr_type == 3)
    {
	expandimg(thisrec);

    }
*/
    else
    {
	expand(thisrec);
	if (n != 0) 
	{
	    if (n >= m_nParas)
	    {
		QMessageBox::information(NULL,
				 QString(PROGNAME),
			       	 QString("Error in link\nPara # too big")
		    );
		return eNone;
	    }
	    unsigned int noff = 0;
	    for (unsigned int i = 0; i < n; i++) noff += m_ParaOffsets[i];
	    n = noff;
	}
	if (n > thishdr_size)
	{
	    QMessageBox::information(NULL,
				 QString(PROGNAME),
				 QString("Error in link\nOffset too big")
		);
	    return eNone;
	}
	//qDebug("Hyper:<%u,%u>", tuid, n);
	while (bufferpos < n && bufferpos < buffercontent) getch_base(true);
/*      // This is faster but the alignment doesn't get set
	mystyle.unset();
	bufferpos = n;
	currentpos += n;
	while (bufferpos >= m_nextPara && m_nextPara >= 0)
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
	}
*/
    }
    return eLink;
}

tchar CPlucker_base::getch_base(bool fast)
{
    int ch = bgetch();
    while (ch == 0)
    {
	ch = bgetch();
//	//qDebug("Function:%x", ch);
	switch (ch)
	{
	    case 0x38:
//		//qDebug("Break:%u", locate());
		if (m_lastBreak == locate())
		{
		    ch = bgetch();
		}
		else
		{
		    ch = 10;
		}
		m_lastBreak = locate();
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
//		    //qDebug("ch:%d, ln:%u", ch, ln);
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
//			//qDebug("ch:%d, ln:%u", ch, ln);
		    }
		}
//		//qDebug("ln:%u", ln);
		mystyle.setLink(true);
		mystyle.setData(ln);
//		mystyle.setColour(255, 0, 0);
		bool hasseen = false;
		for (CList<unsigned long>::iterator it = visited.begin(); it != visited.end(); it++)
		{
		    if (*it == ln)
		    {
			hasseen = true;
			break;
		    }
		}
		if (hasseen)
		{
		    mystyle.setStrikethru();
		}
		else
		{
		    mystyle.setUnderline();
		}
		ch = bgetch();
	    }
	    break;
	    case 0x08:
		ch = bgetch();
//		mystyle.setColour(0, 0, 0);
		mystyle.unsetUnderline();
		mystyle.unsetStrikethru();
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
//		//qDebug("Font:%d",ch);
		mystyle.setVOffset(0);
		mystyle.unsetMono();
		mystyle.unsetBold();
		mystyle.setFontSize(0);
		switch (ch)
		{
		    case 0:
			break;
		    case 1:
			mystyle.setBold();
			mystyle.setFontSize(3);
			break;
		    case 2:
			mystyle.setBold();
			mystyle.setFontSize(2);
			break;
		    case 3:
			mystyle.setBold();
			mystyle.setFontSize(1);
			break;
		    case 4:
			mystyle.setBold();
			break;
		    case 5:
			mystyle.setBold();
			break;
		    case 6:
			mystyle.setBold();
			break;
		    case 7:
			mystyle.setBold();
			break;
		    case 8: // should be fixed width
			//qDebug("Trying fixed width");
			mystyle.setMono();
			break;
		    case 9:
			mystyle.setFontSize(-1);
			break;
		    case 10:
			mystyle.setFontSize(-2);
			mystyle.setVOffset(1);
			break;
		    case 11:
			mystyle.setFontSize(-2);
			mystyle.setVOffset(-1);
			break;
		    default:
			qDebug("Unrecognised font");
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
//			//qDebug("left");
			break;
		    case 1:
			mystyle.setRightJustify();
//			//qDebug("right");
			break;
		    case 2:
			mystyle.setCentreJustify();
//			//qDebug("centre");
			break;
		    case 3:
			mystyle.setFullJustify();
//			//qDebug("full");
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
	    case 0x5c:
	    {
		bool hasalternate = (ch == 0x5c);
		UInt16 ir = bgetch();
		ir = (ir << 8) + bgetch();
		if (hasalternate)
		{
		    //qDebug("Alternate image:%x", ir);
		    UInt16 ir2 = bgetch();
		    ir2 = (ir2 << 8) + bgetch();
		    if (!fast) mystyle.setPicture(true, expandimg(ir2, true), true, ir << 16);
#ifdef LOCALPICTURES
		    UInt32 ln = ir;
		    ln <<= 16;
		    mystyle.setLink(true);
		    mystyle.setData(ln);
#endif
		}
		else
		{
		    if (!fast) mystyle.setPicture(true, expandimg(ir));
		}
//		if (mystyle.getLink()) qDebug("Picture link!");
		ch = '#';
	    }
//	    ch = bgetch();
	    break;
	    case 0x33:
	    {
		UInt8 h = bgetch();
		UInt8 wc = bgetch();
		UInt8 pc = bgetch();
		UInt16 w = wc;
//		//qDebug("h,w,pc [%u, %u, %u]", h, w, pc);
		if (w == 0)
		{
		    w = (m_scrWidth*(unsigned long)pc)/100;
		}
		if (w == 0) w = m_scrWidth;
		mystyle.setPicture(false, hRule(w,h,mystyle.Red(),mystyle.Green(),mystyle.Blue()));
//		if (mystyle.getLink()) //qDebug("hRule link!");
		ch = '#';
	    }
	    break;
	    case 0x60:
		mystyle.setUnderline();
		ch = bgetch();
		break;
	    case 0x68:
		mystyle.unsetUnderline();
		ch = bgetch();
		break;
	    case 0x22:
		ch = bgetch();
		mystyle.setLeftMargin(ch);
//		//qDebug("Left margin:%d", ch);
		ch = bgetch();
		mystyle.setRightMargin(ch);
//		//qDebug("Right margin:%d", ch);
		ch = bgetch();
		break;
	    case 0x70:
		mystyle.setStrikethru();
		ch = bgetch();
		break;
	    case 0x78:
		mystyle.unsetStrikethru();
		ch = bgetch();
		break;
	    case 0x83:
	    {
		int tlen = bgetch();
		ch = bgetch();
		ch <<= 8;
		ch |= (tchar)bgetch();
		for (int i = 0; i < tlen; i++) bgetch();
		//qDebug("Function 83");
	    }
	    break;
	    case 0x85:
	    default:
		qDebug("Function:%x NOT IMPLEMENTED", ch);
		{
		    int skip = ch & 7;
		    for (int i = 0; i < skip; i++)
		    {
			ch = bgetch();
			//qDebug("Arg %d, %d", i, ch);
		    }
		    ch = bgetch();
		}
	}
    }

    if (m_lastIsBreak && !mystyle.isMono())
    {
	while (ch == ' ')
	{
	    ch = getch(false);
	}
    }

    m_lastIsBreak = (ch == 10);

    return (ch == EOF) ? UEOF : ch;
}
