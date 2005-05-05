#include "CDrawBuffer.h"
#include "FontControl.h"
#include <qfontmetrics.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>

CDrawBuffer::~CDrawBuffer()
{
    while (!segs.isEmpty()) segs.erase(0);
}

void CDrawBuffer::setright(CDrawBuffer& rhs, int f)
{
  int i;
  len = rhs.len;
  fc = rhs.fc;
  m_ascent = m_descent = m_lineSpacing = m_lineExtraSpacing = 0;
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
//  //qDebug("Trying 2");
  len = rhs.len;
  m_ascent = rhs.m_ascent;
  m_descent = rhs.m_descent;
  m_lineSpacing = rhs.m_lineSpacing;
  m_lineExtraSpacing = rhs.m_lineExtraSpacing;
  m_showPartial = rhs.m_showPartial;
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
//  //qDebug("Tried 2");
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
    m_bSop = false;
    m_bEop = false;
    m_bBop = false;
    len = 0;
    (*this)[0] = 0;
    while (!segs.isEmpty())
    {
	segs.erase(0);
    }
    segs.push_back(textsegment(0,CStyle()));
    m_ascent = m_descent = m_lineSpacing = m_lineExtraSpacing = 0;
    m_bEof = false;
}

void CDrawBuffer::setendpara(const CStyle& _style/* = ucFontBase*/)
{
   m_bEop = true;
   if (len == 0)
    {
	segs.first().start = 0;
	segs.first().style = _style;
    }
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

int CDrawBuffer::width(int availht, int numchars, bool onscreen, int scwidth, unsigned short _lborder, unsigned short _rborder)
{
    int gzoom = fc->gzoom();
    int currentx = 0, end = 0;
    QString text = (numchars < 0) ? toQString(data()) : toQString(data(), numchars);
    CList<textsegment>::iterator textstart = segs.begin();
    int extraspace = 0;
    bool just = (onscreen && !m_bEop && textstart->style.getJustify() == m_AlignJustify);
    int spaces = 0;
    int spacesofar = 0;
    int spacenumber = 0;
    int nonspace = 0;
    if (just)
    {
	for (int i = 0; i < len; i++)
	{
	    if ((*this)[i] != ' ')
	    {
		nonspace = i;
		break;
	    }
	}
#ifdef _WINDOWS
	for (i = nonspace; i < len; i++)
#else
	for (int i = nonspace; i < len; i++)
#endif
	{
	    if ((*this)[i] == ' ')
	    {
		spaces++;
	    }
	}
	if (spaces == 0)
	{
	    just = false;
	}
	else
	{
	    extraspace = (scwidth - (_lborder+_rborder) - rightMargin() - leftMargin() - width(availht));
	    if (extraspace == 0) just = false;
	}
    }
    CList<textsegment>::iterator textend = textstart;
    do
    {
	textend++;
	end = (textend != segs.end()) ? textend->start : len;
	if (numchars >= 0 && end > numchars)
	{
	    end = numchars;
	}
	CStyle currentstyle = textstart->style;
	if (currentstyle.isPicture())
	{
	    if (currentstyle.canScale())
	    {
	      int ht = (gzoom*currentstyle.getPicture()->height())/100;
	      int wt = (gzoom*currentstyle.getPicture()->width())/100;
	      if (ht > availht)
		{
		  wt = (wt*availht)/ht;
		}
	      currentx += wt;
	    }
	    else
	    {
		currentx += currentstyle.getPicture()->width();
	    }
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
		QFont f(currentstyle.isMono() ? QString(fc->fixedfontname()) : fc->name(), fc->getsize(currentstyle), (currentstyle.isBold()) ? QFont::Bold : QFont::Normal, (currentstyle.isItalic()) );
//	    f.setUnderline(currentstyle.isUnderline());
		QString str = text.mid(textstart->start, end-textstart->start);
		QFontMetrics fm(f);
		if (just)
		{
		    int lastspace = -1;
		    int nsp = 0;
		    int cx = currentx;
		    while ((nsp = str.find(" ", lastspace+1)) >= 0)
		    {
			if (nsp > nonspace)
			{
			    spacenumber++;
			    int nexttoadd = (extraspace*spacenumber+spaces/2)/spaces - spacesofar;
			    QString nstr = str.mid(lastspace+1, nsp-lastspace);
			    int lw = fm.width(nstr);
			    cx += lw+nexttoadd;
			    spacesofar += nexttoadd;
			    lastspace = nsp;
			}
			else
			{
			    QString nstr = str.mid(lastspace+1, nsp-lastspace);
//			    qDebug("str:%s: last:%d new:%d nstr:%s:", (const char*)str, lastspace, nsp, (const char*)nstr);
			    int lw = fm.width(nstr);
			    cx += lw;
			    lastspace = nsp;
			}
		    }
		    QString nstr = str.right(str.length()-1-lastspace);
		    cx += fm.width(nstr);
		    currentx = cx;
		}
		else
		{
		    currentx += fm.width(str);
		}
	    }
	}
	textstart = textend;
    }
    while (textend != segs.end() && end != numchars && textstart->start < len);
    return currentx;
}

