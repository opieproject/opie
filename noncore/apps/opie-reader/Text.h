#ifndef __Text_h
#define __Text_h
#include <stdio.h>
#include <sys/stat.h>
#include "CExpander.h"

class Text: public CExpander {
  FILE* file;
public:
  Text() : file(NULL) {};
  virtual ~Text() { if (file != NULL) fclose(file); }
  virtual int openfile(const tchar *src)
    {
      if (file != NULL) fclose(file);
      return ((file = fopen(src,"rb")) == NULL);
    }
  virtual int getch() { return fgetc(file); }
  virtual unsigned int locate() { return ftell(file); }
  virtual void locate(unsigned int n) { fseek(file,n,SEEK_SET); }
  virtual bool hasrandomaccess() { return true; }
  virtual void sizes(unsigned long& _file, unsigned long& _text)
    {
      struct stat _stat;
      fstat(fileno(file),&_stat);
      _text = _file = _stat.st_size;
    }
};
#endif
