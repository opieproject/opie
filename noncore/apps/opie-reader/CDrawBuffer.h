#ifndef __CDRAWBUFFER_H
#define __CDRAWBUFFER_H

#include <qcolor.h>
#include "StyleConsts.h"
#include "CBuffer.h"
#include "my_list.h"
#include "linktype.h"

class QPainter;
class QImage;

struct textsegment
{
    int start;
    CStyle style;
    textsegment(int _start, const CStyle& _style)
	:
	start(_start), style(_style)
	{}
};

class FontControl;

class CDrawBuffer : public CBuffer
{
   bool m_hastext;
    CList<textsegment> segs;
    int len;
    FontControl* fc;
    int m_ascent, m_descent, m_lineSpacing, m_lineExtraSpacing;
    bool m_bEof, m_bBop;
    bool m_bSop, m_bEop;
    bool m_showPartial;
    CDrawBuffer(const CDrawBuffer&);
    CDrawBuffer& operator=(const tchar*sztmp);
    void Highlight(QPainter*_p, bool drawBackground, int _x, int _y, int w, QColor bgColour);
 public:
    void setstartpara() { m_bSop = true; }
    void setBop() { m_bBop = true; }
    bool isBop() { return m_bBop; }
    void setendpara() { m_bEop = true; }
    void setendpara(const CStyle& cs);
    int leftMargin();
    int rightMargin();
    void setEof() { m_bEof = true; }
    bool eof() { return m_bEof; }
    CDrawBuffer& operator=(CDrawBuffer&);
    CDrawBuffer(FontControl* _fs = NULL)
	:
	fc(_fs)
	{
	    empty();
	}
    ~CDrawBuffer();
/*
    CDrawBuffer()
	:
	size(0)
	{
	    empty();
	}
*/
    int charwidth(int numchars, CStyle& currentstyle);
    int charwidth(int numchars);
    int width(int availht, int numchars = -1, bool onscreen = false, int scwidth = 0, unsigned short _lborder = 0, unsigned short _rborder = 0);
    int offset(int, unsigned short, unsigned short, int);
    void render(QPainter* _p, int _y, bool _bMono, int _charWidth, int scw, unsigned short, unsigned short, const QColor&, int availht);
    void empty();
    void addch(tchar ch, CStyle _style);
    void truncate(int);
    void setright(CDrawBuffer&, int);
    CStyle firststyle();
    CStyle laststyle();
    int ascent() { return m_ascent; }
    int descent() { return m_descent; }
    int lineSpacing() { return m_lineSpacing; }
    int lineExtraSpacing() { return m_lineExtraSpacing; }

//    void frig();
    linkType getLinkType(int numchars, size_t& tgt, size_t& offset, size_t& pictgt, QImage*&);
    void resize(int);
    bool showPartial() { return m_showPartial; }
    CStyle* getNextLink(int&);
    int invertLink(int);
};
#endif
