#include <stdlib.h>
#include <qfileinfo.h>
#include <qdir.h>

#ifdef USEQPE
#include <qpe/global.h>
#endif

class COutput
{
 public:
  virtual ~COutput() {}
  virtual void output(const QString&) = 0;
  virtual QString about() = 0;
};

#ifndef __STATIC
#include <dlfcn.h>
class outputcodec : public COutput
{
  COutput *codec;
  void *handle;
  int status;
 public:
  void output(const QString& q) { codec->output(q); }
  QString about()
    {
      return QString("Plug-in output codec interface (c) Tim Wentford\n")+codec->about();
    }
  outputcodec(const QString& _s) : codec(NULL), handle(NULL), status(-1)
    {
#ifdef USEQPE
#ifdef OPIE
      QString codecpath(getenv("OPIEDIR"));
#else
      QString codecpath(getenv("QTDIR"));
#endif
      codecpath += "/plugins/reader/outcodecs/lib";
#else
      QString codecpath(getenv("READERDIR"));
      codecpath += "/outcodecs/lib";
#endif
      codecpath += _s;
      codecpath += ".so";
      if (QFile::exists(codecpath))
	{
	  qDebug("Codec:%s", (const char*)codecpath);
	  handle = dlopen(codecpath, RTLD_LAZY);
	  if (handle == 0)
	    {
	      qDebug("Can't find codec:%s", dlerror());
	      status = -10;
	      return;
	    }
	  COutput* (*newcodec)();
	  newcodec = (COutput* (*)())dlsym(handle, "newcodec");
	  if (newcodec == NULL)
	    {
	      qDebug("Can't find newcodec");
	      status = -20;
	      return;
	    }
	  codec = (*newcodec)();
	  status = 0;
	}
      else
	{
	  qDebug("Can't find codec:%s", (const char*)codecpath);
	}
      if (codec == NULL)
	{
	  qDebug("Can't do newcodec");
	  status = -30;
	  return;
	}
    }
  virtual ~outputcodec()
    {
      if (codec != NULL) delete codec;
      if (handle != NULL) dlclose(handle);
    }
  int getStatus() { return status; }
};
#endif
