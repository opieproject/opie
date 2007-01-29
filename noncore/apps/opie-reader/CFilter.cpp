#include <qmap.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qdir.h>

#ifdef USEQPE
#include <qpe/global.h>
#endif
#include "CDrawBuffer.h"
#include "CFilter.h"
#include "hrule.h"

#include <qregexp.h>
#include <qimage.h>
#include <qpixmap.h>
//#include <qprogressdialog.h>
//#include <qapplication.h>

void textfmt::mygetch(tchar& ch, CStyle& sty, unsigned long& pos)
{
    if (uselast)
    {
	ch = lastchar;
	uselast = false;
    }
    else
    {
	parent->getch(ch, sty, pos);
    }
}

void textfmt::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
    mygetch(ch, sty, pos);
    do
    {
	sty = currentstyle;
	switch (ch)
	{
	    case 10:
		currentstyle.unset();
		sty = currentstyle;
		break;
// Use this if you want to replace -- by em-dash
	    case '-':
//		parent->getch(ch, sty);
		mygetch(ch, sty, pos);
		if (ch == '-')
		{
		    ch = 0x2014;
		}
		else
		{
		    lastchar = ch;
		    uselast = true;
		    ch = '-';
		}
		break;
	    case '*':
		if (currentstyle.isBold())
		{
// Already bold - time to turn it off?
// The next two lines ensure that * follows a character but it works better without
//		    QChar c(lastchar);
//		    if ((lastchar != '*') && (c.isPunct() || c.isLetterOrNumber()))
		    if (lastchar != '*')
		    {
			currentstyle.unsetBold();
			CStyle dummy;
//			parent->getch(ch, dummy);
			mygetch(ch, dummy, pos);
		    }
		}
		else
		{
// not bold - time to turn it on?
		    CStyle dummy;
//		    parent->getch(ch, dummy);
		    mygetch(ch, dummy, pos);
		    QChar c(ch);
		    if ((ch != '*') && (c.isPunct() || c.isLetterOrNumber()))
		    {
			currentstyle.setBold();
		    }
		    else
		    {
			lastchar = ch;
			uselast = true;
			ch = '*';
		    }
		    
		}
		break;
	    case '_':
		if (currentstyle.isItalic())
		{
// Already bold - time to turn it off?
// The next two lines ensure that * follows a character but it works better without
//		    QChar c(lastchar);
//		    if ((lastchar != '_') && (c.isPunct() || c.isLetterOrNumber()))
		    if (lastchar != '_')
		    {
			currentstyle.unsetItalic();
			CStyle dummy;
//			parent->getch(ch, dummy);
			mygetch(ch, dummy, pos);
		    }
		}
		else
		{
// not bold - time to turn it on?
		    CStyle dummy;
//		    parent->getch(ch, dummy);
		    mygetch(ch, dummy, pos);
		    QChar c(ch);
		    if ((ch != '_') && (c.isPunct() || c.isLetterOrNumber()))
		    {
			currentstyle.setItalic();
		    }
		    else
		    {
			lastchar = ch;
			uselast = true;
			ch = '_';
		    }
		    
		}
		break;
	}
    }
    while (sty != currentstyle);
    if (!uselast) lastchar = ch;
    return;
}

