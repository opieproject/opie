/****************************************************************************
** $Id: QTReader.cpp,v 1.15 2007-01-29 21:53:48 erik Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

const int _SBARHEIGHT = 3;

#include <qpainter.h>
//#include <qdirectpainter_qws.h>
#include <qimage.h>
#include <qtimer.h>
#include "config.h"
#include "QTReader.h"
//#include "QTReaderApp.h"
#include "CDrawBuffer.h"
#ifdef USEQPE
#include <qpe/qpeapplication.h>
#endif
#include <math.h>
#include <ctype.h>
#include <stdio.h> //for sprintf
#ifdef USEQPE
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <qpe/global.h>
#include <qpe/qcopenvelope_qws.h>
#endif
#include <qfileinfo.h>
#include <qdir.h>
#include "TableDialog.h"
#include "outputcodec.h"

/*
#ifdef _UNICODE
const char *QTReader::fonts[] = { "unifont", "Courier", "Times", 0 };
#else
const char *QTReader::fonts[] = { "Helvetica", "Courier", "Times", 0 };
#endif
*/
//const int   QTReader::fontsizes[] = { 8, 10, 12, 14, 18, 24, 30, 40, 50, 60, 70, 80, 90, 100, 0 };

//const tchar *QTReader::fonts[] = { "unifont", "fixed", "micro", "smoothtimes", "Courier", "Times", 0 };
//const int   QTReader::fontsizes[] = {10,16,17,22,0};
//const tchar *QTReader::fonts[] = { "verdana", "Courier", "Times", 0 };
//const int   QTReader::fontsizes[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,0};

tchar QTReader::pluckernextpart[] = { 'C','l','i','c','k',' ','h','e','r','e',' ','f','o','r',' ','t','h','e',' ','n','e','x','t',' ','p','a','r','t',0 };
tchar QTReader::jplucknextpart[] = { 'N','e','x','t',' ','P','a','r','t',' ','>','>',0 };
//tchar QTReader::jplucknextpart[] = { 10,'#',10,'N','e','x','t',' ','P','a','r','t',' ','>','>',0 };

QTReader::QTReader( QWidget *parent, const char *name, WFlags f) :
  QWidget(parent, name, f | WRepaintNoErase | WResizeNoErase),
  m_outofdate(true),
  m_default_fg(0,0,0),
  m_default_bg(255,255,255),
  m_bg(255,255,255),
  m_delay(100),
  m_scrolldy1(0),
  m_scrolldy2(0),
  m_totalscroll(0),
  m_autoScroll(false),
  //textarray(NULL),
  //locnarray(NULL),
  numlines(0),
  m_fontname("unifont"),
  m_fm(NULL),
  mouseUpOn(true),
  m_twotouch(true),
  m_touchone(true),
  bDoUpdates(false),
#ifdef _SCROLLPIPE
  m_pipeout(NULL),
#endif
  m_left_border(2),
  m_right_border(2),
  m_rotated(true),
  pBkmklist(NULL),
  m_scrollpos(0),
  //  bNegative(false),
  bInverse(false),
  m_highlightfilter(NULL),
  m_bgIsScaled(false),
  m_scrollstep(2),
  m_topmargin(5),
  m_bottommargin(5),
  m_reparastring("{\\n[\\n ]}"),
  m_currentlinkstyle(NULL),
  m_currentlinkoffset(-1),
  m_currentlink(-1)
{
  m_output = NULL;
    m_overlap = 1;
    setKeyCompression ( true );

    dbuff = NULL;
    dbp = NULL;
}

/*
QTReader::QTReader( const QString& filename, QWidget *parent=0, const tchar *name=0, WFlags f = 0) :
  QWidget(parent, name, f),
  m_textfont(0),
  m_textsize(1),
  textarray(NULL),
  numlines(0),
  bstripcr(true),
  bunindent(false),
  brepara(false),
  bdblspce(false),
  btight(false),
  bindenter(0),
  m_fm(NULL)
{
  init();
//  //  qDeb2ug("Load_file(1)");
  load_file((const tchar*)filename);
}
*/

/*
void QTReader::mouseMoveEvent(QMouseEvent* _e)
{

    mouseUpOn = !(_e->pos().x() == -1);

    qDebug("MouseMove:[%d, %d]", _e->pos().x(), _e->pos().y());
}
*/
long QTReader::real_delay()
{
    return m_scrollstep*( 8976 + m_delay ) / ( m_linespacing * m_linespacing );
}

void QTReader::mousePressEvent( QMouseEvent* _e )
{
  m_drageligible = false;
  qDebug("Mouse pressed at (%u, %u)", _e->x(), _e->y());
  int x, y, ht, wh;
  if (m_rotated)
    {
      x = _e->y();
      y = width()-_e->x();
      ht = width();
      wh = height();
    }
  else
    {
      x = _e->x();
      y = _e->y();
      ht = height();
      wh = width();
    }
  if (x >= m_left_border && x <= wh - m_right_border && y >= m_topmargin && y <= ht-m_bottommargin)
    {
      if (_e->button() == RightButton)
	{
	  //	qDebug("MousePress");
	  mouseUpOn = false;
	  if (m_swapmouse)
	    {
	      int lineno = 0;
	      /*
		int hgt = textarray[0]->lineSpacing();
		while ((hgt < y) && (lineno < numlines))
		{
		hgt += textarray[++lineno]->lineSpacing();
		}
	      */
	      size_t startpos, startoffset, tgt, tgtoffset, pictgt, tabtgt;
	      QImage* img;
	      getcurrentpos(x, y, wh, ht, lineno, startpos, startoffset, tgt, tgtoffset, pictgt, img, tabtgt);
	      processmousewordevent(startpos, startoffset, _e, lineno);
	    }
	  else
	    {
	      processmousepositionevent(_e);
	    }
	}
    }
  else
    {
      int ln = -1;
      int mp;
      int sectionsize = (buffdoc.endSection()-buffdoc.startSection());
      switch (m_scrollpos)
	{
	case 1:
	  {
	    if (m_rotated)
	      {
		if (_e->x() < m_bottommargin)
		  {
		    ln = height();
		    mp = _e->y();
		  }
	      }
	    else
	      {
		if (_e->y() > height()-m_bottommargin)
		  {
		    ln = width();
		    mp = _e->x();
		  }
	      }
	  }
	  break;
	case 2:
	  {
	    if (m_rotated)
	      {
		if (_e->y() > height() - m_right_border)
		  {
		    ln = width();
		    mp = width()-_e->x();
		  }
	      }
	    else
	      {
		if (_e->x() > width() - m_right_border)
		  {
		    ln = height();
		    mp = _e->y();
		  }
	      }
	  }
	  break;
	case 3:
	  {
	    if (m_rotated)
	      {
		if (_e->y() < m_left_border)
		  {
		    ln = width();
		    mp = width()-_e->x();
		  }
	      }
	    else
	      {
		if (_e->x() < m_left_border)
		  {
		    ln = height();
		    mp = _e->y();
		  }
	      }
	  }
	  break;
	case 0:
	default:
	  ln = -1;
	  break;
	}
      if (ln >= 0)
	{
	  int dp = (sectionsize*mp+ln/2)/ln + buffdoc.startSection();
	  int winsize = locnarray[numlines]-locnarray[0];
	  int slidersize = 10*(sectionsize+ln/2)/ln;
	  if (slidersize > winsize)
	    {
	      int mid = (locnarray[0] + locnarray[numlines])/2;
	      slidersize /= 2;
	      if (dp < mid-slidersize)
		{
		  dopageup();
		}
	      else if (dp > mid+slidersize)
		{
		  dopagedn();
		}
	      //if (mid-slidersize < dp && dp < mid+slidersize)
	      else
		{
		  m_drageligible = true;
		}
	    }
	  else
	    {
	      if (dp < locnarray[0])
		{
		  dopageup();
		}
	      else if (dp > locnarray[numlines])
		{
		  dopagedn();
		}
		//if (locnarray[0] < dp && dp < locnarray[numlines])
	      else
		{
		  m_drageligible = true;
		}
	    }
	  qDebug("Drag eligible:%s", (m_drageligible) ? "true" : "false");
	}
      else
	{
	  if (m_scrollpos == 1)
	    {
	      if (x < m_left_border)
		{
		  lineUp();
		}
	      if (y > ht - m_bottommargin)
		{
		  lineDown();
		}
	    }
	  else if (m_scrollpos != 0)
	    {
	      if (y < m_topmargin)
		{
		  lineUp();
		}
	      if (y > ht - m_bottommargin)
		{
		  lineDown();
		}
	    }
	}
    }
}

void QTReader::processmousepositionevent( QMouseEvent* _e )
{
  int x, y, ht, wh;
  if (m_rotated)
    {
      x = _e->y();
      y = width()-_e->x();
      ht = width();
      wh = height();
    }
  else
    {
      x = _e->x();
      y = _e->y();
      ht = height();
      wh = width();
    }
    if (buffdoc.hasnavigation())
    {
	if (y > (2*ht)/3)
	{
	    goDown();
	}
	else if (y < ht/3)
	{
	    goUp();
	}
	else
	{
	    if (x < wh/3)
	    {
		goBack();
	    }
	    else if (x > (2*wh)/3)
	    {
		goForward();
	    }
	    else
	    {
		goHome();
	    }
	}
    }
    else
    {
	if (y > ht/2)
	{
	    goDown();
	}
	else
	{
	    goUp();
	}
    }
}

void QTReader::goHome()
{
    if (buffdoc.hasnavigation())
    {
 	size_t current=pagelocate();
 	size_t home=buffdoc.getHome();
 	if (current!=home)
	{
	    buffdoc.saveposn(m_lastfile, current);
	    locate(home);
 	}
    }
    else
      locate(0);
}

void QTReader::goBack()
{
  if (buffdoc.hasnavigation())
    {
      size_t target = pagelocate();
      QString nxt = m_lastfile;
      buffdoc.writeposn(m_lastfile, target);
      linkType lt = buffdoc.back(nxt, target);
      if ((lt & eFile) != 0)
	{
	  if (nxt != m_lastfile)
	    {
	      emit NewFileRequest(nxt);
	    }
	  locate(target);
	}
      else if ((lt & eLink) != 0)
	{
	  locate(target);
	}
    }
}

void QTReader::goForward()
{
  if (buffdoc.hasnavigation())
    {
      size_t target = pagelocate();
      QString nxt = m_lastfile;
      linkType lt = buffdoc.forward(nxt, target);
      if ((lt & eFile) != 0)
	{
	  if (nxt != m_lastfile)
	    {
	      emit NewFileRequest(nxt);
	    }
	  locate(target);
	}
      else if ((lt & eLink) != 0)
	{
	  locate(target);
	}
    }
}

linkType QTReader::getcurrentpos(int x, int y, int w, int h, int& lineno, size_t& start, size_t& offset, size_t& tgt, size_t& tgtoffset, size_t& pictgt, QImage*& img, size_t& tabtgt)
{
  int ht;
  if (m_scrolldy == m_topmargin)
    {
      lineno = 0;
      ht = textarray[0]->lineSpacing()-m_scrolldy1 + m_topmargin;
    }
  else
    {
      if (y >= m_scrolldy)
	{
	  lineno = 0;
	  ht = textarray[0]->lineSpacing()-m_scrolldy1+m_scrolldy + m_topmargin;
	}
      else
	{
	  lineno = 0;
	  ht = textarray[0]->lineSpacing()-m_scrolldy1+m_scrolldy+m_topmargin;
	  while ((ht < h) && (lineno < numlines-1))
	    {
	      ht += textarray[++lineno]->lineSpacing();
	    }
	  ht = textarray[lineno]->lineSpacing();
	}
    }
  while ((ht < y) && (lineno < numlines-1))
    {
      ht += textarray[++lineno]->lineSpacing();
    }
  if (ht < y && textarray[numlines]->showPartial()) lineno = numlines;
  start = locnarray[lineno];
  int availht = ((m_rotated) ? width() : height()) - m_topmargin - m_bottommargin;
  if (m_bMonoSpaced)
    {
      offset = (x - textarray[lineno]->offset(w, m_left_border, m_right_border, availht))/m_charWidth;
    }
  else
    {
      int i;
      CDrawBuffer* t = textarray[lineno];
      x = x - t->offset(width(), m_left_border, m_right_border, availht);
      for (i = t->length(); i > 0 && t->width(availht, i, true, w, m_left_border, m_right_border) > x; i--);
      offset = i;
    }
  return textarray[lineno]->getLinkType(offset, tgt, tgtoffset, pictgt, img, tabtgt);
}

