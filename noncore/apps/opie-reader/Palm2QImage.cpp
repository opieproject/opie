/* -*- mode: c; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     /* for link */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

#include <qimage.h>

/***********************************************************************/
/***********************************************************************/
/*****                                                             *****/
/*****   Code to decode the Palm image format to JPEG              *****/
/*****                                                             *****/
/***********************************************************************/
/***********************************************************************/

#define READ_BIGENDIAN_SHORT(p) (((p)[0] << 8)|((p)[1]))
#define READ_BIGENDIAN_LONG(p)  (((p)[0] << 24)|((p)[1] << 16)|((p)[2] << 8)|((p)[3]))

#define PALM_IS_COMPRESSED_FLAG        0x8000
#define PALM_HAS_COLORMAP_FLAG        0x4000
#define PALM_HAS_TRANSPARENCY_FLAG    0x2000
#define PALM_DIRECT_COLOR_FLAG        0x0400
#define PALM_4_BYTE_FIELD_FLAG        0x0200

#define PALM_COMPRESSION_SCANLINE    0x00
#define PALM_COMPRESSION_RLE        0x01
#define PALM_COMPRESSION_PACKBITS    0x02
#define PALM_COMPRESSION_NONE        0xFF

#define PALM_COLORMAP_SIZE 232

typedef struct {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} ColorMapEntry;

