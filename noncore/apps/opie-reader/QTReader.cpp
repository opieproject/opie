/****************************************************************************
** $Id: QTReader.cpp,v 1.6 2002-12-20 01:35:01 llornkcor Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qpainter.h>
#include "config.h"
#include "QTReader.h"
#include "QTReaderApp.h"
#include "CDrawBuffer.h"
#include <qpe/qpeapplication.h>
#include <math.h>
#include <ctype.h>
#include <stdio.h> //for sprintf
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <qfontdatabase.h>
#include <qpe/global.h>
#include <qpe/qcopenvelope_qws.h>
#include "StateData.h"

#ifdef _UNICODE
const char *QTReader::fonts[] = { "unifont", "Courier", "Times", 0 };
#else
const char *QTReader::fonts[] = { "Helvetica", "Courier", "Times", 0 };
#endif
//const int   QTReader::fontsizes[] = { 8, 10, 12, 14, 18, 24, 30, 40, 50, 60, 70, 80, 90, 100, 0 };

//const tchar *QTReader::fonts[] = { "unifont", "fixed", "micro", "smoothtimes", "Courier", "Times", 0 };
//const int   QTReader::fontsizes[] = {10,16,17,22,0};
//const tchar *QTReader::fonts[] = { "verdana", "Courier", "Times", 0 };
//const int   QTReader::fontsizes[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,0};

QTReader::QTReader( QWidget *parent=0, const char *name=0, WFlags f = 0) :
  QWidget(parent, name, f),
  m_delay(100),
  m_scrolldy1(0),
  m_scrolldy2(0),
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
  m_navkeys(true)
{
    m_overlap = 1;
//  init();
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
  //  qDebug("Load_file(1)");
  load_file((const tchar*)filename);
}
*/

long QTReader::real_delay()
{
    return ( 8976 + m_delay ) / ( m_linespacing * m_linespacing );
}

void QTReader::mousePressEvent( QMouseEvent* _e )
{
    buffdoc.unsuspend();
    if (_e->button() == RightButton)
    {
	mouseUpOn = false;
	if (buffdoc.hasnavigation())
	{
	    if (_e->y() > (2*height())/3)
	    {
		goDown();
	    }
	    else if (_e->y() < height()/3)
	    {
		goUp();
	    }
	    else
	    {
		if (_e->x() < width()/3)
		{
		    size_t target = pagelocate();
		    if (buffdoc.back(target))
		    {
			locate(target);
		    }
		}
		else if (_e->x() > (2*width())/3)
		{
		    size_t target = pagelocate();
		    if (buffdoc.forward(target))
		    {
			locate(target);
		    }
		}
		else
		{
		    buffdoc.saveposn(pagelocate());
		    locate(buffdoc.getHome());
		}
	    }
	}
	else
	{
	    if (_e->y() > height()/2)
	    {
		goDown();
	    }
	    else
	    {
		goUp();
	    }
	}
    }
}

linkType QTReader::getcurrentpos(int x, int y, size_t& start, size_t& offset, size_t& tgt)
{
    int lineno = 0;
    int ht = textarray[0]->lineSpacing();
    while ((ht < y) && (lineno < numlines))
    {
	ht += textarray[++lineno]->lineSpacing();
    }
    start = locnarray[lineno];
    if (m_bMonoSpaced)
    {
	offset = x/m_charWidth;
    }
    else
    {
	int i;
	CDrawBuffer* t = textarray[lineno];
	x = x - t->offset(width());
	for (i = t->length(); i >= 0 && t->width(i) > x; i--);
	offset = i;
    }
    return textarray[lineno]->getLinkType(offset, tgt);
}

void QTReader::setTwoTouch(bool _b)
{
    setBackgroundColor( white );
    m_twotouch = m_touchone = _b;
}

void QTReader::setContinuous(bool _b)
{
    buffdoc.unsuspend();
    buffdoc.setContinuous(m_continuousDocument = _b);
}

