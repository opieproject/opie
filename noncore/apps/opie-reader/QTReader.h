#ifndef __QTREADER_H
#define __QTREADER_H

//#define _SCROLLPIPE
#include <qwidget.h>
//#include <qpainter.h>
#include "my_list.h"
#include "BuffDoc.h"
#include "FontControl.h"

//#include <qtimer.h>

class CDrawBuffer;
//class CBuffer;
#include <qpixmap.h>
class QPainter;
class QTimer;
class QImage;

#include "BGType.h"

#define SPECIALSCROLL

class QPainter;
class COutput;

class CStyle;
#define USETIMER

class QTReader : public QWidget
{
    Q_OBJECT
      friend class QTReaderApp;
#ifdef USETIMER
    QTimer* m_dragtimer;
    unsigned long m_dragtarget;
#endif
    COutput* m_output;
    bool checkoutput();
    bool m_outofdate, m_drageligible;
    QPixmap *dbuff;
    QPainter* dbp;
    void drawSingleLine(int lineno);
    void gotoLink();
    void emitRedraw();
    CStyle* m_currentlinkstyle;
    int m_currentlink;
    int m_currentlinkoffset;
    QPixmap m_bgpm;
    bool m_bgIsScaled;
    bground m_bgtype;
    int m_scrollpos;
    unsigned short m_scrollstep;
    void blitRot(int dx, int sx, int sw, int sh, CDrawBuffer* txt);
    void setBackgroundBitmap(const QPixmap& _pm, bground bg)
      {
	m_bgpm = _pm;
	m_bgtype = bg;
	m_bgIsScaled = false;
      }

    QColor m_bg, m_default_bg, m_default_fg, m_negative_fg;
	static tchar pluckernextpart[];
	static tchar jplucknextpart[];
	CList<Bkmk>* pBkmklist;
      void setHyphenThreshold(int _v) { buffdoc.setHyphenThreshold(_v); }
    void ResetScroll();
    void increaseScroll();
    void reduceScroll();
    void drawText(QPainter& p, int x, int y, tchar* text);
    void DrawScroll( QPainter *p, int w, int h );
    void dorollingscroll(bool);
    void doinplacescroll();
    void dostaticscroll();
    void suspend();
    void redrawScroll(QPainter* p);
    int m_delay, m_scrolltype;
    unsigned int m_overlap;
    bool m_autoScroll, m_swapmouse;
    void drawBackground(QPainter*);
      bool m_rotated;
      void setrotated(bool);
    void autoscroll();
    QTimer* timer;
    int m_scrolldy1, m_scrolldy2, m_encd, m_scrollpart, m_totalscroll;
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent*);
    void processmousepositionevent( QMouseEvent* _e );
    void processmousewordevent(size_t startpos, size_t startoffset, QMouseEvent* _e, int lineno);
    bool ChangeFont(int);
    bool getline(CDrawBuffer*);
    int m_charWidth;
    int m_charpc;
    unsigned short m_absleft_border, m_absright_border;
    unsigned short m_left_border, m_right_border;
    FontControl m_fontControl;
    void setBaseSize(unsigned char _s) { m_fontControl.setBaseSize(_s); }
    unsigned char getBaseSize() { return m_fontControl.getBaseSize(); }
    QString m_outputName;
#ifdef _SCROLLPIPE
    FILE* m_pipeout;
    QString m_pipetarget;
    bool m_isPaused;
    bool m_pauseAfterEachPara;
