/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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

#include "unikeyboard.h"

#include <qpe/fontmanager.h>

#include <qpainter.h>
#include <qfontmetrics.h>
#include <qcombobox.h>
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
#include <qwindowsystem_qws.h>
#endif

static const int nw = 8;

typedef struct BlockMap {
    ushort start;
    ushort stop;
    const char *name;
};

//# Start Code; Block Name

static const BlockMap blockMap[] = 
{
{0x0000, 0x007F, "Basic Latin"},
{0x0080, 0x00FF, "Latin-1 Supplement"},
{0x0100, 0x017F, "Latin Extended-A"},
{0x0180, 0x024F, "Latin Extended-B"},
{0x0250, 0x02AF, "IPA Extensions"},
{0x02B0, 0x02FF, "Spacing Modifier Letters"},
{0x0300, 0x036F, "Combining Diacritical Marks"},
{0x0370, 0x03FF, "Greek"},
{0x0400, 0x04FF, "Cyrillic"},
{0x0530, 0x058F, "Armenian"},
{0x0590, 0x05FF, "Hebrew"},
{0x0600, 0x06FF, "Arabic"},
{0x0700, 0x074F, "Syriac  "},
{0x0780, 0x07BF, "Thaana"},
{0x0900, 0x097F, "Devanagari"},
{0x0980, 0x09FF, "Bengali"},
{0x0A00, 0x0A7F, "Gurmukhi"},
{0x0A80, 0x0AFF, "Gujarati"},
{0x0B00, 0x0B7F, "Oriya"},
{0x0B80, 0x0BFF, "Tamil"},
{0x0C00, 0x0C7F, "Telugu"},
{0x0C80, 0x0CFF, "Kannada"},
{0x0D00, 0x0D7F, "Malayalam"},
{0x0D80, 0x0DFF, "Sinhala"},
{0x0E00, 0x0E7F, "Thai"},
{0x0E80, 0x0EFF, "Lao"},
{0x0F00, 0x0FFF, "Tibetan"},
{0x1000, 0x109F, "Myanmar "},
{0x10A0, 0x10FF, "Georgian"},
{0x1100, 0x11FF, "Hangul Jamo"},
{0x1200, 0x137F, "Ethiopic"},
{0x13A0, 0x13FF, "Cherokee"},
{0x1400, 0x167F, "Unified Canadian Aboriginal Syllabics"},
{0x1680, 0x169F, "Ogham"},
{0x16A0, 0x16FF, "Runic"},
{0x1780, 0x17FF, "Khmer"},
{0x1800, 0x18AF, "Mongolian"},
{0x1E00, 0x1EFF, "Latin Extended Additional"},
{0x1F00, 0x1FFF, "Greek Extended"},
{0x2000, 0x206F, "General Punctuation"},
{0x2070, 0x209F, "Superscripts and Subscripts"},
{0x20A0, 0x20CF, "Currency Symbols"},
{0x20D0, 0x20FF, "Combining Marks for Symbols"},
{0x2100, 0x214F, "Letterlike Symbols"},
{0x2150, 0x218F, "Number Forms"},
{0x2190, 0x21FF, "Arrows"},
{0x2200, 0x22FF, "Mathematical Operators"},
{0x2300, 0x23FF, "Miscellaneous Technical"},
{0x2400, 0x243F, "Control Pictures"},
{0x2440, 0x245F, "Optical Character Recognition"},
{0x2460, 0x24FF, "Enclosed Alphanumerics"},
{0x2500, 0x257F, "Box Drawing"},
{0x2580, 0x259F, "Block Elements"},
{0x25A0, 0x25FF, "Geometric Shapes"},
{0x2600, 0x26FF, "Miscellaneous Symbols"},
{0x2700, 0x27BF, "Dingbats"},
{0x2800, 0x28FF, "Braille Patterns"},
{0x2E80, 0x2EFF, "CJK Radicals Supplement"},
{0x2F00, 0x2FDF, "Kangxi Radicals"},
{0x2FF0, 0x2FFF, "Ideographic Description Characters"},
{0x3000, 0x303F, "CJK Symbols and Punctuation"},
{0x3040, 0x309F, "Hiragana"},
{0x30A0, 0x30FF, "Katakana"},
{0x3100, 0x312F, "Bopomofo"},
{0x3130, 0x318F, "Hangul Compatibility Jamo"},
{0x3190, 0x319F, "Kanbun"},
{0x31A0, 0x31BF, "Bopomofo Extended"},
{0x3200, 0x32FF, "Enclosed CJK Letters and Months"},
{0x3300, 0x33FF, "CJK Compatibility"},
{0x3400, 0x4DB5, "CJK Unified Ideographs Extension A"},
{0x4E00, 0x9FFF, "CJK Unified Ideographs"},
{0xA000, 0xA48F, "Yi Syllables"},
{0xA490, 0xA4CF, "Yi Radicals"},
{0xAC00, 0xD7A3, "Hangul Syllables"},
{0xD800, 0xDB7F, "High Surrogates"},
{0xDB80, 0xDBFF, "High Private Use Surrogates"},
{0xDC00, 0xDFFF, "Low Surrogates"},
{0xE000, 0xF8FF, "Private Use"},
{0xF900, 0xFAFF, "CJK Compatibility Ideographs"},
{0xFB00, 0xFB4F, "Alphabetic Presentation Forms"},
{0xFB50, 0xFDFF, "Arabic Presentation Forms-A"},
{0xFE20, 0xFE2F, "Combining Half Marks"},
{0xFE30, 0xFE4F, "CJK Compatibility Forms"},
{0xFE50, 0xFE6F, "Small Form Variants"},
{0xFE70, 0xFEFE, "Arabic Presentation Forms-B"},
{0xFF00, 0xFEFF, "Halfwidth and Fullwidth Forms"},
{0xFFF0, 0xFFEF, "Specials"},
{0xFFFF, 0xFFFF, 	0} };