void remap::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
    if (q[offset] != 0)
    {
	q[offset++];
	sty = currentstyle;
	return;
    }
    parent->getch(ch, sty, pos);
    switch (ch)
    {
	case 0x201a:
	    ch = '\'';
	    break;
	case 0x0192:
	    ch = 'f';
	    break;
	case 0x201e:
	    ch = '"';
	    break;
	case 0x2026:
	    offset = 0;
	    q[0] = '.';
	    q[1] = '.';
	    q[2] = 0;
	    ch = '.'; // should be ...
	    break;
	case 0x0160:
	    ch = 'S';
	    break;
	case 0x2039:
	    ch = '<';
	    break;
	case 0x0152:		
	    offset = 0;
	    q[0] = 'E';
	    q[1] = 0;
	    ch = 'O';
	    break;
	case 0x017d:
	    ch = 'Z';
	    break;
	case 0x2018:
	    ch = '\'';
	    break;
	case 0x2019:
	    ch = '\'';
	    break;
	case 0x201c:
	    ch = '"';
	    break;
	case 0x201d:
	    ch = '"';
	    break;
	case 0x2022:
	    ch = '>';
	    break;
	case 0x2013:
	    ch = '-';
	    break;
	case 0x2014:
	    offset = 0;
	    q[0] = '-';
	    q[1] = 0;
	    ch = '-'; // should be --
	    break;
	case 0x02dc:
	    ch = '~';
	    break;
	case 0x0161:
	    ch = 's';
	    break;
	case 0x203a:
	    ch = '>';
	    break;
	case 0x0153:
	    offset = 0;
	    q[0] = 'e';
	    q[1] = 0;
	    ch = 'o';// should be oe
	    break;
	    /*
	case 0x0009: // tab
	    offset = 0;
	    q[0] = ' ';
	    q[1] = 0;
	    ch = ' ';
	    break;
	    */
	case 0x017e:
	    ch = 'z';
	    break;
	case 0x0178:
	    ch = 'Y';
	    break;
    }
    currentstyle = sty;
}

void PeanutFormatter::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
    CStyle dummy;
    currentstyle.setColour(0,0,0);
    parent->getch(ch, dummy, pos);
    while (ch == '\\')
    {
	parent->getch(ch, dummy, pos);
	if (ch == '\\') break;
	switch(ch)
	{
	    case 'a':
	    {
		int code = 0;
		for (int i = 0; i < 3; i++)
		{
		    parent->getch(ch, dummy, pos);
		    code = 10*code + ch - '0';
		}
		ch = code;
	    }
	    break;
	    case 'v':
	    {
		while (1)
		{
		    parent->getch(ch, dummy, pos);
		    if (ch == '\\')
		    {
			parent->getch(ch, dummy, pos);
			if (ch == 'v')
			{
			    parent->getch(ch, dummy, pos);
			    break;
			}
		    }
		}
	    }
	    break;
	    case 's':
	    case 'n':
		currentstyle.setFontSize(0);
		parent->getch(ch,dummy, pos);
		break;
	    case 'p':
		currentstyle.unset();
//		parent->getch(ch,dummy);
		ch = 10;
		break;
	    case 'l':
		if (currentstyle.getFontSize() == 1)
		{
		    currentstyle.setFontSize(0);
		}
		else
		{
		    currentstyle.setFontSize(1);
		}
		parent->getch(ch, dummy, pos);
		break;
	    case 'x':
		if (currentstyle.getFontSize() == 0)
		{
//		    currentstyle.unset();
//		    currentstyle.setBold();
		    currentstyle.setFontSize(1);
		}
		else
		{
		    currentstyle.unset();
		}
//		parent->getch(ch, dummy);
		ch = 10;
		break;
	    case 'i':
		if (currentstyle.isItalic())
		{
		    currentstyle.unsetItalic();
		}
		else
		{
		    currentstyle.setItalic();
		}
		parent->getch(ch, dummy, pos);
		break;
	    case 'b':
	    case 'B':
		if (currentstyle.isBold())
		{
		    currentstyle.unsetBold();
		}
		else
		{
		    currentstyle.setBold();
		}
		parent->getch(ch, dummy, pos);
		break;
	    case 'c':
		if (currentstyle.getJustify() == m_AlignCentre)
		{
		    currentstyle.setLeftJustify();
		}
		else
		{
		    currentstyle.setCentreJustify();
		}
		parent->getch(ch, dummy, pos);
		break;
	    case 'r':
		if (currentstyle.getJustify() == m_AlignRight)
		{
		    currentstyle.setLeftJustify();
		}
		else
		{
		    currentstyle.setRightJustify();
		}
		parent->getch(ch, dummy, pos);
		break;
	    default:
		currentstyle.setColour(255,0,0);
	}
    }
    sty = currentstyle;
}

void OnePara::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
    parent->getch(ch, sty, pos);
    if (m_lastchar == 10)
    {
	while (ch == 10) parent->getch(ch, sty, pos);
    }
    m_lastchar = ch;
}

