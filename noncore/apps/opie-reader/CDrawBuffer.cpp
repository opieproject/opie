#include "CDrawBuffer.h"
#include "FontControl.h"
#include <qfontmetrics.h>
#include <qpainter.h>
#include <qpixmap.h>
#include "opie.h"

CDrawBuffer::~CDrawBuffer()
{
    while (!segs.isEmpty()) segs.erase(0);
}

void CDrawBuffer::setright(CDrawBuffer& rhs, int f)
{
  int i;
  len = rhs.len;
  fc = rhs.fc;
  m_maxstyle = m_ascent = m_descent = m_lineSpacing = m_lineExtraSpacing = 0;
  while (!segs.isEmpty())
  {
      segs.erase(0);
  }
  for (CList<textsegment>::iterator iter = rhs.segs.begin(); iter != rhs.segs.end(); )
  {
      CList<textsegment>::iterator next = iter;
      iter++;
      if (iter == rhs.segs.end() || iter->start > f)
      {
	  int st = next->start-f;
	  if (st < 0) st = 0;

	  CStyle _style = next->style;

	  segs.push_back(textsegment(st,next->style));
      }
  }
  for (i = f; rhs[i] != '\0'; i++) (*this)[i-f] = rhs[i];
  (*this)[i-f] = '\0';
  len = i;
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
  m_lineExtraSpacing = rhs.m_lineExtraSpacing;
  while (!segs.isEmpty())
  {
      segs.erase(0);
  }
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
  while (!segs.isEmpty())
  {
      segs.erase(0);
  }
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
    while (!segs.isEmpty())
    {
	segs.erase(0);
    }
    segs.push_back(textsegment(0,CStyle()));
    m_maxstyle = m_ascent = m_descent = m_lineSpacing = m_lineExtraSpacing = 0;
    m_bEof = false;
}

void CDrawBuffer::addch(tchar ch, CStyle _style/* = ucFontBase*/)
{
    if (len == 0)
    {
	segs.first().start = 0;
	segs.first().style = _style;
    }
    else if (_style != segs.last().style)
    {
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
	if (currentstyle.isPicture())
	{
	    currentx += currentstyle.getPicture()->width();
	}
	else
	{
	    if (currentstyle.isMono() && !fc->hasCourier())
	    {
		int cw = (7*fc->getsize(currentstyle))/10;
		currentx += cw*(end-textstart->start);
	    }
	    else
	    {
		QFont f(currentstyle.isMono() ? QString("courier") : fc->name(), fc->getsize(currentstyle), (currentstyle.isBold()) ? QFont::Bold : QFont::Normal, (currentstyle.isItalic()) );
//	    f.setUnderline(currentstyle.isUnderline());
		QString str = text.mid(textstart->start, end-textstart->start);
		QFontMetrics fm(f);
		currentx += fm.width(str);
	    }
	}
	textstart = textend;
    }
    while (textend != segs.end() && end != numchars);
    return currentx;
}

int CDrawBuffer::leftMargin()
{
    return (segs.begin()->style.getLeftMargin()*fc->getsize(segs.begin()->style))/6;
}

int CDrawBuffer::rightMargin()
{
    return (segs.begin()->style.getRightMargin()*fc->getsize(segs.begin()->style))/6;
}

int CDrawBuffer::offset(int scwidth)
{
    int currentx = BORDER;
    switch(segs.begin()->style.getJustify())
    {
	case m_AlignRight:
	{
	    currentx = scwidth - BORDER - rightMargin() - width();
	}
	break;
	case m_AlignCentre:
	{
	    currentx = (
		scwidth +
		leftMargin() - rightMargin()
		- width())/2;
	}
	break;
	case m_AlignJustify:
	case m_AlignLeft:
	    currentx = BORDER + leftMargin();
	    break;
    }
    return currentx;
}