int CDrawBuffer::charwidth(int numchars, CStyle& currentstyle)
{
    if (currentstyle.isPicture())
      {
	int gzoom = fc->gzoom();
	if (currentstyle.canScale())
	  {
	    return (gzoom*currentstyle.getPicture()->width())/100;
	  }
	else
	  {
	    return currentstyle.getPicture()->width();
	  }
      }
    else
      {
	if (currentstyle.isMono() && !fc->hasCourier())
	  {
	    return (7*fc->getsize(currentstyle))/10;
	  }
	else
	  {
	    QString text = QChar((data())[numchars]);
	    QFont f(currentstyle.isMono() ? QString(fc->fixedfontname()) : fc->name(), fc->getsize(currentstyle), (currentstyle.isBold()) ? QFont::Bold : QFont::Normal, (currentstyle.isItalic()) );
	    //	    f.setUnderline(currentstyle.isUnderline());
	    QFontMetrics fm(f);
	    return fm.width(text);
	  }
      }
    return 0;
}

int CDrawBuffer::charwidth(int numchars)
{
    int gzoom = fc->gzoom();
    int end = 0;
    QString text = QChar((data())[numchars]);
    CList<textsegment>::iterator textstart = segs.begin();
    CList<textsegment>::iterator textend = textstart;
    do
    {
	textend++;
	end = (textend != segs.end()) ? textend->start : len;
	if (end <= numchars)
	{
	    textstart = textend;
	    continue;
	}
	if (numchars >= 0 && end > numchars)
	{
	    end = numchars;
	}
	CStyle currentstyle = textstart->style;
	if (currentstyle.isPicture())
	{
	    if (currentstyle.canScale())
	    {
		return (gzoom*currentstyle.getPicture()->width())/100;
	    }
	    else
	    {
		return currentstyle.getPicture()->width();
	    }
	}
	else
	{
	    if (currentstyle.isMono() && !fc->hasCourier())
	    {
		return (7*fc->getsize(currentstyle))/10;
	    }
	    else
	    {
		QFont f(currentstyle.isMono() ? QString(fc->fixedfontname()) : fc->name(), fc->getsize(currentstyle), (currentstyle.isBold()) ? QFont::Bold : QFont::Normal, (currentstyle.isItalic()) );
//	    f.setUnderline(currentstyle.isUnderline());
		QFontMetrics fm(f);
		return fm.width(text);
	    }
	}
	textstart = textend;
    }
    while (textend != segs.end() /* && end != numchars*/ && textstart->start < len);
    return 0;
}



int CDrawBuffer::leftMargin()
{
    return (segs.begin()->style.getLeftMargin()*fc->getsize(segs.begin()->style)+3)/6;
}

int CDrawBuffer::rightMargin()
{
    return (segs.begin()->style.getRightMargin()*fc->getsize(segs.begin()->style)+3)/6;
}

