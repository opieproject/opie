#ifndef __Text_h
#define __Text_h
#include <stdio.h>
#include <zlib.h>
#include <sys/stat.h>
#include "useqpe.h"
#include "CExpander.h"

class Text: public CExpander {
  gzFile file;
  unsigned long fsize;
public:
  void suspend()
      {
#ifdef USEQPE
	if (!bSuspended)
	  {
	    bSuspended = true;
	    suspos = gztell(file);
	    gzclose(file);
	    file = NULL;
	    sustime = time(NULL);
	  }
#endif
      }
  void unsuspend()
      {
#ifdef USEQPE
	  if (bSuspended)
	  {
	      bSuspended = false;
	      if (sustime != ((time_t)-1))
		{
		  int delay = time(NULL) - sustime;
		  if (delay < 10) sleep(10-delay);
		}
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
#endif
      }
  Text() : file(NULL) {};
  virtual ~Text()
    {
      if (file != NULL)
	{
#ifdef USEQPE
	  unsuspend();
#endif
	  gzclose(file);
	}
    }
  int OpenFile(const char *src)
    {
       if (file != NULL) gzclose(file);
      struct stat _stat;
      stat(src,&_stat);
      fsize = _stat.st_size;
      return ((file = gzopen(src,"rb")) == NULL);
    }
  int getch()
    {
#ifdef USEQPE
      unsuspend();
#endif
      return gzgetc(file);
    }
  unsigned int locate()
    {
#ifdef USEQPE
      unsuspend();
#endif
      return gztell(file);
    }
  void locate(unsigned int n)
    {
#ifdef USEQPE
      unsuspend();
#endif
      gzseek(file,n,SEEK_SET);
    }
  bool hasrandomaccess() { return true; }
  void sizes(unsigned long& _file, unsigned long& _text)
    {
      _text = _file = fsize;
      FILE* f = fopen(fname, "rb");
      if (f != NULL)
      {
	  unsigned char mn[2];
	  fread(mn, 1, 2, f);
	  if ((mn[0] == 31) && (mn[1] == 139))
	  {
		  int tmp = sizeof(_text);
	      fseek(f,-tmp,SEEK_END);
	      fread(&_text, sizeof(_text), 1, f);
	  }
	  fclose(f);
      }
    }
  MarkupType PreferredMarkup()
      {
	  return cTEXT;
      }
  QString about() { return QString("Text/gzipped text codec (c) Tim Wentford"); }
};
#endif
