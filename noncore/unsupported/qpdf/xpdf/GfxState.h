//========================================================================
//
// GfxState.h
//
// Copyright 1996 Derek B. Noonburg
//
//========================================================================

#ifndef GFXSTATE_H
#define GFXSTATE_H

#ifdef __GNUC__
#pragma interface
#endif

#include "gtypes.h"
#include "Object.h"
#include "Function.h"

class Array;
class GfxFont;
struct PDFRectangle;

//------------------------------------------------------------------------
// GfxColor
//------------------------------------------------------------------------

#define gfxColorMaxComps funcMaxOutputs

struct GfxColor {
  fouble c[gfxColorMaxComps];
};

//------------------------------------------------------------------------
// GfxRGB
//------------------------------------------------------------------------

struct GfxRGB {
  fouble r, g, b;
};

//------------------------------------------------------------------------
// GfxCMYK
//------------------------------------------------------------------------

struct GfxCMYK {
  fouble c, m, y, k;
};

//------------------------------------------------------------------------
// GfxColorSpace
//------------------------------------------------------------------------

enum GfxColorSpaceMode {
  csDeviceGray,
  csCalGray,
  csDeviceRGB,
  csCalRGB,
  csDeviceCMYK,
  csLab,
  csICCBased,
  csIndexed,
  csSeparation,
  csDeviceN,
  csPattern
};

class GfxColorSpace {
public:

  GfxColorSpace();
  virtual ~GfxColorSpace();
  virtual GfxColorSpace *copy() = 0;
  virtual GfxColorSpaceMode getMode() = 0;

  // Construct a color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Object *csObj);

  // Convert to gray, RGB, or CMYK.
  virtual void getGray(GfxColor *color, fouble *gray) = 0;
  virtual void getRGB(GfxColor *color, GfxRGB *rgb) = 0;
  virtual void getCMYK(GfxColor *color, GfxCMYK *cmyk) = 0;

  // Return the number of color components.
  virtual int getNComps() = 0;

  // Return the default ranges for each component, assuming an image
  // with a max pixel value of <maxImgPixel>.
  virtual void getDefaultRanges(fouble *decodeLow, fouble *decodeRange,
				int maxImgPixel);

private:
};

//------------------------------------------------------------------------
// GfxDeviceGrayColorSpace
//------------------------------------------------------------------------

class GfxDeviceGrayColorSpace: public GfxColorSpace {
public:

  GfxDeviceGrayColorSpace();
  virtual ~GfxDeviceGrayColorSpace();
  virtual GfxColorSpace *copy();
  virtual GfxColorSpaceMode getMode() { return csDeviceGray; }

  virtual void getGray(GfxColor *color, fouble *gray);
  virtual void getRGB(GfxColor *color, GfxRGB *rgb);
  virtual void getCMYK(GfxColor *color, GfxCMYK *cmyk);

  virtual int getNComps() { return 1; }

private:
};

//------------------------------------------------------------------------
// GfxCalGrayColorSpace
//------------------------------------------------------------------------

class GfxCalGrayColorSpace: public GfxColorSpace {
public:

  GfxCalGrayColorSpace();
  virtual ~GfxCalGrayColorSpace();
  virtual GfxColorSpace *copy();
  virtual GfxColorSpaceMode getMode() { return csCalGray; }

  // Construct a CalGray color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Array *arr);

  virtual void getGray(GfxColor *color, fouble *gray);
  virtual void getRGB(GfxColor *color, GfxRGB *rgb);
  virtual void getCMYK(GfxColor *color, GfxCMYK *cmyk);

  virtual int getNComps() { return 1; }

  // CalGray-specific access.
  fouble getWhiteX() { return whiteX; }
  fouble getWhiteY() { return whiteY; }
  fouble getWhiteZ() { return whiteZ; }
  fouble getBlackX() { return blackX; }
  fouble getBlackY() { return blackY; }
  fouble getBlackZ() { return blackZ; }
  fouble getGamma() { return gamma; }

private:

