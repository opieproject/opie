#include <stdio.h>
#include <string.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qpe/qcopenvelope_qws.h>
#ifdef LOCALPICTURES
#include <qscrollview.h>
#endif
#include <qpe/global.h>
#include <qclipboard.h>
#include <qpe/qpeapplication.h>

#include "plucker.h"
#include "Aportis.h"
#include "Palm2QImage.h"
#include "name.h"

CPlucker::CPlucker() :
#ifdef LOCALPICTURES
    m_viewer(NULL),
    m_picture(NULL),
#endif
    expandedtextbuffer(NULL),
    compressedtextbuffer(NULL),
    urls(NULL)
 { /*printf("constructing:%x\n",fin);*/ }


void CPlucker::Expand(UInt16 reclen, UInt8 type, UInt8* buffer, UInt16 buffersize)
{
    if (type%2 == 0)
    {
	fread(buffer, reclen, sizeof(char), fin);
    }
    else
    {
	fread(compressedtextbuffer, reclen, sizeof(char), fin);
	switch (ntohs(hdr0.version))
	{
	    case 2:
		UnZip(reclen, buffer, buffersize);
		break;
	    case 1:
		UnDoc(reclen, buffer, buffersize);
		break;
	}
    }
}

int CPlucker::OpenFile(const char *src)
{
    m_lastBreak = 0;
    if (!Cpdb::openfile(src))
    {
	return -1;
    }

//printf("Okay %u\n", 4);

    if (memcmp(&head.type, "DataPlkr", 8) != 0) return -1;

//    qDebug("Cool - this IS plucker");

    EOPPhase = 0;
    gotorecordnumber(0);
    fread(&hdr0, 1, sizeof(hdr0), fin);
//printf("Okay %u\n", 5);
    buffersize = 32*1024;
    compressedtextbuffer = new UInt8[buffersize];
    expandedtextbuffer = new UInt8[buffersize];

    qDebug("Total number of records:%u", ntohs(head.recordList.numRecords));
    
    unsigned int nrecs = ntohs(hdr0.nRecords);
    qDebug("Version %u, no. recs %u", ntohs(hdr0.version), nrecs);
    UInt16 homerecid = 1;
    UInt16 urlid = 0;
    bool urlsfound = false;
    for (unsigned int i = 0; i < nrecs; i++)
    {
	UInt16 id, name;
	fread(&name, 1, sizeof(name), fin);
	fread(&id, 1, sizeof(id), fin);
	qDebug("N:%d, I:%d", ntohs(name), ntohs(id));
	if (ntohs(name) == 0) homerecid = ntohs(id);
	if (ntohs(name) == 2)
	{
	    urlsfound = true;
	    urlid = id;
	    qDebug("Found url index:%d", ntohs(urlid));
	}
//	qDebug("%x", id);
    }

    textlength = 0;
    for (int recptr = 1; recptr < ntohs(head.recordList.numRecords); recptr++)
    {
	CPlucker_dataRecord thisHdr;
	gotorecordnumber(recptr);
	fread(&thisHdr, 1, sizeof(thisHdr), fin);
	if (ntohs(thisHdr.uid) == homerecid)
	{
	    m_homepos = textlength;
	    qDebug("Home pos found after %u records", recptr);
	    break;
	}
	if (thisHdr.type < 2) textlength += ntohs(thisHdr.size);
    }
    textlength = 0;

    if (urlsfound)
    {
	unsigned short recptr = finduid(ntohs(urlid));
	if (recptr != 0)
	{
	    CPlucker_dataRecord thisHdr;
	    gotorecordnumber(recptr);
	    fread(&thisHdr, 1, sizeof(thisHdr), fin);
	    fread(&urlid, 1, sizeof(urlid), fin);
	    fread(&urlid, 1, sizeof(urlid), fin);
	    qDebug("urls are in %d", ntohs(urlid));
	    recptr = finduid(ntohs(urlid));
	    if (recptr != 0)
	    {
		gotorecordnumber(recptr);
		fread(&thisHdr, 1, sizeof(thisHdr), fin);
		qDebug("Found urls:%x",thisHdr.type);
		UInt16 reclen = recordlength(recptr) - sizeof(thisHdr);
		gotorecordnumber(recptr);
		fread(&thisHdr, 1, sizeof(thisHdr), fin);
		urlsize = ntohs(thisHdr.size);
		urls = new char[urlsize];
		Expand(reclen, thisHdr.type, (UInt8*)urls, urlsize);
	    }
	}
    }
/*
    for (int recptr = 1; recptr < ntohs(head.recordList.numRecords); recptr++)
    {
	CPlucker_dataRecord thisHdr;
	gotorecordnumber(recptr);
	fread(&thisHdr, 1, sizeof(thisHdr), fin);
	if (thisHdr.uid == urlid)
	{
	    qDebug("Found urls:%x",thisHdr.type);
	    UInt16 reclen = recordlength(recptr) - sizeof(thisHdr);
	    gotorecordnumber(recptr);
	    fread(&thisHdr, 1, sizeof(thisHdr), fin);
	    urlsize = ntohs(thisHdr.size);
	    urls = new char[urlsize];
	    Expand(reclen, thisHdr.type, (UInt8*)urls, urlsize);
	    break;
	}
    }
*/
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

void CPlucker::sizes(unsigned long& _file, unsigned long& _text)
{
    qDebug("Sizes called:%u",textlength);
    _file = file_length;
    if (textlength == 0)
    {
	for (int recptr = 1; recptr < ntohs(head.recordList.numRecords); recptr++)
	{
	    CPlucker_dataRecord thisHdr;
	    gotorecordnumber(recptr);
	    fread(&thisHdr, 1, sizeof(thisHdr), fin);
	    if (thisHdr.type < 2) textlength += ntohs(thisHdr.size);
	}
    }
    _text = textlength;
//ntohl(hdr0.size);
}


char* CPlucker::geturl(UInt16 i)
{
    if (urls == NULL) return NULL;
    char* ptr = urls;
    int rn = 1;
    while (ptr - urls < urlsize)
    {
	if (rn == i) return ptr;
	ptr += strlen(ptr)+1;
	rn++;
    }
    return NULL;
}

CPlucker::~CPlucker()
{
    if (expandedtextbuffer != NULL) delete [] expandedtextbuffer;
    if (compressedtextbuffer != NULL) delete [] compressedtextbuffer;
    if (urls != NULL) delete [] urls;
#ifdef LOCALPICTURES
    if (m_viewer != NULL) delete m_viewer;
#endif
}

int CPlucker::bgetch()
{
    int ch = EOF;
    if (bufferpos >= buffercontent)
    {
	if (!m_continuous) return EOF;
	if (bufferrec >= ntohs(head.recordList.numRecords) - 1) return EOF;
//	qDebug("Passing through %u", currentpos);
	if (!expand(bufferrec+1)) return EOF;
	mystyle.unset();
	ch = 10;
	EOPPhase = 4;
    }
    else if (bufferpos == m_nextPara)
    {
	while (bufferpos == m_nextPara)
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
	mystyle.unset();
	if (m_lastBreak == locate())
	{
	    currentpos++;
	    ch = expandedtextbuffer[bufferpos++];
	}
	else
	{
	    ch = 10;
	}
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
    mystyle.clearPicture();


    if (EOPPhase > 0)
    {
	int ch = 10;
	switch (EOPPhase)
	{
	    case 4:
		mystyle.setPicture(hRule(100,5));
		mystyle.setCentreJustify();
		ch = '#';
		break;
	    case 3:
		mystyle.setFontSize(3);
		ch = 10;
		break;
	    case 2:
		ch = 10;
		break;
	    case 1:
		mystyle.unset();
	    default:
		ch = 10;
	}
	EOPPhase--;
	return ch;
    }


    int ch = bgetch();
    while (ch == 0)
    {
	ch = bgetch();
//	qDebug("Function:%x", ch);
	switch (ch)
	{
	    case 0x38:
//		qDebug("Break:%u", locate());
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
//		    qDebug("ch:%d, ln:%u", ch, ln);
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
//			qDebug("ch:%d, ln:%u", ch, ln);
		    }
		}
//		qDebug("ln:%u", ln);
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
//		qDebug("Font:%d",ch);
		switch (ch)
		{
		    case 0:
			mystyle.unsetMono();
			mystyle.unsetBold();
			mystyle.setFontSize(0);
			break;
		    case 1:
			mystyle.unsetMono();
			mystyle.setBold();
			mystyle.setFontSize(3);
			break;
		    case 2:
			mystyle.unsetMono();
			mystyle.setBold();
			mystyle.setFontSize(2);
			break;
		    case 3:
			mystyle.unsetMono();
			mystyle.setBold();
//			mystyle.unsetBold();
			mystyle.setFontSize(1);
			break;
		    case 4:
			mystyle.unsetMono();
			mystyle.setBold();
//			mystyle.unsetBold();
			mystyle.setFontSize(0);
			break;
		    case 5:
			mystyle.unsetMono();
			mystyle.setBold();
			mystyle.setFontSize(0);
			break;
		    case 6:
			mystyle.unsetMono();
			mystyle.setBold();
			mystyle.setFontSize(0);
			break;
		    case 7:
			mystyle.unsetMono();
			mystyle.setBold();
			mystyle.setFontSize(0);
			break;
		    case 8: // should be fixed width
			qDebug("Trying fixed width");
			mystyle.unsetBold();
			mystyle.setFontSize(0);
			mystyle.setMono();
			break;
		    default:
			mystyle.unsetBold();
			mystyle.unsetMono();
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
//			qDebug("left");
			break;
		    case 1:
			mystyle.setRightJustify();
//			qDebug("right");
			break;
		    case 2:
			mystyle.setCentreJustify();
//			qDebug("centre");
			break;
		    case 3:
			mystyle.setFullJustify();
//			qDebug("full");
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
		    qDebug("Alternate image:%x", ir);
		    UInt16 ir2 = bgetch();
		    ir2 = (ir2 << 8) + bgetch();
		    mystyle.setPicture(expandimg(ir2, true), true, ir);
#ifdef LOCALPICTURES
		    UInt32 ln = ir;
		    ln <<= 16;
		    mystyle.setLink(true);
		    mystyle.setData(ln);
#endif
		}
		else
		{
		    mystyle.setPicture(expandimg(ir));
		}
		if (mystyle.getLink()) qDebug("Picture link!");
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
//		qDebug("h,w,pc [%u, %u, %u]", h, w, pc);
		if (w == 0)
		{
		    w = (240*(unsigned long)pc)/100;
		}
		if (w == 0) w = 320;
		mystyle.setPicture(hRule(w,h,mystyle.Red(),mystyle.Green(),mystyle.Blue()));
//		if (mystyle.getLink()) qDebug("hRule link!");
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
//		qDebug("Left margin:%d", ch);
		ch = bgetch();
		mystyle.setRightMargin(ch);
//		qDebug("Right margin:%d", ch);
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
	    case 0x85:
	    default:
		qDebug("Function:%x NOT IMPLEMENTED", ch);
		{
		    int skip = ch & 7;
		    for (int i = 0; i < skip; i++)
		    {
			ch = bgetch();
//			qDebug("Arg %d, %d", i, ch);
		    }
		    ch = bgetch();
		}
	}
    }

    if (m_lastIsBreak && !mystyle.isMono())
    {
	while (ch == ' ')
	{
	    ch = getch();
	}
    }

    m_lastIsBreak = (ch == 10);

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
	if (thisHdr.type < 2) locpos += ntohs(thisHdr.size);
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
	    bs = ntohs(thisHdr.size);
	}
	else
	{
	    bs = 0;
	}
    } while (locpos + bs <= n);
    currentpos = locpos;
    expand(thisrec);