void QTReader::suspend()
{
  buffdoc.suspend();
  /*#ifdef OPIE
    if (memcmp("/mnt/", m_lastfile.latin1(), 5) == 0) buffdoc.suspend();
#else
    if (memcmp("/usr/mnt.rom/", m_lastfile.latin1(), 13) == 0) buffdoc.suspend();
#endif
  */
}

void QTReader::setDoubleBuffer(bool _b)
{
  m_doubleBuffered = _b;
  if (_b || m_rotated)
    {
      if (dbuff == NULL)
	{
	  dbuff = new QPixmap();
	  dbp = new QPainter();
	}
      if (m_rotated)
	{
	  dbuff->resize(height(), width());
	}
      else
	{
	  dbuff->resize(width(), height());
	}
      m_outofdate = true;
    }
  else
    {
      if (dbuff != NULL)
	{
	  delete dbuff;
	  delete dbp;
	}
      dbuff = NULL;
      dbp = NULL;
    }
}

void QTReader::setTwoTouch(bool _b)
{
    setBackgroundColor( m_bg );
    m_twotouch = m_touchone = _b;
}

void QTReader::setContinuous(bool _b)
{
    buffdoc.setContinuous(m_continuousDocument = _b);
}

void QTReader::processmousewordevent(size_t startpos, size_t startoffset, QMouseEvent* _e, int lineno)
{
    unsigned long wrdstart = 0, wrdend = 0;
    QString wrd;
    int availht = ((m_rotated) ? width() : height()) - m_topmargin - m_bottommargin;
    if (m_twotouch)
    {
	if (m_touchone)
	{
	    m_touchone = false;
	    m_startpos = startpos;
	    m_startoffset = startoffset;
	    setBackgroundColor( lightGray );
	}
	else
	{
	    m_touchone = true;
	    setBackgroundColor( m_bg );
	    size_t endpos, endoffset;
	    endpos = startpos;
	    endoffset = startoffset;
	    size_t currentpos = locate();
	    if (endpos >= m_startpos)
	    {
		jumpto(m_startpos);
		for (int i = 0; i < m_startoffset; i++)
		{
		    getch();
		}
		wrdstart = buffdoc.explocate();
		if (m_startpos == endpos)
		{
		    for (int i = m_startoffset; i <= endoffset; i++)
		    {
			wrd += QChar(getch());
		    }
		}
		else
		{
		    while (buffdoc.explocate() <= endpos)
		    {
			wrd += QChar(getch());
		    }
		    for (int i = 0; i < endoffset; i++)
		    {
			wrd += QChar(getch());
		    }
		}
		wrdend = buffdoc.explocate();
		jumpto(currentpos);
	    }
	}
    }
    else if (m_bMonoSpaced)
    {
      int chno = (m_rotated) ?
	(_e->y()-textarray[lineno]->offset(height(), m_left_border, m_right_border, availht))/m_charWidth
	:
	(_e->x()-textarray[lineno]->offset(width(), m_left_border, m_right_border, availht))/m_charWidth;
	if (chno < ustrlen(textarray[lineno]->data()))
	{
	    wrd[0] = textarray[lineno]->data()[chno];
	}
    }
    else
    {
	CDrawBuffer* t = textarray[lineno];
	int first = 0;
	int tgt = (m_rotated) ?
	  _e->y() - t->offset(height(), m_left_border, m_right_border, availht) :
	  _e->x() - t->offset(width(), m_left_border, m_right_border, availht);
	while (1)
	{
	    int i = first+1;
	    int availht = ((m_rotated) ? width() : height()) - m_topmargin - m_bottommargin;
	    while (QChar((*t)[i]).isLetter() && (*t)[i] != 0) i++;
	    if (t->width(availht, i, true, (m_rotated) ? height() : width(), m_left_border, m_right_border) > tgt)
	    {
		wrd = toQString(t->data()+first, i - first);
//			    qDebug("Got %s", (const char *)wrd);
		break;
	    }
	    while (!QChar((*t)[i]).isLetter() && (*t)[i] != 0) i++;
	    if ((*t)[i] == 0) break;
	    first = i;
	}		
    }
    if (!wrd.isEmpty())
    {
	qDebug("Selected:%s", (const char*)wrd);
	if (m_twotouch)
	  {
	    emit OnWordSelected(wrd, wrdstart, wrdend, wrd);
	  }
	else
	  {
	    QString line = toQString(textarray[lineno]->data());
	    emit OnWordSelected(wrd, locnarray[lineno], locnarray[lineno]+line.length(), line);
	  }
    }
}

#ifdef USETIMER
void QTReader::actionDrag()
{
  if (m_drageligible)
    {
      int fivepages = 5*((2*width()+m_textsize/2)/m_textsize)*((height()+m_textsize/2)/m_textsize);
      if (m_dragtarget > fivepages && locnarray[numlines] < m_dragtarget - fivepages)
	{
	  int tgt = m_dragtarget - fivepages/2;
	  //qDebug("Jumping to %u (%u)", tgt, fivepages);
	  if (tgt < buffdoc.startSection())
	    {
	      tgt = buffdoc.startSection();
	    }
	  locate(tgt);
	  drawFonts();
	}
      else if (locnarray[0] > m_dragtarget+fivepages)
	{
	  int tgt = m_dragtarget + fivepages/2;
	  //qDebug("Jumping to %u (%u)", tgt, fivepages);
	  if (tgt > buffdoc.endSection())
	    {
	      dopageup();
	    }
	  else
	    {
	      locate(tgt);
	      drawFonts();
	    }
	}
      else if (locnarray[numlines] <= m_dragtarget)
	{
	  dopagedn();
	}
      else if (locnarray[0] > m_dragtarget)
	{
	  dopageup();
	}
    }
  else
    {
      m_dragtimer->stop();
    }
}
#endif

void QTReader::mouseMoveEvent( QMouseEvent* _e )
{
  if (m_drageligible)
    {
      int ht;
      int mp;
      int sectionsize = (buffdoc.endSection()-buffdoc.startSection());
      //qDebug("Mouse moved to (%u, %u)", _e->x(), _e->y());
      switch (m_scrollpos)
	{
	case 1:
	  {
	    if (m_rotated)
	      {
		ht = height();
		mp = _e->y();
	      }
	    else
	      {
		ht = width();
		mp = _e->x();
	      }
	  }
	  break;
	case 2:
	case 3:
	  {
	    if (m_rotated)
	      {
		ht = width();
		mp = width()-_e->x();
	      }
	    else
	      {
		ht = height();
		mp = _e->y();
	      }
	  }
	  break;
	case 0:
	default:
	  ht = -1;
	  break;
	}
      if (ht >= 0)
	{
#ifdef USETIMER
	  m_dragtarget = (sectionsize*mp+ht/2)/ht + buffdoc.startSection();
	  if (!m_dragtimer->isActive())
	    {
	      m_dragtimer->start(0, false);
	    }
#else
	  int dp = (sectionsize*mp+ht/2)/ht + buffdoc.startSection();
	  locate(dp);
#endif
	}
    }
}

void QTReader::mouseReleaseEvent( QMouseEvent* _e )
{
  qDebug("Mouse released at (%u, %u)", _e->x(), _e->y());
  if (m_drageligible)
    {
      m_drageligible = false;
    }
  else
    {
      int x, y, ht, wh;
      if (m_rotated)
	{
	  x = _e->y();
	  y = width()-_e->x();
	  ht = width();
	  wh = height();
	}
      else
	{
	  x = _e->x();
	  y = _e->y();
	  ht = height();
	  wh = width();
	}
      if (_e->button() == LeftButton)
	{
	  if (mouseUpOn)
	    {
	      //	    qDebug("MouseRelease");
	      /*
		switch(m_scrollpos)
		{
		case 1: // Bottom
		if (y > ht - 5)
		{
		locate(buffdoc.startSection()+((buffdoc.endSection()-buffdoc.startSection())*x+wh/2)/wh);
		return;
		}
		break;
		case 2: // right
		if (x > wh - m_right_border)
		{
		locate(buffdoc.startSection()+((buffdoc.endSection()-buffdoc.startSection())*y+ht/2)/ht);
		return;
		}
		break;
		case 3: // left
		if (x < m_left_border)
		{
		locate(buffdoc.startSection()+((buffdoc.endSection()-buffdoc.startSection())*y+ht/2)/ht);
		return;
		}
		break;
		case 0:
		default:
		break;
		}
	      */
	      if (textarray[0] != NULL)
		{
		  QString line;
		  //		int lineno = _e->y()/m_linespacing;
		  int lineno = 0;
		  /*
		    int ht = textarray[0]->lineSpacing();
		    while ((ht < y) && (lineno < numlines))
		    {
		    ht += textarray[++lineno]->lineSpacing();
		    }
		  */
		  size_t startpos, startoffset, tgt, tgtoffset, pictgt, tabtgt;
		  QImage* img;
		  if (m_currentlinkstyle != NULL)
		    {
		      textarray[m_currentlink]->invertLink(m_currentlinkoffset);
		      m_currentlinkstyle = NULL;
		      m_currentlink = -1;
		      m_currentlinkoffset = -1;
		    }
		  linkType glt = getcurrentpos(x, y, wh, ht, lineno, startpos, startoffset, tgt, tgtoffset, pictgt, img, tabtgt);
		  if (bNoInlineTables && ((glt & eTable) != 0))
		    {
		      size_t currentpos = locate();
		      QString tabtext = buffdoc.getTableAsHtml(tabtgt);
		      qDebug("TABLE:%u:%u:%s", currentpos, tabtgt, (const char*)tabtext);
		      QFont f(m_fontname, m_fontControl.currentsize());
#ifdef USEQPE
		      CTableDialog td(f, tabtext, true, this);
#else
		      CTableDialog td(f, tabtext, false, this);
#endif
		      td.exec();
		      jumpto(currentpos);
		    }
		  if ((glt & eLink) != 0)
		    {
		      if ((glt & ePicture) != 0)
			{
			  qDebug("Big Picture:%x", pictgt);
			  if (QMessageBox::warning(this, PROGNAME, "Show picture or goto link?", "Show", "Goto Link") == 0)
			    {
			      QImage* pm = buffdoc.getPicture(pictgt);
			      if (pm != NULL)
				{
				  emit OnShowPicture(*pm);
				  delete pm;
				  return;
				}
			    }
			}
		      else if (img != NULL)
			{
			  if (QMessageBox::warning(this, PROGNAME, "Show picture or goto link?", "Show", "Goto Link") == 0)
			    {
			      emit OnShowPicture(*img);
			      return;
			    }
			}
		      size_t saveposn = pagelocate();
		      QString href, nm;
		      linkType lt = buffdoc.hyperlink(tgt, tgtoffset, href, nm);
		      qDebug("URL(1):%s", (const char*)href);
		      if ((lt & eFile) != 0)
			{
			  buffdoc.saveposn(m_lastfile, saveposn);
#ifdef USEQPE
			  {
			    QCopEnvelope e("QPE/System", "busy()");
			  }
#endif
			  ResetScroll();
			  if (!href.isEmpty())
			    {
			      if (!buffdoc.getFile(href, nm))
				{
				  emit NewFileRequest(href);
				}
			      else
				{
				  qDebug("BEFORE:%u", pagelocate());
				  buffdoc.resetPos();
				  ResetScroll();
				  fillbuffer();
				  qDebug("AFTER:%u", pagelocate());
				  m_outofdate = true;
				  update();
				}
			    }
			  if (!nm.isEmpty())
			    {
			      qDebug("QTReader:Finding %s", (const char*)nm);
			      if (buffdoc.findanchor(nm))
				{
				  buffdoc.resetPos();
				  fillbuffer();
				  m_outofdate = true;
				  update();
				}
			    }
			  //fillbuffer();
			  //update();
#ifdef USEQPE
			  {
			    QCopEnvelope e("QPE/System", "notBusy()");
			  }
#endif
			}
		      else if ((lt & eLink) != 0)
			{
			  buffdoc.saveposn(m_lastfile, saveposn);
			  ResetScroll();
			  fillbuffer();
			  m_outofdate = true;
			  update();
			}
		      else
			{
			  if ((lt & ePicture) != 0)
			    {
			      QImage* pm = buffdoc.getPicture(tgt);
			      if (pm != NULL)
				{
				  emit OnShowPicture(*pm);
				  delete pm;
				}
			    }
			  else
			    {
			      //				QString anchortext = textarray[lineno]->getanchortext(startoffset);
			      if (!href.isEmpty())
				{
				  emit OnURLSelected(href, tgt);
				}
			    }
			}
		      return;
		    }
		  else if ((glt & ePicture) != 0)
		    {
		      qDebug("Big Picture:%x", pictgt);
		      QImage* pm = buffdoc.getPicture(pictgt);
		      if (pm != NULL)
			{
			  emit OnShowPicture(*pm);
			  delete pm;
			}
		      else
			{
			  update();
			}
		      return;
		    }
		  else if (img != NULL)
		    {
		      emit OnShowPicture(*img);
		      return;
		    }
		  if (m_swapmouse)
		    processmousepositionevent(_e);
		  else
		    processmousewordevent(startpos, startoffset, _e, lineno);
		}
	    }
	  else
	    {
	      mouseUpOn = true;
	    }
	}
    }
}

