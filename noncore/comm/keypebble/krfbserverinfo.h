// -*- c++ -*-

#ifndef KRFBSERVERINFO_H
#define KRFBSERVERINFO_H

#include <qstring.h>

typedef unsigned char CARD8;
typedef unsigned short CARD16;
typedef unsigned long CARD32;

class KRFBPixelFormat
{
public:
  CARD8 bpp;
  CARD8 depth;
  CARD8 bigEndian;
  CARD8 trueColor;
  CARD16 redMax;
  CARD16 greenMax;
  CARD16 blueMax;
  CARD8 redShift;
  CARD8 greenShift;
  CARD8 blueShift;
  CARD8 padding[3]; // 3 bytes padding
};

/**
 * Information sent by the server in its init message.
 */
class KRFBServerInfo : public KRFBPixelFormat
{
public:
  CARD16 width;
  CARD16 height;
  CARD32 nameLength;
  QString name;
};

#endif // KRFBSERVERINFO_H