int CDrawBuffer::offset(int scwidth, unsigned short _lborder, unsigned short _rborder, int availht)
{
    int currentx = _lborder;
    switch(segs.begin()->style.getJustify())
    {
	case m_AlignRight:
	{
	    currentx = scwidth - _rborder - rightMargin() - width(availht);
	}
	break;
	case m_AlignCentre:
	{
	    currentx = (
		scwidth - _lborder -_rborder +
		leftMargin() - rightMargin()
		- width(availht))/2 + _lborder;
	}
	break;
	case m_AlignJustify:
	case m_AlignLeft:
	case m_AlignNone:
	    currentx = _lborder + leftMargin();
	    break;
    }
    return currentx;
}

void CDrawBuffer::render(QPainter* _p, int _y, bool _bMono, int _charWidth, int scwidth, unsigned short _lborder, unsigned short _rborder, const QColor& _bg, int availht)
{
    CList<textsegment>::iterator textstart = segs.begin();
   QColor paperColour = QColor(qRgb(textstart->style.pRed(),
				    textstart->style.pGreen(),
				    textstart->style.pBlue()));

   Highlight(_p, !(paperColour == _bg), _lborder, _y, scwidth-(_lborder+_rborder), paperColour);
   //   Highlight(_p, true, _lborder, _y, scwidth-(_lborder+_rborder), paperColour);
    int gzoom = fc->gzoom();
    int currentx = offset(scwidth, _lborder, _rborder, availht);
   if (!m_hastext)
     {
	for (CList<textsegment>::iterator iter = segs.begin(); iter != segs.end(); iter++)
	  {
	     CStyle currentstyle = iter->style;
	     if (currentstyle.isPicture())
	       {
		  CStyle currentstyle = segs.begin()->style;
		  int ht = (gzoom*currentstyle.getPicture()->height())/100;
		  int wt = (gzoom*currentstyle.getPicture()->width())/100;
		  bool willscale = false;
		  if (ht > availht)
		    {
		      wt = (wt*availht)/ht;
		      ht = availht;
		      willscale = true;
		    }

		  QPixmap pc;
		  if ((willscale || gzoom != 100) && currentstyle.canScale())
		    {
		       QImage im = currentstyle.getPicture()->smoothScale(wt,ht);
		       pc.convertFromImage(im);
		    }
		  else
		    {
		       pc.convertFromImage(*currentstyle.getPicture());
		    }
		  int yoffset = pc.height()/2;
		  _p->drawPixmap( currentx, _y-yoffset, pc );
		  currentx += pc.width();
	       }
	  }	  
	return;
     }
    QString text = toQString(data());
    int extraspace = 0;
    bool just = (!m_bEop && textstart->style.getJustify() == m_AlignJustify);
    int spaces = 0;
    int spacesofar = 0;
    int spacenumber = 0;
    int nonspace = 0;
    if (just)
    {
	for (int i = 0; i < len; i++)
	{
	    if ((*this)[i] != ' ')
	    {
		nonspace = i;
		break;
	    }
	}
#ifdef _WINDOWS
	for (i = nonspace; i < len; i++)
#else
	for (int i = nonspace; i < len; i++)
#endif
	{
	    if ((*this)[i] == ' ')
	    {
		spaces++;
	    }
	}
	if (spaces == 0)
	{
	    just = false;
	}
	else
	{
	    extraspace = (scwidth - (_lborder+_rborder) - rightMargin() - leftMargin() - width(availht));
	    if (extraspace == 0) just = false;
	}
    }
    CList<textsegment>::iterator textend = textstart;

   bool drawBackground = false;
    do
    {
	textend++;
	int end = (textend != segs.end()) ? textend->start : len;
	CStyle currentstyle = textstart->style;
	/*
	bool drawBackground = (
			       currentstyle.bRed() != 255
			       ||
			       currentstyle.bGreen() != 255
			       ||
			       currentstyle.bBlue() != 255
			       );
	*/
	QColor bgColour = QColor(qRgb(currentstyle.bRed(),
					      currentstyle.bGreen(),
					      currentstyle.bBlue()));
	drawBackground = !(bgColour == _bg);

//       if (drawBackground) qDebug("Drawing background");
       
	QFont f((currentstyle.isMono() && fc->hasCourier()) ? fc->fixedfontname() : fc->name(), fc->getsize(currentstyle), (currentstyle.isBold()) ? QFont::Bold : QFont::Normal, (currentstyle.isItalic()) );
//	f.setUnderline(currentstyle.isUnderline());
//	if (currentstyle.isUnderline()) qDebug("UNDERLINE");
	_p->setFont(f);
	QString str = text.mid(textstart->start, end-textstart->start);
#if defined(OPIE) || !defined(USEQPE)
	_p->setPen(QPen(QColor(currentstyle.Red(), currentstyle.Green(), currentstyle.Blue()), fc->getsize(currentstyle)/10/*0*/));
#else
	_p->setPen(QPen(QColor(currentstyle.Red(), currentstyle.Green(), currentstyle.Blue()), fc->getsize(currentstyle)/10));
#endif
	int voffset = currentstyle.getVOffset()*fc->getsize(currentstyle)/2;

	if (_bMono)
	{

	  Highlight(_p, drawBackground, currentx, _y, str.length()*_charWidth, bgColour);
	    if (currentstyle.isUnderline())
	    {
		_p->drawLine( currentx, _y+voffset, currentx + str.length()*_charWidth, _y+voffset);
	    }
	    if (currentstyle.isStrikethru())
	    {
		int ascent = fc->ascent(currentstyle)/3;
		_p->drawLine( currentx, _y-ascent+voffset, currentx + str.length()*_charWidth, _y-ascent+voffset);
	    }
	    for (int i = 0; i < str.length(); i++)
	    {
		_p->drawText( currentx + i*_charWidth, _y+voffset, QString(str[i]));
	    }
	    currentx += str.length()*_charWidth;
	}
	else
	{
	    if (currentstyle.isPicture())
	    {
		int ht = (gzoom*currentstyle.getPicture()->height())/100;
		int wt = (gzoom*currentstyle.getPicture()->width())/100;
		  bool willscale = false;
		  if (ht > availht)
		    {
		      wt = (wt*availht)/ht;
		      ht = availht;
		      willscale = true;
		    }
		int ascent = fc->ascent(currentstyle)/2;
		int yoffset = ht/2 + ascent;

		QPixmap pc;
		if ((willscale || gzoom != 100) && currentstyle.canScale())
		{
		    QImage im = currentstyle.getPicture()->smoothScale(wt,ht);
		    pc.convertFromImage(im);
		}
		else
		{
		    pc.convertFromImage(*currentstyle.getPicture());
		}
		_p->drawPixmap( currentx, _y-yoffset, pc );
		currentx += wt;
	    }
	    else
	    {
		if (currentstyle.isMono() && !fc->hasCourier())
		{
		    int cw = (7*fc->getsize(currentstyle))/10;
		    int w = cw*(end-textstart->start);
		    Highlight(_p, drawBackground, currentx, _y, w, bgColour);
		    if (currentstyle.isUnderline())
		    {
			_p->drawLine( currentx, _y+voffset, currentx + w, _y+voffset);
		    }
		    if (currentstyle.isStrikethru())
		    {
			int ascent = fc->ascent(currentstyle)/3;
			_p->drawLine( currentx, _y-ascent+voffset, currentx + w, _y-ascent+voffset);
		    }
		    QString str = text.mid(textstart->start, end-textstart->start);

		    for (unsigned int i = 0; i < str.length(); i++)
		    {
#ifdef _WINDOWS
			_p->drawText( currentx, _y+voffset, QString(str.at(i)));
#else
			_p->drawText( currentx, _y+voffset, QString(str[i]));
#endif
			currentx += cw;
		    }
		}
		else
		{
		    QFontMetrics fm(f);
		    int w;
		    if (just)
		    {
			int lastspace = -1;
			int nsp = 0;
			int cx = currentx;
			while ((nsp = str.find(" ", lastspace+1)) >= 0)
			{
			    if (nsp+textstart->start >= nonspace)
			    {
				spacenumber++;
				int nexttoadd = (extraspace*spacenumber+spaces/2)/spaces - spacesofar;
				QString nstr = str.mid(lastspace+1, nsp-lastspace);
//			    qDebug("str:%s: last:%d new:%d nstr:%s:", (const char*)str, lastspace, nsp, (const char*)nstr);
				int lw = fm.width(nstr);
				Highlight(_p, drawBackground, cx, _y, lw, bgColour);
				_p->drawText( cx, _y+voffset, nstr);
				Highlight(_p, drawBackground, cx+lw, _y, nexttoadd, bgColour);
				cx += lw+nexttoadd;
				spacesofar += nexttoadd;
				lastspace = nsp;
			    }
			    else
			    {
				QString nstr = str.mid(lastspace+1, nsp-lastspace);
//			    qDebug("str:%s: last:%d new:%d nstr:%s:", (const char*)str, lastspace, nsp, (const char*)nstr);
				int lw = fm.width(nstr);
				Highlight(_p, drawBackground, cx, _y, lw, bgColour);
				_p->drawText( cx, _y+voffset, nstr);
				cx += lw;
				lastspace = nsp;
			    }
			}
			QString nstr = str.right(str.length()-1-lastspace);
			int lw = fm.width(nstr);
			Highlight(_p, drawBackground, cx, _y, lw, bgColour);
			_p->drawText( cx, _y+voffset, nstr);
			cx += lw;
			w = cx - currentx;
		    }
		    else
		    {
			int lw = fm.width(str);
			Highlight(_p, drawBackground, currentx, _y, lw, bgColour);
			_p->drawText( currentx, _y+voffset, str);
			w = lw;
		    }
		    if (currentstyle.isUnderline())
		    {
			_p->drawLine( currentx, _y+voffset, currentx + w, _y+voffset);
		    }
		    if (currentstyle.isStrikethru())
		    {
			int ascent = fc->ascent(currentstyle)/3;
			_p->drawLine( currentx, _y-ascent+voffset, currentx + w, _y-ascent+voffset);
		    }
		    currentx += w;
		}
	    }
	}
	textstart = textend;
    }
    while (textend != segs.end() && textstart->start < len);
/*
 if (firstColour ==  bgColour)
     {
	Highlight(_p, drawBackground, currentx, _y, scwidth - (currentx + _lborder + _rborder), bgColour);
     }
 */
}

