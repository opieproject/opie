/*
 * Interface pour le programme de compression
 * (c) 1995 Fabrice Bellard
 */

#include <stdlib.h>
//#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/***************************************************************************
 * Interface avec les routines de compression
 */

#define METHOD_NB      2   /* nombre total de méthodes de compression */

#define METHOD_STORE   0
#define METHOD_PPM     1


#define DEFAULT_SUFFIX  ".st"    /* extension par défault */
/* signature en début de fichier */
#define STAT_MAGIC_SIZE 4
char stat_magic[STAT_MAGIC_SIZE]={'P','P','M','S'};

#include "ppm_expander.h"

ppm_expander::~ppm_expander() {
  if (needppmend) ppm.PPM_End();
  ppm.arith.Arith_DecodeEnd();
  if (buf_in!=NULL) delete [] buf_in;
  if (buf_out!=NULL) delete [] buf_out;
  if (my_read_buf != NULL) delete my_read_buf;
  if (my_file_in != NULL) fclose(my_file_in);
}

int ppm_expander::OpenFile(const char* infile)
{
  my_file_in=fopen(infile,"rb");
  my_read_buf = new PPM_ReadBuf(my_file_in);
  return home();
}

void ppm_expander::sizes(unsigned long& file, unsigned long& text)
{
  struct stat _stat;
  fstat(fileno(my_file_in),&_stat);
  file = _stat.st_size;
  text = numblocks*blocksize;
}

int ppm_expander::home()
{
  fseek(my_file_in,0, SEEK_SET);
  unsigned char header[STAT_MAGIC_SIZE];
  size_t len=fread(header,1,STAT_MAGIC_SIZE,my_file_in);
  if (strncmp((char*)header,stat_magic,STAT_MAGIC_SIZE)!=0)  {
    return 1;
  }
  if (len!=(STAT_MAGIC_SIZE))  {
    return 1;
  }
  if (fread(&maxnode,sizeof(maxnode),1,my_file_in) != 1) return 1;
  if (fread(&blocksize,sizeof(blocksize),1,my_file_in) != 1) return 1;
  if (fread(&numblocks,sizeof(numblocks),1,my_file_in) != 1) return 1;
  //fprintf(stderr,"<%u,%u,%u>\n",maxnode,blocksize,numblocks);
  int err = locate(0,0);
  outbytes = 0;
  return err;
}

void ppm_expander::locate(unsigned int n) {
  locate(n/blocksize, n%blocksize);
  outbytes = n;
}

int ppm_expander::locate(unsigned short block, unsigned int n)
{
  if (needppmend)
    {
      ppm.PPM_End();
      needppmend = false;
    }
  size_t fpos;
  fseek(my_file_in,STAT_MAGIC_SIZE+sizeof(maxnode)+sizeof(blocksize)+sizeof(numblocks)+block*sizeof(fpos),SEEK_SET);
  if (fread(&fpos,sizeof(fpos),1,my_file_in) != 1) return 1;
  fseek(my_file_in,fpos,SEEK_SET);

  ppm.arith.Arith_DecodeInit(my_read_buf,buf_in,bufsize);
  int err=ppm.PPM_Init(maxnode);
  needppmend = true;
  curblock = block;
  for (int i = 0; i < n; i++) getch();
}

int ppm_expander::getch() {
  if (curblock >= numblocks) return EOF;
  int c=ppm.PPM_Decode();
  if (c == SYM_EOF)
    {
      if (++curblock >= numblocks) return EOF;
      locate(curblock,0);
      c = ppm.PPM_Decode();
    }
  outbytes++;
  return (c==SYM_EOF) ? EOF : c;
}
