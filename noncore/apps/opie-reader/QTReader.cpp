/****************************************************************************
** $Id: QTReader.cpp,v 1.2 2002-07-08 23:23:40 llornkcor Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "config.h"
#include "QTReader.h"
#include "QTReaderApp.h"
#include <qpe/qpeapplication.h>
#include <math.h>
#include <ctype.h>
#include <stdio.h> //for sprintf
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <qfontdatabase.h>
#include <qpe/global.h>
#include <qpe/qcopenvelope_qws.h>

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
  m_scrolldy(0),
  m_autoScroll(false),
  textarray(NULL),
  locnarray(NULL),
  numlines(0),
  m_fontname("unifont"),
  m_fm(NULL)
{
    m_overlap = 1;
    fontsizes = NULL;
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

void QTReader::mouseReleaseEvent( QMouseEvent* _e )
//void QTReader::mouseDoubleClickEvent( QMouseEvent* _e )
{
    if (textarray != NULL)
    {
//  printf("(%u, %u)\n", _e->x(), _e->y());
  QString wrd = QString::null;
  int lineno = _e->y()/m_linespacing;
  if (m_bMonoSpaced)
  {
      int chno = _e->x()/m_charWidth;
      if (chno < ustrlen(textarray[lineno]->data()))
      {
    wrd[0] = textarray[lineno]->data()[chno];
      }
  }
  else
  {
      CBuffer* t = textarray[lineno];
      int first = 0;
      while (1)
      {
    int i = first+1;
//    while ((*t)[i] != ' ' && (*t)[i] != 0) i++;
    while (QChar((*t)[i]).isLetter() && (*t)[i] != 0) i++;
    if (m_fm->width(toQString(t->data()), i) > _e->x())
    {
        wrd = toQString(t->data()+first, i - first);
        break;
    }
//    while ((*t)[i] == ' ' && (*t)[i] != 0) i++;
    while (!QChar((*t)[i]).isLetter() && (*t)[i] != 0) i++;
    if ((*t)[i] == 0) break;
    first = i;
      }   
  }
  if (!wrd.isEmpty())
  {
      QClipboard* cb = QApplication::clipboard();
      cb->setText(wrd);
      Global::statusMessage(wrd);
      if (!m_targetapp.isEmpty() && !m_targetmsg.isEmpty())
      {
    QCopEnvelope e(("QPE/Application/"+m_targetapp).utf8(), (m_targetmsg+"(QString)").utf8());
    e << wrd;
      }
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
  m_scrolldy = 0;
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

void QTReader::keyPressEvent(QKeyEvent* e)
{
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
    if (fontsizes[++m_textsize] == 0)
      {
        m_textsize--;
      }
    else
      {
    bool sc = m_autoScroll;
    m_autoScroll = false;
        setfont(NULL);
        locate(pagelocate());
        update();
        m_autoScroll = sc;
        if (m_autoScroll) autoscroll();
      }
  }
      break;
      case Key_Left:
  {
    e->accept();
    if (m_textsize > 0)
      {
    bool sc = m_autoScroll;
    m_autoScroll = false;
        m_textsize--;
        setfont(NULL);
        locate(pagelocate());
        update();
        m_autoScroll = sc;
        if (m_autoScroll) autoscroll();
      }
  }
      break;
  case Key_Space:
//  case Key_Enter:
  case Key_Return:
  {
    e->accept();
      setautoscroll(!m_autoScroll);
      ((QTReaderApp*)parent()->parent())->setScrollState(m_autoScroll);
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
    }
    else
    {
  m_autoScroll = true;
  autoscroll();
    }
}

bool QTReader::getline(CBuffer *buff)
{
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

    if (++m_scrolldy == m_linespacing)
    {
  setfont(&p);
  m_scrolldy = 0;
//        qDrawPlainRect(&p,0,height() -  m_linespacing,width(),m_linespacing,white,1,&b);
  pagepos = locnarray[1];
  CBuffer* buff = textarray[0];
  for (int i = 1; i < numlines; i++)
  {
      textarray[i-1] = textarray[i];
      locnarray[i-1] = locnarray[i];
  }
  locnarray[numlines-1] = locate();
  if (getline(buff))
  {
      textarray[numlines-1] = buff;
      drawText( p, 0, height() -  m_descent - 2, buff->data());
      mylastpos = locate();
  }
  else
  {
//      (*buff)[0] = '\0';
      textarray[numlines-1] = buff;
      m_autoScroll = false;
      ((QTReaderApp*)parent()->parent())->setScrollState(m_autoScroll);
  }
    }
}

void QTReader::drawText(QPainter& p, int x, int y, tchar* _text)
{
    QString text = toQString(_text);
    if (m_bMonoSpaced)
    {
  for (int i = 0; i < text.length(); i++)
  {
      p.drawText( x+i*m_charWidth, y, QString(text[i]) );
  }
    }
    else
    {
  p.drawText( x, y, text );
    }
}

void QTReader::autoscroll()
{
    timer->start(real_delay(), false);
}

void QTReader::setfont(QPainter* p)
{
  //  qDebug("Fontsize = %u",fontsizes[m_textsize]);
  //  qDebug("SetFont %x",p);
    QFont font(m_fontname, fontsizes[m_textsize], (m_bBold) ? QFont::Bold : QFont::Normal );
    m_charWidth = (m_charpc*fontsizes[m_textsize])/100;
    if (m_charWidth <= 0) m_charWidth = 1;
//    font.setFixedPitch(m_bMonoSpaced);
//  qDebug("Raw name = %s", (const char*)font.rawName());
    if (p != NULL) p->setFont( font );
    if (m_fm == NULL)
      {
  m_fm = new QFontMetrics(font);
  buffdoc.setfm(m_fm);
      }
    else
      {
  *m_fm = QFontMetrics(font);
      }
    m_ascent = m_fm->ascent();
    m_descent = m_fm->descent();
    m_linespacing = m_fm->lineSpacing();
}

void QTReader::drawFonts( QPainter *p )
{
  setfont(p);
  if (m_lastwidth != width())
  {
      m_lastwidth = width();
      locate(pagepos);
  }
  else
  {
      int sl = screenlines();
      if (sl < numlines)
      {
//  qDebug("df:<%u,%u>",sl,numlines);
    
    size_t newpos = locnarray[sl];
    CBuffer** nta = new CBuffer*[sl];
    size_t* nla = new size_t[sl];
    for (int i = 0; i < sl; i++)
    {
        nta[i] = textarray[i];
        nla[i] = locnarray[i];
    }
    for (int i = sl; i < numlines; i++) delete textarray[i];
    delete [] textarray;
    delete [] locnarray;
    textarray = nta;
    locnarray = nla;
    numlines = sl;
    jumpto(mylastpos = newpos);
//  locate(pagepos);
      }
      if (sl > numlines)
      {
//  qDebug("df:<%u,%u>",sl,numlines);
    CBuffer** nta = new CBuffer*[sl];
    size_t* nla = new size_t[sl];
    for (int i = 0; i < numlines; i++)
    {
        nta[i] = textarray[i];
        nla[i] = locnarray[i];
    }
    if (locate() != mylastpos) jumpto(mylastpos);
    for (int i = numlines; i < sl; i++)
    {
        nta[i] = new CBuffer;
        nla[i] = locate();
        getline(nta[i]);
    }
    mylastpos = locate();
    delete [] textarray;
    delete [] locnarray;
    textarray = nta;
    locnarray = nla;
    numlines = sl;
      }
      int ypos = (btight) ? 0 : m_ascent-m_linespacing;
      //  int linespacing = (tight) ? m_ascent : m_ascent+m_descent;
      for (int i = 0; i < numlines; i++)
      {
    drawText( *p, 0, ypos += m_linespacing, textarray[i]->data());
      }
      /*
  
  
  
  int nlines = height()/(fontmetric.ascent()+fontmetric.descent());
  tchar buffer[1024];
  for (int i = 0; i < nlines; i++)
  {
  y += fontmetric.ascent();
  sprintf(buffer, "%d:%d:%s[%d]:Lines %d:%s", i+1, m_textfont, fonts[m_textfont], m_fs, nlines, (const tchar*)m_string);
  drawText( *p, 0, y, buffer );
  y += fontmetric.descent();
  }
      */
  }
  m_scrolldy = 0;
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