void CDrawBuffer::Highlight(QPainter* _p, bool drawBackground, int _x, int _y, int w, QColor bgColour)
{
  if (drawBackground)
    {
      _p->fillRect(_x, _y-m_ascent, w, m_lineSpacing, bgColour);
    }
}

CStyle CDrawBuffer::laststyle()
{
    return segs.last().style;
}

CStyle CDrawBuffer::firststyle()
{
    return segs.first().style;
}

linkType CDrawBuffer::getLinkType(int numchars, size_t& tgt, size_t& offset, size_t& pictgt, QImage*& img, size_t& tabletgt)
{
  linkType ret = eNone;
    int end = 0;
    CStyle currentstyle;
    CList<textsegment>::iterator textstart = segs.begin();
    CList<textsegment>::iterator textend = textstart;
    do
    {
	textend++;
	end = (textend != segs.end()) ? textend->start : len;
	currentstyle = textstart->style;
/*
	if (currentstyle.isPicture()) qDebug("Passed thru picture");
	if (currentstyle.getLink()) qDebug("Passed thru link");
	//qDebug("islink:%d - %d", numchars, end);
*/
	textstart = textend;
    }
    while (textend != segs.end() && end <= numchars);
    img = currentstyle.getPicture();
    if (currentstyle.getPictureLink())
    {
	pictgt = currentstyle.getPictureLinkData();
	ret |= ePicture;
    }
    if (currentstyle.getLink())
    {
	tgt = currentstyle.getData();
	offset = currentstyle.getOffset();
	ret |= eLink;
    }
    if (currentstyle.isTable())
      {
	tabletgt = currentstyle.getTable();
	ret |= eTable;
      }	
    return ret;
}