void QTReader::focusInEvent(QFocusEvent* e)
{
    if (m_autoScroll && (m_scrolltype != 4)) timer->start(real_delay(), false);
    update();
}

void QTReader::focusOutEvent(QFocusEvent* e)
{
    if (m_autoScroll)
    {
      if (m_scrolltype != 4)
	{
	  timer->stop();
	}
      else
	{
	  m_autoScroll = false;
	}
//	m_scrolldy1 = m_scrolldy2 = 0;
    }
}

#include <qapplication.h>
#include <qdrawutil.h>
#ifndef _WINDOWS
#include <unistd.h>
#endif

void QTReader::goDown()
{
    if (m_bpagemode)
    {
	dopagedn();
    }
    else
    {
	lineDown();
    }
}

void QTReader::goUp()
{
    if (m_bpagemode)
    {
	dopageup();
    }
    else
    {
	lineUp();
    }
}

void QTReader::NavUp()
{
    if (buffdoc.hasnavigation())
    {
/*
	size_t target = pagelocate();
	if (buffdoc.back(target))
	{
	    locate(target);
	}
*/
	locate(buffdoc.startSection());
    }
    else
    {
	goUp();
    }
}

void QTReader::NavDown()
{
    if (buffdoc.hasnavigation())
    {
/*
	size_t target = pagelocate();
	if (buffdoc.forward(target))
	{
	    locate(target);
	}
*/
	dopageup(buffdoc.endSection());
    }
    else
    {
	goDown();
    }
}

void QTReader::zoomin()
{
    if (m_fontControl.increasesize())
    {
	bool sc = m_autoScroll;
	setautoscroll(false);
	setfont();
	refresh();
	setautoscroll(sc);
    }
}

void QTReader::zoomout()
{
    if (m_fontControl.decreasesize())
    {
	bool sc = m_autoScroll;
	setautoscroll(false);
	setfont();
	refresh();
	setautoscroll(sc);
    }
}

void QTReader::reduceScroll()
{
    if (m_delay < 59049)
    {
	m_delay = (3*m_delay)/2;
	timer->changeInterval(real_delay());
    }
    else
    {
	m_delay = 59049;
    }
}

void QTReader::increaseScroll()
{
    if (m_delay > 454)
    {
	m_delay = (2*m_delay)/3;
	timer->changeInterval(real_delay());
    }
    else
    {
	m_delay = 454;
    }
}

void QTReader::keyPressEvent(QKeyEvent* e)
{

   //((QTReaderApp*)parent()->parent())->handlekey(e);
    emit HandleKeyRequest(e);
//    e->ignore();
    return;
#ifdef _SCROLLPIPE
    if (m_isPaused)
    {
	m_isPaused = false;
	if (e->key() != Key_Space)
	{
	    m_autoScroll = false;
	    if (m_pipeout != NULL)
	    {
		pclose(m_pipeout);
		m_pipeout = NULL;
	    }
	    //((QTReaderApp*)parent()->parent())->setScrollState(m_autoScroll);
	    emit SetScrollState(m_autoScroll);
	    QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
	}
	else
	{
	    timer->start(real_delay(), false);
	}
	e->accept();
	return;
    }
#endif
}

void QTReader::CalculateScrollParameters()
{
  int bmargin = ((m_scrollpos == 1) ? _SBARHEIGHT : 0);
  if (bmargin < m_bottommargin) bmargin = m_bottommargin;
  switch (m_scrolltype)
    {
    case 0:
      {
	if (m_scrolldy == m_topmargin)
	  {
	    m_scrolldy1 = 0;
	    m_scrolldy2 = 0;
	    m_totalscroll = 0;
	    return;
	  }
	if (m_scrolldy < textarray[0]->lineSpacing())
	  {
	    m_scrolldy2 = m_scrolldy;
	    return;
	  }
	int ht = m_scrolldy - m_scrolldy1;
	int i;
	for (i = 0; (ht < ((m_rotated) ? width() : height())-bmargin) && (i < numlines); i++)
	  {
	    ht += textarray[i]->lineSpacing();
	  }
	ht = 0;
	int j;
	i--;
	for (j = i; j < numlines; j++)
	  {
	    ht += textarray[j]->lineSpacing();
	  }
	ht -= (
	       textarray[i]->lineExtraSpacing()
	       +
	       (i != 0) ? textarray[numlines-1]->lineExtraSpacing() : 0
	       )/2-2;
	
	m_scrolldy2 = m_scrolldy-ht;
      }
      break;
    case 1:
    case 2:
    case 3:
      {
	int ypos = m_topmargin;
	for (int i = 0; i < numlines; i++)
	  {
	    ypos += textarray[i]->lineSpacing();
	  }
	ypos -= (
		 textarray[0]->lineExtraSpacing()
		 +
		 ((numlines > 1) ? textarray[numlines-1]->lineExtraSpacing() : 0)
		 )/2 +
	  m_scrolldy1 - 2;
	m_scrolldy2 = ((m_rotated) ? width() : height()) - ypos - bmargin;
      }
      break;
    }
}

void QTReader::setautoscroll(bool _sc)
{
  m_outofdate = true;
    if (_sc == m_autoScroll) return;
    if (m_autoScroll)
    {
	m_autoScroll = false;
#ifdef USEQPE
	QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
#endif
#ifdef _SCROLLPIPE
	if (m_pipeout != NULL)
	{
	    pclose(m_pipeout);
	    m_pipeout = NULL;
	}
#endif
	//	m_scrolldy1 = 0;
	//refresh();
     }
    else
    {
	CDrawBuffer* reusebuffer = textarray[numlines];
	if (reusebuffer == NULL || reusebuffer->eof()) return;
#ifndef __STATIC
	if ((m_scrolltype == 4) && !checkoutput()) return;
#endif
	m_autoScroll = true;
	CalculateScrollParameters();

#ifdef _SCROLLPIPE
	if (!m_pipetarget.isEmpty())
	{
//	    qDebug("Opening pipe to %s", (const char*)m_pipetarget);
	    m_pipeout = popen((const char*)m_pipetarget, "w");
	    m_isPaused = false;
	}
#endif
	autoscroll();
#ifdef USEQPE
	QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Disable; // light is even not dimmed
#endif
    }
}

bool QTReader::getline(CDrawBuffer *buff)
{
    bool bRet;
    int availht = ((m_rotated) ? width() : height()) - m_topmargin - m_bottommargin;
    if (m_bMonoSpaced)
    {
	bRet = buffdoc.getline(buff ,(m_rotated) ? height() : width(), m_charWidth, m_left_border, m_right_border, availht);
    }
    else
    {
	bRet = buffdoc.getline(buff, (m_rotated) ? height() : width(), m_left_border, m_right_border, hyphenate, availht);
    }
    buff->resize(availht);
    return bRet;
}

void QTReader::doscroll()
{
    if (!m_autoScroll)
    {
	timer->stop();
	return;
    }
    switch (m_scrolltype)
      {
      case 0:
	doinplacescroll();
	break;
      case 1:
	dorollingscroll(false);
	break;
      case 2:
	dorollingscroll(true);
	break;
      case 3:
	dostaticscroll();
	break;
      }
}

void QTReader::doinplacescroll()
{
  QPainter p( this );
  //  p.setBackgroundMode(OpaqueMode);
  int wh, ht;
  int bmargin = ((m_scrollpos == 1) ? _SBARHEIGHT : 0);
  if (bmargin < m_bottommargin) bmargin = m_bottommargin;
  if (m_rotated)
    {
      ht = width()-bmargin;
      wh = height();
    }
  else
    {
      ht = height()-bmargin;
      wh = width();
    }
  int lastdy = m_scrolldy;
  m_scrolldy += m_scrollstep;
  if ((m_scrolldy1 = m_scrolldy1+m_scrollstep) >= textarray[0]->lineSpacing())
    {
      int ht = textarray[0]->lineSpacing();
#ifdef _SCROLLPIPE
      if (m_pipeout != NULL)
	{
	  QString outstr = toQString(textarray[0]->data());
	  if (!outstr.isEmpty())
	    {
	      fprintf(m_pipeout, "%s\n", (const char*)outstr);
	      fflush(m_pipeout);
	    }
	  else if (m_pauseAfterEachPara)
	    {
	      m_isPaused = true;
	      timer->stop();
	    }
	}
#endif
      CDrawBuffer* buff = textarray[0];
      for (int i = 1; i <= numlines; i++)
	{
	  textarray[i-1] = textarray[i];
	  locnarray[i-1] = locnarray[i];
	}
      textarray[numlines] = buff;
      --numlines;
      m_scrolldy1 -= ht;
    }
  if ((m_scrolldy2 = m_scrolldy2+m_scrollstep) >= textarray[numlines]->lineSpacing())
    {
      m_scrolldy2 -= textarray[numlines]->lineSpacing();
      numlines++;

      if (textarray[numlines] == NULL)
	{
	  textarray[numlines] = new CDrawBuffer(&m_fontControl);
	}
      locnarray[numlines] = locate();
      int ch = getline(textarray[numlines]);
      if (m_rotated)
	{
	  blitRot(width()-m_scrolldy, 0, height(), -1, textarray[numlines-1]);
	}
      else
	{
	  if (m_bgpm.isNull())
	    {
	      p.fillRect(m_left_border,m_scrolldy-textarray[numlines-1]->lineSpacing(),width()-(m_left_border+m_right_border),textarray[numlines-1]->lineSpacing(),m_bg);
	    }
	  else
	    {
	      int h_tmp = textarray[numlines-1]->lineSpacing();
	      bitBlt(this, m_left_border, m_scrolldy-h_tmp, dbuff, m_left_border, m_scrolldy-h_tmp, width()-(m_left_border+m_right_border), h_tmp);
	    }
	  textarray[numlines-1]->render(&p, m_scrolldy -textarray[numlines-1]->lineSpacing()+ textarray[numlines-1]->ascent(), m_bMonoSpaced, m_charWidth, width(), m_left_border, m_right_border, m_bg, height()-m_topmargin-m_bottommargin);
	}
      mylastpos = locate();
      if (!ch)
	{
	  m_scrolldy = m_topmargin;
	  m_autoScroll = false;
#ifdef _SCROLLPIPE
	  for (int i = 0; i < numlines; i++)
	    {
	      if (m_pipeout != NULL)
		{
		  QString outstr = toQString(textarray[i]->data());
		  if (!outstr.isEmpty())
		    {
		      fprintf(m_pipeout, "%s\n", (const char*)outstr);
		      fflush(m_pipeout);
		    }
		}
	    }
#endif
	  emit SetScrollState(m_autoScroll);
#ifdef USEQPE
	  QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
#endif
	}
      if (m_scrolldy > ht-textarray[numlines]->lineSpacing())
	{
	  if (m_rotated)
	    {
	      if (m_bgpm.isNull())
		{
		  p.fillRect(bmargin, m_left_border, ht-m_scrolldy, wh-(m_left_border+m_right_border), m_bg);
		}
	      else
		{
		  blitRot(bmargin, 0,height(), ht-m_scrolldy, NULL);
		}
	    }
	  else
	      if (m_bgpm.isNull())
		{
		  p.fillRect(m_left_border,m_scrolldy,width()-(m_left_border+m_right_border),height()-m_scrolldy,m_bg);
		}
	      else
		{
		  bitBlt(this,m_left_border,m_scrolldy,dbuff,m_left_border,m_scrolldy,width()-(m_left_border+m_right_border), height()-m_scrolldy);
		}
	  m_scrolldy = m_topmargin;
	  m_scrolldy2 = 0;
	}
      redrawScroll(&p);
      emitRedraw();
      lastdy = -1;
    }
  //  else
    {
      if (m_rotated)
	{
	  if (lastdy >= 0) 
	    {
	      if (m_bgpm.isNull())
		{
		  p.fillRect(width()-lastdy, m_left_border, m_scrollstep, wh-(m_left_border+m_right_border),m_bg);
		}
	      else
		{
		  blitRot(width()-lastdy,m_left_border,wh-(m_left_border+m_right_border), m_scrollstep, NULL);
		}
	    }
	  p.fillRect(width()-m_scrolldy, m_left_border, 1,wh-(m_left_border+m_right_border),m_scrollcolor);
	}
      else
	{
	  if (lastdy >= 0) 
	    {
	      if (m_bgpm.isNull())
		{
		  p.fillRect(m_left_border,lastdy,width()-(m_left_border+m_right_border),m_scrollstep,m_bg);
		}
	      else
		{
		  bitBlt(this, m_left_border, lastdy, dbuff, m_left_border, lastdy, width()-(m_left_border+m_right_border), m_scrollstep);
		}
	    }
	  p.fillRect(m_left_border,m_scrolldy,width()-(m_left_border+m_right_border),1,m_scrollcolor);
	}
    }
}