UniScrollview::UniScrollview(QWidget* parent, const char* name, int f) :
    QScrollView(parent, name, f)
{
    //    smallFont.setRawName( "-adobe-courier-medium-r-normal--10-100-75-75-m-60-iso8859-1" ); //######
    smallFont = QFont( "Helvetica", 8 );
    QFontMetrics sfm( smallFont );
    xoff = sfm.width( "AAA" );
    setFont( FontManager::unicodeFont( FontManager::Fixed ) );
    QFontMetrics fm( font() );
    cellsize = fm.lineSpacing() + 2;
    resizeContents( cellsize*nw, cellsize*65536/nw );
    verticalScrollBar()->setLineStep(cellsize);
    
    viewport()->setBackgroundMode( QWidget::PaletteBase );
}



void UniScrollview::contentsMousePressEvent(QMouseEvent* e)
{
    if ( e->x() < xoff || e->x() > xoff + nw*cellsize )
	return;
    int row = e->y()/cellsize;
    int col = (e->x()-xoff)/cellsize;
    int u = row*nw+col;
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    emit key( u, 0, 0, true, false );
    emit key( u, 0, 0, false, false );
#endif
}


void UniScrollview::contentsMouseReleaseEvent(QMouseEvent*)
{
}

void UniScrollview::scrollTo( int unicode )
{
    int row = unicode / nw;
    setContentsPos( 0, row*cellsize );
}


void UniScrollview::drawContents( QPainter *p, int /*cx*/, int cy, int /*cw*/, int ch )
{
    QFontMetrics fm = fontMetrics();
    int row = cy / cellsize;
    int y = row*cellsize;
    while ( y < cy+ch ) {
	p->drawLine( xoff, y, xoff+nw*cellsize, y );
	if ( row*nw%16 == 0 ) {
	    p->setFont( smallFont );
	    QString s;
	    s.sprintf( "%03X", row*nw/16 ); 
	    p->drawText( 0, y, xoff, cellsize, AlignLeft, s );
	    p->setFont( font() );
	}
	for ( int i = 0; i < nw; i++ ) {
	    p->drawLine( xoff+i*cellsize, y, xoff+i*cellsize, y+cellsize );
	    QChar u = row*nw + i;
	    if ( fm.inFont( u ) )
		p->drawText( xoff+i*cellsize, y, cellsize, cellsize, AlignCenter,
			     u );
	}
	p->drawLine( xoff+nw*cellsize, y, xoff+nw*cellsize, y+cellsize );
	row++;
	y += cellsize;
    }
}




UniKeyboard::UniKeyboard(QWidget* parent, const char* name, int f )
    : QFrame( parent, name, f )
{
    setFrameStyle( NoFrame );
    sv = new UniScrollview( this );
    cb = new QComboBox( FALSE, this );
    currentBlock = 0;
    QFontMetrics fm = sv->fontMetrics();
    cbmap = new int[sizeof(blockMap)/sizeof(blockMap[0])];
    for ( int i = 0; blockMap[i].name; i++ ) {
	bool any=FALSE;
	for ( int c=blockMap[i].start; !any && c<=blockMap[i].stop; c++ )
	    any = fm.inFont(QChar(c));
	if ( any ) {
	    cbmap[cb->count()]=i;
	    cb->insertItem( blockMap[i].name );
	}
    }
    connect( cb, SIGNAL( activated(int)), this, SLOT( handleCombo(int)) );
    connect( sv, SIGNAL( contentsMoving(int,int)), this, SLOT( svMove(int,int)) );
    connect( sv, SIGNAL( key(ushort,ushort,ushort,bool,bool)),
	    this, SIGNAL( key(ushort,ushort,ushort,bool,bool)) );
}

UniKeyboard::~UniKeyboard()
{
    delete [] cbmap;
}

void UniKeyboard::resizeEvent(QResizeEvent *)
{
    int d = frameWidth();
    cb->setGeometry( d, d, width()-2*d, cb->sizeHint().height() );
    sv->setGeometry( d, cb->height()+d, width()-2*d, height()-cb->height()-2*d );
}

void UniKeyboard::svMove( int /*x*/, int y )
{
    int cs = sv->cellSize();
    int u = ((y+cs-1)/cs) * nw;
    int i = currentBlock;
    while ( i > 0 && blockMap[i].start > u ) {
       i--;
    }
    while ( blockMap[i+1].name && blockMap[i+1].start < u ) {
	i++;
    }
    if ( i != currentBlock ) {
	currentBlock = i;
	for (int ind=0; ind<cb->count(); ind++) {
	    if ( cbmap[ind] == i ) {
		cb->setCurrentItem( ind );
		break;
	    }
	}
    }
}

void UniKeyboard::handleCombo( int i )
{
    currentBlock = cbmap[i];
    sv->scrollTo( blockMap[currentBlock].start );
}

void UniKeyboard::resetState()
{
}

QSize UniKeyboard::sizeHint() const
{
    return QSize( 240, 2+sv->cellSize()*4+cb->sizeHint().height() );
}
