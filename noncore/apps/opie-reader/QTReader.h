#ifndef __QTREADER_H
#define __QTREADER_H

#include <qwidget.h>
#include <qpainter.h>
#include <qclipboard.h>
#include "CBuffer.h"
#include "my_list.h"
#include "BuffDoc.h"
#include <qtimer.h>

class QTReader : public QWidget
{
    Q_OBJECT

      friend class QTReaderApp;
    void drawText(QPainter& p, int x, int y, tchar* text);
    int m_delay;
    unsigned int m_overlap;
    bool m_autoScroll;
    void autoscroll();
    QTimer* timer;
    int m_scrolldy, m_encd;
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent*);
    void ChangeFont(int);
    bool getline(CBuffer*);
    int m_charWidth;
    int m_charpc;
public:
    QTReader( QWidget *parent=0, const char *name=0, WFlags f = 0);
    //    QTReader( const QString& filename, QWidget *parent=0, const tchar *name=0, WFlags f = 0);
    ~QTReader();
    bool empty();
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
    void setstripcr(bool _b)
      {
	bstripcr = _b;
	setfilter(getfilter());
      }
    void setstriphtml(bool _b)
      {
	bstriphtml = _b;
	setfilter(getfilter());
      }
    void setdehyphen(bool _b)
      {
	bdehyphen = _b;
	setfilter(getfilter());
      }
    void setunindent(bool _b)
      {
	bunindent = _b;
	setfilter(getfilter());
      }
    void setrepara(bool _b)
      {
	brepara = _b;
	setfilter(getfilter());
      }
    void setdblspce(bool _b)
      {
	bdblspce = _b;
	setfilter(getfilter());
      }
    void indentplus()
      {
	if (bindenter < 15) bindenter += 2;
	setfilter(getfilter());
      }
    void indentminus()
      {
	if (bindenter > 1) bindenter -= 2;
	setfilter(getfilter());
      }
    void setpagemode(bool _b)
      {
	m_bpagemode = _b;
      }
    void setmono(bool _b)
      {
	m_bMonoSpaced = _b;
	ChangeFont(fontsizes[m_textsize]);
	locate(pagepos);
      }
    void setencoding(int _f)
	{
	    m_encd = _f;
	    setfilter(getfilter());
	}
    CEncoding* getencoding()
	{
	    switch (m_encd)
	    {
		case 5:
		    return new Ccp1252;
		case 4:
		    return new CPalm;
		case 1:
		    return new CUtf8;
		case 2:
		    return new CUcs16be;
		case 3:
		    return new CUcs16le;
		case 0:
		default:
		    return new CAscii;
	    }
	}
  CFilterChain* getfilter()
  {
    CFilterChain * filt = new CFilterChain(getencoding());
    if (bstripcr) filt->addfilter(new stripcr);
    if (bstriphtml) filt->addfilter(new striphtml);
    if (bdehyphen) filt->addfilter(new dehyphen);
    if (bunindent) filt->addfilter(new unindent);
    if (brepara) filt->addfilter(new repara);
    if (bindenter) filt->addfilter(new indenter(bindenter));
    if (bdblspce) filt->addfilter(new dblspce);
    return filt;
  }


private slots:
	void doscroll();
    void   drawIt( QPainter * );
    void   paintEvent( QPaintEvent * );
//    void   resizeEvent( QResizeEvent * p ) { update(); }
  void keyPressEvent(QKeyEvent*);
  void drawFonts(QPainter*);
 private:
  void init();
  void mouseReleaseEvent( QMouseEvent* );
//  void mouseDoubleClickEvent( QMouseEvent* );
  QString m_string, m_fontname;
  void setfont(QPainter*);
  //myoutput stuff
 private:
  void dopageup();
  void lineDown();
  void lineUp();
  void dopagedn();
  long real_delay();
  int m_textsize;
  int m_lastwidth;
  CBuffer** textarray;
  size_t*   locnarray;
  unsigned int numlines;
  bool bstripcr, bstriphtml, bdehyphen, bunindent, brepara, bdblspce, btight, m_bBold;
  bool m_bpagemode, m_bMonoSpaced;
  QString m_targetapp, m_targetmsg;
  unsigned char bindenter;
  QString m_lastfile;
  size_t m_lastposn;
 public:
  int getch() { return buffdoc.getch(); }
  bool tight;
  bool load_file(const char *newfile, unsigned int lcn=0);
  BuffDoc buffdoc;
  CList<Bkmk>* getbkmklist() { return buffdoc.getbkmklist(); }
  bool locate(unsigned long n);
  void jumpto(unsigned long n) { buffdoc.locate(n); }
  unsigned long locate() { return buffdoc.locate(); }
  unsigned long pagelocate() { return pagepos; }
  unsigned long pagepos, mylastpos;
  void setfilter(CFilterChain *f) { buffdoc.setfilter(f); locate(pagepos); }
  void restore() { jumpto(mylastpos); }
  void goUp();
  void refresh() { locate(pagepos); }
  void goDown();
  //  bool bold;
  int textsize() { return m_textsize; }
  void textsize(int ts) { m_textsize = ts; }
  bool fillbuffer();
  unsigned int screenlines();
  void sizes(unsigned long& fs, unsigned long& ts) { buffdoc.sizes(fs,ts); }
  static const char *fonts[];
  unsigned int   *fontsizes;
  int m_ascent, m_descent, m_linespacing;
  QFontMetrics* m_fm;
  QString firstword();
};

#endif
