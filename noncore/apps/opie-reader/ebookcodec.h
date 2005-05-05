#include "CExpander.h"

#include <qfileinfo.h>
#include <qdir.h>

#ifdef USEQPE
#include <qpe/global.h>
#endif

#ifndef __STATIC
#include <dlfcn.h>
class ebookcodec : public CExpander_Interface
{
  CExpander *codec;
  void *handle;
  int status;
 public:
  QString getTableAsHtml(unsigned long loc) { return codec->getTableAsHtml(loc); }
  QString about()
    {
      return QString("Plug-in ebook codec interface (c) Tim Wentford\n")+codec->about();
    }
  ebookcodec(const QString& _s) : codec(NULL), handle(NULL), status(0)
    {
#ifdef USEQPE
#ifdef OPIE
      QString codecpath(getenv("OPIEDIR"));
#else
		QString codecpath(getenv("QTDIR"));
#endif
      codecpath += "/plugins/reader/codecs/";
#else
      QString codecpath(getenv("READERDIR"));
      codecpath += "/codecs/";
#endif
      codecpath += _s;
      if (QFile::exists(codecpath))
	{
	  qDebug("Codec:%s", (const char*)codecpath);
	  handle = dlopen(codecpath, RTLD_LAZY);
	  if (handle == 0)
	    {
	      /*
	      QString wrn(dlerror());
	      QString fmt;
	      while (wrn.length() > 10)
		{
		  fmt += wrn.left(10);
		  fmt += '\n';
		  wrn = wrn.right(wrn.length()-10);
		}
	      fmt += wrn;
	      QMessageBox::warning(NULL, PROGNAME, fmt);
	      */
	      qDebug("Can't find codec:%s", dlerror());
	      status = -10;
	      return;
	    }
	  CExpander* (*newcodec)();
	  newcodec = (CExpander* (*)())dlsym(handle, "newcodec");
	  if (newcodec == NULL)
	    {
	      qDebug("Can't find newcodec");
	      status = -20;
	      return;
	    }
	  codec = (*newcodec)();
	}
      else
	{
	  qDebug("Can't find codec");
	}
      if (codec == NULL)
	{
	  qDebug("Can't do newcodec");
	  status = -30;
	  return;
	}
    }
  virtual ~ebookcodec()
    {
      if (codec != NULL) delete codec;
      if (handle != NULL) dlclose(handle);
    }
  size_t getHome() { return codec->getHome(); }
#ifdef USEQPE
  void suspend() { codec->suspend(); }
  void unsuspend() { codec->unsuspend(); }
  void suspend(FILE*& fin) { codec->suspend(fin); }
  void unsuspend(FILE*& fin) { codec->unsuspend(fin); }
#endif
  unsigned int locate() { return codec->locate(); }
  void locate(unsigned int n) { codec->locate(n); }
  bool hasrandomaccess() { return codec->hasrandomaccess(); }
  void sizes(unsigned long& file, unsigned long& text)
    {
      codec->sizes(file, text);
      //qDebug("Codec sizes:(%u, %u)", file, text);
    }
  CList<Bkmk>* getbkmklist() { return codec->getbkmklist(); }
  void getch(tchar& ch, CStyle& sty, unsigned long& pos) { codec->getch(ch, sty, pos); }
  int getch() { return codec->getch(); }
  linkType hyperlink(unsigned int n, unsigned int noff, QString& wrd, QString& nm) { return codec->hyperlink(n, noff, wrd, nm); }
  MarkupType PreferredMarkup() { return codec->PreferredMarkup(); }
  void saveposn(size_t posn) { codec->saveposn(posn); }
  void writeposn(size_t posn) { codec->writeposn(posn); }
  linkType forward(size_t& loc) { return codec->forward(loc); }
  linkType back(size_t& loc) { return codec->back(loc); }
  bool hasnavigation() { return codec->hasnavigation(); }
  void start2endSection() { codec->start2endSection(); }
  QImage* getPicture(unsigned long tgt) { return codec->getPicture(tgt); }
  void setSaveData(unsigned char*& data, unsigned short& len, unsigned char* src, unsigned short srclen) { return codec->setSaveData(data, len, src, srclen); }
  void putSaveData(unsigned char*& src, unsigned short& srclen) { codec->putSaveData(src, srclen); }
  void setContinuous(bool _b) { codec->setContinuous(_b); }
  void setwidth(int w) { codec->setwidth(w); }
  unsigned long startSection() { return codec->startSection(); }
  unsigned long endSection() { return codec->endSection(); }
  int openfile(const char *src)
    {
      //qDebug("ebook openfile:%s", src);
      return (status != 0) ? status : codec->openfile(src);
    }
  int getwidth() { return codec->getwidth(); }
  QImage* getPicture(const QString& href) { return codec->getPicture(href); }
  bool getFile(const QString& href, const QString& nm) { return codec->getFile(href, nm); }
  bool findanchor(const QString& nm)
    {
      return codec->findanchor(nm);
    }
};
#endif
