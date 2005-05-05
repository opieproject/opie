#ifndef __MODEL_H
#define __MODEL_H

extern "C"
{
  size_t PluckerDecompress3(unsigned char* a, size_t b, unsigned char* c, size_t d);
  size_t PluckerDecompress4(unsigned char* a, size_t b, unsigned char* c, size_t d);
  size_t RebDecompress(unsigned char* a, size_t b, unsigned char* c, size_t d);
}

#endif
