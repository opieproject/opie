#ifndef __STYLECONSTS_H
#define __STYLECONSTS_H

typedef unsigned short StyleType;

class CStyle
{
// 15     14    13-5     4      3    2   1   0
//bold  italic  spare  align  align  fs  fs  fs
    static const StyleType m_Bold = 1 << 15;
    static const StyleType m_Italic = 1 << 14;
    static const StyleType m_FontMask = 7;
    static const StyleType m_FontBase = 3;
    
    static const StyleType m_AlignShift = 3;
    static const StyleType m_AlignMask = 3 << m_AlignShift;
    static const StyleType m_EveryBit = 0xffff;


    StyleType sty;

    void unjustify() { sty &= m_EveryBit ^ m_AlignMask; }
    unsigned char red, green, blue;
    unsigned long data;
    bool isLink;
 public:
    unsigned char Red() { return red; }
    unsigned char Green() { return green; }
    unsigned char Blue() { return blue; }
    void setColour(unsigned char r, unsigned char g, unsigned char b)
	{
	    red = r;
	    green = g;
	    blue = b;
	}
    static const StyleType m_AlignLeft = 0;
    static const StyleType m_AlignRight = 1 << m_AlignShift;
    static const StyleType m_AlignCentre = 2 << m_AlignShift;
    static const StyleType m_AlignJustify = 3 << m_AlignShift;
    CStyle()
	:
	sty(m_FontBase),
	red(0), green(0), blue(0),
	data(0), isLink(false)
	{}
//    CStyle(const int _fs) : sty(m_FontBase+_fs) {}

    void unset()
	{
	    sty = m_FontBase;
	    red = green = blue = 0;
	    data = 0;
	    isLink = false;
	}

    void setBold() { sty |= m_Bold; }
    void setItalic() { sty |= m_Italic; }
    void unsetBold() { sty &= m_EveryBit ^ m_Bold; }
    void unsetItalic() { sty &= m_EveryBit ^ m_Italic; }
    bool isBold() { return ((sty & m_Bold) != 0); }
    bool isItalic() { return ((sty & m_Italic) != 0); }

    void setLeftJustify()
	{
	    unjustify();
	    sty |= m_AlignLeft;
	}
    void setRightJustify()
	{
	    unjustify();
	    sty |= m_AlignRight;
	}
    void setCentreJustify()
	{
	    unjustify();
	    sty |= m_AlignCentre;
	}
    void setFullJustify()
	{
	    unjustify();
	    sty |= m_AlignJustify;
	}
    StyleType getJustify()
	{
	    return sty & m_AlignMask;
	}

    void setFontSize(int _fs)
	{
	    sty &= m_EveryBit ^ m_FontMask;
	    sty |= m_FontBase + _fs;
	}
    int getFontSize()
	{
	    return (sty & m_FontMask) - m_FontBase;
	}
    bool operator!=(const CStyle& rhs)
	{
	    return
	     (
		(sty != rhs.sty) || (red != rhs.red) || (green != rhs.green) ||
		(blue != rhs.blue) ||
		(data != rhs.data) ||
		(isLink != rhs.isLink)
		 );
	}
    void setLink(bool _l) { isLink = _l; }
    bool getLink() { return isLink; }
    void setData(unsigned long _d) { data = _d; }
    unsigned long getData() { return data; }
};

#endif