  fouble whiteX, whiteY, whiteZ;    // white point
  fouble blackX, blackY, blackZ;    // black point
  fouble gamma;			    // gamma value
};

//------------------------------------------------------------------------
// GfxDeviceRGBColorSpace
//------------------------------------------------------------------------

class GfxDeviceRGBColorSpace: public GfxColorSpace {
public:

  GfxDeviceRGBColorSpace();
  virtual ~GfxDeviceRGBColorSpace();
  virtual GfxColorSpace *copy();
  virtual GfxColorSpaceMode getMode() { return csDeviceRGB; }

  virtual void getGray(GfxColor *color, fouble *gray);
  virtual void getRGB(GfxColor *color, GfxRGB *rgb);
  virtual void getCMYK(GfxColor *color, GfxCMYK *cmyk);

  virtual int getNComps() { return 3; }

private:
};

//------------------------------------------------------------------------
// GfxCalRGBColorSpace
//------------------------------------------------------------------------

class GfxCalRGBColorSpace: public GfxColorSpace {
public:

  GfxCalRGBColorSpace();
  virtual ~GfxCalRGBColorSpace();
  virtual GfxColorSpace *copy();
  virtual GfxColorSpaceMode getMode() { return csCalRGB; }

  // Construct a CalRGB color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Array *arr);

  virtual void getGray(GfxColor *color, fouble *gray);
  virtual void getRGB(GfxColor *color, GfxRGB *rgb);
  virtual void getCMYK(GfxColor *color, GfxCMYK *cmyk);

  virtual int getNComps() { return 3; }

  // CalRGB-specific access.
  fouble getWhiteX() { return whiteX; }
  fouble getWhiteY() { return whiteY; }
  fouble getWhiteZ() { return whiteZ; }
  fouble getBlackX() { return blackX; }
  fouble getBlackY() { return blackY; }
  fouble getBlackZ() { return blackZ; }
  fouble getGammaR() { return gammaR; }
  fouble getGammaG() { return gammaG; }
  fouble getGammaB() { return gammaB; }
  fouble *getMatrix() { return mat; }

private:

  fouble whiteX, whiteY, whiteZ;    // white point
  fouble blackX, blackY, blackZ;    // black point
  fouble gammaR, gammaG, gammaB;    // gamma values
  fouble mat[9];		// ABC -> XYZ transform matrix
};

//------------------------------------------------------------------------
// GfxDeviceCMYKColorSpace
//------------------------------------------------------------------------

class GfxDeviceCMYKColorSpace: public GfxColorSpace {
public:

  GfxDeviceCMYKColorSpace();
  virtual ~GfxDeviceCMYKColorSpace();
  virtual GfxColorSpace *copy();
  virtual GfxColorSpaceMode getMode() { return csDeviceCMYK; }

  virtual void getGray(GfxColor *color, fouble *gray);
  virtual void getRGB(GfxColor *color, GfxRGB *rgb);
  virtual void getCMYK(GfxColor *color, GfxCMYK *cmyk);

  virtual int getNComps() { return 4; }

private:
};

//------------------------------------------------------------------------
// GfxLabColorSpace
//------------------------------------------------------------------------

class GfxLabColorSpace: public GfxColorSpace {
public:

  GfxLabColorSpace();
  virtual ~GfxLabColorSpace();
  virtual GfxColorSpace *copy();
  virtual GfxColorSpaceMode getMode() { return csLab; }

  // Construct a Lab color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Array *arr);

  virtual void getGray(GfxColor *color, fouble *gray);
  virtual void getRGB(GfxColor *color, GfxRGB *rgb);
  virtual void getCMYK(GfxColor *color, GfxCMYK *cmyk);

  virtual int getNComps() { return 3; }

  virtual void getDefaultRanges(fouble *decodeLow, fouble *decodeRange,
				int maxImgPixel);