void QTReader::mouseReleaseEvent( QMouseEvent* _e )
{
    buffdoc.unsuspend();
    if (_e->button() == LeftButton)
    {
	if (mouseUpOn)
	{
	    if (textarray[0] != NULL)
	    {
		QString wrd, line;
//		int lineno = _e->y()/m_linespacing;
		int lineno = 0;
		int ht = textarray[0]->lineSpacing();
		while ((ht < _e->y()) && (lineno < numlines))
		{
		    ht += textarray[++lineno]->lineSpacing();
		}
		size_t startpos, startoffset, tgt;
		switch (getcurrentpos(_e->x(), _e->y(), startpos, startoffset, tgt))
		{
		    case eLink:
		    {
			size_t saveposn = pagelocate();
			if (buffdoc.hyperlink(tgt))
			{
			    buffdoc.saveposn(saveposn);
			    fillbuffer();
			    update();
			}
			else
			{
			    locate(pagelocate());
			}
			return;
		    }
		    case ePicture:
		    {
			qDebug("Picture:%x", tgt);
			QPixmap* pm = buffdoc.getPicture(tgt);
			if (pm != NULL)
			{
			    emit OnShowPicture(*pm);
			    delete pm;
			}
			else
			{
			    locate(pagelocate());
			}
			return;
		    }
		    case eNone:
			break;
		    default:
			qDebug("Unknown linktype");
			return;
		}
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
			setBackgroundColor( white );
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
			    jumpto(currentpos);
			}
		    }
		}
		else if (m_bMonoSpaced)
		{
		    int chno = _e->x()/m_charWidth;
		    if (chno < ustrlen(textarray[lineno]->data()))
		    {
			wrd[0] = textarray[lineno]->data()[chno];
		    }
		}
		else
		{
		    CDrawBuffer* t = textarray[lineno];
		    int first = 0;
		    int tgt = _e->x() - t->offset(width());
		    while (1)
		    {
			int i = first+1;
			while (QChar((*t)[i]).isLetter() && (*t)[i] != 0) i++;
			if (t->width(i) > tgt)
			{
			    wrd = toQString(t->data()+first, i - first);
			    break;
			}
			while (!QChar((*t)[i]).isLetter() && (*t)[i] != 0) i++;
			if ((*t)[i] == 0) break;
			first = i;
		    }		
		}
		if (!wrd.isEmpty())
		{
		    emit OnWordSelected(wrd, locnarray[lineno], (m_twotouch) ? wrd : toQString(textarray[lineno]->data()));
		}
	    }
	}
	else
	{
	    mouseUpOn = true;
	}
    }
}

void QTReader::focusInEvent(QFocusEvent* e)
{
    if (m_autoScroll) timer->start(real_delay(), false);
    update();
}

void QTReader::focusOutEvent(QFocusEvent* e)
{
    if (m_autoScroll)
    {
	timer->stop();
	m_scrolldy1 = m_scrolldy2 = 0;
    }
}

#include <qapplication.h>
#include <qdrawutil.h>
#include <unistd.h>

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
    buffdoc.unsuspend();
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
    buffdoc.unsuspend();
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
	setfont();
	m_autoScroll = false;
	locate(pagelocate());
	update();
	m_autoScroll = sc;
	if (m_autoScroll) autoscroll();
    }
}

void QTReader::zoomout()
{
    if (m_fontControl.decreasesize())
    {
	bool sc = m_autoScroll;
	m_autoScroll = false;
	setfont();
	locate(pagelocate());
	update();
	m_autoScroll = sc;
	if (m_autoScroll) autoscroll();
    }
}

void QTReader::keyPressEvent(QKeyEvent* e)
{
    buffdoc.unsuspend();
  switch (e->key())
    {
      case Key_Down:
	{
	  e->accept();
	  if (m_autoScroll)
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
	  else
	  {
	      goDown();
	  }
	}
      break;
      case Key_Up:
	{
	  e->accept();
	  if (m_autoScroll)
	  {
	      if (m_delay > 1024)
	      {
		  m_delay = (2*m_delay)/3;
		  timer->changeInterval(real_delay());
	      }
	      else
	      {
		  m_delay = 1024;
	      }
	  }
	  else
	  {
	      goUp();
	  }
	}
      break;
      /*
      case Key_Left:
	{
	  e->accept();
	  if (m_textfont > 0)
	    {
	      m_textfont--;
	      setfont(NULL);
	      locate(pagelocate());
	      update();
	    }
	}
      break;
      case Key_Right:
	{
	  e->accept();
	  if (fonts[++m_textfont] == 0)
	    {
	      m_textfont--;
	    }
	  else
	    {
	      setfont(NULL);
	      locate(pagelocate());
	      update();
	    }
	}
      break;
      */
	case Key_Right:
	{
	    e->accept();
	    if (m_navkeys && buffdoc.hasnavigation())
	    {
		size_t target = pagelocate();
		if (buffdoc.forward(target))
		{
		    locate(target);
		}
	    }
	    else zoomin();
	}
	break;
	case Key_Left:
	{
	    e->accept();
	    if (m_navkeys && buffdoc.hasnavigation())
	    {
		size_t target = pagelocate();
		if (buffdoc.back(target))
		{
		    locate(target);
		}
	    }
	    else zoomout();
	}
	break;
	case Key_Space:
	case Key_Return:
	{
	  e->accept();
	  emit OnActionPressed();
	}
	break;
	default:
	    e->ignore();
    }
}

