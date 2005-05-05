#ifndef __CENCODING_H
#define __CENCODING_H

#include "CExpander.h"

#define MAX_ENCODING 6

class CEncoding : public CCharacterSource
{
  friend class CFilterChain;
 protected:
  CExpander_Interface* parent;
  linkType hyperlink(unsigned int n, unsigned int noff, QString& t, QString& nm) { return parent->hyperlink(n, noff, t, nm); }
public:
  virtual QString getTableAsHtml(unsigned long loc)
    {
      qDebug("CEncoding::getTableAsHtml()");
      return parent->getTableAsHtml(loc);
    }
  CEncoding() : parent(NULL) {}
  void setparent(CExpander_Interface* p) { parent = p; }
  virtual ~CEncoding() {};
  void locate(unsigned int n) { parent->locate(n); }
  bool findanchor(const QString& nm)
    {
      return parent->findanchor(nm);
    }
  void saveposn(const QString& f, size_t posn) { parent->saveposn(posn); }
  void writeposn(const QString& f, size_t posn) { parent->writeposn(posn); }
  linkType forward(QString& f, size_t& loc) { return parent->forward(loc); }
  linkType back(QString& f, size_t& loc) { return parent->back(loc); }
  bool hasnavigation() { return parent->hasnavigation(); }
  virtual int getwidth() { return parent->getwidth(); }
  QImage* getPicture(unsigned long tgt) { return parent->getPicture(tgt); }
  QImage* getPicture(const QString& href) { return parent->getPicture(href); }
  bool getFile(const QString& href, const QString& nm) { qDebug("Encoding Get File"); return parent->getFile(href, nm);}
  unsigned long startSection() { return parent->startSection(); }
};

class CUtf8 : public CEncoding
{
public:
    void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    QString about() { return QString("UTF8 decoder (c) Tim Wentford\n")+parent->about(); }
};

class CUcs16be : public CEncoding
{
public:
    void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    QString about() { return QString("UCS16 decoder (c) Tim Wentford\n")+parent->about(); }
};

class CUcs16le : public CEncoding
{
public:
    void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    QString about() { return QString("UCS16 decoder (c) Tim Wentford\n")+parent->about(); }
};

class Ccp1252 : public CEncoding
{
public:
    void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    QString about() { return QString("Codepage 1252 decoder (c) Tim Wentford\n")+parent->about(); }
};

class CPalm : public Ccp1252
{
public:
    void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    QString about() { return QString("Palm decoder (c) Tim Wentford\n")+parent->about(); }
};

class CAscii : public CEncoding
{
public:
    void getch(tchar& ch, CStyle& sty, unsigned long& pos);
    QString about() { return QString("Vanilla Ascii decoder (c) Tim Wentford\n")+parent->about(); }
};

#include "CEncoding_tables.h"

class CGeneral8Bit : public CEncoding
{
    int m_index;
 public:
    CGeneral8Bit(int _i) : m_index(_i)
	{
//	    qDebug("8Bit:%d", _i);
//	    qDebug("%s", unicodetable::iterator(_i)->mime);
	}
    void getch(tchar& ch, CStyle& sty, unsigned long& pos)
	{
	    parent->getch(ch, sty, pos);
	    ch = unicodetable::unicodevalue(m_index, ch);
	}
    QString about() { return QString("8-bit decoder (c) Tim Wentford - based on QT code\n")+parent->about(); }
};

#endif