  // Lab-specific access.
  fouble getWhiteX() { return whiteX; }
  fouble getWhiteY() { return whiteY; }
  fouble getWhiteZ() { return whiteZ; }
  fouble getBlackX() { return blackX; }
  fouble getBlackY() { return blackY; }
  fouble getBlackZ() { return blackZ; }
  fouble getAMin() { return aMin; }
  fouble getAMax() { return aMax; }
  fouble getBMin() { return bMin; }
  fouble getBMax() { return bMax; }

private:

  fouble whiteX, whiteY, whiteZ;    // white point
  fouble blackX, blackY, blackZ;    // black point
  fouble aMin, aMax, bMin, bMax;    // range for the a and b components
  fouble kr, kg, kb;		    // gamut mapping mulitpliers
};

//------------------------------------------------------------------------
// GfxICCBasedColorSpace
//------------------------------------------------------------------------

class GfxICCBasedColorSpace: public GfxColorSpace {
public:

  GfxICCBasedColorSpace(int nCompsA, GfxColorSpace *altA,
			Ref *iccProfileStreamA);
  virtual ~GfxICCBasedColorSpace();
  virtual GfxColorSpace *copy();
  virtual GfxColorSpaceMode getMode() { return csICCBased; }

  // Construct an ICCBased color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Array *arr);

  virtual void getGray(GfxColor *color, fouble *gray);
  virtual void getRGB(GfxColor *color, GfxRGB *rgb);
  virtual void getCMYK(GfxColor *color, GfxCMYK *cmyk);

  virtual int getNComps() { return nComps; }

  virtual void getDefaultRanges(fouble *decodeLow, fouble *decodeRange,
				int maxImgPixel);

  // ICCBased-specific access.
  GfxColorSpace *getAlt() { return alt; }

private:

  int nComps;			// number of color components (1, 3, or 4)
  GfxColorSpace *alt;		// alternate color space
  fouble rangeMin[4];		// min values for each component
  fouble rangeMax[4];		// max values for each component
  Ref iccProfileStream;		// the ICC profile
};

//------------------------------------------------------------------------
// GfxIndexedColorSpace
//------------------------------------------------------------------------

class GfxIndexedColorSpace: public GfxColorSpace {
public:

  GfxIndexedColorSpace(GfxColorSpace *baseA, int indexHighA);
  virtual ~GfxIndexedColorSpace();
  virtual GfxColorSpace *copy();
  virtual GfxColorSpaceMode getMode() { return csIndexed; }

  // Construct a Lab color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Array *arr);

  virtual void getGray(GfxColor *color, fouble *gray);
  virtual void getRGB(GfxColor *color, GfxRGB *rgb);
  virtual void getCMYK(GfxColor *color, GfxCMYK *cmyk);

  virtual int getNComps() { return 1; }

  virtual void getDefaultRanges(fouble *decodeLow, fouble *decodeRange,
				int maxImgPixel);

  // Indexed-specific access.
  GfxColorSpace *getBase() { return base; }
  int getIndexHigh() { return indexHigh; }
  Guchar *getLookup() { return lookup; }

private:

  GfxColorSpace *base;		// base color space
  int indexHigh;		// max pixel value
  Guchar *lookup;		// lookup table
};

//------------------------------------------------------------------------
// GfxSeparationColorSpace
//------------------------------------------------------------------------

class GfxSeparationColorSpace: public GfxColorSpace {
public:

  GfxSeparationColorSpace(GString *nameA, GfxColorSpace *altA,
			  Function *funcA);
  virtual ~GfxSeparationColorSpace();
  virtual GfxColorSpace *copy();
  virtual GfxColorSpaceMode getMode() { return csSeparation; }

  // Construct a Separation color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Array *arr);

