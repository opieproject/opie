#ifndef __Text_h
#define __Text_h
#include <stdio.h>
#include <zlib.h>
#include <sys/stat.h>

#include "CExpander.h"

class Text: public CExpander {
  gzFile file;
  unsigned long fsize;
public:
  Text() : file(NULL) {};
  virtual ~Text()
    {
      if (file != NULL) gzclose(file);
    }
  virtual int openfile(const char *src)
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
};
#endif
