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
class QPainter;
class QTimer;
class QPixmap;

class QTReader : public QWidget
{
    Q_OBJECT

	static tchar pluckernextpart[];
	static tchar jplucknextpart[];
      friend class QTReaderApp;
    void suspend();
    void increaseScroll();
    void reduceScroll();
    void drawText(QPainter& p, int x, int y, tchar* text);
    int m_delay;
    unsigned int m_overlap;
    bool m_autoScroll, m_swapmouse;
    void autoscroll();
    QTimer* timer;
    int m_scrolldy1, m_scrolldy2, m_encd, m_scrollpart;
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent*);
    void processmousepositionevent( QMouseEvent* _e );
    void processmousewordevent(size_t startpos, size_t startoffset, QMouseEvent* _e, int lineno);
    bool ChangeFont(int);
    bool getline(CDrawBuffer*);
    int m_charWidth;
    int m_charpc;
    unsigned char m_border;
    FontControl m_fontControl;
    void setBaseSize(unsigned char _s) { m_fontControl.setBaseSize(_s); }
    unsigned char getBaseSize() { return m_fontControl.getBaseSize(); }
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
    void setText(const QString& n, const QString& s) { m_string = n; load_file((const char*)s); };
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
  CFilterChain* getfilter()
  {
    CFilterChain * filt = new CFilterChain(getencoding());
    if (bstripcr) filt->addfilter(new stripcr);

    if (btextfmt || (bautofmt && (PreferredMarkup() == cTEXT))) filt->addfilter(new textfmt);
    if (bpeanut || (bautofmt && (PreferredMarkup() == cPML))) filt->addfilter(new PeanutFormatter);
    if (bstriphtml || (bautofmt && (PreferredMarkup() == cHTML))) filt->addfilter(new striphtml);

    if (bdehyphen) filt->addfilter(new dehyphen);
    if (bunindent) filt->addfilter(new unindent);
    if (brepara) filt->addfilter(new repara);
    if (bonespace) filt->addfilter(new OnePara);
    if (bindenter) filt->addfilter(new indenter(bindenter));
    if (bdblspce) filt->addfilter(new dblspce);
#ifdef REPALM
    if (brepalm) filt->addfilter(new repalm);
#endif
    if (bremap) filt->addfilter(new remap);
    if (bdepluck) filt->addfilter(new DePluck(pluckernextpart));
    if (bdejpluck) filt->addfilter(new DePluck(jplucknextpart));
    if (bmakebold) filt->addfilter(new embolden);
    if (bfulljust) filt->addfilter(new FullJust);
    return filt;
  }


private slots:
    void goHome();
    void goBack();
    void goForward();
	void doscroll();
    void   drawIt( QPainter * );
    void   paintEvent( QPaintEvent * );
//    void   resizeEvent( QResizeEvent * p ) { update(); }
  void keyPressEvent(QKeyEvent*);
  void drawFonts(QPainter*);
 private:
  void setTwoTouch(bool _b);
  void init();
  void mousePressEvent( QMouseEvent* );
  void mouseReleaseEvent( QMouseEvent* );
//  void mouseDoubleClickEvent( QMouseEvent* );
  QString m_string, m_fontname;
  void setfont();
  //myoutput stuff
 private:
  bool mouseUpOn;
  linkType getcurrentpos(int x, int y, size_t& start, size_t& offset, size_t& tgt);
  bool m_twotouch, m_touchone;
  size_t m_startpos, m_startoffset;
  void dopageup(unsigned int);
  void dopageup();
  void lineDown();
  void lineUp();
  void dopagedn();
  long real_delay();
  int m_textsize;
  int m_lastwidth, m_lastheight;
  CBufferFace<CDrawBuffer*> textarray;
  CBufferFace<size_t>   locnarray;
  unsigned int numlines;
//  bool m_showlast;
  bool bstripcr, btextfmt, bstriphtml, bdehyphen, bdepluck, bdejpluck, bunindent, brepara, bdblspce, btight, bmakebold, bremap, bpeanut, bautofmt, bonespace, bfulljust;
#ifdef REPALM
  bool brepalm;
#endif
  bool m_bpagemode, m_bMonoSpaced, m_continuousDocument;
  unsigned char bindenter;
  QString m_lastfile;
  size_t m_lastposn;
 public:
  bool bDoUpdates;
  void NavUp();
  void NavDown();
  tchar getch() { return buffdoc.getch(); }
  bool synch(size_t, size_t);
  bool tight;
  bool load_file(const char *newfile, unsigned int lcn=0);
  BuffDoc buffdoc;
  CList<Bkmk>* getbkmklist() { return buffdoc.getbkmklist(); }
  bool locate(unsigned long n);
  void jumpto(unsigned long n) { buffdoc.unsuspend(); buffdoc.locate(n); }
  unsigned long locate() { buffdoc.unsuspend(); return buffdoc.locate(); }
  unsigned long explocate() { buffdoc.unsuspend(); return buffdoc.explocate(); }
  unsigned long pagelocate() { return locnarray[0]; }
  unsigned long mylastpos;
  void setfilter(CFilterChain *f) { buffdoc.unsuspend(); buffdoc.setfilter(f); locate(pagelocate()); }
  void restore() { jumpto(mylastpos); }
  void goUp();
  void refresh() { locate(pagelocate()); }
  void goDown();
  //  bool bold;
  int textsize() { return m_textsize; }
  void textsize(int ts) { m_textsize = ts; }
  bool fillbuffer(int ru = 0, int ht = 0, int newht = -1);
  unsigned int screenlines();
  void sizes(unsigned long& fs, unsigned long& ts) { buffdoc.unsuspend(); buffdoc.sizes(fs,ts); }
  static const char *fonts[];
//  unsigned int   *fontsizes;
  int m_ascent, m_descent, m_linespacing;
  QFontMetrics* m_fm;
  QString firstword();

 signals:
  void OnRedraw();
  void OnWordSelected(const QString&, size_t, const QString&);
  void OnShowPicture(QImage&);
  void OnURLSelected(const QString&);
};

#endif