void QTReader::dorollingscroll(bool _statbord)
{
  bool bredrawscroll = false;
    QPainter p( this );
    // 2 = right, 3 = left
    int tmargin = (_statbord) ? m_topmargin : 0;
    int lmargin = (m_scrollpos == 3 || _statbord) ? m_left_border : 0;
    int rmargin = (m_scrollpos == 2 || _statbord) ? m_right_border : 0;
    int hmargin = ((m_scrollpos == 1) ? _SBARHEIGHT : 0);
    if (hmargin < m_bottommargin) hmargin = m_bottommargin;
    if (m_rotated)
      {
	m_totalscroll = (m_totalscroll+m_scrollstep) % width();
	bitBlt(this, m_scrollstep+hmargin, lmargin, this, hmargin, lmargin, width()-tmargin-hmargin, height()-(lmargin+rmargin));
	if (!m_bgpm.isNull())
	  {
	    blitRot(hmargin, tmargin, height(), m_scrollstep, NULL);
	  }
	else
	  {
	    p.fillRect(hmargin, rmargin, m_scrollstep, height()-lmargin-rmargin, m_bg);
	  }
      }
    else
      {
	m_totalscroll = (m_totalscroll+m_scrollstep) % height();
	bitBlt(this,lmargin,tmargin,this,lmargin,tmargin+m_scrollstep,width()-(lmargin+rmargin),height() - tmargin - hmargin - m_scrollstep);
	if (m_bgpm.isNull())
	  {
	    p.fillRect(0, height() - (m_scrollstep+1) - hmargin, width(), (m_scrollstep+1), m_bg);
	  }
	else
	  {
	    int loff = (_statbord) ? 0 : m_totalscroll;
	    bitBlt(this,0,height() - (m_scrollstep+1) - hmargin, dbuff, 0, (loff+height() - (m_scrollstep+1) - hmargin) % height(), width(), (m_scrollstep+1));
	  }
      }

    if ((m_scrolldy1 = m_scrolldy1+m_scrollstep) >= textarray[0]->lineSpacing())
    {
      int ht = textarray[0]->lineSpacing();
      bredrawscroll = true;
#ifdef _SCROLLPIPE
	if (m_pipeout != NULL)
	{
	    QString outstr = toQString(textarray[0]->data());
	    if (!outstr.isEmpty())
	    {
		fprintf(m_pipeout, "%s\n", (const char*)outstr);
		fflush(m_pipeout);
	    }
	    else if (m_pauseAfterEachPara)
	    {
		m_isPaused = true;
		timer->stop();
	    }
	}
#endif
	CDrawBuffer* buff = textarray[0];
	for (int i = 1; i <= numlines; i++)
	{
	    textarray[i-1] = textarray[i];
	    locnarray[i-1] = locnarray[i];
	}
	textarray[numlines] = buff;
	--numlines;
	m_scrolldy1 -= ht;
    }
    if ((m_scrolldy2 = m_scrolldy2+m_scrollstep) >= textarray[numlines]->lineSpacing())
    {
      bredrawscroll = true;
	m_scrolldy2 -= textarray[numlines]->lineSpacing();
	numlines++;

	if (textarray[numlines] == NULL)
	{
	    textarray[numlines] = new CDrawBuffer(&m_fontControl);
	}
	locnarray[numlines] = locate();
	int ch = getline(textarray[numlines]);
	if (m_rotated)
	  {
	    blitRot(hmargin, 0, height(), -1, textarray[numlines-1]);
	  }
	else
	  {
	    //	    textarray[numlines-1]->render(&p, height() - textarray[numlines-1]->descent() - 2 - hmargin, m_bMonoSpaced, m_charWidth, width(), m_left_border, m_right_border, m_bg);
	    textarray[numlines-1]->render(&p, height() - textarray[numlines-1]->descent() - textarray[numlines-1]->lineExtraSpacing() - 1 - hmargin, m_bMonoSpaced, m_charWidth, width(), m_left_border, m_right_border, m_bg, height()-m_topmargin-m_bottommargin);

	  }
	mylastpos = locate();
	if (!ch)
	{
	  redrawScroll(&p);
	  emitRedraw();
	    m_autoScroll = false;
#ifdef _SCROLLPIPE
	    for (int i = 0; i < numlines; i++)
	    {
		if (m_pipeout != NULL)
		{
		    QString outstr = toQString(textarray[i]->data());
		    if (!outstr.isEmpty())
		    {
			fprintf(m_pipeout, "%s\n", (const char*)outstr);
			fflush(m_pipeout);
		    }
		}
	    }
#endif
	    emit SetScrollState(m_autoScroll);
#ifdef USEQPE
	    QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
#endif
	    return;
	}
    }
    if (!bredrawscroll && ((m_scrolldy2/m_scrollstep) % 10 == 5) && textarray[numlines]->showPartial())
      {
	if (m_rotated)
	  {
	    blitRot(hmargin + m_scrolldy2 - textarray[numlines]->lineSpacing(), 0, height(), -1, textarray[numlines]);
	  }
	else
	  {
	    textarray[numlines]->render( &p, height() + textarray[numlines]->lineSpacing() - textarray[numlines]->descent() - 2 - hmargin-m_scrolldy2, m_bMonoSpaced, m_charWidth, width(), m_left_border, m_right_border, m_bg, height()-m_topmargin-m_bottommargin);
	  }
      }
    if (m_scrollpos != 0)
      {
	redrawScroll(&p);
      }
    if (bredrawscroll) emitRedraw();
}

void QTReader::dostaticscroll()
{
    redrawall();
  bool bredraw = false;
    if ((m_scrolldy1 = m_scrolldy1+m_scrollstep) >= textarray[0]->lineSpacing())
    {
      int ht = textarray[0]->lineSpacing();
      bredraw = true;
#ifdef _SCROLLPIPE
	if (m_pipeout != NULL)
	{
	    QString outstr = toQString(textarray[0]->data());
	    if (!outstr.isEmpty())
	    {
		fprintf(m_pipeout, "%s\n", (const char*)outstr);
		fflush(m_pipeout);
	    }
	    else if (m_pauseAfterEachPara)
	    {
		m_isPaused = true;
		timer->stop();
	    }
	}
#endif
	CDrawBuffer* buff = textarray[0];
	for (int i = 1; i <= numlines; i++)
	{
	    textarray[i-1] = textarray[i];
	    locnarray[i-1] = locnarray[i];
	}
	textarray[numlines] = buff;
	--numlines;
	m_scrolldy1 -= ht;
    }
    if ((m_scrolldy2 = m_scrolldy2 + m_scrollstep) >= textarray[numlines]->lineSpacing())
    {
      bredraw = true;
	m_scrolldy2 -= textarray[numlines]->lineSpacing();
	numlines++;

	if (textarray[numlines] == NULL)
	{
	    textarray[numlines] = new CDrawBuffer(&m_fontControl);
	}
	locnarray[numlines] = locate();
	int ch = getline(textarray[numlines]);
	mylastpos = locate();
	if (!ch)
	{
	  redrawall();
	  emitRedraw();
	    m_autoScroll = false;
#ifdef _SCROLLPIPE
	    for (int i = 0; i < numlines; i++)
	    {
		if (m_pipeout != NULL)
		{
		    QString outstr = toQString(textarray[i]->data());
		    if (!outstr.isEmpty())
		    {
			fprintf(m_pipeout, "%s\n", (const char*)outstr);
			fflush(m_pipeout);
		    }
		}
	    }
#endif
	    emit SetScrollState(m_autoScroll);
#ifdef USEQPE
	    QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
#endif
	    return;
	}
    }
  if (bredraw) emitRedraw();
}

void QTReader::redrawScroll(QPainter* p)
{
  int offset = (m_scrolltype == 1) ? m_totalscroll : 0;
  switch (m_scrollpos)
    {
    case 1:
      {
	int hmargin = ((m_scrollpos == 1) ? _SBARHEIGHT : 0);
	if (hmargin < m_bottommargin) hmargin = m_bottommargin;
	if (m_rotated)
	  {
	    if (m_bgpm.isNull())
	      {
		p->fillRect(0,0,hmargin,height(),m_bg);
	      }
	    else
	      {
		blitRot(0,0, height(), hmargin, NULL);
	      }
	  }
	else
	  {
	    if (m_bgpm.isNull())
	      {
		p->fillRect(0,height() -  hmargin,width(),hmargin,m_bg);
	      }
	    else
	      {
		int toffset = (offset+height()-hmargin) % height();
		if (toffset+hmargin > height())
		  {
		    int fp = height()-toffset;
		    bitBlt(this,
			   0,height() -  hmargin,
			   dbuff,
			   0, toffset, width(), fp);
		    bitBlt(this,
			   0,height()-hmargin+fp,
			   dbuff,
			   0, 0, width(), hmargin-fp);
		  }
		else
		  {
		    bitBlt(this,
			   0,height() - hmargin,
			   dbuff,
			   0, toffset, width(), hmargin);
		  }
	      }
	  }
      }
      break;
    case 2: //right
	  if (m_rotated)
	    {
	      if (m_bgpm.isNull())
		{
		  p->fillRect(0,height()-m_right_border,width(),m_right_border,m_bg);
		}
	      else
		{
		  blitRot(0,height()-m_right_border, m_right_border, width(), NULL);
		}
	    }
	  else
	    {
	      if (m_bgpm.isNull())
		{
		  p->fillRect(width()-m_right_border,0,m_right_border,height(),m_bg);
		}
	      else
		{
		  int x = width() - m_right_border;
		  int fp = height()-offset;
		  bitBlt(this, x,  0, dbuff, x, offset, m_right_border, fp);
		  bitBlt(this, x, fp, dbuff, x,      0, m_right_border, height()-fp);
		}
	    }
	  break;
    case 3: //left
	  if (m_rotated)
	    {
	      if (m_bgpm.isNull())
		{
		  p->fillRect(0,0,width(),m_left_border,m_bg);
		}
	      else
		{
		  blitRot(0,0, m_left_border, width(), NULL);
		}
	    }
	  else
	    {
	      if (m_bgpm.isNull())
		{
		  p->fillRect(0,0,m_left_border,height(),m_bg);
		}
	      else
		{
		  int fp = height()-offset;
		  bitBlt(this, 0,  0, dbuff, 0, offset, m_left_border, fp);
		  bitBlt(this, 0, fp, dbuff, 0,      0, m_left_border, height()-fp);
		}
	    }
	  break;
    case 0:
    default:
      break;
    }
  if (m_scrollpos != 0) DrawScroll(p, width(), height());
}