  virtual void getGray(GfxColor *color, fouble *gray);
  virtual void getRGB(GfxColor *color, GfxRGB *rgb);
  virtual void getCMYK(GfxColor *color, GfxCMYK *cmyk);

  virtual int getNComps() { return 1; }

  // Separation-specific access.
  GString *getName() { return name; }
  GfxColorSpace *getAlt() { return alt; }
  Function *getFunc() { return func; }

private:

  GString *name;		// colorant name
  GfxColorSpace *alt;		// alternate color space
  Function *func;		// tint transform (into alternate color space)
};

//------------------------------------------------------------------------
// GfxDeviceNColorSpace
//------------------------------------------------------------------------

class GfxDeviceNColorSpace: public GfxColorSpace {
public:

  GfxDeviceNColorSpace(int nComps, GfxColorSpace *alt, Function *func);
  virtual ~GfxDeviceNColorSpace();
  virtual GfxColorSpace *copy();
  virtual GfxColorSpaceMode getMode() { return csDeviceN; }

  // Construct a DeviceN color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Array *arr);

  virtual void getGray(GfxColor *color, fouble *gray);
  virtual void getRGB(GfxColor *color, GfxRGB *rgb);
  virtual void getCMYK(GfxColor *color, GfxCMYK *cmyk);

  virtual int getNComps() { return nComps; }

  // DeviceN-specific access.
  GfxColorSpace *getAlt() { return alt; }

private:

  int nComps;			// number of components
  GString			// colorant names
    *names[gfxColorMaxComps];
  GfxColorSpace *alt;		// alternate color space
  Function *func;		// tint transform (into alternate color space)
  
};

//------------------------------------------------------------------------
// GfxPatternColorSpace
//------------------------------------------------------------------------

class GfxPatternColorSpace: public GfxColorSpace {
public:

  GfxPatternColorSpace(GfxColorSpace *underA);
  virtual ~GfxPatternColorSpace();
  virtual GfxColorSpace *copy();
  virtual GfxColorSpaceMode getMode() { return csPattern; }

  // Construct a Pattern color space.  Returns NULL if unsuccessful.
  static GfxColorSpace *parse(Array *arr);

  virtual void getGray(GfxColor *color, fouble *gray);
  virtual void getRGB(GfxColor *color, GfxRGB *rgb);
  virtual void getCMYK(GfxColor *color, GfxCMYK *cmyk);

  virtual int getNComps() { return 0; }

  // Pattern-specific access.
  GfxColorSpace *getUnder() { return under; }

private:

  GfxColorSpace *under;		// underlying color space (for uncolored
				//   patterns)
};

//------------------------------------------------------------------------
// GfxPattern
//------------------------------------------------------------------------

class GfxPattern {
public:

  GfxPattern(int typeA);
  virtual ~GfxPattern();

  static GfxPattern *parse(Object *obj);

  virtual GfxPattern *copy() = 0;

  int getType() { return type; }

private:

  int type;
};

//------------------------------------------------------------------------
// GfxTilingPattern
//------------------------------------------------------------------------

class GfxTilingPattern: public GfxPattern {
public:

  GfxTilingPattern(Dict *streamDict, Object *stream);
  virtual ~GfxTilingPattern();

  virtual GfxPattern *copy();

  int getPaintType() { return paintType; }
  int getTilingType() { return tilingType; }
  fouble *getBBox() { return bbox; }
  fouble getXStep() { return xStep; }
  fouble getYStep() { return yStep; }
  Dict *getResDict()
    { return resDict.isDict() ? resDict.getDict() : (Dict *)NULL; }
  fouble *getMatrix() { return matrix; }
  Object *getContentStream() { return &contentStream; }

private:

  GfxTilingPattern(GfxTilingPattern *pat);

  int paintType;
  int tilingType;
  fouble bbox[4];
  fouble xStep, yStep;
  Object resDict;
  fouble matrix[6];
  Object contentStream;
};

//------------------------------------------------------------------------
// GfxShading
//------------------------------------------------------------------------