void repalm::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
    parent->getch(ch, sty, pos);
    switch (ch)
    {
	case 0x80:
	    ch = 0x20ac;
	    break;
	case 0x82:
	    ch = 0x201a;
	    break;
	case 0x83:
	    ch = 0x0192;
	    break;
	case 0x84:
	    ch = 0x201e;
	    break;
	case 0x85:
	    ch = 0x2026;
	    break;
	case 0x86:
	    ch = 0x2020;
	    break;
	case 0x87:
	    ch = 0x2021;
	    break;
	case 0x88:
	    ch = 0x02c6;
	    break;
	case 0x89:
	    ch = 0x2030;
	    break;
	case 0x8a:
	    ch = 0x0160;
	    break;
	case 0x8b:
	    ch = 0x2039;
	    break;
	case 0x8c:
	    ch = 0x0152;
	    break;
/*
	case 0x8e:
	    ch = 0x017d;
	    break;
*/
	case 0x91:
	    ch = 0x2018;
	    break;
	case 0x92:
	    ch = 0x2019;
	    break;
	case 0x93:
	    ch = 0x201c;
	    break;
	case 0x94:
	    ch = 0x201d;
	    break;
	case 0x95:
	    ch = 0x2022;
	    break;
	case 0x96:
	    ch = 0x2013;
	    break;
	case 0x97:
	    ch = 0x2014;
	    break;
	case 0x98:
	    ch = 0x02dc;
	    break;
	case 0x99:
	    ch = 0x2122;
	    break;
	case 0x9a:
	    ch = 0x0161;
	    break;
	case 0x9b:
	    ch = 0x203a;
	    break;
	case 0x9c:
	    ch = 0x0153;
	    break;
	case 0x9e:
	    ch = 0x017e;
	    break;
	case 0x9f:
	    ch = 0x0178;
	    break;
	case 0x18:
	    ch = 0x2026;
	    break;
	case 0x19:
	    ch = 0x2007;
	    break;
	case 0x8d:
	    ch = 0x2662;
	    break;
	case 0x8e:
	    ch = 0x2663;
	    break;
	case 0x8f:
	    ch = 0x2661;
	    break;
	case 0x90:
	    ch = 0x2660;
	    break;
	default:
	    break;
    }
}

//static tchar nextpart[] = { 'C','l','i','c','k',' ','h','e','r','e',' ','f','o','r',' ','t','h','e',' ','n','e','x','t',' ','p','a','r','t',0 };
//static tchar prevpart[] = { 'C','l','i','c','k',' ','h','e','r','e',' ','f','o','r',' ','t','h','e',' ','p','r','e','v','i','o','u','s',' ','p','a','r','t',0 };

void DePluck::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
    if (m_buffed > 0)
    {
	sty = m_laststyle;
	ch = nextpart[m_current++];
	if (m_current == m_buffed)
	{
	    m_current = m_buffed = 0;
	}
    }
    else
    {
	if (m_buffer != 0)
	{
	    ch = m_buffer;
	    m_buffer = 0;
	    return;
	}
	unsigned long lnk = 0, lnkoff = 0;
	do
	{
	    if (nextpart[m_buffed] == 0) break;
	    parent->getch(ch, sty, pos);
	    m_laststyle = sty;
	    if (sty.getLink())
	      {
		lnk = sty.getData();
		lnkoff = sty.getOffset();
	      }
	} while (ch == nextpart[m_buffed] && sty.getLink() && ++m_buffed);
	m_current = 0;
	if (nextpart[m_buffed] == 0)
	{
	    m_buffed = 0;
	    QString dmy, dmy2;
	    parent->hyperlink(lnk, lnkoff, dmy, dmy2);
	    do
	    {
		parent->getch(ch, sty, pos);
	    }
	    while (ch != 10);
	    parent->getch(ch, sty, pos);
	}
	else if (m_buffed > 0)
	{
	    m_buffer = ch;
	    ch = nextpart[0];
	    if (m_buffed == 1)
	    {
		m_buffed = 0;
	    }
	    else m_current = 1;
	}
    }

    return;
}