void QTReader::ChangeFont(int tgt)
{

    QValueList<int>::Iterator it;

//    QValueList<int> sizes = QFontDatabase::pointSizes(m_fontname, (m_bBold) ? QFont::Bold : QFont::Normal);
    QFontDatabase fdb;
/*
    QStringList styles = fdb.styles(m_fontname);
    for ( QStringList::Iterator it = styles.begin(); it != styles.end(); ++it )
    {
  printf( "%s \n", (*it).latin1() );
    }
*/
    QValueList<int> sizes = fdb.pointSizes(m_fontname, (m_bBold) ? QString("Bold") : QString::null);
    uint n = sizes.count();
    if (fontsizes != NULL) delete [] fontsizes;
    fontsizes = new unsigned int[n+1];
    uint i = 0;
    uint best = 0;
    for (it = sizes.begin(); it != sizes.end(); it++)
    {
  fontsizes[i] = (*it)/10;
  if (abs(tgt-fontsizes[i]) < abs(tgt-fontsizes[best]))
  {
      best = i;
  }
  i++;
    }
    m_textsize = best;
    fontsizes[i] = 0;
    setfont(NULL);
    QFont font(m_fontname, fontsizes[m_textsize], (m_bBold) ? QFont::Bold : QFont::Normal );
    if (m_fm == NULL)
    {
  m_fm = new QFontMetrics(font);
  buffdoc.setfm(m_fm);
    }
}