class GfxShading {
public:

  GfxShading();
  virtual ~GfxShading();

  static GfxShading *parse(Object *obj);

  int getType() { return type; }
  GfxColorSpace *getColorSpace() { return colorSpace; }
  GfxColor *getBackground() { return &background; }
  GBool getHasBackground() { return hasBackground; }
  void getBBox(fouble *xMinA, fouble *yMinA, fouble *xMaxA, fouble *yMaxA)
    { *xMinA = xMin; *yMinA = yMin; *xMaxA = xMax; *yMaxA = yMax; }
  GBool getHasBBox() { return hasBBox; }

private:

  int type;
  GfxColorSpace *colorSpace;
  GfxColor background;
  GBool hasBackground;
  fouble xMin, yMin, xMax, yMax;
  GBool hasBBox;
};

//------------------------------------------------------------------------
// GfxAxialShading
//------------------------------------------------------------------------

class GfxAxialShading: public GfxShading {
public:

  GfxAxialShading(fouble x0A, fouble y0A,
		  fouble x1A, fouble y1A,
		  fouble t0A, fouble t1A,
		  Function **funcsA, int nFuncsA,
		  GBool extend0A, GBool extend1A);
  virtual ~GfxAxialShading();

  static GfxAxialShading *parse(Dict *dict);

  void getCoords(fouble *x0A, fouble *y0A, fouble *x1A, fouble *y1A)
    { *x0A = x0; *y0A = y0; *x1A = x1; *y1A = y1; }
  fouble getDomain0() { return t0; }
  fouble getDomain1() { return t1; }
  void getColor(fouble t, GfxColor *color);
  GBool getExtend0() { return extend0; }
  GBool getExtend1() { return extend1; }

private:

  fouble x0, y0, x1, y1;
  fouble t0, t1;
  Function *funcs[gfxColorMaxComps];
  int nFuncs;
  GBool extend0, extend1;
};

//------------------------------------------------------------------------
// GfxImageColorMap
//------------------------------------------------------------------------

class GfxImageColorMap {
public:

  // Constructor.
  GfxImageColorMap(int bitsA, Object *decode, GfxColorSpace *colorSpaceA);

  // Destructor.
  ~GfxImageColorMap();

  // Is color map valid?
  GBool isOk() { return ok; }

  // Get the color space.
  GfxColorSpace *getColorSpace() { return colorSpace; }

  // Get stream decoding info.
  int getNumPixelComps() { return nComps; }
  int getBits() { return bits; }

  // Get decode table.
  fouble getDecodeLow(int i) { return decodeLow[i]; }
  fouble getDecodeHigh(int i) { return decodeLow[i] + decodeRange[i]; }

  // Convert an image pixel to a color.
  void getGray(Guchar *x, fouble *gray);
  void getRGB(Guchar *x, GfxRGB *rgb);
  void getCMYK(Guchar *x, GfxCMYK *cmyk);

private:

  GfxColorSpace *colorSpace;	// the image color space
  int bits;			// bits per component
  int nComps;			// number of components in a pixel
  GfxColorSpace *colorSpace2;	// secondary color space
  int nComps2;			// number of components in colorSpace2
  fouble *lookup;		// lookup table
  fouble			// minimum values for each component
    decodeLow[gfxColorMaxComps];
  fouble			// max - min value for each component
    decodeRange[gfxColorMaxComps];
  GBool ok;
};

//------------------------------------------------------------------------
// GfxSubpath and GfxPath
//------------------------------------------------------------------------

class GfxSubpath {
public:

  // Constructor.
  GfxSubpath(fouble x1, fouble y1);

  // Destructor.
  ~GfxSubpath();

  // Copy.
  GfxSubpath *copy() { return new GfxSubpath(this); }

  // Get points.
  int getNumPoints() { return n; }
  fouble getX(int i) { return x[i]; }
  fouble getY(int i) { return y[i]; }
  GBool getCurve(int i) { return curve[i]; }

