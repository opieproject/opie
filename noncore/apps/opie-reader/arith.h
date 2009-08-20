/*
 * Encodage & décodage arithmétique
 */

#ifndef ARITH_H

#define ARITH_H

#include "utypes.h"
#include <stdio.h>

class ppm_expander;

class PPM_ReadBuf
{
  FILE *my_file_in;
  ppm_expander* parent;
public:
  PPM_ReadBuf(FILE* f, ppm_expander* _parent) : my_file_in(f), parent(_parent) {}
  UINT readbuf(UCHAR *buf,UINT len);
};

class ArithClass
{

UCHAR *ainbuf;
UCHAR *apinbuf,*aendinbuf;
UINT ainbufsize;
USHORT avalue,alow,ahigh;
PPM_ReadBuf* areadbuf;
UCHAR abitcnt;
USHORT abitbuf;

 public:
void Arith_DecodeInit(PPM_ReadBuf* readbuf,UCHAR *buf,UINT bufsize);
UINT Arith_DecodeVal(UINT size);
void Arith_Decode(UINT min,UINT max,UINT size);
void Arith_DecodeEnd(void);

};
#endif
