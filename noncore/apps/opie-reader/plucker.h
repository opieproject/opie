#ifndef __plucker_h
#define __plucker_h

#include "plucker_base.h"

#ifdef LOCALPICTURES
class QScrollView;
class QWidget;
#endif

class CPlucker : public CPlucker_base
{
    void start2endSection()
	{
	    m_currentstart = currentpos-bufferpos;
	    m_currentend = m_currentstart+buffercontent;
	}
    void setbuffersize()
	{
	    compressedbuffersize = buffersize = 32*1024;
	}
    int HeaderSize();
    void GetHeader(UInt16& uid, UInt16& nParagraphs, UInt32& size, UInt8& type, UInt8& reserved);
    int bgetch();
    tchar getch(bool fast);
    UInt8 EOPPhase;



    bool CorrectDecoder(); // Virtual
    void setlink(QString&, const QString&); // Virtual
    QImage* imagefromdata(UInt8*, UInt32); // virtual
 public:
    CPlucker();
};
#endif