void QTReader::autoscroll()
{
  if (m_scrolltype == 4)
    {
      readAloud();
    }
  else
    {
      if (dbuff != NULL)
	{
	  dbp->begin(dbuff);
	  drawBackground(dbp);
	  dbp->end();
	}
      timer->start(real_delay(), false);
    }
}

void QTReader::setfont()
{
//    m_fontControl.Change
    m_charWidth = (m_charpc*m_fontControl.currentsize())/100;
    if (m_charWidth <= 0) m_charWidth = 1;
    m_ascent = m_fontControl.ascent();
    m_descent = m_fontControl.descent();
    m_linespacing = m_fontControl.lineSpacing();
}

void QTReader::DrawStraight(QPainter* p, int w, int h)
{
  if (m_scrolldy == m_topmargin)
    {
      int ypos = textarray[0]->ascent()-m_scrolldy1+m_topmargin;
      textarray[0]->render( p, ypos, m_bMonoSpaced, m_charWidth, w, m_left_border, m_right_border, m_bg, h-m_topmargin-m_bottommargin);
      int i;
      for (i = 1; i < numlines; i++)
	{
	  ypos += (textarray[i-1]->descent() + textarray[i]->ascent())+
	    (textarray[i-1]->lineExtraSpacing() + textarray[i]->lineExtraSpacing())/2;
	  textarray[i]->render( p, ypos, m_bMonoSpaced, m_charWidth, w, m_left_border, m_right_border, m_bg, h-m_topmargin-m_bottommargin);
	}
      if (textarray[i]->showPartial())
	{
	  ypos += (textarray[i-1]->descent() + textarray[i]->ascent())+
	    (textarray[i-1]->lineExtraSpacing() + textarray[i]->lineExtraSpacing())/2;
	  textarray[i]->render( p, ypos, m_bMonoSpaced, m_charWidth, w, m_left_border, m_right_border, m_bg, h-m_topmargin-m_bottommargin);
	}
    }
  else
    {
      int ypos = textarray[0]->ascent()-m_scrolldy1+m_scrolldy+m_topmargin;
      textarray[0]->render( p, ypos, m_bMonoSpaced, m_charWidth, w, m_left_border, m_right_border, m_bg, h-m_topmargin-m_bottommargin);
      //      p->fillRect(m_border, 0, w-2*m_border, m_scrolldy, m_bg);
      for (int i = 1; i < numlines; i++)
	{
	  ypos += (textarray[i-1]->descent() + textarray[i]->ascent())+
	    (textarray[i-1]->lineExtraSpacing() + textarray[i]->lineExtraSpacing())/2;
	  if (ypos+textarray[i]->descent() > h)
	    {
	      ypos = textarray[i]->ascent();
	    }
	  textarray[i]->render( p, ypos, m_bMonoSpaced, m_charWidth, w, m_left_border, m_right_border, m_bg, h-m_topmargin-m_bottommargin);
	}
      p->fillRect(m_left_border,m_scrolldy,w-(m_left_border+m_right_border),1,m_scrollcolor);
    }
  bool wasrotated = m_rotated;
  m_rotated = false;
  DrawScroll(p, w, h);
  m_rotated = wasrotated;

}

void QTReader::redrawall()
{
  if (m_rotated)
    {
      if (dbuff != NULL)
	{
	  dbp->begin(dbuff);
	  drawBackground(dbp);
	  DrawStraight(dbp, height(), width());
	  dbp->end();
	  
	  QWMatrix m;
	  m.rotate(90);
	  QPixmap rp = dbuff->xForm(m);
	  bitBlt(this, 0,0,&rp,0,0,-1,-1);
	}
      else
	{
	  qDebug("This shouldn't happen but it doesn't matter if it does (rotated == double buffered)");
	  QPixmap dbuff(height(), width());
	  QPainter dbp(&dbuff);

	  drawBackground(&dbp);
	  DrawStraight(&dbp, height(), width());
	  
	  QWMatrix m;
	  m.rotate(90);
	  QPixmap rp = dbuff.xForm(m);
	  bitBlt(this, 0,0,&rp,0,0,-1,-1);
	}
    }
  else
    {
      if (dbuff != NULL)
	{
	  dbp->begin(dbuff);
	  drawBackground(dbp);
	  DrawStraight(dbp, width(), height());
	  dbp->end();
	  bitBlt(this, 0,0,dbuff,0,0,-1,-1);
	}
      else
	{
	  QPainter p(this);
	  drawBackground(&p);
	  DrawStraight(&p, width(), height());
	}
    }
}

void QTReader::drawFonts()
{
    if (bDoUpdates)
    {
      int hmargin = ((m_scrollpos == 1) ? _SBARHEIGHT : 0);
      if (hmargin < m_bottommargin) hmargin = m_bottommargin;
//	qDebug("How refreshing...");
	if (buffdoc.empty())
	  {
	    if (dbuff != NULL)
	      {
		dbp->begin(dbuff);
		drawBackground(dbp);
		dbp->end();
	      }
	    else
	      {
		QPainter p(this);
		drawBackground(&p);
	      }
	    return;
	  }
	setfont();
	//	if (!m_autoScroll) m_scrolldy1 = 0;
	if (dbuff != NULL && (dbuff->width() != width() || dbuff->height() != height()))
	  {
	    qDebug("Oh no! A resize event was missed...");
	    if (m_rotated)
	      {
		dbuff->resize(height(), width());
	      }
	    else
	      {
		dbuff->resize(width(), height());
	      }
	    m_lastwidth = 0;
	  }
	if (m_lastwidth != ((m_rotated) ? height() : width()))
	  {
	    m_scrolldy = m_topmargin;
	    //	    qDebug("Not Optimised %d", m_lastwidth);
	    m_lastwidth = ((m_rotated) ? height() : width());
	    m_lastheight = ((m_rotated) ? width() : height());
	    buffdoc.setwidth(m_lastwidth-(m_left_border+m_right_border));
	    buffdoc.locate(pagelocate());
	    fillbuffer();
	    redrawall();
	    //	    qDebug("Not Optimised %d", m_lastwidth);
	  }
	else 
	  {
	    int newht = ((m_rotated) ? width() : height());
	    if (m_lastheight > newht)
	      {
		//		qDebug("Optimised < %d %d %d", numlines, m_lastheight, newht);
		m_scrolldy = m_topmargin;
		int ypos = m_scrolldy1+m_topmargin;
		for (int i = 0; i < numlines; i++)
		  {
		    if ((ypos += textarray[i]->lineSpacing()) > newht - hmargin)		      
		      {
			numlines = i;
			jumpto(mylastpos = locnarray[i+1]);
			break;
		      }
		  }
		//		qDebug("Optimised < %d", numlines);
		m_lastheight = newht;
	      }
	    else if (m_lastheight < newht)
	      {
		m_scrolldy = m_topmargin;
		//		qDebug("Optimised > %d", numlines);
		int ypos = m_scrolldy1+m_topmargin;
		for (int i = 0; i <= numlines; i++)
		  {
		    ypos += textarray[i]->lineSpacing();
		  }
		fillbuffer(numlines+1, ypos, newht);
		//		qDebug("Optimised > %d", numlines);
	      }
	    if (numlines > 0)
	      {
		redrawall();
	      }
	  }
	emitRedraw();
    }
/*
    else
    {
	qDebug("Not so refreshing...");
    }
*/
}

void QTReader::DrawScroll( QPainter *p, int _w, int _h )
{
  if (!buffdoc.empty())
    {
      QBrush checkered = QBrush( Dense4Pattern );
      checkered.setColor(m_scrollbarcolor);
      switch (m_scrollpos)
	{
	case 1:
	  if (m_rotated)
	    {
	      p->fillRect(0, 0, 2, _h, checkered);
	    }
	  else
	    {
	      p->fillRect(0, _h-2, _w, 2, checkered);
	    }
	  break;
	case 2:
	  if (m_rotated)
	    {
	      p->fillRect(0, _h-2, _w, 2, checkered);
	    }
	  else
	    {
	      p->fillRect(_w-2, 0, 2, _h, checkered);
	    }
	  break;
	case 3:
	  if (m_rotated)
	    {
	      p->fillRect(0, 0, _w, 2, checkered);
	    }
	  else
	    {
	      p->fillRect(0, 0, 2, _h, checkered);
	    }
	  break;
	case 0:
	default:
	  break;
	}
      switch (m_scrollpos)
	{
	case 1:
	  {
	    int ht;
	    if (m_rotated)
	      {
		ht = _h;
	      }
	    else
	      {
		ht = _w;
	      }
	    int sectionsize = (buffdoc.endSection()-buffdoc.startSection());
	    int mid = (ht*(locnarray[numlines]+locnarray[0]-2*buffdoc.startSection())+sectionsize)/(2*sectionsize);
	    int sliderheight = ((locnarray[numlines]-locnarray[0])*ht+sectionsize/2)/sectionsize;
	    int sliderpos;
	    if (sliderheight < 10)
	      {
		sliderheight = 10;
		sliderpos = mid-5;
	      }
	    else
	      {
		sliderpos = (ht*(locnarray[0]-buffdoc.startSection())+sectionsize/2)/sectionsize;
	      }
	    if (m_rotated)
	      {
		p->fillRect(0, sliderpos, 3, sliderheight, m_scrollbarcolor);
	      }
	    else
	      {
		p->fillRect(sliderpos, _h-3, sliderheight, 3, m_scrollbarcolor);
	      }
	  }
	  break;
	case 2:
	case 3:
	  {
	    int ht;
	    if (m_rotated)
	      {
		ht = _w;
	      }
	    else
	      {
		ht = _h;
	      }
	    int sectionsize = (buffdoc.endSection()-buffdoc.startSection());
	    int mid = (ht*(locnarray[numlines]+locnarray[0]-2*buffdoc.startSection())+sectionsize)/(2*sectionsize);
	    int sliderheight = ((locnarray[numlines]-locnarray[0])*ht+sectionsize/2)/sectionsize;
	    int sliderpos;
	    if (sliderheight < 10)
	      {
		sliderheight = 10;
		sliderpos = mid-5;
	      }
	    else
	      {
		sliderpos = (ht*(locnarray[0]-buffdoc.startSection())+sectionsize/2)/sectionsize;
	      }
	    if (m_rotated)
	      {
		int hoff;
		if (m_scrollpos == 2) //right
		  {
		    hoff = _h-3;
		  }
		else
		  {
		    hoff = 0;
		  }
		p->fillRect(ht-sliderpos-sliderheight, hoff, sliderheight, 3, m_scrollbarcolor);
	      }
	    else
	      {
		int hoff;
		if (m_scrollpos == 2) //right
		  {
		    hoff = _w-3;
		  }
		else
		  {
		    hoff = 0;
		  }
		p->fillRect(hoff, sliderpos, 3, sliderheight, m_scrollbarcolor);
	      }
	  }
	  break;
	case 0:
	default:
	  break;
	}
    }
}

/*
void QTReader::DrawScroll( QPainter *p )
{
  if (m_border > 5 && !buffdoc.empty())
    {
      int ht, wh;
      if (m_rotated)
	{
	  ht = width();
	  wh = height()g;
	  p->fillRect(0, wh-2, ht, 2, cyan);
	}
      else
	{
	  ht = height();
	  wh = width();
	  p->fillRect(wh-2, 0, 2, ht, cyan);
	}
      int sectionsize = (buffdoc.endSection()-buffdoc.startSection());
      int mid = (ht*(locnarray[numlines]+locnarray[0]-2*buffdoc.startSection())+sectionsize)/(2*sectionsize);
      if (m_rotated)
	{
	  p->fillRect(ht-mid-5, wh-2, 10, 2, yellow);
	  p->fillRect(ht-(ht*(locnarray[numlines]-buffdoc.startSection())+sectionsize/2)/sectionsize, wh-2, ((locnarray[numlines]-locnarray[0])*ht+sectionsize/2)/sectionsize, 2, magenta);
	}
      else
	{
	  p->fillRect(wh-2, mid-5, 2, 10, yellow);
	  p->fillRect(wh-2, (ht*(locnarray[0]-buffdoc.startSection())+sectionsize/2)/sectionsize, 2, ((locnarray[numlines]-locnarray[0])*ht+sectionsize/2)/sectionsize, magenta);
	}
    }
}
*/
QString QTReader::firstword()
{
    if (m_bMonoSpaced)
    {
	return toQString(textarray[0]->data());
    }
    else
    {
	int start, end, len, j;
	for (j = 0; j < numlines; j++)
	{
	    len = textarray[j]->length();
	    for (start = 0; start < len && !isalpha((*textarray[j])[start]); start++);
	    if (start < len) break;
	}
	if (j < numlines)
	{
	    QString ret = "";
	    for (end = start; end < len && isalpha((*textarray[j])[end]); end++)
		ret += (*textarray[j])[end];
	    if (ret.isEmpty()) ret = "Current position";
	    return ret;
	}
	else
	    return "Current position";
    }
}