static ColorMapEntry Palm8BitColormap[] = {
  { 255, 255, 255 }, { 255, 204, 255 }, { 255, 153, 255 }, { 255, 102, 255 }, 
  { 255,  51, 255 }, { 255,   0, 255 }, { 255, 255, 204 }, { 255, 204, 204 }, 
  { 255, 153, 204 }, { 255, 102, 204 }, { 255,  51, 204 }, { 255,   0, 204 }, 
  { 255, 255, 153 }, { 255, 204, 153 }, { 255, 153, 153 }, { 255, 102, 153 }, 
  { 255,  51, 153 }, { 255,   0, 153 }, { 204, 255, 255 }, { 204, 204, 255 },
  { 204, 153, 255 }, { 204, 102, 255 }, { 204,  51, 255 }, { 204,   0, 255 },
  { 204, 255, 204 }, { 204, 204, 204 }, { 204, 153, 204 }, { 204, 102, 204 },
  { 204,  51, 204 }, { 204,   0, 204 }, { 204, 255, 153 }, { 204, 204, 153 },
  { 204, 153, 153 }, { 204, 102, 153 }, { 204,  51, 153 }, { 204,   0, 153 },
  { 153, 255, 255 }, { 153, 204, 255 }, { 153, 153, 255 }, { 153, 102, 255 },
  { 153,  51, 255 }, { 153,   0, 255 }, { 153, 255, 204 }, { 153, 204, 204 },
  { 153, 153, 204 }, { 153, 102, 204 }, { 153,  51, 204 }, { 153,   0, 204 },
  { 153, 255, 153 }, { 153, 204, 153 }, { 153, 153, 153 }, { 153, 102, 153 },
  { 153,  51, 153 }, { 153,   0, 153 }, { 102, 255, 255 }, { 102, 204, 255 },
  { 102, 153, 255 }, { 102, 102, 255 }, { 102,  51, 255 }, { 102,   0, 255 },
  { 102, 255, 204 }, { 102, 204, 204 }, { 102, 153, 204 }, { 102, 102, 204 },
  { 102,  51, 204 }, { 102,   0, 204 }, { 102, 255, 153 }, { 102, 204, 153 },
  { 102, 153, 153 }, { 102, 102, 153 }, { 102,  51, 153 }, { 102,   0, 153 },
  {  51, 255, 255 }, {  51, 204, 255 }, {  51, 153, 255 }, {  51, 102, 255 },
  {  51,  51, 255 }, {  51,   0, 255 }, {  51, 255, 204 }, {  51, 204, 204 },
  {  51, 153, 204 }, {  51, 102, 204 }, {  51,  51, 204 }, {  51,   0, 204 },
  {  51, 255, 153 }, {  51, 204, 153 }, {  51, 153, 153 }, {  51, 102, 153 },
  {  51,  51, 153 }, {  51,   0, 153 }, {   0, 255, 255 }, {   0, 204, 255 },
  {   0, 153, 255 }, {   0, 102, 255 }, {   0,  51, 255 }, {   0,   0, 255 },
  {   0, 255, 204 }, {   0, 204, 204 }, {   0, 153, 204 }, {   0, 102, 204 },
  {   0,  51, 204 }, {   0,   0, 204 }, {   0, 255, 153 }, {   0, 204, 153 },
  {   0, 153, 153 }, {   0, 102, 153 }, {   0,  51, 153 }, {   0,   0, 153 },
  { 255, 255, 102 }, { 255, 204, 102 }, { 255, 153, 102 }, { 255, 102, 102 },
  { 255,  51, 102 }, { 255,   0, 102 }, { 255, 255,  51 }, { 255, 204,  51 },
  { 255, 153,  51 }, { 255, 102,  51 }, { 255,  51,  51 }, { 255,   0,  51 },
  { 255, 255,   0 }, { 255, 204,   0 }, { 255, 153,   0 }, { 255, 102,   0 },
  { 255,  51,   0 }, { 255,   0,   0 }, { 204, 255, 102 }, { 204, 204, 102 },
  { 204, 153, 102 }, { 204, 102, 102 }, { 204,  51, 102 }, { 204,   0, 102 },
  { 204, 255,  51 }, { 204, 204,  51 }, { 204, 153,  51 }, { 204, 102,  51 },
  { 204,  51,  51 }, { 204,   0,  51 }, { 204, 255,   0 }, { 204, 204,   0 },
  { 204, 153,   0 }, { 204, 102,   0 }, { 204,  51,   0 }, { 204,   0,   0 },
  { 153, 255, 102 }, { 153, 204, 102 }, { 153, 153, 102 }, { 153, 102, 102 },
  { 153,  51, 102 }, { 153,   0, 102 }, { 153, 255,  51 }, { 153, 204,  51 },
  { 153, 153,  51 }, { 153, 102,  51 }, { 153,  51,  51 }, { 153,   0,  51 },
  { 153, 255,   0 }, { 153, 204,   0 }, { 153, 153,   0 }, { 153, 102,   0 },
  { 153,  51,   0 }, { 153,   0,   0 }, { 102, 255, 102 }, { 102, 204, 102 },
  { 102, 153, 102 }, { 102, 102, 102 }, { 102,  51, 102 }, { 102,   0, 102 },
  { 102, 255,  51 }, { 102, 204,  51 }, { 102, 153,  51 }, { 102, 102,  51 },
  { 102,  51,  51 }, { 102,   0,  51 }, { 102, 255,   0 }, { 102, 204,   0 },
  { 102, 153,   0 }, { 102, 102,   0 }, { 102,  51,   0 }, { 102,   0,   0 },
  {  51, 255, 102 }, {  51, 204, 102 }, {  51, 153, 102 }, {  51, 102, 102 },
  {  51,  51, 102 }, {  51,   0, 102 }, {  51, 255,  51 }, {  51, 204,  51 },
  {  51, 153,  51 }, {  51, 102,  51 }, {  51,  51,  51 }, {  51,   0,  51 },
  {  51, 255,   0 }, {  51, 204,   0 }, {  51, 153,   0 }, {  51, 102,   0 },
  {  51,  51,   0 }, {  51,   0,   0 }, {   0, 255, 102 }, {   0, 204, 102 },
  {   0, 153, 102 }, {   0, 102, 102 }, {   0,  51, 102 }, {   0,   0, 102 },
  {   0, 255,  51 }, {   0, 204,  51 }, {   0, 153,  51 }, {   0, 102,  51 },
  {   0,  51,  51 }, {   0,   0,  51 }, {   0, 255,   0 }, {   0, 204,   0 },
  {   0, 153,   0 }, {   0, 102,   0 }, {   0,  51,   0 }, {  17,  17,  17 },
  {  34,  34,  34 }, {  68,  68,  68 }, {  85,  85,  85 }, { 119, 119, 119 },
  { 136, 136, 136 }, { 170, 170, 170 }, { 187, 187, 187 }, { 221, 221, 221 },
  { 238, 238, 238 }, { 192, 192, 192 }, { 128,   0,   0 }, { 128,   0, 128 },
  {   0, 128,   0 }, {   0, 128, 128 }, {   0,   0,   0 }, {   0,   0,   0 },
  {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 },
  {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 },
  {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 },
  {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 },
  {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 },
  {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 }};

static ColorMapEntry Palm1BitColormap[] = {{ 255, 255, 255 }, { 0, 0, 0 }};

