
#include "CDrawBuffer.h"
#include "FontControl.h"
#include <qfontmetrics.h>
#include <qpainter.h>

void CDrawBuffer::setright(CDrawBuffer& rhs, int f)
{
  int i;
//  qDebug("Trying 1:%d:%s", f, (const char*)toQString(rhs.data()));
  len = rhs.len;
  m_maxstyle = rhs.m_maxstyle;
  m_ascent = rhs.m_ascent;
  m_descent = rhs.m_descent;
  m_lineSpacing = rhs.m_lineSpacing;
  while (!segs.isEmpty()) segs.erase(0);
  for (CList<textsegment>::iterator iter = rhs.segs.begin(); iter != rhs.segs.end(); )
  {
      CList<textsegment>::iterator next = iter;
      iter++;
      if (iter == rhs.segs.end() || iter->start > f)
      {
	  int st = next->start-f;
	  if (st < 0) st = 0;
	  segs.push_back(textsegment(st,next->style));
      }
  }
  for (i = f; rhs[i] != '\0'; i++) (*this)[i-f] = rhs[i];
  (*this)[i-f] = '\0';
  len = i;
//  qDebug("Tried 1");
}

CDrawBuffer& CDrawBuffer::operator=(CDrawBuffer& rhs)
{
  int i;
//  qDebug("Trying 2");
  len = rhs.len;
  m_maxstyle = rhs.m_maxstyle;
  m_ascent = rhs.m_ascent;
  m_descent = rhs.m_descent;
  m_lineSpacing = rhs.m_lineSpacing;
  while (!segs.isEmpty()) segs.erase(0);
  for (CList<textsegment>::iterator iter = rhs.segs.begin(); iter != rhs.segs.end(); iter++)
  {
      segs.push_back(*iter);
  }
  for (i = 0; rhs[i] != '\0'; i++) (*this)[i] = rhs[i];
  (*this)[i] = '\0';
  len = i;
//  qDebug("Tried 2");
  return *this;
}

CDrawBuffer& CDrawBuffer::operator=(const tchar*sztmp)
{
  int i;
  while (!segs.isEmpty()) segs.erase(0);
  segs.push_back(textsegment(0, CStyle()));
  for (i = 0; sztmp[i] != '\0'; i++) (*this)[i] = sztmp[i];
  (*this)[i] = '\0';
  len = i;
  return *this;
}

void CDrawBuffer::empty()
{
    len = 0;
    (*this)[0] = 0;
    while (!segs.isEmpty()) segs.erase(0);
    segs.push_back(textsegment(0,CStyle()));
    m_maxstyle = m_ascent = m_descent = m_lineSpacing = 0;
}

void CDrawBuffer::addch(tchar ch, CStyle _style/* = ucFontBase*/)
{
    if (len == 0)
    {
	int thissize = fc->getsize(_style);
	m_maxstyle = thissize;
	m_ascent = fc->ascent(_style);
	m_descent = fc->descent(_style);
	m_lineSpacing = fc->lineSpacing(_style);
	segs.first().start = 0;
	segs.first().style = _style;
    }
    else if (_style != segs.last().style)
    {
	int thissize = fc->getsize(_style);
	if (thissize > m_maxstyle)
	{
	    m_maxstyle = thissize;
	    m_ascent = fc->ascent(_style);
	    m_descent = fc->descent(_style);
	    m_lineSpacing = fc->lineSpacing(_style);
	}
	segs.push_back(textsegment(len, _style));
    }
    (*this)[len++] = ch;
}

void CDrawBuffer::truncate(int n)
{
    len = n;
    (*this)[n] = 0;
}

int CDrawBuffer::width(int numchars)
{
    int currentx = 0, end = 0;
    QString text = toQString(data());
    CList<textsegment>::iterator textstart = segs.begin();
    CList<textsegment>::iterator textend = textstart;
    do
    {
	textend++;
	end = (textend != segs.end()) ? textend->start : length();
	if (numchars >= 0 && end > numchars)
	{
	    end = numchars;
	}
	CStyle currentstyle = textstart->style;
	QFont f(fc->name(), fc->getsize(currentstyle), (currentstyle.isBold()) ? QFont::Bold : QFont::Normal, (currentstyle.isItalic()) );
	QString str = text.mid(textstart->start, end-textstart->start);
	QFontMetrics fm(f);
	currentx += fm.width(str);
	textstart = textend;
    }
    while (textend != segs.end() && end != numchars);
    return currentx;
}

void CDrawBuffer::render(QPainter* _p, int _y, bool _bMono, int _charWidth, int scwidth)
{
    int currentx = 0;
    QString text = toQString(data());
    CList<textsegment>::iterator textstart = segs.begin();
    StyleType align = textstart->style.getJustify();
    switch (align)
    {
	case CStyle::m_AlignRight:
	{
//	    int linelength = width();
	    currentx = scwidth - width();
	}
	break;
	case CStyle::m_AlignCentre:
	{
//	    int linelength = width();
	    currentx = (scwidth - width())/2;
	}
	break;
	case CStyle::m_AlignJustify:
	case CStyle::m_AlignLeft:
	    break;
    }
    CList<textsegment>::iterator textend = textstart;
    do
    {
	textend++;
	int end = (textend != segs.end()) ? textend->start : length();
	CStyle currentstyle = textstart->style;
	QFont f(fc->name(), fc->getsize(currentstyle), (currentstyle.isBold()) ? QFont::Bold : QFont::Normal, (currentstyle.isItalic()) );
	_p->setFont(f);
	QString str = text.mid(textstart->start, end-textstart->start);
	_p->setPen(QColor(currentstyle.Red(), currentstyle.Green(), currentstyle.Blue()));
	if (_bMono)
	{
	    for (int i = 0; i < str.length(); i++)
	    {
		_p->drawText( currentx + i*_charWidth, _y, QString(str[i]));
	    }
	    currentx += str.length()*_charWidth;
	}
	else
	{
	    _p->drawText( currentx, _y, str);
	    QFontMetrics fm(f);
	    currentx += fm.width(str);
	}
	textstart = textend;
    }
    while (textend != segs.end());
}

CStyle CDrawBuffer::laststyle()
{
    return segs.last().style;
}

bool CDrawBuffer::isLink(int numchars, size_t& tgt)
{
    int end = 0;
    CStyle currentstyle;
    CList<textsegment>::iterator textstart = segs.begin();
    CList<textsegment>::iterator textend = textstart;
    do
    {
	textend++;
	end = (textend != segs.end()) ? textend->start : length();
	if (numchars >= 0 && end > numchars)
	{
	    end = numchars;
	}
	currentstyle = textstart->style;
	textstart = textend;
    }
    while (textend != segs.end() && end != numchars);
    tgt = currentstyle.getData();
    return currentstyle.getLink();
}