//
// Construct the QTReader with buttons.
//

bool QTReader::ChangeFont(int tgt)
{
    return m_fontControl.ChangeFont(m_fontname, tgt);
}

void QTReader::init()
{
  setBackgroundColor( m_bg );
  buffdoc.setfilter(getfilter());
  ChangeFont(m_textsize);
  setFocusPolicy(QWidget::StrongFocus);
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(doscroll()));
#ifdef USETIMER
  m_dragtimer = new QTimer(this);
  connect(m_dragtimer, SIGNAL(timeout()), this, SLOT(actionDrag()));
#endif
  //  QMessageBox::information(this, "init", m_lastfile, 1);
  setfont();
}

//
// Clean up
//
QTReader::~QTReader()
{
  if (m_output != NULL)
    {
      delete m_output;
    }
  if (dbuff != NULL)
    {
      delete dbuff;
      delete dbp;
    }
#ifdef USEQPE
    if (m_autoScroll)
    {
	QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
    }
#endif
#ifdef _SCROLLPIPE
    if (m_pipeout != NULL)
    {
	fclose(m_pipeout);
    }
#endif
}

//
// Called when the print button is clicked.
//
/*
void QTReader::printIt()
{
#ifndef QT_NO_PRINTER
    if ( printer->setup( this ) ) {
        QPainter paint;
        if ( !paint.begin( printer ) )
            return;
        drawIt( &paint );
    }
#endif
}
*/
//
// Called when the widget needs to be updated.
//

void QTReader::paintEvent( QPaintEvent * p )
{
  if ((dbuff != NULL) && !m_outofdate)
    {
      if (m_rotated)
	{
	  if ((p->rect().width() != width()) || (p->rect().height() != height()))
	    {
	      qDebug("Partial paint");
	      QRect r;
	      r.setTop(width()-p->rect().right()-1);
	      r.setLeft(p->rect().top());
	      r.setHeight(p->rect().width());
	      r.setWidth(p->rect().height());
	      QPixmap p1(r.width(), r.height());
	      bitBlt(&p1, QPoint(0, 0), dbuff, r);
	      QWMatrix m;
	      m.rotate(90);
	      QPixmap p2 = p1.xForm(m);
	      bitBlt(this, p->rect().left(), p->rect().top(), &p2, 0, 0, -1, -1);
	    }
	  else
	    {
	      qDebug("Full paint");
	      QWMatrix m;
	      m.rotate(90);
	      QPixmap rp = dbuff->xForm(m);
	      bitBlt(this, 0,0,&rp,0,0,-1,-1);
	    }
	}
      else
	{
	  //bitBlt(this, 0,0,dbuff,0,0,-1,-1);
	  bitBlt(this,p->rect().topLeft(),dbuff,p->rect());
	}
    }
  else
    {
      drawFonts();
    }
  m_outofdate = false;
}

//
// Called when the widget has been resized.
// Moves the button group to the upper right corner
// of the widget.

/*
void QTReader::resizeEvent( QResizeEvent * )
{
//  //  qDebug("resize:(%u,%u)", width(), height());
  //    bgroup->move( width()-bgroup->width(), 0 );
}
*/

//
// Create and display our widget.
//
/*
int main( int argc, tchar **argv )
{
    QApplication app( argc, argv );
    QTReader   draw;
    app.setMainWidget( &draw );
    draw.setCaption("Qt Example - Drawdemo");
    draw.show();
    return app.exec();
}
*/


bool QTReader::locate(unsigned long n)
{
  m_outofdate = true;
  m_lastwidth = 0;
  locnarray[0] = n;
  ResetScroll();
  update();
  return true;
}
/*
bool QTReader::locate(unsigned long n) {
  //printf("Locate\n");
  buffdoc.locate(n);
//  //  qDebug("&buffdoc.located");
  ResetScroll();
  fillbuffer();
  m_outofdate = true;
//  //  qDebug("&Buffer filled");
  update();
//  //  qDebug("&Located");
  emitRedraw();
  return true;
}
*/
unsigned int QTReader::screenlines()
{
  //  int linespacing = (tight) ? m_ascent : m_ascent+m_descent;
  //  return (height()-m_descent)/(m_linespacing);
  return (height()-2)/(m_linespacing);
};

bool QTReader::fillbuffer(int reuse, int ht, int newht)
{
    int hmargin = ((m_scrollpos == 1) ? _SBARHEIGHT : 0);
    if (hmargin < m_bottommargin) hmargin = m_bottommargin;
    if (ht < 0) ht = m_topmargin;
    if (buffdoc.empty()) return false;
    if (newht < 0)
      m_lastheight = (m_rotated) ? width() : height();
    else
      m_lastheight = newht;
    int ch;
    bool ret = false;
    unsigned int oldpagepos = locnarray[reuse];
    int lastypos = ht, ypos = ht;
    numlines = reuse;
    while (ypos < m_lastheight - hmargin || numlines < 2)
    {
	lastypos = ypos;
	if (textarray[numlines] == NULL)
	{
	    textarray[numlines] = new CDrawBuffer(&m_fontControl);
	}
	locnarray[numlines] = locate();
	int ch = getline(textarray[numlines]);
	ypos += textarray[numlines]->lineSpacing();
	/*
	QString tmp = toQString(textarray[numlines]->data());
	printf("[%u, %u, %u](%s):%s\n", lastypos, m_lastheight-hmargin, ypos,
	       ((textarray[numlines]->showPartial()) ? "TRUE" : "FALSE"),
	       (const char*)tmp);
	*/
	numlines++;
	if (!ch)
	{
	  if (numlines - reuse == 1 /*&& locnarray[numlines] == buffdoc.locate()*/)
	    {
	      qDebug("FALSE");
	      if (oldpagepos < buffdoc.endSection())
		locate(oldpagepos);
	      else
		dopageup(buffdoc.endSection());
	      return false;
	    }
	    else
	    {
	      qDebug("TRUE");
		--numlines;
		mylastpos = locate();
		return true;
	    }
	}
	if (numlines > 1 && textarray[numlines-2]->isBop())
	{
	  --numlines;
	  mylastpos = locate();
	  return true;
	}
    }
    
    --numlines;
    mylastpos = locate();
    m_scrollpart = m_lastheight - lastypos - hmargin;
    if (m_autoScroll)
      {
	CalculateScrollParameters();
      }
    return true;
}

void QTReader::dopagedn()
{
//    qDebug("HEIGHT(2):%d", m_lastheight);
    ResetScroll();
    int skip = 0, ypos = m_topmargin;
    if (locate() != mylastpos)
    {
	jumpto(mylastpos);
    }
    CDrawBuffer* reusebuffer = textarray[numlines];
    if (reusebuffer != NULL)
    {
      if (reusebuffer->eof()) return;
	for (int i = 0; i <= m_overlap; i++)
	{
	    int offset = numlines - m_overlap + i;
	    reusebuffer = textarray[offset];
	    size_t reuselocn = locnarray[offset];
	    textarray[offset] = textarray[i];
	    textarray[i] = reusebuffer;
//	    reusebuffer->empty();
	    locnarray[offset] = locnarray[i];
	    locnarray[i] = reuselocn;
	    ypos += textarray[i]->lineSpacing();
	    skip++;
	}
    }
    if (numlines <= 1)
      {
	skip = 0;
	ypos = 0;
	qDebug("Doing extra skip");
      }
    if (fillbuffer(skip, ypos))
    {
	drawFonts();
    }
}

void QTReader::dopageup()
{
  dopageup(locnarray[(m_overlap < numlines) ? m_overlap : numlines/2]);
}

bool QTReader::synch(size_t start, size_t end)
{
    jumpto(start);
    while (start++ < end)
    {
	tchar ch = getch();
	if (ch == 10) return true;
	if ((ch == UEOF) || (ch == 6))
	{
	  return false;
	}
    }
    return false;
}

void QTReader::dopageup(unsigned int target)
{
    ResetScroll();
    CBufferFace<CDrawBuffer*> buff;
    CBufferFace<size_t> loc;
    size_t delta, guess = 2*(locate()-pagelocate()), lastdelta = 0;
    qDebug("dopageup:: locate():%u pagelocate():%u guess:%u", locate(), pagelocate(), guess);
  bool ch = true;
  int nbfl, ypos = m_topmargin;
  if (guess < 128) guess = 128;
  while (1)
  {
    //      qDebug("Guess:%u", guess);

      ch = true;
      if (target < guess)
      {
	  delta = 0; // 0 is a flag to say don't guess any more
	  jumpto( (m_continuousDocument) ? 0 : buffdoc.startSection() );
      }
      else if (!m_continuousDocument && (target - guess < buffdoc.startSection()))
      {
	  delta = 0; // 0 is a flag to say don't guess any more
	  qDebug("Jumping to startsection:%d", buffdoc.startSection());
	  jumpto(buffdoc.startSection());
      }
      else
      {
	  delta = guess;
	  if (!synch(target-delta, target-lastdelta))
	  {
	    lastdelta = delta;
	    if (guess < 4000)
	      {
		guess <<= 1;
		continue;
	      }
	    else
	      {
		jumpto(target-delta);
	      }
	  }
      }
      
      nbfl = 0;
      ypos = m_topmargin;

      while (locate() < target) 
      {
	  if (buff[nbfl] == NULL) buff[nbfl] = new CDrawBuffer(&m_fontControl);
	  loc[nbfl] = locate();
	  ch = getline(buff[nbfl]);
	  ypos += buff[nbfl]->lineSpacing();
	  nbfl++;
	  if (!ch) break;
      }
      if (guess < 4000 && ypos < ((m_rotated) ? width() : height())-(m_bottommargin) && (delta != 0))
      {
	  for (int i = 0; i < nbfl; i++)
	  {
	      delete buff[i];
	      buff[i] = NULL;
	  }
	  guess <<= 1;
	  continue;
      }
      break;
  }
  if (ch)
  {
      if (buff[nbfl] == NULL) buff[nbfl] = new CDrawBuffer(&m_fontControl);
      loc[nbfl] = locate();
      int ch = getline(buff[nbfl]);
      nbfl++;
  }
/*
  ypos = 0;
  numlines = 0;
  while (ypos < height() && numlines <= nbfl-1)
  {
      ypos += buff[nbfl - numlines - 1]->lineSpacing();
      numlines++;
  }
  --numlines;
*/

  ypos = m_topmargin;
  numlines = 0;
  while (ypos < ((m_rotated) ? width() : height())-m_bottommargin && numlines+2 <= nbfl)
  {
      ypos += buff[nbfl - numlines - 2]->lineSpacing();
      numlines++;
  }
  if (numlines > 0) --numlines;
  if (numlines == 0 && nbfl > 1) numlines = 1;

  int offset = nbfl-1;
  offset -= numlines;
  ypos = m_topmargin;
  for (int i = 0; i <= numlines; i++)
  {
      delete textarray[i];
      textarray[i] = buff[offset+i];
      locnarray[i] = loc[offset + i];
      ypos += textarray[i]->lineSpacing();
  }
#ifdef _WINDOWS
  for (i = 0; i < nbfl - numlines - 1; i++)
#else
  for (int i = 0; i < nbfl - numlines - 1; i++)
#endif
  {
      delete buff[i];
  }

  while (ypos < ((m_rotated) ? width() : height())-m_bottommargin)
  {
      numlines++;
      locnarray[numlines] = locate();
      if (textarray[numlines] == NULL) textarray[numlines] = new CDrawBuffer(&m_fontControl);
      if (!getline(textarray[numlines])) break;
      ypos += textarray[numlines]->lineSpacing();
  }

  mylastpos = locate();
  CalculateScrollParameters();
  drawFonts();
	//  repaint();
}

