#ifdef USENEF
#include <qimage.h>
#include "arrierego.h"

struct CArriere_dataRecord
{
    UInt16 uid;
    UInt16 nParagraphs;
    UInt32 size;
    UInt8 type;
    UInt8 reserved;
};

int CArriere::HeaderSize()
{
    return 10;
}

void CArriere::GetHeader(UInt16& uid, UInt16& nParagraphs, UInt32& size, UInt8& type, UInt8& reserved)
{
    CArriere_dataRecord thishdr;
    unsuspend();
    fread(&thishdr, 1, HeaderSize(), fin);
    uid = ntohs(thishdr.uid);
    nParagraphs = ntohs(thishdr.nParagraphs);
    size = ntohl(thishdr.size);
    type = thishdr.type;
    reserved = thishdr.reserved;
    //    qDebug("UID:%u Paras:%u Size:%u Type:%u Reserved:%u", uid, nParagraphs, size, (unsigned int)type, (unsigned int)reserved);
//    for (int i = 0; i < sizeof(thishdr); i++)
//	printf("%d %x\n", i, (int)(((char *)(&thishdr))[i]));
}

bool CArriere::CorrectDecoder()
{
  char  * type = (char*)(&(head.type));
  for (int i = 0; i < 8; ++i) qDebug("%c", type[i]);
    return (memcmp(&head.type, "ArriereG", 8) == 0);
}

int CArriere::bgetch()
{
    int ch = EOF;
    if (bufferpos >= buffercontent)
    {
	if (bufferrec >= ntohs(head.recordList.numRecords) - 1)
	{
		return EOF;
	}
	if (isEndOfSection(bufferrec))
	{
		return EOF;
	}
	if (!expand(bufferrec+1))
	{
		return EOF;
	}
	mystyle.unset();
	if (m_ParaOffsets[m_nextParaIndex] == 0)
	{
	    while (m_ParaOffsets[m_nextParaIndex+1] == 0)
	    {
		m_nextParaIndex++;
	    }
	}
	mystyle.setExtraSpace((m_ParaAttrs[m_nextParaIndex]&7)*2);
    }
    if (bufferpos == m_nextPara)
    {
	while (bufferpos == m_nextPara)
	{
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
	mystyle.setExtraSpace((m_ParaAttrs[m_nextParaIndex]&7)*2);
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

tchar CArriere::getch(bool fast)
{
    mystyle.clearPicture();
    return getch_base(fast);
}

/*
void CArriere::setlink(QString& fn, const QString& wrd)
{
    fn = wrd;
}
*/

QImage* CArriere::imagefromdata(UInt8* imgbuffer, UInt32 imgsize)
{
    QByteArray arr;
    arr.assign((const char*)imgbuffer, imgsize);

    QImage* qimage = new QImage(arr);

    return qimage;
}

void CArriere::start2endSection()
{
    m_currentstart = NEFstartSection();
    m_currentend = NEFendSection();
}

unsigned long CArriere::NEFstartSection()
{
//inefficient - Should calc start/end of section on entry to section?
    UInt16 thishdr_uid, thishdr_nParagraphs;
    UInt32 thishdr_size;
    UInt8 thishdr_type, thishdr_reserved;
    unsigned long textlength = currentpos-bufferpos;
    for (UInt16 recptr = bufferrec-1; recptr >= 1; recptr--)
    {
	gotorecordnumber(recptr);
	//qDebug("recptr:%u", recptr);
	GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
	if (thishdr_type < 2)
	{
	    if ((thishdr_reserved && continuation_bit) == 0) break;
	    textlength -= thishdr_size;
	    //qDebug("Textlength:%u, reserved:%u, recptr:%u", textlength, thishdr_reserved, recptr);
	}
    }
    return textlength;
}

unsigned long CArriere::NEFendSection()
{
//inefficient - Should calc start/end of section on entry to section?
    unsigned long textlength = currentpos-bufferpos+buffercontent;
    gotorecordnumber(bufferrec);
    UInt16 thishdr_uid, thishdr_nParagraphs;
    UInt32 thishdr_size;
    UInt8 thishdr_type, thishdr_reserved;
    GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
    if ((thishdr_reserved && continuation_bit) != 0)
      {
	for (UInt16 recptr = bufferrec+1; recptr < ntohs(head.recordList.numRecords); recptr++)
	  {
	    gotorecordnumber(recptr);
	    UInt16 thishdr_uid, thishdr_nParagraphs;
	    UInt32 thishdr_size;
	    UInt8 thishdr_type, thishdr_reserved;
	    GetHeader(thishdr_uid, thishdr_nParagraphs, thishdr_size, thishdr_type, thishdr_reserved);
	    //	qDebug("recptr %u bufferrec %u type %u m_reserved %u", recptr, bufferrec, typ
	    if (thishdr_type < 2)
	      {
		textlength += thishdr_size;
		if ((thishdr_reserved && continuation_bit) == 0) break;
	      }
	  }
      }
    return textlength;
}

#ifndef __STATIC
extern "C"
{
  CExpander* newcodec() { return new CArriere; }
}
#endif
#endif