  // Get last point.
  fouble getLastX() { return x[n-1]; }
  fouble getLastY() { return y[n-1]; }

  // Add a line segment.
  void lineTo(fouble x1, fouble y1);

  // Add a Bezier curve.
  void curveTo(fouble x1, fouble y1, fouble x2, fouble y2,
	       fouble x3, fouble y3);

  // Close the subpath.
  void close();
  GBool isClosed() { return closed; }

private:

  fouble *x, *y;		// points
  GBool *curve;			// curve[i] => point i is a control point
				//   for a Bezier curve
  int n;			// number of points
  int size;			// size of x/y arrays
  GBool closed;			// set if path is closed

  GfxSubpath(GfxSubpath *subpath);
};

class GfxPath {
public:

  // Constructor.
  GfxPath();

  // Destructor.
  ~GfxPath();

  // Copy.
  GfxPath *copy()
    { return new GfxPath(justMoved, firstX, firstY, subpaths, n, size); }

  // Is there a current point?
  GBool isCurPt() { return n > 0 || justMoved; }

  // Is the path non-empty, i.e., is there at least one segment?
  GBool isPath() { return n > 0; }

  // Get subpaths.
  int getNumSubpaths() { return n; }
  GfxSubpath *getSubpath(int i) { return subpaths[i]; }

  // Get last point on last subpath.
  fouble getLastX() { return subpaths[n-1]->getLastX(); }
  fouble getLastY() { return subpaths[n-1]->getLastY(); }

  // Move the current point.
  void moveTo(fouble x, fouble y);

  // Add a segment to the last subpath.
  void lineTo(fouble x, fouble y);

  // Add a Bezier curve to the last subpath
  void curveTo(fouble x1, fouble y1, fouble x2, fouble y2,
	       fouble x3, fouble y3);

  // Close the last subpath.
  void close();

private:

  GBool justMoved;		// set if a new subpath was just started
  fouble firstX, firstY;	// first point in new subpath
  GfxSubpath **subpaths;	// subpaths
  int n;			// number of subpaths
  int size;			// size of subpaths array

  GfxPath(GBool justMoved1, fouble firstX1, fouble firstY1,
	  GfxSubpath **subpaths1, int n1, int size1);
};

//------------------------------------------------------------------------
// GfxState
//------------------------------------------------------------------------

class GfxState {
public:

  // Construct a default GfxState, for a device with resolution <dpi>,
  // page box <pageBox>, page rotation <rotate>, and coordinate system
  // specified by <upsideDown>.
  GfxState(fouble dpi, PDFRectangle *pageBox, int rotate,
	   GBool upsideDown);

  // Destructor.
  ~GfxState();

  // Copy.
  GfxState *copy() { return new GfxState(this); }