bool QTReader::load_file(const char *newfile, unsigned int _lcn)
{
//  QMessageBox::information(this, "Name", name, 1);
//  QMessageBox::information(this, "load_file", newfile, 1);
  int prog = 0;
  bool bRC = false;
  unsigned int lcn = _lcn;
  bDoUpdates = false;
  ResetScroll();
  if (m_lastfile == newfile && lcn == 0)
    {
      lcn = m_lastposn;
    }
  //  QMessageBox::information(0, "Opening...", newfile);
  if (m_rotated)
    {
      m_lastwidth = height();
      m_lastheight = width();
    }
  else
    {
      m_lastwidth = width();
      m_lastheight = height();
    }
  if (buffdoc.openfile(this,newfile) == 0)
  {
      m_lastfile = newfile;
      buffdoc.setwidth(m_lastwidth-(m_left_border+m_right_border));
      bRC = true;
      buffdoc.setContinuous(m_continuousDocument);
      qDebug("buffdoc.openfile done");
    }
  setfilter(getfilter());
  qDebug("Updated");
  bDoUpdates = true;
  locate(lcn);
  return bRC;
}

void QTReader::lineDown()
{
    int ypos = m_topmargin;
    ResetScroll();
    int offset = numlines;

    for (int i = 0; i <= numlines; i++)
    {
	if ((ypos += textarray[numlines-i]->lineSpacing()) > ((m_rotated) ? width() : height()))
	{
	    offset = i-1;
	    break;
	}
    }
    offset = numlines - offset;
#ifdef _WINDOWS
    for (i = offset; i <= numlines; i++)
#else
    for (int i = offset; i <= numlines; i++)
#endif
    {
	CDrawBuffer* buff = textarray[i-offset];
	textarray[i-offset] = textarray[i];
	locnarray[i-offset] = locnarray[i];
	textarray[i] = buff;
    }
    numlines = numlines - offset + 1;
    locnarray[numlines] = locate();
    if (textarray[numlines] == NULL)
    {
	textarray[numlines] = new CDrawBuffer(&m_fontControl);
    }
    getline(textarray[numlines]);
    mylastpos = locate();
    m_outofdate = true;
    update();
}
/*
void QTReader::lineUp()
{
  CBuffer** buff = textarray;
  unsigned int *loc = new unsigned int[numlines];
  int cbptr = 0;
  if (locate() != mylastpos) jumpto(mylastpos);
  unsigned int target = locnarray[numlines-1];
  if (buffdoc.hasrandomaccess())
    {
      unsigned int delta = locate()-pagelocate();
      if (delta < 64) delta = 64;
      do
	{
	  delta <<= 1;
	  if (delta >= target)
	    {
	      delta = target;
	      jumpto(0);
	      for (int i = 0; i < numlines; i++)
		{
		  loc[i] = locate();
		  getline(buff[i]);
		}
	      break;
	    }
	  jumpto(target-delta);
	  do
	    {
	      buffdoc.getline(buff[0],width());
#ifdef WS
	      //printf("Trying:%s\n",buff[0]);
#endif
	      if (locate() > target) continue;
	    }
	  while (!buffdoc.iseol());
	  for (int i = 0; i < numlines; i++)
	    {
	      loc[i] = locate();
	      buffdoc.getline(buff[i],width());
#ifdef WS
	      //printf("Filling:%s\n",buff[i]);
#endif
	    }
	}
      while (locate() >= target && delta < 4096);
#ifdef WS
      //printf("Delta:%u\n",delta);
#endif
    }
  else
    {
      jumpto(0);
      for (int i = 0; i < numlines; i++)
	{
	  loc[i] = locate();
	  buffdoc.getline(buff[i],width());
	}
    }
  cbptr = 0;
  while (locate() < target)
    {
      loc[cbptr] = locate();
      buffdoc.getline(buff[cbptr], width());
#ifdef WS
      //printf("Adding:%s\n",buff[cbptr]->data());
#endif
      cbptr = (cbptr+1) % numlines;
    }
  pagepos = loc[cbptr];
  textarray = new CBuffer*[numlines];
  for (int i = 0; i < numlines; i++)
    {
      int j = (cbptr+i)%numlines;
      textarray[i] = buff[j];
      locnarray[i] = loc[j];
    }
  delete [] buff;
  delete [] loc;
  mylastpos = locate();
  update();
}
*/

void QTReader::ResetScroll()
{
  m_totalscroll = 0;
  m_scrolldy1 = 0;
  m_scrolldy = m_topmargin;
  if (m_autoScroll && ((m_scrolltype == 0) || !m_bgpm.isNull()))
    {
      setautoscroll(false);
    }
}

void QTReader::lineUp()
{
  dopageup(locnarray[numlines-1]);

  /*
  buffdoc.unsuspend();
  ResetScroll();

  CDrawBuffer* buff = textarray[numlines];
  unsigned int loc;
  unsigned int end = locnarray[numlines];
  int cbptr = 0;
  if (locate() != mylastpos) jumpto(mylastpos);
  unsigned int target = locnarray[0];
  if (target == 0) return;
  if (!m_continuousDocument && (target == buffdoc.startSection())) return;
  if (buffdoc.hasrandomaccess())
    {
      unsigned int delta = locate()-pagelocate();
      if (delta < 64) delta = 64;
      do
	{
	  delta <<= 1;
	  if (delta >= target)
	    {
	      delta = target;
	      jumpto(0);
	      loc = locate();
	      getline(buff);
	      break;
	    }
	  else if (!m_continuousDocument && (target - delta < buffdoc.startSection()))
	  {
	      delta = target-buffdoc.startSection();
	      jumpto(buffdoc.startSection());
	      loc = locate();
	      getline(buff);
	      break;
	  }
	  jumpto(target-delta);
	  do
	    {
	      getline(buff);
#ifdef WS
	      //printf("Trying:%s\n",buff[0]);
#endif
	      if (locate() > target) continue;
	    }
	  while (!buffdoc.iseol());
	  loc = locate();
	  getline(buff);
	}
      while (locate() >= target && delta < 4096);
    }
  else
    {
      jumpto(0);
      loc = locate();
      getline(buff);
    }
  cbptr = 0;
  while (locate() < target)
    {
      loc = locate();
      getline(buff);
    }
  for (int i = numlines; i > 0; i--)
    {
      textarray[i] = textarray[i-1];
      locnarray[i] = locnarray[i-1];
    }
  textarray[0] = buff;
  locnarray[0] = loc;
  int start = numlines;
  int ypos = m_topmargin;
#ifdef _WINDOWS
  for (i = 0; i <= numlines; i++)
#else
	for (int i = 0; i <= numlines; i++)
#endif
  {
      ypos += textarray[i]->lineSpacing();
      if (ypos > ((m_rotated) ? width() : height()))
      {
	  start = i;
	  ypos -= textarray[i]->lineSpacing();
	  break;
      }
  }
  jumpto(locnarray[start]);
  fillbuffer(start, ypos);
  repaint();
  */
}

bool QTReader::empty()
{
    return buffdoc.empty();
}

MarkupType QTReader::PreferredMarkup()
{
    MarkupType m = buffdoc.PreferredMarkup();
    if (m == cTEXT)
    {
	int ext = m_lastfile.findRev('.');
	if (ext >= 0)
	{
	    QString ft = m_lastfile.right(m_lastfile.length()-ext-1).upper();
	    if (ft.left(3) == "HTM")
	    {
		m = cHTML;
	    }
	}
    }
    return m;
}

void QTReader::resizeEvent( QResizeEvent * p )
{
  qDebug("Resizing");
  m_outofdate = true;
  if (dbuff != NULL)
    {
      if (m_rotated)
	{
	  dbuff->resize(p->size().height(),p->size().width());
	}
      else
	{
	  dbuff->resize(p->size());
	}
    }
  m_bgIsScaled = false;
  if (m_bgtype == bgStretched)
    {
      emit RefreshBitmap();
    }

  {
    int h, w;
    if (m_rotated)
      {
	h = p->size().width();
	w = p->size().height();
      }
    else
      {
	w = p->size().width();
	h = p->size().height();
      }
    m_topmargin = (h*m_abstopmargin+500)/1000;
    m_bottommargin = (h*m_absbottommargin+500)/1000;
    m_left_border = (w*m_absleft_border+500)/1000;
    m_right_border = (w*m_absright_border+500)/1000;
  }
  if (dbuff != NULL && buffdoc.empty())
    {
      dbp->begin(dbuff);
      drawBackground(dbp);
      dbp->end();
    }
}

void QTReader::setrotated(bool sfs)
{
  qDebug("Rotating");
  m_rotated = sfs;
  setDoubleBuffer(m_doubleBuffered);
  m_bgIsScaled = false;
  m_outofdate = true;
  /*
    int h, w;
    if (m_rotated)
      {
	h = width();
	w = height();
      }
    else
      {
	w = width();
	h = height();
      }
    m_topmargin = (h*m_abstopmargin+500)/1000;
    m_bottommargin = (h*m_absbottommargin+500)/1000;
    m_left_border = (w*m_absleft_border+500)/1000;
    m_right_border = (w*m_absright_border+500)/1000;
    
    qDebug("Top margin:%u", m_topmargin );
    qDebug("Bottom margin:%u", m_bottommargin );
    qDebug("Left margin:%u", m_left_border );
    qDebug("Right margin:%u", m_right_border );
  */
}

void QTReader::drawBackground(QPainter *p)
{
  //  p->setBackgroundMode(OpaqueMode);
  p->setBackgroundColor(m_bg);
  if (dbuff != NULL)
    {
      p->eraseRect(dbuff->rect());
    }
  else
    {
      if (m_rotated)
	{
	  p->eraseRect(0,0,height(),width());
	}
      else
	{
	  p->eraseRect(rect());
	}
    }
  if (!m_bgpm.isNull())
    {
      //      p->setBackgroundMode(TransparentMode);
      switch (m_bgtype)
	{
	case bgCentred:
	  {
	    if (dbuff == NULL)
	      {
		p->drawPixmap(width(),height(),m_bgpm);
	      }
	    else
	      {
		int w = (dbuff->rect().width()-m_bgpm.width())/2;
		int h = (dbuff->rect().height()-m_bgpm.height())/2;
		p->drawPixmap(w,h,m_bgpm);
	      }
	  }
	  break;
	case bgTiled:
	  {
	    if (dbuff == NULL)
	      {
		p->drawTiledPixmap(0,0,width(),height(),m_bgpm);
	      }
	    else
	      {
		p->drawTiledPixmap(0,0,dbuff->rect().width(),dbuff->rect().height(),m_bgpm);
	      }
	  }
	  break;
	case bgStretched:
	  {
	    if (!m_bgIsScaled)
	      {
		m_bgIsScaled = true;
		QImage im = m_bgpm.convertToImage();
		if (dbuff == NULL)
		  {
		    m_bgpm.convertFromImage(im.smoothScale(width(),height()));
		  }
		else
		  {
		    m_bgpm.convertFromImage(im.smoothScale(dbuff->rect().width(), dbuff->rect().height()));
		  }
	      }
	    p->drawPixmap(0,0,m_bgpm);
	  }
	  break;
	default:
	  qDebug("Unknown background type");
	}
      //      p->setBackgroundMode(OpaqueMode);
    } 
}

void QTReader::blitRot(int dx, int dy, int sw, int sh, CDrawBuffer* txt)
{
  if (txt != NULL)
    {
      sh = txt->lineSpacing();
    }
  int sy = width()-dx-sh;
  if (m_autoScroll && !(m_scrolltype == 0))
    {
      sy = (sy+m_totalscroll+1)%width();
    }

  QPixmap pm(sw, sh);
 
  QPainter pd(&pm, this);
  if (m_bgpm.isNull())
    {
      pd.eraseRect(pm.rect());
    }
  else
    {
      if (sy+pm.height() > dbuff->height())
	{
	  //	  pd.eraseRect(pm.rect());
	  int fh = dbuff->height() - sy;
	  if (sy+fh > dbuff->height())
	    {
	      qDebug("Oh no!");
	    }
	  
	  if (fh > pm.height())
	    {
	      qDebug("Oh no! - 2");
	    }
	  
	  bitBlt(&pm,0,0,dbuff,dy,sy,pm.width(),fh);
	  bitBlt(&pm,0,fh,dbuff,dy,0,pm.width(),pm.height()-fh);
	}
      else
	{
	  bitBlt(&pm,0,0,dbuff,dy,sy,pm.width(),pm.height());
	}
    }
  if (txt != NULL)
    {
      //      txt->render(&pd, txt->lineSpacing() - txt->descent() - txt->lineExtraSpacing(), m_bMonoSpaced, m_charWidth, sw, m_left_border, m_right_border, m_bg);
      txt->render(&pd, txt->lineSpacing() - txt->descent() - txt->lineExtraSpacing(), m_bMonoSpaced, m_charWidth, sw, m_left_border, m_right_border, m_bg, width()-m_topmargin-m_bottommargin);
    }
  QWMatrix m;
  m.rotate(90);
  QPixmap rp = pm.xForm(m);
  /*
  p.drawPixmap(QPoint(dx, dy), rp);
  */
  bitBlt(this, dx, dy, &rp, 0, 0, -1, -1, CopyROP);
}

