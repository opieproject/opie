#if (!defined(__ARRIEREGO_H)) && defined(USENEF)
#define __ARRIEREGO_H
#include "plucker_base.h"

#ifdef LOCALPICTURES
class QScrollView;
class QWidget;
#endif

class CArriere : public CPlucker_base
{
protected:
    bool isEndOfSection(int thisrec)
      {
	return !m_bufferisreserved;
      }
    unsigned long NEFstartSection();
    unsigned long NEFendSection();
    void setbuffersize()
	{
	    compressedbuffersize = buffersize = 64*1024;
	}
    void GetHeader(UInt16&, UInt16&, UInt32&, UInt8&, UInt8&);    
    int HeaderSize();
    int bgetch();
    tchar getch(bool);

    bool CorrectDecoder();
//    void setlink(QString&, const QString&);
    QImage* imagefromdata(UInt8*, UInt32);
    unsigned char m_reserved;
public:
    void start2endSection();
    QString about() { return CPlucker_base::about()+QString("\nArriereGo codec (c) Tim Wentford"); }
};

#endif