HighlightFilter::HighlightFilter(QTReader* _p) : pReader(_p), lastpos(0), nextpos(0), red(255), green(255), blue(255)
{
}

#include "Bkmks.h"
#include "QTReader.h"

void HighlightFilter::refresh(unsigned long pos)
{
  bkmks = pReader->Bkmklist();
  
  red = green = blue = 255;
  
  if (bkmks == NULL)
    {
      lastpos = 0;
      nextpos = 0xffffffff;
    }
  else
    {
      lastpos = 0;
      nextpos = 0xffffffff;
      for (CList<Bkmk>::iterator i = bkmks->begin(); i != bkmks->end(); i++)
	{
	  if ((*i).value() <= pos && pos < (*i).value2())
	    {
	      red = i->red();
	      green = i->green();
	      blue = i->blue();
	      lastpos = (*i).value();
	      nextpos = (*i).value2();
	      break;
	    }
	  if ((*i).value() > pos)
	    {
	      nextpos = (*i).value();
	      break;
	    }
	  lastpos = (*i).value();
	}
    }
}

void HighlightFilter::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
  parent->getch(ch, sty, pos);
  if (bkmks != pReader->Bkmklist() || pos <= lastpos || pos >= nextpos)
    {
      //      qDebug("Recalc <%lu, %lu, %lu>", lastpos, pos, nextpos);
      refresh(pos);
      //      qDebug("Recalc(2) <%lu, %lu, %lu>", lastpos, pos, nextpos);
    }
   int r = sty.bRed(), g = sty.bGreen(), b = sty.bBlue();
   if (r == 255 && g == 255 && b == 255)
     {
	sty.setBackground(red, green, blue);
     }
}

void kern::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
  if (uselast)
    {
      ch = lastchar;
      sty = laststy;
      uselast = false;
      return;
    }
  else
    {
      parent->getch(ch, sty, pos);
    }
  switch (ch)
    {
    case 'f':
      {
	tchar savedchar = 'f';
	parent->getch(ch, sty, pos);
	switch (ch)
	  {
	  case 'i':
	    ch = (251 << 8) + 1;
	    break;
	  case 'l':
	    ch = (251 << 8) + 2;
	    break;
	  default:
	    lastchar = ch;
	    uselast = true;
	    laststy = sty;
	    ch = savedchar;
	  }
      }
      break;
    default:
      break;
    }
}

class ErrorFilter : public CFilter
{
  QString error;
  int currentpos;
 public:
  ErrorFilter(const QString& _s) : error(_s), currentpos(0) {}
  ~ErrorFilter() {}
  void getch(tchar& ch, CStyle& sty, unsigned long& pos)
  {
    if (currentpos == error.length())
      {
	ch = UEOF;
	currentpos = 0;
      }
    else
      {
	ch = error[currentpos++].unicode();
      }
  }
  QString about() { return parent->about(); }
};

#ifndef __STATIC
ExternFilter::ExternFilter(const QString& nm, const QString& optional) : filt(NULL), handle(NULL)
{
#ifdef USEQPE
#ifdef OPIE
  QString filterpath(getenv("OPIEDIR"));
#else
  QString filterpath(getenv("QTDIR"));
#endif
  filterpath += "/plugins/reader/filters/lib";
#else
  QString filterpath(getenv("READERDIR"));
  filterpath += "/filters/lib";
#endif
  filterpath += nm;
  filterpath += ".so";
  if (QFile::exists(filterpath))
    {
      qDebug("Filter:%s", (const char*)filterpath);
      handle = dlopen(filterpath, RTLD_LAZY);
      if (handle == 0)
	{
	  qDebug("Can't find filter:%s", dlerror());
	  //	  status = -10;
	  filt = new ErrorFilter(QString("Can't find plugin:")+nm);
	  return;
	}
      CFilter* (*newfilter)(const QString&);
      newfilter = (CFilter* (*)(const QString&))dlsym(handle, "newfilter");
      if (newfilter == NULL)
	{
	  qDebug("Can't find newfilter");
	  filt = new ErrorFilter(QString("Can't find entry point in plugin:")+nm);
	  return;
	}
      filt = (*newfilter)(optional);
    }
  else
    {
      qDebug("No filter path:%s", (const char*)filterpath);
      filt = new ErrorFilter(QString("No filter plugins installed:")+nm);
    }
  if (filt == NULL)
    {
      qDebug("Can't do newfilter");
      filt = new ErrorFilter(QString("Filter creation failed:")+nm);
      return;
    }
}
#endif

