#include "useqpe.h"
#include <stdio.h>
#include <string.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#ifdef USEQPE
#include <qpe/qcopenvelope_qws.h>
#endif
#ifdef LOCALPICTURES
#include <qscrollview.h>
#endif
#ifdef USEQPE
#include <qpe/global.h>
#include <qpe/qpeapplication.h>
#else
#include <qapplication.h>
#endif
#include <qclipboard.h>

#include "plucker.h"
#include "Aportis.h"
#include "Palm2QImage.h"
#include "static.h"


struct CPlucker_dataRecord
{
    UInt16 uid;
    UInt16 nParagraphs;
    UInt16 size;
    UInt8 type;
    UInt8 reserved;
};

int CPlucker::HeaderSize()
{
    return sizeof(CPlucker_dataRecord);
}

void CPlucker::GetHeader(UInt16& uid, UInt16& nParagraphs, UInt32& size, UInt8& type, UInt8& reserved)
{
    CPlucker_dataRecord thishdr;
    fread(&thishdr, 1, HeaderSize(), fin);
    uid = ntohs(thishdr.uid);
    nParagraphs = ntohs(thishdr.nParagraphs);
    size = ntohs(thishdr.size);
    type = thishdr.type;
    reserved = thishdr.reserved;
}

CPlucker::CPlucker()
 { /*printf("constructing:%x\n",fin);*/
     EOPPhase = 0;
 }

bool CPlucker::CorrectDecoder()
{
    return (memcmp(&head.type, "DataPlkr", 8) == 0);
}

int CPlucker::bgetch()
{
    int ch = EOF;
    if (bufferpos >= buffercontent)
    {
	if (isEndOfSection(bufferrec))
	{
	  if (!m_continuous) return EOF;
	}
	if (bufferrec >= ntohs(head.recordList.numRecords) - 1)
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
//		qDebug("Skipping extraspace:%d", m_ParaAttrs[m_nextParaIndex]&7);
		m_nextParaIndex++;
	    }
	}
	mystyle.setExtraSpace((m_ParaAttrs[m_nextParaIndex]&7)*2);
//	qDebug("Using extraspace:%d", m_ParaAttrs[m_nextParaIndex]&7);
	ch = 10;
	if (m_continuous) EOPPhase = 4;
    }
    else if (bufferpos == m_nextPara)
    {
	while (bufferpos == m_nextPara)
	{
	    UInt16 attr = m_ParaAttrs[m_nextParaIndex];
	    m_nextParaIndex++;
//	    qDebug("Skipping extraspace:%d", m_ParaAttrs[m_nextParaIndex]&7);
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
//	qDebug("Using extraspace:%d", m_ParaAttrs[m_nextParaIndex]&7);
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

tchar CPlucker::getch(bool fast)
{
    mystyle.clearPicture();

    if (EOPPhase > 0)
    {
	int ch = 10;
	switch (EOPPhase)
	{
	    case 4:
		if (!fast) mystyle.setPicture(false, hRule(100,5));
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

    return getch_base(fast);
}

QImage* CPlucker::imagefromdata(UInt8* imgbuffer, UInt32 imgsize)
{
    QImage* qimage = Palm2QImage(imgbuffer, imgsize);
    delete [] imgbuffer;
    return qimage;
}

void CPlucker::start2endSection() // to plucker base
{
    m_currentstart = NEFstartSection();
    m_currentend = NEFendSection();
}

unsigned long CPlucker::NEFstartSection() // virtual in plucker base - differs only in obsoleted nef so move implementation
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

unsigned long CPlucker::NEFendSection() // virtual in plucker base - differs only in obsoleted nef so move implementation
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
  CExpander* newcodec() { return new CPlucker; }
}
#endif