#endif
public:
    QTReader( QWidget *parent=0, const char *name=0, WFlags f = 0);
    //    QTReader( const QString& filename, QWidget *parent=0, const tchar *name=0, WFlags f = 0);
    ~QTReader();
    QString about();
    void readAloud();
    CList<Bkmk>* Bkmklist() { return pBkmklist; }
    void setBackground(const QColor& _c)
      {
	m_default_bg = _c;
	reset_bg();
      }
    void setForeground(const QColor& _c)
      {
	m_default_fg = _c;
	int r,g,b;
	m_default_fg.rgb(&r, &g, &b);
	r = 255-r;
	g = 255-g;
	b = 255-b;
	m_negative_fg.setRgb(r,g,b);
      }
    void zoomin();
    void zoomout();
    void setSaveData(unsigned char*& data, unsigned short& len, unsigned char* src, unsigned short srclen)
	{
	    buffdoc.setSaveData(data, len, src, srclen);
	}
    void putSaveData(unsigned char*& src, unsigned short& srclen)
	{
	    buffdoc.putSaveData(src, srclen);
	}
    bool empty();
    void setContinuous(bool _b);
    void toggle_autoscroll();
    void setautoscroll(bool);
    void disableAutoscroll() { m_autoScroll = false; }
    void copy()
      {
/*
        size_t nd = locate();
	jumpto(m_mark);
        QString text;
        while (m_mark < nd)
          {
            text += buffdoc.getch();
            m_mark++;
          }
        QApplication::clipboard()->setText(text);
        jumpto(nd);
*/
      };
    void clear() {};
    void setText(const QString& n, const QString& s, unsigned int lcn = 0) { m_string = n; load_file((const char*)s, lcn); };
    /*
    void setText(bool oldfile)
      {
	if (oldfile)
	  {
	    m_string = m_lastfile;
	    load_file((const tchar*)m_string);
	  }
	else
	  {
	    m_string = QString::null;
	  }
      };
    */
    void setlead(int _lead)
	{
	    m_fontControl.setlead(_lead);
	}
    int getlead()
	{
	    return m_fontControl.getlead();
	}
    void setextraspace(int _lead)
	{
	    m_fontControl.setextraspace(_lead);
	}
    int getextraspace()
	{
	    return m_fontControl.getextraspace();
	}
    void setpagemode(bool _b)
      {
	m_bpagemode = _b;
      }
    void setmono(bool _b)
      {
	m_bMonoSpaced = _b;
	ChangeFont(m_fontControl.currentsize());
	locate(pagelocate());
      }
    void setencoding(int _f)
	{
	    m_encd = _f;
	    setfilter(getfilter());
	}
    MarkupType PreferredMarkup();
    CEncoding* getencoding()
	{
//	    qDebug("m_encd:%d", m_encd);
	    switch (m_encd)
	    {
		case 4:
//		    qDebug("palm");
		    return new CPalm;
		case 1:
//		    qDebug("utf8");
		    return new CUtf8;
		case 2:
//		    qDebug("ucs16be");
		    return new CUcs16be;
		case 3:
//		    qDebug("ucs16le");
		    return new CUcs16le;
		case 0:
//		    qDebug("ascii");
		    return new CAscii;
		default:
		    return new CGeneral8Bit(m_encd-MAX_ENCODING+1);
	    }
	}
    HighlightFilter* m_highlightfilter;
    CFilterChain* getfilter();

private slots:
#ifdef USETIMER
  void actionDrag();
#endif
  void dopageup();
  void lineDown();
  void lineUp();
  void dopagedn();
    void goHome();
    void goBack();
    void goForward();
	void doscroll();
    void   paintEvent( QPaintEvent * );

    void   resizeEvent( QResizeEvent * p );

  void keyPressEvent(QKeyEvent*);
 private:
  //    void   drawIt( QPainter * );
  void redrawall();
  void drawFonts();
  void DrawStraight(QPainter* p, int w, int h);
  QColor m_scrollcolor, m_scrollbarcolor;
  void setTwoTouch(bool _b);
  void init();
  void mouseMoveEvent( QMouseEvent* );
  void mousePressEvent( QMouseEvent* );
  void mouseReleaseEvent( QMouseEvent* );
//  void mouseDoubleClickEvent( QMouseEvent* );
  QString m_string, m_fontname, m_reparastring;
  void setfont();
  bool m_doubleBuffered;

  void setDoubleBuffer(bool _b);
  //myoutput stuff
 private:
#ifdef SPECIALSCROLL
  int m_scrolldy;