  // Accessors.
  fouble *getCTM() { return ctm; }
  fouble getX1() { return px1; }
  fouble getY1() { return py1; }
  fouble getX2() { return px2; }
  fouble getY2() { return py2; }
  fouble getPageWidth() { return pageWidth; }
  fouble getPageHeight() { return pageHeight; }
  GfxColor *getFillColor() { return &fillColor; }
  GfxColor *getStrokeColor() { return &strokeColor; }
  void getFillGray(fouble *gray)
    { fillColorSpace->getGray(&fillColor, gray); }
  void getStrokeGray(fouble *gray)
    { strokeColorSpace->getGray(&fillColor, gray); }
  void getFillRGB(GfxRGB *rgb)
    { fillColorSpace->getRGB(&fillColor, rgb); }
  void getStrokeRGB(GfxRGB *rgb)
    { strokeColorSpace->getRGB(&strokeColor, rgb); }
  void getFillCMYK(GfxCMYK *cmyk)
    { fillColorSpace->getCMYK(&fillColor, cmyk); }
  void getStrokeCMYK(GfxCMYK *cmyk)
    { strokeColorSpace->getCMYK(&strokeColor, cmyk); }
  GfxColorSpace *getFillColorSpace() { return fillColorSpace; }
  GfxColorSpace *getStrokeColorSpace() { return strokeColorSpace; }
  GfxPattern *getFillPattern() { return fillPattern; }
  GfxPattern *getStrokePattern() { return strokePattern; }
  fouble getFillOpacity() { return fillOpacity; }
  fouble getStrokeOpacity() { return strokeOpacity; }
  fouble getLineWidth() { return lineWidth; }
  void getLineDash(fouble **dash, int *length, fouble *start)
    { *dash = lineDash; *length = lineDashLength; *start = lineDashStart; }
  int getFlatness() { return flatness; }
  int getLineJoin() { return lineJoin; }
  int getLineCap() { return lineCap; }
  fouble getMiterLimit() { return miterLimit; }
  GfxFont *getFont() { return font; }
  fouble getFontSize() { return fontSize; }
  fouble *getTextMat() { return textMat; }
  fouble getCharSpace() { return charSpace; }
  fouble getWordSpace() { return wordSpace; }
  fouble getHorizScaling() { return horizScaling; }
  fouble getLeading() { return leading; }
  fouble getRise() { return rise; }
  int getRender() { return render; }
  GfxPath *getPath() { return path; }
  fouble getCurX() { return curX; }
  fouble getCurY() { return curY; }
  void getClipBBox(fouble *xMin, fouble *yMin, fouble *xMax, fouble *yMax)
    { *xMin = clipXMin; *yMin = clipYMin; *xMax = clipXMax; *yMax = clipYMax; }
  fouble getLineX() { return lineX; }
  fouble getLineY() { return lineY; }

  // Is there a current point/path?
  GBool isCurPt() { return path->isCurPt(); }
  GBool isPath() { return path->isPath(); }

  // Transforms.
  void transform(fouble x1, fouble y1, fouble *x2, fouble *y2)
    { *x2 = ctm[0] * x1 + ctm[2] * y1 + ctm[4];
      *y2 = ctm[1] * x1 + ctm[3] * y1 + ctm[5]; }
  void transformDelta(fouble x1, fouble y1, fouble *x2, fouble *y2)
    { *x2 = ctm[0] * x1 + ctm[2] * y1;
      *y2 = ctm[1] * x1 + ctm[3] * y1; }
  void textTransform(fouble x1, fouble y1, fouble *x2, fouble *y2)
    { *x2 = textMat[0] * x1 + textMat[2] * y1 + textMat[4];
      *y2 = textMat[1] * x1 + textMat[3] * y1 + textMat[5]; }
  void textTransformDelta(fouble x1, fouble y1, fouble *x2, fouble *y2)
    { *x2 = textMat[0] * x1 + textMat[2] * y1;
      *y2 = textMat[1] * x1 + textMat[3] * y1; }
  fouble transformWidth(fouble w);
  fouble getTransformedLineWidth()
    { return transformWidth(lineWidth); }
  fouble getTransformedFontSize();
  void getFontTransMat(fouble *m11, fouble *m12, fouble *m21, fouble *m22);

