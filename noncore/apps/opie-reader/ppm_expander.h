#ifndef __ppm_expander_h
#define __ppm_expander_h

#include "CExpander.h"
#include <sys/stat.h>


#include "utypes.h"
#include "ppm.h"
#include "arith.h"


#define SYM_EOF 256

class ppm_expander : public CExpander {
  UCHAR *buf_in,*buf_out;
  unsigned int bufsize;
  unsigned int outbytes;
  unsigned long blocksize;
  unsigned short numblocks;
  unsigned short curblock;
  unsigned short maxnode;
  bool needppmend;
  int home();
  FILE* my_file_in;
  PPM_ReadBuf* my_read_buf;
  ppm_worker ppm;
 public:
  ppm_expander() : needppmend(false), my_file_in(NULL), my_read_buf(NULL)
    {
    bufsize = 1024;
    buf_in = new UCHAR[bufsize];
    buf_out = new UCHAR[bufsize];
    outbytes = 0;
  }
  virtual int openfile(const char* infile);
  virtual int getch();
  int locate(unsigned short block, unsigned int n);
  virtual ~ppm_expander();
  virtual unsigned int locate() { return outbytes; }
  virtual void locate(unsigned int n);
  virtual bool hasrandomaccess() { return (numblocks > 1); }
  virtual void sizes(unsigned long& file, unsigned long& text);
  virtual MarkupType PreferredMarkup()
      {
	  return cTEXT;
      }
};

#endif