#ifdef _FAST
    while (currentpos < n && bufferpos < buffercontent) bgetch();
#else
    while (currentpos < n && bufferpos < buffercontent) getch();
#endif
}

bool CPlucker::hyperlink(unsigned int n)
{
    visited.push_front(n);
    UInt16 tuid = (n >> 16);
    n &= 0xffff;
//    qDebug("Hyper:<%u,%u>", tuid, n);
    UInt16 thisrec = 1;
    currentpos = 0;
    gotorecordnumber(thisrec);
    CPlucker_dataRecord thisHdr;
    while (1)
    {
	fread(&thisHdr, 1, sizeof(thisHdr), fin);
	if (tuid == ntohs(thisHdr.uid)) break;
	if (thisHdr.type < 2) currentpos += ntohs(thisHdr.size);
//	qDebug("hyper-cp:%u", currentpos);
	thisrec++;
	if (thisrec >= ntohs(head.recordList.numRecords))
	{
	    if (urls == NULL)
	    {
		QMessageBox::information(NULL,
				 PROGNAME,
				 QString("No external links\nin this pluck")
		    );
	    }
	    else
	    {
		char *turl = geturl(tuid);
		if (turl == NULL)
		{
		    QMessageBox::information(NULL,
					     PROGNAME,
					     QString("Couldn't find link")
			);
		}
		else
		{
		    QString wrd(turl);
		    QClipboard* cb = QApplication::clipboard();
		    cb->setText(wrd);
		    if (wrd.length() > 10)
		    {
			Global::statusMessage(wrd.left(8) + "..");
		    }
		}
	    }
	    return false;
	}
	gotorecordnumber(thisrec);
    }
    if (thisHdr.type > 1)
    {
	if (thisHdr.type == 4)
	{
	    QMessageBox::information(NULL,
				     PROGNAME,
				     QString("Mailto links\nnot yet supported (2)"));
	}
	else
	{
#ifdef LOCALPICTURES
	    if (thisHdr.type > 3)
	    {
#endif
		QMessageBox::information(NULL,
					 PROGNAME,
					 QString("External links\nnot yet supported (2)")
		    );
#ifdef LOCALPICTURES
	    }
	    else
	    {
		showimg(tuid);
	    }
#endif
	}
	return false;
    }
/*
    if (thisHdr.type == 2 || thisHdr.type == 3)
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
					 PROGNAME,
					 QString("Error in link\nPara # too big")
		    );
		return false;
	    }
	    unsigned int noff = 0;
	    for (int i = 0; i < n; i++) noff += m_ParaOffsets[i];
	    n = noff;
	}
	if (n > ntohs(thisHdr.size))
	{
	    QMessageBox::information(NULL,
				     PROGNAME,
				 QString("Error in link\nOffset too big")
		);
	    return false;
	}
	qDebug("Hyper:<%u,%u>", tuid, n);
	while (bufferpos < n && bufferpos < buffercontent) getch();
    }
    return true;
}
/*
bool CPlucker::hyperlink(unsigned int n)
{
    visited.push_front(n);
    UInt16 tuid = (n >> 16);
    n &= 0xffff;
//    qDebug("Hyper:<%u,%u>", tuid, n);
    UInt16 thisrec = finduid(tuid);
    if (thisrec == 0)
    {
	if (urls == NULL)
	{
	    QMessageBox::information(NULL,
				     PROGNAME,
				     QString("No external links\nin this pluck")
		);
	}
	else
	{
	    char *turl = geturl(tuid);
	    if (turl == NULL)
	    {
		QMessageBox::information(NULL,
		PROGNAME,
					 QString("Couldn't find link")
		    );
	    }
	    else
	    {
		QString wrd(turl);
		QClipboard* cb = QApplication::clipboard();
		cb->setText(wrd);
		if (wrd.length() > 10)
		{
		    Global::statusMessage(wrd.left(8) + "..");
		}
	    }
	}
	return false;
    }
    else
    {
	currentpos = 0;
	gotorecordnumber(thisrec);
	CPlucker_dataRecord thisHdr;
	fread(&thisHdr, 1, sizeof(thisHdr), fin);

	if (thisHdr.type > 1)
	{
	    if (thisHdr.type == 4)
	    {
		QMessageBox::information(NULL,
		PROGNAME,
					 QString("Mailto links\nnot yet supported (2)"));
	    }
	    else
	    {
#ifdef LOCALPICTURES
		if (thisHdr.type > 3)
		{
#endif
		    QMessageBox::information(NULL,
					     PROGNAME,
					     QString("External links\nnot yet supported (2)")
			);
#ifdef LOCALPICTURES
		}
		else
		{
		    showimg(tuid);
		}
#endif
	    }
	    return false;
	}
//  if (thisHdr.type == 2 || thisHdr.type == 3)
//  {
//  expandimg(thisrec);
//  }
	else
	{
	    expand(thisrec);
	    if (n != 0) 
	    {
		if (n >= m_nParas)
		{
		    QMessageBox::information(NULL,
					     PROGNAME,
					     QString("Error in link\nPara # too big")
			);
		    return false;
		}
		unsigned int noff = 0;
		for (int i = 0; i < n; i++) noff += m_ParaOffsets[i];
		n = noff;
	    }
	    if (n > ntohs(thisHdr.size))
	    {
		QMessageBox::information(NULL,
					 PROGNAME,
					 QString("Error in link\nOffset too big")
		    );
		return false;
	    }
	    qDebug("Hyper:<%u,%u>", tuid, n);
	    while (bufferpos < n && bufferpos < buffercontent) getch();
	}
	return true;
    }
}
*/
bool CPlucker::expand(int thisrec)
{
    mystyle.unset();
    size_t reclen = recordlength(thisrec);
    gotorecordnumber(thisrec);
    CPlucker_dataRecord thisHdr;
    while (1)
    {
	fread(&thisHdr, 1, sizeof(thisHdr), fin);
//	qDebug("This (%d) type is %d, uid is %u", thisrec, thisHdr.type, ntohs(thisHdr.uid));
	if (thisHdr.type < 2) break;
	qDebug("Skipping paragraph of type %d", thisHdr.type);
	if (++thisrec >= ntohs(head.recordList.numRecords) - 1) return false;
	reclen = recordlength(thisrec);
	gotorecordnumber(thisrec);
    }
    m_nParas = ntohs(thisHdr.nParagraphs);
//	qDebug("It has %u paragraphs and is %u bytes", ntohs(thisHdr.nParagraphs), ntohs(thisHdr.size));
    uid = ntohs(thisHdr.uid);
    for (int i = 0; i < m_nParas; i++)
    {
	UInt16 ubytes, attrs;
	fread(&ubytes, 1, sizeof(ubytes), fin);
	fread(&attrs, 1, sizeof(attrs), fin);
	m_ParaOffsets[i] = ntohs(ubytes);
	m_ParaAttrs[i] = ntohs(attrs);
//	qDebug("Bytes %u, Attr %x", ntohs(ubytes), attrs);
    }
    if (m_nParas > 0)
    {
	m_nextPara = m_ParaOffsets[0];
//	qDebug("First offset = %u", m_nextPara);
	m_nextParaIndex = 0;
    }
    else
    {
	m_nextPara = -1;
    }

    reclen -= sizeof(thisHdr)+4*m_nParas;

    buffercontent = ntohs(thisHdr.size);

    Expand(reclen, thisHdr.type, expandedtextbuffer, buffercontent);
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

//	qDebug("err:%d - %u", err, zstream.avail_in);

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

QImage* CPlucker::getimg(UInt16 tgt)
{
//    static int imageno;
//    char* file = "tmp1";
//    sprintf(file, "image%04u.tbmp", imageno++);
//    qDebug("Image:%u", tgt);
    CPlucker_dataRecord thisHdr;
    size_t reclen;
    UInt16 thisrec = finduid(tgt);
    reclen = recordlength(thisrec);
    gotorecordnumber(thisrec);
    fread(&thisHdr, 1, sizeof(thisHdr), fin);
/*
    UInt16 thisrec = 0;
    do
    {
	hthisrec++;
	reclen = recordlength(thisrec);
	gotorecordnumber(thisrec);
//	qDebug("thisrec:%u.%u", ftell(fin),thisrec);
	fread(&thisHdr, 1, sizeof(thisHdr), fin);
    }
    while (ntohs(thisHdr.uid) != tgt);
*/
    reclen -= sizeof(thisHdr);

    UInt16 imgsize = ntohs(thisHdr.size);
    UInt8* imgbuffer = new UInt8[imgsize];
    
//    qDebug("type:%u", thisHdr.type);
    Expand(reclen, thisHdr.type, imgbuffer, imgsize);

    QImage* qimage = Palm2QImage(imgbuffer, imgsize);

    delete [] imgbuffer;

    return qimage;
}

#include <qnamespace.h>

QPixmap* CPlucker::expandimg(UInt16 tgt, bool border)
{
    QImage* qimage = getimg(tgt);
    if (qimage == NULL) return NULL;
    QPixmap* image = new QPixmap(0,0);
    QPixmap* ret;
//    qDebug("New image");
    image->convertFromImage(*qimage);
    delete qimage;
    if (border)
    {
	ret = new QPixmap(image->width()+4, image->height()+4);
	ret->fill(Qt::red);
	bitBlt(ret, 2, 2, image, 0, 0, -1, -1);//, Qt::RasterOp::CopyROP);
	delete image;
    }
    else
    {
	ret = image;
    }
    return ret;
}

#ifdef _BUFFERPICS
#include <qmap.h>
#endif

QPixmap* CPlucker::getPicture(unsigned long tgt)
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
    return expandimg(tgt);
#endif
}

#ifdef LOCALPICTURES
#include <unistd.h>
#include <qpe/global.h>
void CPlucker::showimg(UInt16 tgt)
{
    qDebug("Crassssssh!");
    QPixmap* qimage = expandimg(tgt);
    m_picture->setFixedSize(qimage->size());
    m_picture->setBackgroundPixmap(*qimage);
    delete qimage;
    m_viewer->show();

/*
    char tmp[] = "uqtreader.XXXXXX";
    QImage* qimage = getimg(tgt);
    QPixmap* image = new QPixmap(0,0);
//    qDebug("New image");
    image->convertFromImage(*qimage);
    delete qimage;
    char tmpfile[sizeof(tmp)+1];
    strcpy(tmpfile,tmp);
    int f = mkstemp(tmpfile);
    close(f);
    qDebug("TMPFILE:%s", tmpfile);
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

void CPlucker::setSaveData(unsigned char*& data, unsigned short& len, unsigned char* src, unsigned short srclen)
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
    for (CList<unsigned long>::iterator it = visited.begin(); it != visited.end(); it++)
    {
	unsigned long t = *it;
	qDebug("[%u]", t);
	memcpy(newdata, &t, sizeof(t));
	newdata += sizeof(t);
    }
    m_nav.setSaveData(data, len, pdata, newlen);
    delete [] pdata;
}

void CPlucker::putSaveData(unsigned char*& src, unsigned short& srclen)
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
	    qDebug("[%u]", t);
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

unsigned short CPlucker::finduid(unsigned short urlid)
{
//    qDebug("Finding %u", urlid);
    unsigned short jmin = 1, jmax = ntohs(head.recordList.numRecords);
    unsigned short jmid = (jmin+jmax) >> 1;
    while (jmax - jmin > 1)
    {
	CPlucker_dataRecord thisHdr;
	gotorecordnumber(jmid);
	fread(&thisHdr, 1, sizeof(thisHdr), fin);
	unsigned short luid = ntohs(thisHdr.uid);
//	qDebug("%u %u %u : %u", jmin, jmid, jmax, urlid);
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
    CPlucker_dataRecord thisHdr;
    gotorecordnumber(jmin);
    fread(&thisHdr, 1, sizeof(thisHdr), fin);
    unsigned short luid = ntohs(thisHdr.uid);
    qDebug("jmin at end:%u,%u", jmin, luid);
    if (luid == urlid)
    {
	return jmin;
    }
    gotorecordnumber(jmax);
    fread(&thisHdr, 1, sizeof(thisHdr), fin);
    luid = ntohs(thisHdr.uid);
    qDebug("jmax at end:%u,%u", jmax, luid);
    if (luid == urlid)
    {
	return jmax;
    }
    qDebug("Couldn't find %u", urlid);
    return 0; // Not found!
}