  // Change state parameters.
  void setCTM(fouble a, fouble b, fouble c,
	      fouble d, fouble e, fouble f);
  void concatCTM(fouble a, fouble b, fouble c,
		 fouble d, fouble e, fouble f);
  void setFillColorSpace(GfxColorSpace *colorSpace);
  void setStrokeColorSpace(GfxColorSpace *colorSpace);
  void setFillColor(GfxColor *color) { fillColor = *color; }
  void setStrokeColor(GfxColor *color) { strokeColor = *color; }
  void setFillPattern(GfxPattern *pattern);
  void setStrokePattern(GfxPattern *pattern);
  void setFillOpacity(fouble opac) { fillOpacity = opac; }
  void setStrokeOpacity(fouble opac) { strokeOpacity = opac; }
  void setLineWidth(fouble width) { lineWidth = width; }
  void setLineDash(fouble *dash, int length, fouble start);
  void setFlatness(int flatness1) { flatness = flatness1; }
  void setLineJoin(int lineJoin1) { lineJoin = lineJoin1; }
  void setLineCap(int lineCap1) { lineCap = lineCap1; }
  void setMiterLimit(fouble limit) { miterLimit = limit; }
  void setFont(GfxFont *fontA, fouble fontSizeA)
    { font = fontA; fontSize = fontSizeA; }
  void setTextMat(fouble a, fouble b, fouble c,
		  fouble d, fouble e, fouble f)
    { textMat[0] = a; textMat[1] = b; textMat[2] = c;
      textMat[3] = d; textMat[4] = e; textMat[5] = f; }
  void setCharSpace(fouble space)
    { charSpace = space; }
  void setWordSpace(fouble space)
    { wordSpace = space; }
  void setHorizScaling(fouble scale)
    { horizScaling = 0.01 * scale; }
  void setLeading(fouble leadingA)
    { leading = leadingA; }
  void setRise(fouble riseA)
    { rise = riseA; }
  void setRender(int renderA)
    { render = renderA; }

  // Add to path.
  void moveTo(fouble x, fouble y)
    { path->moveTo(curX = x, curY = y); }
  void lineTo(fouble x, fouble y)
    { path->lineTo(curX = x, curY = y); }
  void curveTo(fouble x1, fouble y1, fouble x2, fouble y2,
	       fouble x3, fouble y3)
    { path->curveTo(x1, y1, x2, y2, curX = x3, curY = y3); }
  void closePath()
    { path->close(); curX = path->getLastX(); curY = path->getLastY(); }
  void clearPath();

  // Update clip region.
  void clip();

  // Text position.
  void textMoveTo(fouble tx, fouble ty)
    { lineX = tx; lineY = ty; textTransform(tx, ty, &curX, &curY); }
  void textShift(fouble tx);
  void shift(fouble dx, fouble dy);

  // Push/pop GfxState on/off stack.
  GfxState *save();
  GfxState *restore();
  GBool hasSaves() { return saved != NULL; }

private:

  fouble ctm[6];		// coord transform matrix
  fouble px1, py1, px2, py2;	// page corners (user coords)
  fouble pageWidth, pageHeight;	// page size (pixels)

  GfxColorSpace *fillColorSpace;   // fill color space
  GfxColorSpace *strokeColorSpace; // stroke color space
  GfxColor fillColor;		// fill color
  GfxColor strokeColor;		// stroke color
  GfxPattern *fillPattern;	// fill pattern
  GfxPattern *strokePattern;	// stroke pattern
  fouble fillOpacity;		// fill opacity
  fouble strokeOpacity;		// stroke opacity

  fouble lineWidth;		// line width
  fouble *lineDash;		// line dash
  int lineDashLength;
  fouble lineDashStart;
  int flatness;			// curve flatness
  int lineJoin;			// line join style
  int lineCap;			// line cap style
  fouble miterLimit;		// line miter limit

  GfxFont *font;		// font
  fouble fontSize;		// font size
  fouble textMat[6];		// text matrix
  fouble charSpace;		// character spacing
  fouble wordSpace;		// word spacing
  fouble horizScaling;		// horizontal scaling
  fouble leading;		// text leading
  fouble rise;			// text rise
  int render;			// text rendering mode

  GfxPath *path;		// array of path elements
  fouble curX, curY;		// current point (user coords)
  fouble lineX, lineY;		// start of current text line (text coords)

  fouble clipXMin, clipYMin,	// bounding box for clip region
         clipXMax, clipYMax;

  GfxState *saved;		// next GfxState on stack

  GfxState(GfxState *state);
};

#endif