void QTReader::setautoscroll(bool _sc)
{
    if (_sc == m_autoScroll) return;
    if (m_autoScroll)
    {
	m_autoScroll = false;
	QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
     }
    else
    {
	CDrawBuffer* reusebuffer = textarray[numlines];
	if (reusebuffer == NULL || reusebuffer->eof()) return;
	m_autoScroll = true;
	autoscroll();
	QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Disable; // light is even not dimmed
    }
}

bool QTReader::getline(CDrawBuffer *buff)
{
    buffdoc.unsuspend();
    if (m_bMonoSpaced)
    {
	return buffdoc.getline(buff ,width(), m_charWidth);
    }
    else
    {
	return buffdoc.getline(buff, width());
    }
}

void QTReader::doscroll()
{
    if (!m_autoScroll)
    {
	timer->stop();
	return;
    }
//    timer->changeInterval(real_delay());
    QPainter p( this );
    QBrush b( white);
    bitBlt(this,0,0,this,0,1,width(),-1);
    qDrawPlainRect(&p,0,height() -  2,width(),2,white,1,&b);

    if (++m_scrolldy1 == textarray[0]->lineSpacing())
    {
	CDrawBuffer* buff = textarray[0];
	for (int i = 1; i <= numlines; i++)
	{
	    textarray[i-1] = textarray[i];
	    locnarray[i-1] = locnarray[i];
	}
	textarray[numlines] = buff;
	--numlines;
	m_scrolldy1 = 0;
    }
    if (++m_scrolldy2 == textarray[numlines]->lineSpacing())
    {
	m_scrolldy2 = 0;
	numlines++;

	if (textarray[numlines] == NULL)
	{
	    textarray[numlines] = new CDrawBuffer(&m_fontControl);
	}
	locnarray[numlines] = locate();
	int ch = getline(textarray[numlines]);
	textarray[numlines-1]->render(&p, height() - textarray[numlines-1]->descent() - 2, m_bMonoSpaced, m_charWidth, width());
	mylastpos = locate();
	if (!ch)
	{
	    m_autoScroll = false;
	    ((QTReaderApp*)parent()->parent())->setScrollState(m_autoScroll);
	}
	emit OnRedraw();
    }
}

void QTReader::autoscroll()
{
    timer->start(real_delay(), false);
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

void QTReader::drawFonts( QPainter *p )
{
    if (bDoUpdates)
    {
	qDebug("How refreshing...");
	if (buffdoc.empty()) return;
	setfont();
	if (m_lastwidth != width())
	{
	    qDebug("Not Optimised %d", m_lastwidth);
	    m_lastwidth = width();
	    m_lastheight = height();
	    locate(pagelocate());
	    qDebug("Not Optimised %d", m_lastwidth);
	}
	else 
	{
	    if (m_lastheight > height())
	    {
		qDebug("Optimised < %d", numlines);
		int ypos = 0;
		for (int i = 0; i < numlines; i++)
		{
		    if ((ypos += textarray[i]->lineSpacing()) > height())
		    {
			numlines = i;
			jumpto(locnarray[i+1]);
			break;
		    }
		}
		qDebug("Optimised < %d", numlines);
		m_lastheight = height();
	    }
	    else if (m_lastheight < height())
	    {
		qDebug("Optimised > %d", numlines);
		int ypos = 0;
		for (int i = 0; i <= numlines; i++)
		{
		    ypos += textarray[i]->lineSpacing();
		}
		fillbuffer(numlines+1, ypos);
		qDebug("Optimised > %d", numlines);
		m_lastheight = height();
	    }
	    if (numlines > 0)
	    {
		int ypos = textarray[0]->ascent();
		textarray[0]->render( p, ypos, m_bMonoSpaced, m_charWidth, width());
		for (int i = 1; i < numlines; i++)
		{
//	  ypos += (textarray[i-1]->lineSpacing() + textarray[i]->lineSpacing())/2;
		    ypos += (textarray[i-1]->descent() + textarray[i]->ascent())+
			(textarray[i-1]->lineExtraSpacing() + textarray[i]->lineExtraSpacing())/2;
		    textarray[i]->render( p, ypos, m_bMonoSpaced, m_charWidth, width());
		}
//      mylastpos = locate();
	    }
	}
	m_scrolldy1 = m_scrolldy2 = 0;
	emit OnRedraw();
    }
    else
    {
	qDebug("Not so refreshing...");
    }
}

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
  //    setCaption( "Qt Draw Demo Application" );

    buffdoc.unsuspend();
  setBackgroundColor( white );
//  QPainter p(this);
//  p.setBackgroundMode( Qt::OpaqueMode );
  buffdoc.setfilter(getfilter());
  ChangeFont(m_textsize);
  setFocusPolicy(QWidget::StrongFocus);
    //    resize( 240, 320 );
  //setFocus();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(doscroll()));
//  QMessageBox::information(this, "init", m_lastfile, 1);
    setfont();
/*
    if (!m_lastfile.isEmpty())
    {
	m_string = DocLnk(m_lastfile).name();
	load_file(m_lastfile);
    }
*/
}

