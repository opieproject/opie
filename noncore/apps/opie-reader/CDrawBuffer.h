#ifndef __CDRAWBUFFER_H
#define __CDRAWBUFFER_H

#include "StyleConsts.h"
#include "CBuffer.h"
#include "my_list.h"

class QPainter;

struct textsegment
{
    int start;
    CStyle style;
    textsegment(int _start, CStyle _style)
	:
	start(_start), style(_style)
	{}
};

class FontControl;

class CDrawBuffer : public CBuffer
{
    CList<textsegment> segs;
    int len;
    FontControl* fc;
    int m_maxstyle, m_ascent, m_descent, m_lineSpacing;
 public:
    CDrawBuffer(FontControl* _fs = NULL)
	:
	fc(_fs)
	{
	    empty();
	}
/*
    CDrawBuffer()
	:
	size(0)
	{
	    empty();
	}
*/
    int width(int numchars = -1);
    void render(QPainter* _p, int _y, bool _bMono, int _charWidth, int scw);
    CDrawBuffer& operator=(const tchar*sztmp);
    CDrawBuffer& operator=(CDrawBuffer&);
    void empty();
    void addch(tchar ch, CStyle _style);
    void truncate(int);
    void setright(CDrawBuffer&, int);
    CStyle laststyle();
    int ascent() { return m_ascent; }
    int descent() { return m_descent; }
    int lineSpacing() { return m_lineSpacing; }

//    void frig();
    bool isLink(int numchars, size_t& tgt);
};
#endif
