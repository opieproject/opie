#ifndef __Text_h
#define __Text_h
#include <stdio.h>
#include "zlib/zlib.h"
#include <sys/stat.h>

#include "CExpander.h"

class Text: public CExpander {
  gzFile file;
  unsigned long fsize;
public:
  virtual void suspend()
      {
	  bSuspended = true;
	  suspos = gztell(file);
	  gzclose(file);
	  file = NULL;
	  sustime = time(NULL);
      }
  virtual void unsuspend()
      {
	  if (bSuspended)
	  {
	      bSuspended = false;
	      int delay = time(NULL) - sustime;
	      if (delay < 10) sleep(10-delay);
	      file = gzopen(fname, "rb");
	      for (int i = 0; file == NULL && i < 5; i++)
	      {
		  sleep(5);
		  file = gzopen(fname, "rb");
	      }
	      if (file == NULL)
	      {
		  QMessageBox::warning(NULL, PROGNAME, "Couldn't reopen file");
		  exit(0);
	      }
	      suspos = gzseek(file, suspos, SEEK_SET);
	  }
      }
  Text() : file(NULL) {};
  virtual ~Text()
    {
      if (file != NULL) gzclose(file);
    }
  virtual int OpenFile(const char *src)
    {
       if (file != NULL) gzclose(file);
      struct stat _stat;
      stat(src,&_stat);
      fsize = _stat.st_size;
      return ((file = gzopen(src,"rb")) == NULL);
    }
  virtual int getch() { return gzgetc(file); }
  virtual unsigned int locate() { return gztell(file); }
  virtual void locate(unsigned int n) { gzseek(file,n,SEEK_SET); }
  virtual bool hasrandomaccess() { return true; }
  virtual void sizes(unsigned long& _file, unsigned long& _text)
    {
      _text = _file = fsize;
    }
  virtual MarkupType PreferredMarkup()
      {
	  return cTEXT;
      }
};
#endif