void CDrawBuffer::render(QPainter* _p, int _y, bool _bMono, int _charWidth, int scwidth)
{
    int currentx = offset(scwidth);
    QString text = toQString(data());
    CList<textsegment>::iterator textstart = segs.begin();
/*
    StyleType align = textstart->style.getJustify();
    switch (align)
    {
	case CStyle::m_AlignRight:
	{
	    currentx = scwidth - width() - 2*BORDER;
	}
	break;
	case CStyle::m_AlignCentre:
	{
	    currentx = (scwidth - width())/2 - BORDER;
	}
	break;
	case CStyle::m_AlignJustify:
	case CStyle::m_AlignLeft:
	    break;
    }
*/
    CList<textsegment>::iterator textend = textstart;
    do
    {
	textend++;
	int end = (textend != segs.end()) ? textend->start : length();
	CStyle currentstyle = textstart->style;
	QFont f((currentstyle.isMono() && fc->hasCourier()) ? QString("courier") : fc->name(), fc->getsize(currentstyle), (currentstyle.isBold()) ? QFont::Bold : QFont::Normal, (currentstyle.isItalic()) );
//	f.setUnderline(currentstyle.isUnderline());
//	if (currentstyle.isUnderline()) qDebug("UNDERLINE");
	_p->setFont(f);
	QString str = text.mid(textstart->start, end-textstart->start);
#ifdef OPIE
	_p->setPen(QPen(QColor(currentstyle.Red(), currentstyle.Green(), currentstyle.Blue()), fc->getsize(currentstyle)/100));
#else
	_p->setPen(QPen(QColor(currentstyle.Red(), currentstyle.Green(), currentstyle.Blue()), fc->getsize(currentstyle)/10));
#endif
	if (_bMono)
	{
	    if (currentstyle.isUnderline())
	    {
		_p->drawLine( currentx, _y, currentx + str.length()*_charWidth, _y);
	    }
	    if (currentstyle.isStrikethru())
	    {
		int ascent = fc->ascent(currentstyle)/3;
		_p->drawLine( currentx, _y-ascent, currentx + str.length()*_charWidth, _y-ascent);
	    }
	    for (int i = 0; i < str.length(); i++)
	    {
		_p->drawText( currentx + i*_charWidth, _y, QString(str[i]));
	    }
	    currentx += str.length()*_charWidth;
	}
	else
	{
	    if (currentstyle.isPicture())
	    {
		int ascent = fc->ascent(currentstyle)/2;
		int yoffset = currentstyle.getPicture()->height()/2 + ascent;
		_p->drawPixmap( currentx, _y-yoffset, *(currentstyle.getPicture()));
		currentx += currentstyle.getPicture()->width();
	    }
	    else
	    {
		if (currentstyle.isMono() && !fc->hasCourier())
		{
		    int cw = (7*fc->getsize(currentstyle))/10;
		    int w = cw*(end-textstart->start);
		    if (currentstyle.isUnderline())
		    {
			_p->drawLine( currentx, _y, currentx + w, _y);
		    }
		    if (currentstyle.isStrikethru())
		    {
			int ascent = fc->ascent(currentstyle)/3;
			_p->drawLine( currentx, _y-ascent, currentx + w, _y-ascent);
		    }
		    QString str = text.mid(textstart->start, end-textstart->start);

		    for (int i = 0; i < str.length(); i++)
		    {
			_p->drawText( currentx, _y, QString(str[i]));
			currentx += cw;
		    }
		}
		else
		{
		    QFontMetrics fm(f);
		    int w = fm.width(str);
		    if (currentstyle.isUnderline())
		    {
			_p->drawLine( currentx, _y, currentx + w, _y);
		    }
		    if (currentstyle.isStrikethru())
		    {
			int ascent = fc->ascent(currentstyle)/3;
			_p->drawLine( currentx, _y-ascent, currentx + w, _y-ascent);
		    }
		    _p->drawText( currentx, _y, str);
		    currentx += w;
		}
	    }
	}
	textstart = textend;
    }
    while (textend != segs.end() && textstart->start < length()-1);
}

CStyle CDrawBuffer::laststyle()
{
    return segs.last().style;
}

linkType CDrawBuffer::getLinkType(int numchars, size_t& tgt)
{
    int end = 0;
    CStyle currentstyle;
    CList<textsegment>::iterator textstart = segs.begin();
    CList<textsegment>::iterator textend = textstart;
    do
    {
	textend++;
	end = (textend != segs.end()) ? textend->start : length();
	currentstyle = textstart->style;
/*
	if (currentstyle.isPicture()) qDebug("Passed thru picture");
	if (currentstyle.getLink()) qDebug("Passed thru link");
	qDebug("islink:%d - %d", numchars, end);
*/
	textstart = textend;
    }
    while (textend != segs.end() && end <= numchars);
//    if (currentstyle.isPicture()) qDebug("Clicked on picture");
    if (currentstyle.getPictureLink())
    {
	tgt = currentstyle.getPictureLinkData();
	return ePicture;
    }
    if (currentstyle.getLink())
    {
	tgt = currentstyle.getData();
	return eLink;
    }
    return eNone;
}

void CDrawBuffer::resize()
{
    int i;
    m_maxstyle = m_ascent = m_descent = m_lineSpacing = m_lineExtraSpacing = 0;
    for (CList<textsegment>::iterator iter = segs.begin(); iter != segs.end() && iter->start <= length(); )
    {
	CList<textsegment>::iterator next = iter;
	iter++;
	int st = next->start;
	if (st < 0) st = 0;

	CStyle _style = next->style;

	int linespacing, ascent, descent, extra;

	ascent = fc->ascent(_style);
	descent = fc->descent(_style);
	linespacing = fc->lineSpacing(_style);
	extra = linespacing - ascent - descent;
	if (_style.isPicture())
	{
	    descent = (_style.getPicture()->height()-ascent)/2;
	    ascent = (_style.getPicture()->height()+ascent)/2;
	}
/*
	else if (fc != NULL)
	{
	    ascent = fc->ascent(_style);
	    descent = fc->descent(_style);
	    linespacing = fc->lineSpacing(_style);
	    extra = linespacing - ascent - descent;
	}
*/
	if (ascent > m_ascent) m_ascent = ascent;
	if (descent > m_descent) m_descent = descent;
	if (extra > m_lineExtraSpacing) m_lineExtraSpacing = extra;
	m_lineSpacing = m_ascent+m_descent+m_lineExtraSpacing;
    }
}