void QTReader::init()
{
  //    setCaption( "Qt Draw Demo Application" );

  setBackgroundColor( white );
//  QPainter p(this);
//  p.setBackgroundMode( Qt::OpaqueMode );
  buffdoc.setfilter(getfilter());
  ChangeFont(m_textsize);
  //  setFocusPolicy(QWidget::StrongFocus);
    //    resize( 240, 320 );
  //setFocus();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(doscroll()));
//  QMessageBox::information(this, "init", m_lastfile, 1);
    m_lastwidth = width();
    if (!m_lastfile.isEmpty())
    {
  m_string = DocLnk(m_lastfile).name();
  load_file(m_lastfile);
    }
}

//
// Clean up
//
QTReader::~QTReader()
{
    if (fontsizes != NULL) delete [] fontsizes;
#ifndef QT_NO_PRINTER
  //    delete printer;
#endif
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

bool QTReader::fillbuffer() {
  //printf("Fillbuffer\n");
    m_scrolldy = 0;
  int ch;
  bool ret = false;
  int delta = screenlines();
  //  qDebug("fillbuffer:%u-%u",delta,numlines);
  if (delta != numlines)
    {
      if (textarray != NULL)
  {
    for (int i = 0; i < numlines; i++) delete textarray[i];
    delete [] textarray;
          delete [] locnarray;
  }
      numlines = delta;
      textarray = new CBuffer*[numlines];
      locnarray = new size_t[numlines];
      for (int i = 0; i < numlines; i++) textarray[i] = new CBuffer;
    }
  //  qDebug("fillbuffer:pagepos:%u",pagepos);
  unsigned int oldpagepos = pagepos;
//  if (textarray != NULL)
//      pagepos = locnarray[0];
//  else
  pagepos = locate();
  for (int i = 0; i < delta; i++)
    {
      locnarray[i] = locate();
      ch = getline(textarray[i]);
      //      if (ch == EOF) {
      if (!ch)
  {
    if (i == 0)
      {
          locate(oldpagepos);
        return false;
      }
    else
      {
        ret = true;
        for (int j = i+1; j < delta; j++)
                {
                  locnarray[j] = locnarray[j-1];
                  (*(textarray[j]))[0] = '\0';
                }
        break;
      }
  }
      if (ch == '\012') ret = true;
    }
  mylastpos = locate();
  //  qDebug("fillbuffer:lastpos:%u",mylastpos);
  return true;
}


void QTReader::dopagedn()
{
    if (m_overlap == 0)
    {
  if (locate() != mylastpos) jumpto(mylastpos);
    }
    else
    {
  if (m_overlap >= screenlines()) m_overlap = screenlines()/2;
  jumpto(locnarray[screenlines()-m_overlap]);
    }
    if (fillbuffer())
    {
  update();
    }
}

void QTReader::dopageup()
{
  CBuffer** buff = textarray;
  unsigned int *loc = new unsigned int[numlines];
  int cbptr = 0;
  if (locate() != mylastpos) jumpto(mylastpos);
  if (m_overlap >= screenlines()) m_overlap = screenlines()/2;
  unsigned int target = locnarray[m_overlap];
  if (buffdoc.hasrandomaccess())
    {
      unsigned int delta = locate()-pagelocate();
      if (delta < 64) delta = 64;
      if (delta % 2 != 0) delta++;
      if (target % 2 != 0) target++;
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
        getline(buff[0]);
#ifdef WS
        //printf("Trying:%s\n",buff[0]);
#endif
        if (locate() > target) continue;
      }
    while (!buffdoc.iseol());
    for (int i = 0; i < numlines; i++)
      {
        loc[i] = locate();
        getline(buff[i]);
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
    getline(buff[i]);
  }
    }
  cbptr = 0;
  while (locate() < target)
    {
      loc[cbptr] = locate();
      getline(buff[cbptr]);
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
  if (buffdoc.openfile(this,newfile) == 0)
    {
      bRC = true;
      //  qDebug("buffdoc.openfile done");
      locate(lcn);
      //  qDebug("buffdoc.locate done");
    }
  update();
  //  qDebug("Updated");
  return bRC;
}

void QTReader::lineDown()
{
    pagepos = locnarray[1];
    CBuffer* buff = textarray[0];
    for (int i = 1; i < numlines; i++)
    {
  textarray[i-1] = textarray[i];
  locnarray[i-1] = locnarray[i];
    }
    locnarray[numlines-1] = locate();
    if (getline(buff))
    {
  textarray[numlines-1] = buff;
  mylastpos = locate();
    }
    else
    {
  textarray[numlines-1] = buff;
    }
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
  CBuffer* buff = textarray[numlines-1];
  unsigned int loc;
  unsigned int end = locnarray[numlines-1];
  int cbptr = 0;
  if (locate() != mylastpos) jumpto(mylastpos);
  unsigned int target = locnarray[0];
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
      loc = locate();
      getline(buff);
    }
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
  pagepos = loc;
  for (int i = numlines-1; i > 0; i--)
    {
      textarray[i] = textarray[i-1];
      locnarray[i] = locnarray[i-1];
    }
  textarray[0] = buff;
  locnarray[0] = loc;
//  delete [] buff;
//  delete [] loc;
  mylastpos = locate();
  jumpto(end);
  update();
}

bool QTReader::empty()
{
    return buffdoc.empty();
}