#endif
  bool mouseUpOn;
  linkType getcurrentpos(int x, int y, int w, int h, int& lineno, size_t& start, size_t& offset, size_t& tgt, size_t& tgtoffset, size_t& pictgt, QImage*&, size_t&);
  bool m_twotouch, m_touchone;
  size_t m_startpos, m_startoffset;
  void dopageup(unsigned int);
  long real_delay();
  int m_textsize;
  int m_lastwidth, m_lastheight;
  CBufferFace<CDrawBuffer*> textarray;
  CBufferFace<size_t>   locnarray;
  unsigned int numlines;
//  bool m_showlast;
  bool bstripcr, btextfmt, bstriphtml, bdehyphen, bdepluck, bdejpluck, bunindent, brepara, bdblspce, btight, bmakebold, bremap, bpeanut, bautofmt, bonespace, bfulljust, /*bNegative,*/ bInverse, bNoInlineTables;
  bool bkern, brepalm, bunderlineLink;
  bool m_bpagemode, m_bMonoSpaced, m_continuousDocument;
  unsigned char bindenter;
  QString m_lastfile;
  size_t m_lastposn;
  bool bDoUpdates;
 public:
  bool doOutput(const QString& wrd);
  void setDoUpdates(bool b) { bDoUpdates = b; }
  void setStripCR(bool b) { bstripcr = b; }
  void NavUp();
  void NavDown();
  tchar getch() { return buffdoc.getch(); }
  bool synch(size_t, size_t);
  bool tight;
  bool load_file(const char *newfile, unsigned int lcn=0);
  BuffDoc buffdoc;
  CList<Bkmk>* getbkmklist() { return buffdoc.getbkmklist(); }
  bool locate(unsigned long n);
  void jumpto(unsigned long n) { buffdoc.locate(n); }
  unsigned long locate() { return buffdoc.locate(); }
  unsigned long explocate() { return buffdoc.explocate(); }
  unsigned long pagelocate() { return locnarray[0]; }
  unsigned long mylastpos;
  void getNextLink();
  void setfilter(CFilterChain *f) { buffdoc.setfilter(f); if (bDoUpdates) locate(pagelocate()); }
  void restore() { jumpto(mylastpos); }
  void goUp();
  void refresh(bool full = false);
  void goDown();
  //  bool bold;
  int textsize() { return m_textsize; }
  void textsize(int ts) { m_textsize = ts; }
  bool fillbuffer(int ru = 0, int ht = -1, int newht = -1);
  void CalculateScrollParameters();
  unsigned int screenlines();
  void sizes(unsigned long& fs, unsigned long& ts) { buffdoc.sizes(fs,ts); }
  static const char *fonts[];
//  unsigned int   *fontsizes;
  int m_ascent, m_descent, m_linespacing;
  int m_topmargin, m_bottommargin;
  int m_abstopmargin, m_absbottommargin;
  QFontMetrics* m_fm;
  QString firstword();
  bool hyphenate;
  void reset_bg()
    {
      int r,g,b;
      m_default_bg.rgb(&r, &g, &b);
      if (bInverse)
	{
	  r = 255-r;
	  g = 255-g;
	  b = 255-b;
	}
      m_bg.setRgb(r,g,b);
      /*
      int h,s,v;
      m_bg.hsv(&h, &s, &v);
      if (bNegative)
	{
	  v = 255-v;
	  m_bg.setHsv(h,s,v);
	}
      */
      setBackgroundColor( m_bg );
    }
  void setInverse(bool b)
    {
      bInverse = b;
      reset_bg();
    }
  /*
  void setNegative()
    {
      bNegative = !bNegative;
      reset_bg();
    }
  */
 signals:
  void OnRedraw();
  void OnWordSelected(const QString&, size_t, size_t, const QString&);
  void OnShowPicture(QImage&);
  void OnURLSelected(const QString&, const size_t);
  void NewFileRequest(const QString&);
  void HandleKeyRequest(QKeyEvent*);
  void SetScrollState(bool);
  void RefreshBitmap();
};

#endif