void makeInverse::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
  parent->getch(ch, sty, pos);
  int r,g,b;
  r = 255 - sty.Red(), g = 255 - sty.Green(), b = 255 - sty.Blue();
  sty.setColour(r,g,b);
  r = 255 - sty.bRed(), g = 255 - sty.bGreen(), b = 255 - sty.bBlue();
  sty.setBackground(r,g,b);
  r = 255 - sty.pRed(), g = 255 - sty.pGreen(), b = 255 - sty.pBlue();
  sty.setPaper(r,g,b);
}
/*
void makeNegative::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
  parent->getch(ch, sty, pos);
  QColor fg(sty.Red(), sty.Green(), sty.Blue());
  int h,s,v;
  fg.hsv(&h,&s,&v);
  fg.setHsv(h,s,255-v);
  int r,g,b;
  fg.rgb(&r,&g,&b);
  sty.setColour(r,g,b);
  
  fg = QColor(sty.bRed(), sty.bGreen(), sty.bBlue());
  fg.hsv(&h,&s,&v);
  fg.setHsv(h,s,255-v);
  fg.rgb(&r,&g,&b);
  sty.setBackground(r,g,b);
}
*/
void setbg::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
  parent->getch(ch, sty, pos);
 int r = sty.pRed(), g = sty.pGreen(), b = sty.pBlue();
  if (r == 255 && g == 255 && b == 255)
    {
      sty.setPaper(m_r,m_g,m_b);
    }
   else
     {
	qDebug("We have background [%x%x%x]", r, g, b);
     }
   r = sty.bRed(), g = sty.bGreen(), b = sty.bBlue();
  if (r == 255 && g == 255 && b == 255)
    {
      sty.setBackground(m_r,m_g,m_b);
    }
   else
     {
	qDebug("We have background [%x%x%x]", r, g, b);
     }
}

void setfg::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
  parent->getch(ch, sty, pos);
  int r = sty.Red(), g = sty.Green(), b = sty.Blue();
  if (r == 0 && g == 0 && b == 0)
    {
      sty.setColour(m_r,m_g,m_b);
    }
}

#include "CRegExp.h"

repara::repara(const QString& pat) : tch(0)
{
  //  QString pat("{\\n[A-Z\"]}");
  flt = new CRegExpFilt(pat, false);
  qDebug("Construction done");
}

repara::~repara()
{
  delete flt;
}

void repara::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
  if (flt->empty())
    {
      while (flt->empty())
	{
	  parent->getch(ch, sty, pos);
	  flt->addch(ch);
	}
    }
  ch = flt->pop();
  /*
  parent->getch(ch, sty, pos);
  if (ch == 10 || ch == ' ')
    {
      if (tch == 10)
	{
	  tch = ch;
	  ch = 10;
	  return;
	}
      else
	{
	  tch = ch;
	  ch = ' ';
	  return;
	}
	    }
  tch = ch;
  */
  return;
}

void tableLink::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
  if (offset >= (int)text.length())
    {
      offset = -1;
      sty.setColour(m_r, m_g, m_b);
      do
	{
	  parent->getch(ch, sty, pos);
	}
      while (sty.isTable());
      return;
    }
  if (offset >= 0)
    {
      ch = text[offset++].unicode();
      return;
    }
  parent->getch(ch, sty, pos);
  if (sty.isTable())
    {
      offset = 1;
      ch = text[0].unicode();
      m_r = sty.Red(), m_g = sty.Green(), m_b = sty.Blue();
      sty.setColour(255, 0, 0);
    }
  return;      
}

void underlineLink::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
  parent->getch(ch, sty, pos);
  if (sty.getLink()) sty.setUnderline();
  //if (isLink && !sty.getLink()) sty.unsetUnderline();
  //isLink = sty.getLink();
}
