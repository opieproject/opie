#ifndef __STRIPHTML_H
#define __STRIPHTML_H

#include "CFilter.h"
#include "Navigation.h"
#include "CBuffer.h"
#include "CExpander.h"
#include "my_list.h"

class striphtml : public CFilter
{
  bool ignorespace;
  bool m_bchm;
   CList<CStyle> stylestack;
  static CNavigation_base<htmlmark> m_nav;
  QMap<QString, tchar>* entmap;
    CStyle currentstyle;
    bool isPre;
    unsigned short skip_ws();
    unsigned short skip_ws_end();
    unsigned short skip_ws_end(unsigned long&);
    unsigned short parse_m();
    QMap<QString, unsigned long>* href2filepos;
    QMap<unsigned long, QString>* id2href;
    unsigned long currentid;
    tchar lastch;
    QString currentfile;
    QString q;
    bool forcecentre, m_inblock;
    QString text_q;
    int indent;
    int m_listtype[m_cmaxdepth];
    int m_ctr[m_cmaxdepth];
    void mygetch(tchar& ch, CStyle& sty, unsigned long& pos);
    void initentmap();
    QString getname(tchar& ch, const QString& nd);
    void skipblock(const QString&);
    QString getattr(tchar&);
    QString dehtml(const QString& _info);
    void parse_paragraph(CStyle& currentstyle, tchar& ch, unsigned long);
    int getpara(CBuffer&, unsigned long&);
 public:
    QString about() { return QString("HTML filter (c) Tim Wentford\n")+parent->about(); }
    striphtml(const QString&);
    ~striphtml();
    void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    virtual linkType hyperlink(unsigned int n, unsigned int noff, QString& w, QString& nm);
    bool findanchor(const QString&);


    void saveposn(const QString& f, size_t posn) { m_nav.saveposn(htmlmark(f, posn)); }
    void writeposn(const QString& f, size_t posn) { m_nav.writeposn(htmlmark(f, posn)); }
    linkType forward(QString& f, size_t& loc)
      {
	htmlmark m(f, loc);
	linkType ret =  (m_nav.forward(m)) ? eFile : eNone;
	if (f == m.filename())
	  {
	    if (ret == eFile)
	      {
		ret = eLink;
	      }
	  }
	else
	  {
	    f = m.filename();
	  }
	loc = m.posn();
	return ret;
      }
    linkType back(QString& f, size_t& loc)
      {
	htmlmark m(f, loc);
	linkType ret =  (m_nav.back(m)) ? eFile : eNone;
	if (f == m.filename())
	  {
	    if (ret == eFile)
	      {
		ret = eLink;
	      }
	  }
	else
	  {
	    f = m.filename();
	  }
	loc = m.posn();
	return ret;
      }
    bool hasnavigation() { return true; }
    virtual void locate(unsigned int n);
    void setchm(bool _b) { m_bchm = _b; }
};
#endif