//
// Clean up
//
QTReader::~QTReader()
{
}

//
// Calls the drawing function as specified by the radio buttons.
//

void QTReader::drawIt( QPainter *p )
{
    drawFonts(p);
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

void QTReader::paintEvent( QPaintEvent * )
{
    QPainter paint( this );
    drawIt( &paint );
}

//
// Called when the widget has been resized.
// Moves the button group to the upper right corner
// of the widget.

/*
void QTReader::resizeEvent( QResizeEvent * )
{
  //  qDebug("resize:(%u,%u)", width(), height());
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


bool QTReader::locate(unsigned long n) {
  //printf("Locate\n");
    buffdoc.unsuspend();
  buffdoc.locate(n);
  //  qDebug("&buffdoc.located");
  fillbuffer();
  //  qDebug("&Buffer filled");
  update();
  //  qDebug("&Located");
  return true;
}

unsigned int QTReader::screenlines()
{
  //  int linespacing = (tight) ? m_ascent : m_ascent+m_descent;
  //  return (height()-m_descent)/(m_linespacing);
  return (height()-2)/(m_linespacing);
};

bool QTReader::fillbuffer(int reuse, int ht)
{
    buffdoc.unsuspend();
    if (buffdoc.empty()) return false;
    m_scrolldy1 = m_scrolldy2 = 0;
    int ch;
    bool ret = false;
    unsigned int oldpagepos = locnarray[reuse];
    int ypos = ht;
    numlines = reuse;
    while (ypos < height() || numlines < 2)
    {
	if (textarray[numlines] == NULL)
	{
	    textarray[numlines] = new CDrawBuffer(&m_fontControl);
	}
	locnarray[numlines] = locate();
	int ch = getline(textarray[numlines]);
	ypos += textarray[numlines]->lineSpacing();
	numlines++;
	if (!ch)
	{
	    if (numlines - reuse == 1 /*&& locnarray[numlines] == buffdoc.locate()*/)
	    {
		locate(oldpagepos);
		return false;
	    }
	    else
	    {
		--numlines;
		mylastpos = locate();
		return true;
	    }
	}
    }
    
    --numlines;
    mylastpos = locate();
    
    return true;
}