QString QTReader::about()
{
  QString ab = QString("QTReader widget (c) Tim Wentford\n")+buffdoc.about() + "\nMini-scrollbar by Markus Gritsch\nNavigation History fixes by Frantisek Dufka";
  if (m_output != NULL)
    {
      ab += QString("\n") + m_output->about();
    }
  return ab;
}

void QTReader::getNextLink()
{
  if (m_scrolldy != 0)
    {
      setautoscroll(false);
      ResetScroll();
      redrawall();
    }
  bool redraw = false;
  bool found = false;
  if (m_currentlink >= 0)
    {
      m_currentlinkoffset = textarray[m_currentlink]->invertLink(m_currentlinkoffset);
      if ((m_currentlinkstyle = textarray[m_currentlink]->getNextLink(m_currentlinkoffset)) != NULL)
	{
	  qDebug("Found a link at %u", m_currentlinkoffset);
	  int offset = textarray[m_currentlink]->invertLink(m_currentlinkoffset);
	  qDebug("Finishes at %u", offset);
	  found = true;
	}
      redraw = true;
      drawSingleLine(m_currentlink);
      //      if (found) return;
    }
  if (!found)
    {
      m_currentlinkoffset = -1;
      for (int i = m_currentlink+1; i < numlines; ++i)
	{
	  if ((m_currentlinkstyle = textarray[i]->getNextLink(m_currentlinkoffset)) != NULL)
	    {
	      m_currentlink = i;
	      qDebug("Found a link at %u", m_currentlinkoffset);
	      int offset = textarray[m_currentlink]->invertLink(m_currentlinkoffset);
	      qDebug("Finishes at %u", offset);
	      //drawSingleLine(i);
	      redraw = true;
	      found = true;
	      drawSingleLine(m_currentlink);
	      break;
	    }
	}
    }
  if (redraw)
    {
      //      redrawall();
    }
  if (!found)
    {
      m_currentlink = -1;
      m_currentlinkstyle = NULL;
      m_currentlinkoffset = -1;
      dopagedn();
    }
}

void QTReader::emitRedraw()
{
  m_currentlinkstyle = NULL;
  m_currentlink = -1;
  m_currentlinkoffset = -1;
  emit OnRedraw();
};

void QTReader::drawSingleLine(int lineno)
{
  QPainter p( this );
  int ypos = textarray[0]->ascent()+m_topmargin;
  if (lineno == 0)
    {
      if (m_rotated)
	{
	  blitRot(width()-(ypos+textarray[lineno]->descent()+textarray[lineno]->lineExtraSpacing()), 0, height(), -1, textarray[lineno]);
	}
      else
	{
	  if (m_bgpm.isNull())
	    {
	      p.fillRect(m_left_border,ypos-textarray[lineno]->ascent(),width()-(m_left_border+m_right_border),textarray[lineno]->lineSpacing(),m_bg);
	    }
	  else
	    {
	      bitBlt(this, m_left_border, ypos-textarray[lineno]->ascent(), dbuff, m_left_border, ypos-textarray[lineno]->ascent(), width()-(m_left_border+m_right_border), textarray[lineno]->lineSpacing());
	    }
	  textarray[lineno]->render( &p, ypos, m_bMonoSpaced, m_charWidth, width(), m_left_border, m_right_border, m_bg, height()-m_topmargin-m_bottommargin);
	}
    }
  for (int i = 1; i < numlines; i++)
    {
      ypos += (textarray[i-1]->descent() + textarray[i]->ascent())+
	(textarray[i-1]->lineExtraSpacing() + textarray[i]->lineExtraSpacing())/2;
      if (i == lineno)
	{
	  if (m_rotated)
	    {
	      blitRot(width()-(ypos+textarray[i]->descent()+textarray[i]->lineExtraSpacing()), 0, height(), -1, textarray[i]);
	    }
	  else
	    {
	      if (m_bgpm.isNull())
		{
		  p.fillRect(m_left_border,ypos-textarray[lineno]->ascent(),width()-(m_left_border+m_right_border),textarray[lineno]->lineSpacing(),m_bg);
		}
	      else
		{
		  bitBlt(this, m_left_border, ypos-textarray[lineno]->ascent(), dbuff, m_left_border, ypos-textarray[lineno]->ascent(), width()-(m_left_border+m_right_border), textarray[lineno]->lineSpacing());
		}
	      textarray[i]->render( &p, ypos, m_bMonoSpaced, m_charWidth, width(), m_left_border, m_right_border, m_bg, height()-m_topmargin-m_bottommargin);
	    }
	}
    }
}


void QTReader::gotoLink()
{
  if (m_currentlinkstyle == NULL) return;
  textarray[m_currentlink]->invertLink(m_currentlinkoffset);
  size_t saveposn = pagelocate();
  QString href, nm;
  unsigned long tgt = m_currentlinkstyle->getData();
  unsigned long tgtoffset = m_currentlinkstyle->getOffset();
  linkType lt = buffdoc.hyperlink(tgt, tgtoffset, href, nm);
  qDebug("URL(1):%s", (const char*)href);
  if ((lt & eFile) != 0)
    {
      buffdoc.saveposn(m_lastfile, saveposn);
#ifdef USEQPE
      {
	QCopEnvelope e("QPE/System", "busy()");
      }
#endif
      ResetScroll();
      if (!href.isEmpty())
	{
	  if (!buffdoc.getFile(href, nm))
	    {
	      emit NewFileRequest(href);
	    }
	  else
	    {
	      ResetScroll();
	      fillbuffer();
	      m_outofdate = true;
	      update();
	    }
	}
      if (!nm.isEmpty())
	{
	  qDebug("QTReader:Finding %s", (const char*)nm);
	  if (buffdoc.findanchor(nm))
	    {
	      buffdoc.resetPos();
	      fillbuffer();
	      m_outofdate = true;
	      update();
	    }
	}
      //fillbuffer();
      //update();
#ifdef USEQPE
      {
	QCopEnvelope e("QPE/System", "notBusy()");
      }
#endif
    }
  else if ((lt & eLink) != 0)
    {
      buffdoc.saveposn(m_lastfile, saveposn);
      ResetScroll();
      fillbuffer();
      m_outofdate = true;
      update();
    }
  else
    {
      if ((lt & ePicture) != 0)
	{
	  QImage* pm = buffdoc.getPicture(tgt);
	  if (pm != NULL)
	    {
	      emit OnShowPicture(*pm);
	      delete pm;
	    }
	}
      else
	{
	  //				QString anchortext = textarray[lineno]->getanchortext(startoffset);
	  if (!href.isEmpty())
	    {
	      emit OnURLSelected(href, tgt);
	      refresh();
	    }
	}
    }
  m_currentlinkstyle = NULL;
  m_currentlink = -1;
  m_currentlinkoffset = -1;
}

void QTReader::refresh(bool full)
{
  qDebug("Refreshing");
  int h, w;
  if (m_rotated)
    {
      h = width();
      w = height();
    }
  else
    {
      w = width();
      h = height();
    }
  m_topmargin = (h*m_abstopmargin+500)/1000;
  m_bottommargin = (h*m_absbottommargin+500)/1000;
  m_left_border = (w*m_absleft_border+500)/1000;
  m_right_border = (w*m_absright_border+500)/1000;
  
  qDebug("Top margin:%u", m_topmargin );
  qDebug("Bottom margin:%u", m_bottommargin );
  qDebug("Left margin:%u", m_left_border );
  qDebug("Right margin:%u", m_right_border );
  if (full && m_highlightfilter) m_highlightfilter->refresh(pagelocate());
  m_outofdate = true;
  locate(pagelocate());
}

#include "striphtml.h"

CFilterChain* QTReader::getfilter()
{
  CFilterChain * filt = new CFilterChain(getencoding());
  if (bstripcr) filt->addfilter(new stripcr);
  
  if (btextfmt || (bautofmt && (PreferredMarkup() == cTEXT))) filt->addfilter(new textfmt);
  if (bpeanut || (bautofmt && (PreferredMarkup() == cPML))) filt->addfilter(new PeanutFormatter);
  //    if (bstriphtml || (bautofmt && (PreferredMarkup() == cHTML))) filt->addfilter(new striphtml(m_lastfile));
  
#ifdef __STATIC
  if (bstriphtml || (bautofmt && (PreferredMarkup() == cHTML))) filt->addfilter(new striphtml(m_lastfile));
  if (bautofmt && (PreferredMarkup() == cCHM))
    {
      striphtml* f = new striphtml(m_lastfile);
      f->setchm(true);
      filt->addfilter(f);
    }
#else
  if (bstriphtml || (bautofmt && (PreferredMarkup() == cHTML))) filt->addfilter(new ExternFilter("HTMLfilter", m_lastfile));
  if (bautofmt && (PreferredMarkup() == cCHM))
    {
      ExternFilter* f = new ExternFilter("HTMLfilter",m_lastfile);
      ((striphtml*)f->filter())->setchm(true);
      filt->addfilter(f);
    }
#endif
  m_highlightfilter = new HighlightFilter(this);
  filt->addfilter(m_highlightfilter);
  
  if (bdehyphen) filt->addfilter(new dehyphen);
  if (bunindent) filt->addfilter(new unindent);
  if (brepara) filt->addfilter(new repara(m_reparastring));
  if (bonespace) filt->addfilter(new OnePara);
  if (bindenter) filt->addfilter(new indenter(bindenter));
  if (bdblspce) filt->addfilter(new dblspce);
  if (bdepluck) filt->addfilter(new DePluck(pluckernextpart));
  if (bdejpluck) filt->addfilter(new DePluck(jplucknextpart));
  if (brepalm) filt->addfilter(new repalm);
  if (bunderlineLink) filt->addfilter(new underlineLink);
  if (bkern) filt->addfilter(new kern);
  if (bremap) filt->addfilter(new remap);
  if (bmakebold) filt->addfilter(new embolden);
  if (bfulljust) filt->addfilter(new FullJust);
  int r,g,b;
  m_default_bg.rgb(&r, &g, &b);
  if (r != 255 || g != 255 || b != 255)
    filt->addfilter(new setbg(r,g,b));
  m_default_fg.rgb(&r, &g, &b);
  if (r != 0 || g != 0 || b != 0)
    filt->addfilter(new setfg(r,g,b));
  //    if (bNegative) filt->addfilter(new makeNegative);
  if (bInverse) filt->addfilter(new makeInverse);
  if (bNoInlineTables) filt->addfilter(new tableLink);
  return filt;
}

void QTReader::readAloud()
{
#ifdef __STATIC
  return;
#else
  CBuffer para;
  jumpto(pagelocate());
  while (m_autoScroll && (buffdoc.getpara(para) != -1))
    {
      if (para.length() > 0)
        {
          unsigned long lastpos = buffdoc.explocate();
          while (lastpos > mylastpos)
            {
              dopagedn();
              qApp->processEvents();
            }
          jumpto(lastpos);
          QString txt = toQString(para.data());

	  doOutput(txt);
        }
      qApp->processEvents();
    }
#endif
}

bool QTReader::doOutput(const QString& wrd)
{
  if (m_output != NULL)
    {
      m_output->output(wrd);
      return true;
    }
  else
    {
      return false;
    }
}

bool QTReader::checkoutput()
{
  if (m_output == NULL)
    {
      m_output = new outputcodec(m_outputName);
      if (reinterpret_cast<outputcodec*>(m_output)->getStatus() != 0)
	{
	  delete m_output;
	  m_output = NULL;
	  QMessageBox::warning(this, PROGNAME, QString("Couldn't find output codec\n")+m_outputName);
	  return false;
	}
    }
  return true;
}