void CDrawBuffer::resize(int availht)
{
    int gzoom = fc->gzoom();
    m_ascent = m_descent = m_lineSpacing = m_lineExtraSpacing = 0;
    /*
    int t_ascent = 0;
    int t_descent = 0;
    int t_lineExtraSpacing = 0;
    */
    int t_lineSpacing = 0;
   m_hastext = false;
    for (CList<textsegment>::iterator iter = segs.begin(); iter != segs.end() && iter->start <= len; )
    {
       CStyle _style = iter->style;
       if (!_style.isPicture())
	 {
	    m_hastext = true;
	    break;
	 }
       iter++;
    }
   if (m_hastext)
     {
       int p_linespacing = 0;
	for (CList<textsegment>::iterator iter = segs.begin(); iter != segs.end() && iter->start <= len; )
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
	     /*
	      if (ascent > t_ascent) t_ascent = ascent;
	      if (descent > t_descent) t_descent = descent;
	      if (extra > t_lineExtraSpacing) t_lineExtraSpacing = extra;
	      t_lineSpacing = t_ascent+t_descent+t_lineExtraSpacing;
	      */
	     if (linespacing > t_lineSpacing) t_lineSpacing = linespacing;
	     if (_style.isPicture())
	       {
		  int ht = (gzoom*_style.getPicture()->height())/100;
		  int wt = (gzoom*_style.getPicture()->width())/100;

		  bool willscale = false;
		  if (ht > availht)
		    {
		      wt = (wt*availht)/ht;
		      ht = availht;
		      willscale = true;
		    }
		  if (willscale || _style.canScale())
		    {
		       descent = ((gzoom*_style.getPicture()->height())/100-ascent)/2;
		       ascent = ((gzoom*_style.getPicture()->height())/100+ascent)/2;
		    }
		  else
		    {
		       descent = (_style.getPicture()->height()-ascent)/2;
		       ascent = (_style.getPicture()->height()+ascent)/2;
		    }
		  int lineSpacing = ascent+descent;
		  if (lineSpacing > p_linespacing)
		    {
		      p_linespacing = lineSpacing;
		    }
		  extra = 0;
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
	m_showPartial = (p_linespacing > t_lineSpacing);
	int lead = fc->getlead();
	if (lead != 0)
	  {
	     int xt = (lead*t_lineSpacing+5)/10;
	     m_lineExtraSpacing += xt;
	     m_lineSpacing += xt;
	  }
	if (m_bSop)
	  {
	     int xt = ((segs.begin()->style.getExtraSpace()+fc->getextraspace())*fc->getsize(segs.begin()->style)+5)/10;
	     //	qDebug("ExtraSpace:%d", xt);
	     m_ascent += xt;
	     m_lineSpacing += xt;
	  }
     }
   else
     {
	m_showPartial = true;
	int maxheight = 0;
	for (CList<textsegment>::iterator iter = segs.begin(); iter != segs.end(); iter++)
	  {
	     CStyle _style = iter->style;
	     if (_style.isPicture())
	       {
		  int ht = 0;
		  if (_style.canScale())
		    {
		       ht = ((gzoom*_style.getPicture()->height())/100);
		    }
		  else
		    {
		       ht = _style.getPicture()->height();
		    }
		  if (ht > availht)
		    {
		      ht = availht;
		    }
		  if (ht > maxheight) maxheight = ht;
	       }
	  }
	m_ascent = maxheight/2;
	m_descent = maxheight-m_ascent;
	m_lineExtraSpacing = 0;
	m_lineSpacing = m_ascent+m_descent;
     }
}

CStyle* CDrawBuffer::getNextLink(int& offset)
{
  CStyle* ret = NULL;
  for (CList<textsegment>::iterator iter = segs.begin(); iter != segs.end(); ++iter)
  {
    if (offset < iter->start && iter->style.getLink())
      {
	ret = &(iter->style);
	offset = iter->start;
	qDebug("Found a link at offset %d!", offset);
	break;
      }
  }
  return ret;
}

int CDrawBuffer::invertLink(int offset)
{
  for (CList<textsegment>::iterator iter = segs.begin(); iter != segs.end(); ++iter)
  {
    if (offset <= iter->start && iter->style.getLink())
      {
	iter->style.invert();
	unsigned long currentvalue = iter->style.getData();
	qDebug("Link data %u", currentvalue);
	CList<textsegment>::iterator next = iter;
	++next;
	while (next != segs.end() && (currentvalue == next->style.getData()) && next->style.getLink())
	  {
	    next->style.invert();
	    offset = next->start;
	    iter++;
	    next++;
	  }
	return offset;
      }
  }
  qDebug("Failed to invert");
  return offset;
}