void QTReader::dopagedn()
{
    buffdoc.unsuspend();
    int skip = 0, ypos = 0;
    if (locate() != mylastpos)
    {
//	qDebug("Jumping to %u", mylastpos);
	jumpto(mylastpos);
    }
    CDrawBuffer* reusebuffer = textarray[numlines];
    if (reusebuffer != NULL && reusebuffer->eof()) return;
    if (reusebuffer != NULL)
    {
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
    if (fillbuffer(skip, ypos))
    {
	update();
    }
}

void QTReader::dopageup()
{
    buffdoc.unsuspend();
  dopageup(locnarray[(m_overlap < numlines) ? m_overlap : numlines/2]);
}

void QTReader::dopageup(unsigned int target)
{
    buffdoc.unsuspend();
    CBufferFace<CDrawBuffer*> buff;
    CBufferFace<size_t> loc;

  size_t delta, guess = 2048;
  bool ch = true;
  int nbfl, ypos = 0;

  while (1)
  {
      ch = true;
      nbfl = 0;
      if (target < guess)
      {
	  delta = 0; // 0 is a flag to say don't guess any more
	  jumpto( (m_continuousDocument) ? 0 : buffdoc.startSection() );
      }
      else if (!m_continuousDocument && (target - guess < buffdoc.startSection()))
      {
	  delta = 0; // 0 is a flag to say don't guess any more
	  jumpto(buffdoc.startSection());
      }
      else
      {
	  delta = guess;
	  
	  jumpto(target - delta);
	  
	  buff[0] = new CDrawBuffer(&m_fontControl);
	  
	  do
	  {
	      
	      if (!getline(buff[0])) break;
	      
	      if (locate() > target) break;
	  }
	  while (!buffdoc.iseol());
      }
      
      ypos = 0;
      while (locate() < target) 
      {
	  if (buff[nbfl] == NULL) buff[nbfl] = new CDrawBuffer(&m_fontControl);
	  loc[nbfl] = locate();
	  ch = getline(buff[nbfl]);
	  ypos += buff[nbfl]->lineSpacing();
	  nbfl++;
	  if (!ch) break;
      }
      if (ypos < height() && (delta != 0))
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
  ypos = 0;
  numlines = 0;
  while (ypos < height() && numlines <= nbfl-1)
  {
      ypos += buff[nbfl - numlines - 1]->lineSpacing();
      numlines++;
  }
  --numlines;
  int offset = nbfl-1;
  offset -= numlines;
  ypos = 0;
  for (int i = 0; i <= numlines; i++)
  {
      delete textarray[i];
      textarray[i] = buff[offset+i];
      locnarray[i] = loc[offset + i];
      ypos += textarray[i]->lineSpacing();
  }
  for (int i = 0; i < nbfl - numlines - 1; i++)
  {
      delete buff[i];
  }

  while (ypos < height())
  {
      numlines++;
      locnarray[numlines] = locate();
      if (textarray[numlines] == NULL) textarray[numlines] = new CDrawBuffer(&m_fontControl);
      if (!getline(textarray[numlines])) break;
      ypos += textarray[numlines]->lineSpacing();
  }

  mylastpos = locate();

  update();
}

bool QTReader::load_file(const char *newfile, unsigned int _lcn)
{
//  QMessageBox::information(this, "Name", name, 1);
//  QMessageBox::information(this, "load_file", newfile, 1);

  bool bRC = false;
  unsigned int lcn = _lcn;
  if (m_lastfile == newfile)
    {
      lcn = m_lastposn;
    }
  m_lastfile = newfile;
  //  QMessageBox::information(0, "Opening...", newfile);
  m_lastwidth = width();
  m_lastheight = height();
  if (buffdoc.openfile(this,newfile) == 0)
    {
      bRC = true;
      buffdoc.setContinuous(m_continuousDocument);
      //  qDebug("buffdoc.openfile done");
      locate(lcn);
      //  qDebug("buffdoc.locate done");
    }
  setfilter(getfilter());
  update();
  //  qDebug("Updated");
  return bRC;
}

void QTReader::lineDown()
{
    int ypos = 0;
    int offset = numlines;

    for (int i = 0; i <= numlines; i++)
    {
	if ((ypos += textarray[numlines-i]->lineSpacing()) > height())
	{
	    offset = i-1;
	    break;
	}
    }
    offset = numlines - offset;
    for (int i = offset; i <= numlines; i++)
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
void QTReader::lineUp()
{
    buffdoc.unsuspend();
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
  int ypos = 0;
  for (int i = 0; i <= numlines; i++)
  {
      ypos += textarray[i]->lineSpacing();
      if (ypos > height())
      {
	  start = i;
	  ypos -= textarray[i]->lineSpacing();
	  break;
      }
  }
  jumpto(locnarray[start]);
  fillbuffer(start, ypos);
  update();
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

void QTReader::setstate(const statedata& sd)
{
    bstripcr = sd.bstripcr;
    btextfmt = sd.btextfmt;
    bautofmt = sd.bautofmt;
    bstriphtml = sd.bstriphtml;
    bpeanut = sd.bpeanut;
    bdehyphen = sd.bdehyphen;
    bonespace = sd.bonespace;
    bunindent = sd.bunindent;
    brepara = sd.brepara;
    bdblspce = sd.bdblspce;
    m_bpagemode = sd.m_bpagemode;
    m_navkeys = sd.m_navkeys;
    m_bMonoSpaced = sd.m_bMonoSpaced;
    bremap = sd.bremap;
    bmakebold = sd.bmakebold;
    m_continuousDocument = sd.Continuous;
#ifdef REPALM
    brepalm = sd.brepalm;
#endif
    bindenter = sd.bindenter;
    m_encd = sd.m_charpc;
    m_fontname = sd.m_fontname;
    setContinuous(sd.Continuous);
    ChangeFont(sd.m_textsize);
    refresh();
}
