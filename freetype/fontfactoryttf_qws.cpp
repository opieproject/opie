/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/


#include "fontfactoryttf_qws.h"

#ifdef QT_NO_FREETYPE

#include "qfontdata_p.h"
#include <string.h>
#include <stdio.h>

#define FLOOR(x)  ((x) & -64)
#define CEIL(x)   (((x)+63) & -64)
#define TRUNC(x)  ((x) >> 6)


extern "C" {
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/ftbbox.h>
}

class QDiskFontFT : public QDiskFontPrivate {
public:
    FT_Face face;
};

class QRenderedFontFT : public QRenderedFont {
public:
    QRenderedFontFT(QDiskFont* f, const QFontDef &d) :
	QRenderedFont(f,d)
    {
	QDiskFontFT *df = (QDiskFontFT*)(f->p);
	myface=df->face;
	selectThisSize();
	// A 1-pixel baseline is excluded in Qt/Windows/X11 fontmetrics
	// (see QFontMetrics::height())
	//
	fascent=CEIL(myface->size->metrics.ascender)/64;
	fdescent=-FLOOR(myface->size->metrics.descender)/64-1;
	fmaxwidth=CEIL(myface->size->metrics.max_advance)/64;
	fleading=CEIL(myface->size->metrics.height)/64
	    - fascent - fdescent + 1;

	// FT has these in font units
	funderlinepos = ptsize/200+1;
	funderlinewidth = ptsize/200+1;
    }

    ~QRenderedFontFT()
    {
	// When inter-process glyph sharing is implemented, the glyph data
	// for this font can be dereferenced here.
    }

    bool unicode(int & i) const
    {
	int ret;

	FT_Face theface=myface;

	ret=FT_Get_Char_Index(theface,i);

	if (ret==0) {
	    return FALSE;
	} else {
	    i=ret;
	    return TRUE;
	}
    }

    bool inFont(QChar ch) const
    {
	int index = ch.unicode();
	return unicode(index);
    }

    QGlyph render(QChar ch)
    {
	selectThisSize();

	int index = ch.unicode();
	if ( !unicode(index) )
	    index = 0;
	QGlyph result;

	FT_Error err;

	err=FT_Load_Glyph(myface,index,FT_LOAD_DEFAULT);
	if(err)
	    qFatal("Load glyph error %x",err);

	int width=0,height=0,pitch=0,size=0;
	FT_Glyph glyph;
	err=FT_Get_Glyph( myface->glyph, &glyph );
	if(err)
	    qFatal("Get glyph error %x",err);

	FT_BBox bbox;
	FT_Glyph_Get_CBox(glyph, ft_glyph_bbox_gridfit, &bbox);

	FT_Vector origin;
	origin.x = -bbox.xMin;
	origin.y = -bbox.yMin;

	if ( FT_IS_SCALABLE(myface) ) {
	    err=FT_Glyph_To_Bitmap(&glyph,
		smooth ? ft_render_mode_normal : ft_render_mode_mono,
		&origin, 1); // destroy original glyph
	    if(err)
		qWarning("Get bitmap error %d",err);
	}

	if ( !err ) {
	    FT_Bitmap bm = ((FT_BitmapGlyph)glyph)->bitmap;
	    pitch = bm.pitch;
	    size=pitch*bm.rows;
	    result.data = new uchar[size]; // XXX memory manage me
	    width=bm.width;
	    height=bm.rows;
	    if ( size ) {
		memcpy( result.data, bm.buffer, size );
	    } else {
		result.data = new uchar[0]; // XXX memory manage me
	    }
	} else {
	    result.data = new uchar[0]; // XXX memory manage me
	}

	result.metrics = new QGlyphMetrics;
	memset((char*)result.metrics, 0, sizeof(QGlyphMetrics));
	result.metrics->bearingx=myface->glyph->metrics.horiBearingX/64;
	result.metrics->advance=myface->glyph->metrics.horiAdvance/64;
	result.metrics->bearingy=myface->glyph->metrics.horiBearingY/64;

	result.metrics->linestep=pitch;
	result.metrics->width=width;
	result.metrics->height=height;

	// XXX memory manage me
	// At some point inter-process glyph data sharing must be implemented
	// and the flag below can be set to prevent Qt from deleting the glyph
	// data.
	// result.metrics->flags = QGlyphMetrics::RendererOwnsData;

	FT_Done_Glyph( glyph );

	return result;
    }

    FT_Face myface;

private:
    void selectThisSize()
    {
	int psize=(ptsize<<6)/10;

	// Assume 72 dpi for now
	const int dpi=72;
	FT_Error err;
	err=FT_Set_Char_Size(myface, psize,psize,dpi,dpi);
	if (err) {
	    if (FT_IS_SCALABLE(myface) ) {
		qWarning("Set char size error %x for size %d",err,ptsize);
	    } else {
		int best=-1;
		int bdh=99;
		for (int i=0; i<myface->num_fixed_sizes; i++) {
		    FT_Bitmap_Size& sz=myface->available_sizes[i];
		    int dh = sz.height - ptsize*dpi/72/10;
		    dh = QABS(dh);
		    if ( dh < bdh ) {
			bdh=dh;
			best=i;
		    }
		}
		if ( best >= 0 )
		    err=FT_Set_Pixel_Sizes(myface,
			myface->available_sizes[best].width,
			myface->available_sizes[best].height);
		if ( err )
		    qWarning("Set char size error %x for size %d",err,ptsize);
	    }
	}
    }
};

QFontFactoryFT::QFontFactoryFT()
{
    FT_Error err;
    err=FT_Init_FreeType(&library);
    if(err) {
	qFatal("Couldn't initialise Freetype library");
    }
}

QFontFactoryFT::~QFontFactoryFT()
{
}

QString QFontFactoryFT::name()
{
    return "FT";
}

QRenderedFont * QFontFactoryFT::get(const QFontDef & f,QDiskFont * f2)
{
    return new QRenderedFontFT(f2, f);
}

void QFontFactoryFT::load(QDiskFont * qdf) const
{
    if(qdf->loaded)
	return;
    QDiskFontFT *f = new QDiskFontFT;
    qdf->p=f;
    FT_Error err;
    err=FT_New_Face(library,qdf->file.ascii(),0,&(f->face));
    if(err) {
	qFatal("Error %d opening face",err);
    }
    qdf->loaded=true;
}


#endif // QT_NO_FREETYPE