static ColorMapEntry Palm2BitColormap[] = {
  { 255, 255, 255 }, { 192, 192, 192 }, { 128, 128, 128 }, { 0, 0, 0 }};

static ColorMapEntry Palm4BitColormap[] = {
  { 255, 255, 255 }, { 238, 238, 238 }, { 221, 221, 221 }, { 204, 204, 204 },
  { 187, 187, 187 }, { 170, 170, 170 }, { 153, 153, 153 }, { 136, 136, 136 },
  { 119, 119, 119 }, { 102, 102, 102 }, {  85,  85,  85 }, {  68,  68,  68 },
  {  51,  51,  51 }, {  34,  34,  34 }, {  17,  17,  17 }, {   0,   0,   0 }};

QImage* Palm2QImage
  (unsigned char *image_bytes_in, int byte_count_in)
{
  unsigned int width, height, bytes_per_row, flags, next_depth_offset;
  unsigned int bits_per_pixel, version, transparent_index, compression_type, i, j, inval, inbit, mask, incount;
  unsigned int palm_red_bits, palm_green_bits, palm_blue_bits;
  unsigned char *palm_ptr, *x_ptr, *imagedata, *inbyte, *rowbuf, *lastrow,
  *imagedatastart, *palmimage;
  ColorMapEntry *colormap;

  palmimage = image_bytes_in;
  width = READ_BIGENDIAN_SHORT(palmimage + 0);
  height = READ_BIGENDIAN_SHORT(palmimage + 2);
  bytes_per_row = READ_BIGENDIAN_SHORT(palmimage + 4);
  flags = READ_BIGENDIAN_SHORT(palmimage + 6);
  bits_per_pixel = palmimage[8];
  version = palmimage[9];
  next_depth_offset = READ_BIGENDIAN_SHORT(palmimage + 10);
  transparent_index = palmimage[12];
  compression_type = palmimage[13];
  /* bytes 14 and 15 are reserved by Palm and always 0 */
  
#if 0
  qDebug ("Palm image is %dx%d, %d bpp, version %d, flags 0x%x, compression %d",
		width, height, bits_per_pixel, version, flags, compression_type);
#endif

  if (compression_type == PALM_COMPRESSION_PACKBITS) {
    qDebug ("Image uses packbits compression; not yet supported");
    return NULL;
  } else if ((compression_type != PALM_COMPRESSION_NONE) &&
    (compression_type != PALM_COMPRESSION_RLE) &&
      (compression_type != PALM_COMPRESSION_SCANLINE)) {
    qDebug ("Image uses unknown compression, code 0x%x", compression_type);
    return NULL;
  }

  /* as of PalmOS 4.0, there are 6 different kinds of Palm pixmaps:
     
     1, 2, or 4 bit grayscale
     8-bit StaticColor using the Palm standard colormap
     8-bit PseudoColor using a user-specified colormap
     16-bit DirectColor using 5 bits for red, 6 for green, and 5 for blue
     
     Each of these can be compressed with one of four compression schemes,
     "RLE", "Scanline", "PackBits", or none.
     
     We begin by constructing the colormap.
     */

  if (flags & PALM_HAS_COLORMAP_FLAG) {
    qDebug("Palm images with custom colormaps are not currently supported.\n");
    return NULL;
  } else if (bits_per_pixel == 1) {
    colormap = Palm1BitColormap;
    imagedatastart = palmimage + 16;
  } else if (bits_per_pixel == 2) {
    colormap = Palm2BitColormap;
    imagedatastart = palmimage + 16;
  } else if (bits_per_pixel == 4) {
    colormap = Palm4BitColormap;
    imagedatastart = palmimage + 16;
  } else if (bits_per_pixel == 8) {
    colormap = Palm8BitColormap;
    imagedatastart = palmimage + 16;
  } else if (bits_per_pixel == 16 && (flags & PALM_DIRECT_COLOR_FLAG)) {
    colormap = NULL;
    palm_red_bits = palmimage[16];
    palm_green_bits = palmimage[17];
    palm_blue_bits = palmimage[18];
    if (palm_blue_bits > 8 || palm_green_bits > 8 || palm_red_bits > 8) {
      qDebug("Can't handle this format DirectColor image -- too wide in some color (%d:%d:%d)\n",
		   palm_red_bits, palm_green_bits, palm_blue_bits);
      return NULL;
    }
    if (bits_per_pixel > (8 * sizeof(unsigned long))) {
      qDebug ("Can't handle this format DirectColor image -- too many bits per pixel (%d)\n",
		    bits_per_pixel);
      return NULL;
    }
    imagedatastart = palmimage + 24;
  } else {
    qDebug("Unknown bits-per-pixel of %d encountered.\n", bits_per_pixel);
    return NULL;
  }

      QImage* qimage = new QImage(width, height, 16);

  /* row by row, uncompress the Palm image and copy it to the JPEG buffer */
  rowbuf = new unsigned char[bytes_per_row * width];
  lastrow = new unsigned char[bytes_per_row * width];

  for (i=0, palm_ptr = imagedatastart , x_ptr = imagedata;  i < height;  ++i) {

    /* first, uncompress the Palm image */
    if ((flags & PALM_IS_COMPRESSED_FLAG) && (compression_type == PALM_COMPRESSION_RLE)) {
      for (j = 0;  j < bytes_per_row;  ) {
	incount = *palm_ptr++;
	inval = *palm_ptr++;
	memset(rowbuf + j, inval, incount);
	j += incount;
      }
    } else if ((flags & PALM_IS_COMPRESSED_FLAG) && (compression_type == PALM_COMPRESSION_SCANLINE)) {
      for (j = 0;  j < bytes_per_row;  j += 8) {
	incount = *palm_ptr++;
	inval = ((bytes_per_row - j) < 8) ? (bytes_per_row - j) : 8;
	for (inbit = 0;  inbit < inval;  inbit += 1) {
	  if (incount & (1 << (7 - inbit)))
	    rowbuf[j + inbit] = *palm_ptr++;
	  else
	    rowbuf[j + inbit] = lastrow[j + inbit];
	}
      }
      memcpy (lastrow, rowbuf, bytes_per_row);
    } else if (((flags & PALM_IS_COMPRESSED_FLAG) &&
		(compression_type == PALM_COMPRESSION_NONE)) ||
		  (flags && PALM_IS_COMPRESSED_FLAG) == 0) {
      memcpy (rowbuf, palm_ptr, bytes_per_row);
      palm_ptr += bytes_per_row;
    }

    /* next, write it to the GDK bitmap */
    if (colormap) {
      mask = (1 << bits_per_pixel) - 1;
      for (inbit = 8 - bits_per_pixel, inbyte = rowbuf, j = 0; j < width; ++j) {
	inval = ((*inbyte) & (mask << inbit)) >> inbit;
	/* correct for oddity of the 8-bit color Palm pixmap... */
	if ((bits_per_pixel == 8) && (inval == 0xFF)) inval = 231;
	/* now lookup the correct color and set the pixel in the GTK bitmap */
	QRgb colour = qRgb(colormap[inval].red, colormap[inval].green, colormap[inval].blue);
        qimage->setPixel(j, i, colour);
	if (!inbit) {
	  ++inbyte;
	  inbit = 8 - bits_per_pixel;
	} else {
	  inbit -= bits_per_pixel;
	}
      }
    } else if (!colormap &&
      bits_per_pixel == 16) {
      for (inbyte = rowbuf, j = 0; j < width; ++j) {
	inval = (inbyte[0] << 8) | inbyte[1];
#if 0
	qDebug ("pixel is %d,%d (%02x:%02x:%02x)",
		      j, i,
		      (inval >> (bits_per_pixel - palm_red_bits)) & ((1 << palm_red_bits) - 1),
		      (inval >> palm_blue_bits) & ((1 << palm_green_bits) - 1),
		      (inval >> 0) & ((1 << palm_blue_bits) - 1));
#endif
	QRgb colour = qRgb(
            (inval >> (bits_per_pixel - palm_red_bits)) & ((1 << palm_red_bits) - 1), 
            (inval >> palm_blue_bits) & ((1 << palm_green_bits) - 1),
            (inval >> 0) & ((1 << palm_blue_bits) - 1));
        qimage->setPixel(j, i, colour);
	inbyte += 2;
      }
    }
  }

  delete [] rowbuf;
  delete [] lastrow;

  return qimage;
}

QPixmap* hRule(int w, int h, unsigned char r, unsigned char g, unsigned char b)
{
//    qDebug("hrule [%d, %d]", w, h);
    QPixmap* qimage = new QPixmap(w, h);
    qimage->fill(QColor(r,g,b));
    return qimage;
}
