/* This file is part of the KDE libraries
  Copyright (C) 1999 Daniel M. Duley <mosfet@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License version 2 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
*/
#include "othemebase.h"
#include "ogfxeffect.h"
#include <qpe/qpeapplication.h> 
//#include <kdebug.h>
//#include <klocale.h>
#include <qpe/config.h> 
//#include <kglobal.h>
//#include <kglobalsettings.h>
//#include <kstddirs.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <stdlib.h> 
#include <qstringlist.h>

#include <stdio.h>


template class QIntCache<OThemePixmap>
;

static const char *widgetEntries[] =
    { // unsunken widgets (see header)
        "PushButton", "ComboBox", "HSBarSlider", "VSBarSlider", "Bevel", "ToolButton",
        "ScrollButton", "HScrollDeco", "VScrollDeco", "ComboDeco", "MenuItem", "Tab",
        "ArrowUp", "ArrowDown", "ArrowLeft", "ArrowRight",
        // sunken widgets
        "PushButtonDown", "ComboBoxDown", "HSBarSliderDown", "VSBarSliderDown",
        "BevelDown", "ToolButtonDown", "ScrollButtonDown", "HScrollDecoDown",
        "VScrollDecoDown", "ComboDecoDown", "MenuItemDown", "TabDown", "SunkenArrowUp",
        "SunkenArrowDown", "SunkenArrowLeft", "SunkenArrowRight",
        // everything else
        "HScrollGroove", "VScrollGroove", "Slider", "SliderGroove", "CheckBoxDown",
        "CheckBox", "RadioDown", "Radio", "HBarHandle", "VBarHandle",
        "ToolBar", "Splitter", "CheckMark", "MenuBar", "DisableArrowUp",
        "DisableArrowDown", "DisableArrowLeft", "DisableArrowRight", "ProgressBar",
        "ProgressBackground", "MenuBarItem", "Background"
    };

#define INHERIT_ITEMS 16


// This is used to encode the keys. I used to use masks but I think this
// bitfield is nicer :) I don't know why C++ coders don't use these more..
// (mosfet)
struct kthemeKeyData
{
unsigned int id :
	6;
unsigned int width :
	12;
unsigned int height :
	12;
unsigned int border :
	1;
unsigned int mask :
	1;
};

union kthemeKey{
	kthemeKeyData data;
	unsigned int cacheKey;
};

class MyConfig : public Config
{
public:
	MyConfig ( const QString &f, Domain d ) : Config ( f, d )
	{ }

	bool hasGroup ( const QString &gname ) const
	{
		QMap< QString, ConfigGroup>::ConstIterator it = groups. find ( gname );
		return ( it != groups.end() );
	}
};

void OThemeBase::generateBorderPix( int i )
{
	// separate pixmap into separate components
	if ( pbPixmaps[ i ] ) {
		// evidently I have to do masks manually...
		const QBitmap * srcMask = pbPixmaps[ i ] ->mask();
		QBitmap destMask( pbWidth[ i ], pbWidth[ i ] );
		QPixmap tmp( pbWidth[ i ], pbWidth[ i ] );

		bitBlt( &tmp, 0, 0, pbPixmaps[ i ], 0, 0, pbWidth[ i ], pbWidth[ i ],
		        Qt::CopyROP, false );
		if ( srcMask ) {
			bitBlt( &destMask, 0, 0, srcMask, 0, 0, pbWidth[ i ], pbWidth[ i ],
			        Qt::CopyROP, false );
			tmp.setMask( destMask );
		}
		pbPixmaps[ i ] ->setBorder( OThemePixmap::TopLeft, tmp );

		bitBlt( &tmp, 0, 0, pbPixmaps[ i ], pbPixmaps[ i ] ->width() - pbWidth[ i ], 0,
		        pbWidth[ i ], pbWidth[ i ], Qt::CopyROP, false );
		if ( srcMask ) {
			bitBlt( &destMask, 0, 0, srcMask, pbPixmaps[ i ] ->width() - pbWidth[ i ],
			        0, pbWidth[ i ], pbWidth[ i ], Qt::CopyROP, false );
			tmp.setMask( destMask );
		}
		pbPixmaps[ i ] ->setBorder( OThemePixmap::TopRight, tmp );

		bitBlt( &tmp, 0, 0, pbPixmaps[ i ], 0, pbPixmaps[ i ] ->height() - pbWidth[ i ],
		        pbWidth[ i ], pbWidth[ i ], Qt::CopyROP, false );
		if ( srcMask ) {
			bitBlt( &destMask, 0, 0, srcMask, 0, pbPixmaps[ i ] ->height() - pbWidth[ i ],
			        pbWidth[ i ], pbWidth[ i ], Qt::CopyROP, false );
			tmp.setMask( destMask );
		}
		pbPixmaps[ i ] ->setBorder( OThemePixmap::BottomLeft, tmp );

		bitBlt( &tmp, 0, 0, pbPixmaps[ i ], pbPixmaps[ i ] ->width() - pbWidth[ i ],
		        pbPixmaps[ i ] ->height() - pbWidth[ i ], pbWidth[ i ], pbWidth[ i ],
		        Qt::CopyROP, false );
		if ( srcMask ) {
			bitBlt( &destMask, 0, 0, srcMask, pbPixmaps[ i ] ->width() - pbWidth[ i ],
			        pbPixmaps[ i ] ->height() - pbWidth[ i ], pbWidth[ i ], pbWidth[ i ],
			        Qt::CopyROP, false );
			tmp.setMask( destMask );
		}
		pbPixmaps[ i ] ->setBorder( OThemePixmap::BottomRight, tmp );

		tmp.resize( pbPixmaps[ i ] ->width() - pbWidth[ i ] * 2, pbWidth[ i ] );
		destMask.resize( pbPixmaps[ i ] ->width() - pbWidth[ i ] * 2, pbWidth[ i ] );
		bitBlt( &tmp, 0, 0, pbPixmaps[ i ], pbWidth[ i ], 0,
		        pbPixmaps[ i ] ->width() - pbWidth[ i ] * 2, pbWidth[ i ], Qt::CopyROP, false );
		if ( srcMask ) {
			bitBlt( &destMask, 0, 0, srcMask, pbWidth[ i ], 0,
			        pbPixmaps[ i ] ->width() - pbWidth[ i ] * 2, pbWidth[ i ],
			        Qt::CopyROP, false );
			tmp.setMask( destMask );
		}
		pbPixmaps[ i ] ->setBorder( OThemePixmap::Top, tmp );

		bitBlt( &tmp, 0, 0, pbPixmaps[ i ], pbWidth[ i ],
		        pbPixmaps[ i ] ->height() - pbWidth[ i ],
		        pbPixmaps[ i ] ->width() - pbWidth[ i ] * 2, pbWidth[ i ], Qt::CopyROP, false );
		if ( srcMask ) {
			bitBlt( &destMask, 0, 0, srcMask, pbWidth[ i ],
			        pbPixmaps[ i ] ->height() - pbWidth[ i ],
			        pbPixmaps[ i ] ->width() - pbWidth[ i ] * 2, pbWidth[ i ], Qt::CopyROP, false );
			tmp.setMask( destMask );
		}
		pbPixmaps[ i ] ->setBorder( OThemePixmap::Bottom, tmp );

		tmp.resize( pbWidth[ i ], pbPixmaps[ i ] ->height() - pbWidth[ i ] * 2 );
		destMask.resize( pbWidth[ i ], pbPixmaps[ i ] ->height() - pbWidth[ i ] * 2 );
		bitBlt( &tmp, 0, 0, pbPixmaps[ i ], 0, pbWidth[ i ], pbWidth[ i ],
		        pbPixmaps[ i ] ->height() - pbWidth[ i ] * 2, Qt::CopyROP, false );
		if ( srcMask ) {
			bitBlt( &destMask, 0, 0, srcMask, 0, pbWidth[ i ], pbWidth[ i ],
			        pbPixmaps[ i ] ->height() - pbWidth[ i ] * 2, Qt::CopyROP, false );
			tmp.setMask( destMask );
		}

		pbPixmaps[ i ] ->setBorder( OThemePixmap::Left, tmp );

		bitBlt( &tmp, 0, 0, pbPixmaps[ i ], pbPixmaps[ i ] ->width() - pbWidth[ i ],
		        pbWidth[ i ], pbWidth[ i ], pbPixmaps[ i ] ->height() - pbWidth[ i ] * 2,
		        Qt::CopyROP, false );
		if ( srcMask ) {
			bitBlt( &destMask, 0, 0, srcMask, pbPixmaps[ i ] ->width() - pbWidth[ i ],
			        pbWidth[ i ], pbWidth[ i ], pbPixmaps[ i ] ->height() - pbWidth[ i ] * 2,
			        Qt::CopyROP, false );
			tmp.setMask( destMask );
		}
		pbPixmaps[ i ] ->setBorder( OThemePixmap::Right, tmp );
	}
	else
		qDebug ( "OThemeBase: Tried making border from empty pixmap" );
}


void OThemeBase::copyWidgetConfig( int sourceID, int destID, QString *pixnames,
                                   QString *brdnames )
{
	scaleHints[ destID ] = scaleHints[ sourceID ];
	gradients[ destID ] = gradients[ sourceID ];
	blends[ destID ] = blends[ sourceID ];
	bContrasts[ destID ] = bContrasts[ sourceID ];
	borders[ destID ] = borders[ sourceID ];
	highlights[ destID ] = highlights[ sourceID ];

	if ( grLowColors[ sourceID ] )
		grLowColors[ destID ] = new QColor( *grLowColors[ sourceID ] );
	else
		grLowColors[ destID ] = NULL;

	if ( grHighColors[ sourceID ] )
		grHighColors[ destID ] = new QColor( *grHighColors[ sourceID ] );
	else
		grHighColors[ destID ] = NULL;

	if ( colors[ sourceID ] )
		colors[ destID ] = new QColorGroup( *colors[ sourceID ] );
	else
		colors[ destID ] = NULL;

	// pixmap
	pixnames[ destID ] = pixnames[ sourceID ];
	duplicate[ destID ] = false;
	pixmaps[ destID ] = NULL;
	images[ destID ] = NULL;
	if ( !pixnames[ destID ].isEmpty() ) {
		if ( scaleHints[ sourceID ] == TileScale && blends[ sourceID ] == 0.0 ) {
			pixmaps[ destID ] = pixmaps[ sourceID ];
			duplicate[ destID ] = true;
		}
		if ( !duplicate[ destID ] ) {
			pixmaps[ destID ] = loadPixmap( pixnames[ destID ] );
			if ( scaleHints[ destID ] == TileScale && blends[ destID ] == 0.0 )
				images[ destID ] = NULL;
			else
				images[ destID ] = loadImage( pixnames[ destID ] );
		}
	}

	// border pixmap
	pbDuplicate[ destID ] = false;
	pbPixmaps[ destID ] = NULL;
	pbWidth[ destID ] = pbWidth[ sourceID ];
	brdnames[ destID ] = brdnames[ sourceID ];
	if ( !brdnames[ destID ].isEmpty() ) {
		pbPixmaps[ destID ] = pbPixmaps[ sourceID ];
		pbDuplicate[ destID ] = true;
	}

	if ( sourceID == ActiveTab && destID == InactiveTab )
		aTabLine = iTabLine;
	else if ( sourceID == InactiveTab && destID == ActiveTab )
		iTabLine = aTabLine;
}

void OThemeBase::readConfig( Qt::GUIStyle /*style*/ )
{
#define PREBLEND_ITEMS 12
	static WidgetType preBlend[] = {Slider, IndicatorOn, IndicatorOff,
	                                ExIndicatorOn, ExIndicatorOff, HScrollDeco, VScrollDeco, HScrollDecoDown,
	                                VScrollDecoDown, ComboDeco, ComboDecoDown, CheckMark};

	int i;
	QString tmpStr;
	QString copyfrom[ WIDGETS ];
	QString pixnames[ WIDGETS ]; // used for duplicate check
	QString brdnames[ WIDGETS ];
	bool loaded[ WIDGETS ]; // used for preloading for CopyWidget

	if ( configFileName.isEmpty() ) {
		Config cfg ( "qpe" );
		cfg. setGroup ( "Appearance" );
	
		configFileName = cfg. readEntry ( "Theme", "default" );
	}
	MyConfig config( configFilePath + "/themes/" + configFileName + ".themerc" , Config::File );

//	printf ( "Opened config file: %s\n", ( configFilePath + "/themes/" + configFileName + ".themerc" ). ascii());

	// Are we initalized?
	applyMiscResourceGroup( &config );
	for ( i = 0; i < INHERIT_ITEMS; ++i ) {
		applyResourceGroup( &config, i, copyfrom, pixnames, brdnames );		
//		printf ( "%d [%s]: copy=%s, pix=%s, brd=%s\n", i, widgetEntries [i], copyfrom [i].latin1(), pixnames[i].latin1(),brdnames[i].latin1() );
	}
	for ( ; i < INHERIT_ITEMS*2; ++i ) {
		if ( config.hasGroup( QString( widgetEntries[ i ] ) ) ) {
			applyResourceGroup( &config, i, copyfrom, pixnames, brdnames );
//			printf ( "%d [%s]: copy=%s, pix=%s, brd=%s\n", i, widgetEntries [i], copyfrom [i].latin1(), pixnames[i].latin1(),brdnames[i].latin1() );
		}
		else {
			copyfrom [ i ] = widgetEntries[ i - INHERIT_ITEMS ];
//			printf ( "%d [%s]: copy=%s\n", i, widgetEntries [i], copyfrom [i].latin1());
		}
	}
	for ( ; i < WIDGETS; ++i ) {
		applyResourceGroup( &config, i, copyfrom, pixnames, brdnames );
//		printf ( "%d [%s]: copy=%s, pix=%s, brd=%s\n", i, widgetEntries [i], copyfrom [i].latin1(), pixnames[i].latin1(),brdnames[i].latin1() );
	}

	// initalize defaults that may not be read
	for ( i = 0; i < WIDGETS; ++i )
		loaded[ i ] = false;
	btnXShift = btnYShift = focus3DOffset = 0;
	aTabLine = iTabLine = true;
	roundedButton = roundedCombo = roundedSlider = focus3D = false;
	splitterWidth = 10;

	for ( i = 0; i < WIDGETS; ++i ) {
		readResourceGroup( i, copyfrom, pixnames, brdnames, loaded );
//		printf ( "%d [%s]: copy=%s, pix=%s, brd=%s, colors=%s\n", i, widgetEntries [i], copyfrom [i].latin1(), pixnames[i].latin1(),brdnames[i].latin1(), (colors[i]?colors[i]->background().name().latin1():"<none)" ));
	}

	// misc items
	readMiscResourceGroup();

	// Handle preblend items
	for ( i = 0; i < PREBLEND_ITEMS; ++i ) {
		if ( pixmaps[ preBlend[ i ] ] != NULL && blends[ preBlend[ i ] ] != 0.0 )
			blend( preBlend[ i ] );
	}
}

OThemeBase::OThemeBase( const QString & configFile )
		: QWindowsStyle()
{
	configFilePath = QPEApplication::qpeDir ( ) + "/plugins/styles/";
	configFileName = configFile;

	readConfig( Qt::WindowsStyle );
	cache = new OThemeCache( cacheSize );
}

void OThemeBase::applyConfigFile( const QString &/*file*/ )
{
#if 0
	// handle std color scheme
	Config inConfig( file, Config::File );
	Config globalConfig ( "qpe" );
	
	globalConfig. setGroup ( "Apperance" );
	inConfig. setGroup( "General" );
	
	if ( inConfig.hasKey( "foreground" ) )
		globalConfig.writeEntry( "Text",                    inConfig.readEntry( "foreground", " " ) );
	if ( inConfig.hasKey( "background" ) )
		globalConfig.writeEntry( "Background",                    inConfig.readEntry( "background", " " ) );
	if ( inConfig.hasKey( "selectForeground" ) )
		globalConfig.writeEntry( "HighlightedText",              inConfig.readEntry( "selectForeground", " " ) );
	if ( inConfig.hasKey( "selectBackground" ) )
		globalConfig.writeEntry( "Highlight",                     inConfig.readEntry( "selectBackground", " " ) );
	if ( inConfig.hasKey( "windowForeground" ) )
		globalConfig.writeEntry( "Text",              inConfig.readEntry( "windowForeground", " " ) );
	if ( inConfig.hasKey( "windowBackground" ) )
		globalConfig.writeEntry( "Base",              inConfig.readEntry( "windowBackground", " " ) );

	// Keep track of the current theme so that we can select the right one
	// in the KControl module.
	globalConfig.writeEntry ( "CurrentTheme", file );

	globalConfig.write();
#endif
}

OThemeBase::~OThemeBase()
{
	int i;
	for ( i = 0; i < WIDGETS; ++i ) {
		if ( !duplicate[ i ] ) {
			if ( images[ i ] )
				delete images[ i ];
			if ( pixmaps[ i ] )
				delete pixmaps[ i ];
		}
		if ( !pbDuplicate[ i ] && pbPixmaps[ i ] )
			delete pbPixmaps[ i ];
		if ( colors[ i ] )
			delete( colors[ i ] );
		if ( grLowColors[ i ] )
			delete( grLowColors[ i ] );
		if ( grHighColors[ i ] )
			delete( grHighColors[ i ] );
	}
	delete cache;
}

QImage* OThemeBase::loadImage( QString &name )
{
	QImage * image = new QImage;
	QString path = configFilePath + "/pixmaps/" + name;
	image->load( path );
	if ( !image->isNull() )
		return ( image );
	qDebug ( "OThemeBase: Unable to load image %s\n", name.ascii ( ) );
	delete image;
	return ( NULL );
}

OThemePixmap* OThemeBase::loadPixmap( QString &name )
{
	OThemePixmap * pixmap = new OThemePixmap( false );
	QString path = configFilePath + "/pixmaps/" + name;
	pixmap->load( path );
	if ( !pixmap->isNull() )
		return pixmap;
	qDebug ( "OThemeBase: Unable to load pixmap %s\n", name.ascii() );
	delete pixmap;
	return ( NULL );
}

OThemePixmap* OThemeBase::scale( int w, int h, WidgetType widget )
{
	if ( scaleHints[ widget ] == FullScale ) {
		if ( !pixmaps[ widget ] || pixmaps[ widget ] ->width() != w ||
		        pixmaps[ widget ] ->height() != h ) {
			OThemePixmap * cachePix = cache->pixmap( w, h, widget );
			if ( cachePix ) {
				cachePix = new OThemePixmap( *cachePix );
				if ( pixmaps[ widget ] )
					cache->insert( pixmaps[ widget ], OThemeCache::FullScale,
					               widget );
				else
					qDebug( "We would have inserted a null pixmap!\n" );
				pixmaps[ widget ] = cachePix;
			}
			else {
				cache->insert( pixmaps[ widget ], OThemeCache::FullScale, widget );
				QImage tmpImg = images[ widget ] ->smoothScale( w, h );
				pixmaps[ widget ] = new OThemePixmap;
				pixmaps[ widget ] ->convertFromImage( tmpImg );
				if ( blends[ widget ] != 0.0 )
					blend( widget );
			}
		}
	}
	else if ( scaleHints[ widget ] == HorizontalScale ) {
		if ( pixmaps[ widget ] ->width() != w ) {
			OThemePixmap * cachePix = cache->horizontalPixmap( w, widget );
			if ( cachePix ) {
				cachePix = new OThemePixmap( *cachePix );
				if ( pixmaps[ widget ] )
					cache->insert( pixmaps[ widget ], OThemeCache::HorizontalScale, widget );
				else
					qDebug ( "We would have inserted a null pixmap!\n" );
				pixmaps[ widget ] = cachePix;
			}
			else {
				cache->insert( pixmaps[ widget ], OThemeCache::HorizontalScale, widget );
				QImage tmpImg = images[ widget ] ->
				                smoothScale( w, images[ widget ] ->height() );
				pixmaps[ widget ] = new OThemePixmap;
				pixmaps[ widget ] ->convertFromImage( tmpImg );
				if ( blends[ widget ] != 0.0 )
					blend( widget );
			}
		}
	}
	else if ( scaleHints[ widget ] == VerticalScale ) {
		if ( pixmaps[ widget ] ->height() != h ) {
			OThemePixmap * cachePix = cache->verticalPixmap( w, widget );
			if ( cachePix ) {
				cachePix = new OThemePixmap( *cachePix );
				if ( pixmaps[ widget ] )
					cache->insert( pixmaps[ widget ], OThemeCache::VerticalScale, widget );
				else
					qDebug ( "We would have inserted a null pixmap!\n" );
				pixmaps[ widget ] = cachePix;
			}
			else {
				cache->insert( pixmaps[ widget ], OThemeCache::VerticalScale, widget );
				QImage tmpImg =
				    images[ widget ] ->smoothScale( images[ widget ] ->width(), h );
				pixmaps[ widget ] = new OThemePixmap;
				pixmaps[ widget ] ->convertFromImage( tmpImg );
				if ( blends[ widget ] != 0.0 )
					blend( widget );
			}
		}
	}
	// If blended tile here so the blend is scaled properly
	else if ( scaleHints[ widget ] == TileScale && blends[ widget ] != 0.0 ) {
		if ( !pixmaps[ widget ] || pixmaps[ widget ] ->width() != w ||
		        pixmaps[ widget ] ->height() != h ) {
			OThemePixmap * cachePix = cache->pixmap( w, h, widget );
			if ( cachePix ) {
				cachePix = new OThemePixmap( *cachePix );
				cache->insert( pixmaps[ widget ], OThemeCache::FullScale, widget );
				pixmaps[ widget ] = cachePix;
			}
			else {
				cache->insert( pixmaps[ widget ], OThemeCache::FullScale, widget );
				QPixmap tile;
				tile.convertFromImage( *images[ widget ] );
				pixmaps[ widget ] = new OThemePixmap;
				pixmaps[ widget ] ->resize( w, h );
				QPainter p( pixmaps[ widget ] );
				p.drawTiledPixmap( 0, 0, w, h, tile );
				if ( blends[ widget ] != 0.0 )
					blend( widget );
			}
		}
	}
	return ( pixmaps[ widget ] );
}

OThemePixmap* OThemeBase::scaleBorder( int w, int h, WidgetType widget )
{
	OThemePixmap * pixmap = NULL;
	if ( !pbPixmaps[ widget ] && !pbWidth[ widget ] )
		return ( NULL );
	pixmap = cache->pixmap( w, h, widget, true );
	if ( pixmap ) {
		pixmap = new OThemePixmap( *pixmap );
	}
	else {
		pixmap = new OThemePixmap();
		pixmap->resize( w, h );
		QBitmap mask;
		mask.resize( w, h );
		mask.fill( color0 );
		QPainter mPainter;
		mPainter.begin( &mask );

		QPixmap *tmp = borderPixmap( widget ) ->border( OThemePixmap::TopLeft );
		const QBitmap *srcMask = tmp->mask();
		int bdWidth = tmp->width();

		bitBlt( pixmap, 0, 0, tmp, 0, 0, bdWidth, bdWidth,
		        Qt::CopyROP, false );
		if ( srcMask )
			bitBlt( &mask, 0, 0, srcMask, 0, 0, bdWidth, bdWidth,
			        Qt::CopyROP, false );
		else
			mPainter.fillRect( 0, 0, bdWidth, bdWidth, color1 );


		tmp = borderPixmap( widget ) ->border( OThemePixmap::TopRight );
		srcMask = tmp->mask();
		bitBlt( pixmap, w - bdWidth, 0, tmp, 0, 0, bdWidth,
		        bdWidth, Qt::CopyROP, false );
		if ( srcMask )
			bitBlt( &mask, w - bdWidth, 0, srcMask, 0, 0, bdWidth,
			        bdWidth, Qt::CopyROP, false );
		else
			mPainter.fillRect( w - bdWidth, 0, bdWidth, bdWidth, color1 );

		tmp = borderPixmap( widget ) ->border( OThemePixmap::BottomLeft );
		srcMask = tmp->mask();
		bitBlt( pixmap, 0, h - bdWidth, tmp, 0, 0, bdWidth,
		        bdWidth, Qt::CopyROP, false );
		if ( srcMask )
			bitBlt( &mask, 0, h - bdWidth, srcMask, 0, 0, bdWidth,
			        bdWidth, Qt::CopyROP, false );
		else
			mPainter.fillRect( 0, h - bdWidth, bdWidth, bdWidth, color1 );

		tmp = borderPixmap( widget ) ->border( OThemePixmap::BottomRight );
		srcMask = tmp->mask();
		bitBlt( pixmap, w - bdWidth, h - bdWidth, tmp, 0, 0,
		        bdWidth, bdWidth, Qt::CopyROP, false );
		if ( srcMask )
			bitBlt( &mask, w - bdWidth, h - bdWidth, srcMask, 0, 0,
			        bdWidth, bdWidth, Qt::CopyROP, false );
		else
			mPainter.fillRect( w - bdWidth, h - bdWidth, bdWidth, bdWidth, color1 );

		QPainter p;
		p.begin( pixmap );
		if ( w - bdWidth * 2 > 0 ) {
			tmp = borderPixmap( widget ) ->border( OThemePixmap::Top );
			srcMask = tmp->mask();
			p.drawTiledPixmap( bdWidth, 0, w - bdWidth * 2, bdWidth, *tmp );
			if ( srcMask )
				mPainter.drawTiledPixmap( bdWidth, 0, w - bdWidth * 2, bdWidth, *srcMask );
			else
				mPainter.fillRect( bdWidth, 0, w - bdWidth * 2, bdWidth, color1 );

			tmp = borderPixmap( widget ) ->border( OThemePixmap::Bottom );
			srcMask = tmp->mask();
			p.drawTiledPixmap( bdWidth, h - bdWidth, w - bdWidth * 2, bdWidth,
			                   *tmp );
			if ( srcMask )
				mPainter.drawTiledPixmap( bdWidth, h - bdWidth, w - bdWidth * 2, bdWidth, *srcMask );
			else
				mPainter.fillRect( bdWidth, h - bdWidth, w - bdWidth * 2, bdWidth,
				                   color1 );
		}
		if ( h - bdWidth * 2 > 0 ) {
			tmp = borderPixmap( widget ) ->border( OThemePixmap::Left );
			srcMask = tmp->mask();
			p.drawTiledPixmap( 0, bdWidth, bdWidth, h - bdWidth * 2, *tmp );
			if ( srcMask )
				mPainter.drawTiledPixmap( 0, bdWidth, bdWidth, h - bdWidth * 2, *srcMask );
			else
				mPainter.fillRect( 0, bdWidth, bdWidth, h - bdWidth * 2, color1 );

			tmp = borderPixmap( widget ) ->border( OThemePixmap::Right );
			srcMask = tmp->mask();
			p.drawTiledPixmap( w - bdWidth, bdWidth, bdWidth, h - bdWidth * 2,
			                   *tmp );
			if ( srcMask )
				mPainter.drawTiledPixmap(  w - bdWidth, bdWidth,  bdWidth, h - bdWidth * 2, *srcMask );
			else
				mPainter.fillRect( w - bdWidth, bdWidth, bdWidth, h - bdWidth * 2, color1 );
		}
		p.end();
		mPainter.end();
		pixmap->setMask( mask );
		cache->insert( pixmap, OThemeCache::FullScale, widget, true );
		if ( !pixmap->mask() )
			qDebug ( "No mask for border pixmap!\n" );
	}
	return ( pixmap );
}


OThemePixmap* OThemeBase::blend( WidgetType widget )
{
	OGfxEffect::GradientType g;
	switch ( gradients[ widget ] ) {
		case GrHorizontal:
			g = OGfxEffect::HorizontalGradient;
			break;
		case GrVertical:
			g = OGfxEffect::VerticalGradient;
			break;
		case GrPyramid:
			g = OGfxEffect::PyramidGradient;
			break;
		case GrRectangle:
			g = OGfxEffect::RectangleGradient;
			break;
		case GrElliptic:
			g = OGfxEffect::EllipticGradient;
			break;
		default:
			g = OGfxEffect::DiagonalGradient;
			break;
	}
	OGfxEffect::blend( *pixmaps[ widget ], blends[ widget ], *grLowColors[ widget ],
	                   g, false );
	return ( pixmaps[ widget ] );
}

OThemePixmap* OThemeBase::gradient( int w, int h, WidgetType widget )
{
	if ( gradients[ widget ] == GrVertical ) {
		if ( !pixmaps[ widget ] || pixmaps[ widget ] ->height() != h ) {
			OThemePixmap * cachePix = cache->verticalPixmap( h, widget );
			if ( cachePix ) {
				cachePix = new OThemePixmap( *cachePix );
				if ( pixmaps[ widget ] )
					cache->insert( pixmaps[ widget ], OThemeCache::VerticalScale,
					               widget );
				pixmaps[ widget ] = cachePix;
			}
			else {
				if ( pixmaps[ widget ] )
					cache->insert( pixmaps[ widget ], OThemeCache::VerticalScale,
					               widget );
				pixmaps[ widget ] = new OThemePixmap;
				pixmaps[ widget ] ->resize( w, h );
				OGfxEffect::gradient( *pixmaps[ widget ], *grHighColors[ widget ],
				                      *grLowColors[ widget ],
				                      OGfxEffect::VerticalGradient );
			}
		}
	}
	else if ( gradients[ widget ] == GrHorizontal ) {
		if ( !pixmaps[ widget ] || pixmaps[ widget ] ->width() != w ) {
			OThemePixmap * cachePix = cache->horizontalPixmap( w, widget );
			if ( cachePix ) {
				cachePix = new OThemePixmap( *cachePix );
				if ( pixmaps[ widget ] )
					cache->insert( pixmaps[ widget ],
					               OThemeCache::HorizontalScale, widget );
				pixmaps[ widget ] = cachePix;
			}
			else {
				if ( pixmaps[ widget ] )
					cache->insert( pixmaps[ widget ],
					               OThemeCache::HorizontalScale, widget );
				pixmaps[ widget ] = new OThemePixmap;
				pixmaps[ widget ] ->resize( w, h );
				OGfxEffect::gradient( *pixmaps[ widget ], *grHighColors[ widget ],
				                      *grLowColors[ widget ],
				                      OGfxEffect::HorizontalGradient );
			}
		}
	}
	else if ( gradients[ widget ] == GrReverseBevel ) {
		if ( !pixmaps[ widget ] || pixmaps[ widget ] ->width() != w ||
		        pixmaps[ widget ] ->height() != h ) {
			OThemePixmap * cachePix = cache->pixmap( w, h, widget );
			if ( cachePix ) {
				cachePix = new OThemePixmap( *cachePix );
				if ( pixmaps[ widget ] )
					cache->insert( pixmaps[ widget ], OThemeCache::FullScale,
					               widget );
				pixmaps[ widget ] = cachePix;
			}
			else {
				if ( pixmaps[ widget ] )
					cache->insert( pixmaps[ widget ], OThemeCache::FullScale,
					               widget );
				pixmaps[ widget ] = new OThemePixmap;
				pixmaps[ widget ] ->resize( w, h );

				QPixmap s;
				int offset = decoWidth( widget );
				s.resize( w - offset * 2, h - offset * 2 );
				QColor lc( *grLowColors[ widget ] );
				QColor hc( *grHighColors[ widget ] );
				if ( bevelContrast( widget ) ) {
					int bc = bevelContrast( widget );
					// want single increments, not factors like light()/dark()
					lc.setRgb( lc.red() - bc, lc.green() - bc, lc.blue() - bc );
					hc.setRgb( hc.red() + bc, hc.green() + bc, hc.blue() + bc );
				}
				OGfxEffect::gradient( *pixmaps[ widget ],
				                      lc, hc,
				                      OGfxEffect::DiagonalGradient );
				OGfxEffect::gradient( s, *grHighColors[ widget ],
				                      *grLowColors[ widget ],
				                      OGfxEffect::DiagonalGradient );
				bitBlt( pixmaps[ widget ], offset, offset, &s, 0, 0, w - offset * 2,
				        h - offset * 2, Qt::CopyROP );
			}
		}
	}
	else {
		OGfxEffect::GradientType g;
		switch ( gradients[ widget ] ) {
			case GrPyramid:
				g = OGfxEffect::PyramidGradient;
				break;
			case GrRectangle:
				g = OGfxEffect::RectangleGradient;
				break;
			case GrElliptic:
				g = OGfxEffect::EllipticGradient;
				break;
			default:
				g = OGfxEffect::DiagonalGradient;
				break;
		}
		if ( !pixmaps[ widget ] || pixmaps[ widget ] ->width() != w ||
		        pixmaps[ widget ] ->height() != h ) {
			OThemePixmap * cachePix = cache->pixmap( w, h, widget );
			if ( cachePix ) {
				cachePix = new OThemePixmap( *cachePix );
				if ( pixmaps[ widget ] )
					cache->insert( pixmaps[ widget ], OThemeCache::FullScale,
					               widget );
				pixmaps[ widget ] = cachePix;
			}
			else {
				if ( pixmaps[ widget ] )
					cache->insert( pixmaps[ widget ], OThemeCache::FullScale,
					               widget );
				pixmaps[ widget ] = new OThemePixmap;
				pixmaps[ widget ] ->resize( w, h );
				OGfxEffect::gradient( *pixmaps[ widget ], *grHighColors[ widget ],
				                      *grLowColors[ widget ], g );
			}
		}
	}
	return ( pixmaps[ widget ] );
}

OThemePixmap* OThemeBase::scalePixmap( int w, int h, WidgetType widget )
{

	if ( gradients[ widget ] && blends[ widget ] == 0.0 )
		return ( gradient( w, h, widget ) );

	return ( scale( w, h, widget ) );
}

QColorGroup* OThemeBase::makeColorGroup( QColor &fg, QColor &bg,
        Qt::GUIStyle )
{
	if ( shading == Motif ) {
		int highlightVal, lowlightVal;
		highlightVal = 100 + ( 2* /*KGlobalSettings::contrast()*/ 3 + 4 ) * 16 / 10;
		lowlightVal = 100 + ( ( 2* /*KGlobalSettings::contrast()*/ 3 + 4 ) * 10 );
		return ( new QColorGroup( fg, bg, bg.light( highlightVal ),
		                          bg.dark( lowlightVal ), bg.dark( 120 ),
		                          fg, qApp->palette().normal().base() ) );
	}
	else
		return ( new QColorGroup( fg, bg, bg.light( 150 ), bg.dark(),
		                          bg.dark( 120 ), fg,
		                          qApp->palette().normal().base() ) );
}

static QColor strToColor ( const QString &str )
{
	QString str2 = str. stripWhiteSpace ( );
	
	if ( str2 [0] == '#' )
		return QColor ( str2 );
	else {
		QStringList sl = QStringList::split ( ',', str2 );
		
		if ( sl. count ( ) >= 3 )
			return QColor ( sl [0]. toInt ( ), sl [1]. toInt ( ), sl [2]. toInt ( ));
	}
	return QColor ( 0, 0, 0 );
}



void OThemeBase::applyMiscResourceGroup( Config *config )
{
	config-> setGroup ( "Misc" );
	QString tmpStr;

	tmpStr = config->readEntry( "SButtonPosition" );
	if ( tmpStr == "BottomLeft" )
		sbPlacement = SBBottomLeft;
	else if ( tmpStr == "BottomRight" )
		sbPlacement = SBBottomRight;
	else {
		if ( tmpStr != "Opposite" && !tmpStr.isEmpty() )
			qDebug ( "OThemeBase: Unrecognized sb button option %s, using Opposite.\n", tmpStr.ascii() );
		sbPlacement = SBOpposite;
	}
	tmpStr = config->readEntry( "ArrowType" );
	if ( tmpStr == "Small" )
		arrowStyle = SmallArrow;
	else if ( tmpStr == "3D" )
		arrowStyle = MotifArrow;
	else {
		if ( tmpStr != "Normal" && !tmpStr.isEmpty() )
			qDebug ( "OThemeBase: Unrecognized arrow option %s, using Normal.\n", tmpStr.ascii() );
		arrowStyle = LargeArrow;
	}
	tmpStr = config->readEntry( "ShadeStyle" );
	if ( tmpStr == "Motif" )
		shading = Motif;
	else if ( tmpStr == "Next" )
		shading = Next;
	else if ( tmpStr == "KDE" )
		shading = KDE;
	else
		shading = Windows;

	defaultFrame = config->readNumEntry( "FrameWidth", 2 );
	cacheSize = config->readNumEntry( "Cache", 1024 );
	sbExtent = config->readNumEntry( "ScrollBarExtent", 16 );

	config-> setGroup ( "General" );
	
	if ( config-> hasKey ( "foreground" ))        fgcolor    = strToColor ( config-> readEntry ( "foreground" ));
	if ( config-> hasKey ( "background" ))        bgcolor    = strToColor ( config-> readEntry ( "background" ));
	if ( config-> hasKey ( "selectForeground" ))  selfgcolor = strToColor ( config-> readEntry ( "selectForeground" ));
	if ( config-> hasKey ( "selectBackground" ))  selbgcolor = strToColor ( config-> readEntry ( "selectBackground" ));
	if ( config-> hasKey ( "windowForeground" ))  winfgcolor = strToColor ( config-> readEntry ( "windowForeground" ));
	if ( config-> hasKey ( "windowBackground" ))  winbgcolor = strToColor ( config-> readEntry ( "windowBackground" ));
}

void OThemeBase::readMiscResourceGroup()
{}

void OThemeBase::applyResourceGroup( Config *config, int i, QString *copyfrom, QString *pixnames, QString *brdnames )
{
	QString tmpStr;

	config-> setGroup ( widgetEntries [ i ] );

	tmpStr = config->readEntry( "CopyWidget", "" );
	copyfrom [ i ] = tmpStr;
	if ( !tmpStr.isEmpty() )
		return ;

	// Scale hint
	tmpStr = config->readEntry( "Scale" );
	if ( tmpStr == "Full" )
		scaleHints [ i ] = FullScale;
	else if ( tmpStr == "Horizontal" )
		scaleHints [ i ] = HorizontalScale;
	else if ( tmpStr == "Vertical" )
		scaleHints [ i ] = VerticalScale;
	else {
		if ( tmpStr != "Tile" && !tmpStr.isEmpty() )
			qDebug ( "OThemeBase: Unrecognized scale option %s, using Tile.\n", tmpStr.ascii() );
		scaleHints [ i ] = TileScale;
	}


	// Gradient type
	tmpStr = config->readEntry( "Gradient" );
	if ( tmpStr == "Diagonal" )
		gradients [ i ] = GrDiagonal;
	else if ( tmpStr == "Horizontal" )
		gradients [ i ] = GrHorizontal;
	else if ( tmpStr == "Vertical" )
		gradients [ i ] = GrVertical;
	else if ( tmpStr == "Pyramid" )
		gradients [ i ] = GrPyramid;
	else if ( tmpStr == "Rectangle" )
		gradients [ i ] = GrRectangle;
	else if ( tmpStr == "Elliptic" )
		gradients [ i ] = GrElliptic;
	else if ( tmpStr == "ReverseBevel" )
		gradients [ i ] = GrReverseBevel;
	else {
		if ( tmpStr != "None" && !tmpStr.isEmpty() )
			qDebug ( "OThemeBase: Unrecognized gradient option %s, using None.\n", tmpStr.ascii() );
		gradients [ i ] = GrNone;
	}

	// Blend intensity
	blends[ i ] = config->readEntry( "BlendIntensity", "0.0" ).toDouble();

	// Bevel contrast
	bContrasts[ i ] = config->readNumEntry( "BevelContrast", 0 );

	// Border width
	borders [ i ] = config->readNumEntry( "Border", 1 );

	// Highlight width
	highlights [ i ] = config->readNumEntry( "Highlight", 1 );

	// Gradient low color or blend background
	if ( config->hasKey( "GradientLow" ) && ( gradients[ i ] != GrNone || blends[ i ] != 0.0 ))
		grLowColors[ i ] = new QColor( strToColor ( config->readEntry( "GradientLow", qApp->palette().normal().background().name() )));
	else
		grLowColors[ i ] = NULL;
	

	// Gradient high color
	if ( config->hasKey( "GradientHigh" ) && ( gradients[ i ] != GrNone )) 
		grHighColors[ i ] = new QColor( strToColor ( config->readEntry( "GradientHigh", qApp->palette().normal().background().name() )));
	else
		grHighColors[ i ] = NULL;

	// Extended color attributes
	if ( config->hasKey( "Foreground" ) || config->hasKey( "Background" ) ) {
		QColor bg = strToColor( config->readEntry( "Background", qApp->palette().normal().background().name() ));
		QColor fg = strToColor( config->readEntry( "Foreground", qApp->palette().normal().foreground().name() ));

		colors[ i ] = makeColorGroup( fg, bg, Qt::WindowsStyle );
	}
	else
		colors[ i ] = NULL;

	// Pixmap
	tmpStr = config->readEntry( "Pixmap", "" );
	pixnames[ i ] = tmpStr;
	duplicate[ i ] = false;
	pixmaps[ i ] = NULL;
	images[ i ] = NULL;


	// Pixmap border
	tmpStr = config->readEntry( "PixmapBorder", "" );
	brdnames[ i ] = tmpStr;
	pbDuplicate[ i ] = false;
	pbPixmaps[ i ] = NULL;
	pbWidth[ i ] = 0;
	if ( !tmpStr.isEmpty() ) {
		pbWidth[ i ] = config->readNumEntry( "PixmapBWidth", 0 );
		if ( pbWidth[ i ] == 0 ) {
			qDebug ( "OThemeBase: No border width specified for pixmapped border widget %s\n", widgetEntries[ i ] );
			qDebug ( "OThemeBase: Using default of 2.\n" );
			pbWidth[ i ] = 2;
		}
	}


	// Various widget specific settings. This was more efficent when bunched
	// together in the misc group, but this makes an easier to read config.
	if ( i == SliderGroove )
		roundedSlider = config->readBoolEntry( "SmallGroove", false );
	else if ( i == ActiveTab ) {
		aTabLine = config->readBoolEntry( "BottomLine", true );
	}
	else if ( i == InactiveTab ) {
		iTabLine = config->readBoolEntry( "BottomLine", true );
	}
	else if ( i == Splitter )
		splitterWidth = config->readNumEntry( "Width", 10 );
	else if ( i == ComboBox || i == ComboBoxDown ) {
		roundedCombo = config->readBoolEntry( "Round", false );
	}
	else if ( i == PushButton || i == PushButtonDown ) {
		btnXShift = config->readNumEntry( "XShift", 0 );
		btnYShift = config->readNumEntry( "YShift", 0 );
		focus3D = config->readBoolEntry( "3DFocusRect", false );
		focus3DOffset = config->readBoolEntry( "3DFocusOffset", 0 );
		roundedButton = config->readBoolEntry( "Round", false );
	}
}


void OThemeBase::readResourceGroup( int i, QString *copyfrom, QString *pixnames, QString *brdnames,
                                    bool *loadArray )
{
	if ( loadArray[ i ] == true ) {
		return ; // already been preloaded.
	}

	int tmpVal;
	QString tmpStr;

	tmpStr = copyfrom [ i ];
	if ( !tmpStr.isEmpty() ) { // Duplicate another widget's config
		int sIndex;
		loadArray[ i ] = true;
		for ( sIndex = 0; sIndex < WIDGETS; ++sIndex ) {
			if ( tmpStr == widgetEntries[ sIndex ] ) {
				if ( !loadArray[ sIndex ] )  // hasn't been loaded yet
					readResourceGroup( sIndex, copyfrom, pixnames, brdnames,
					                   loadArray );
				break;
			}
		}
		if ( loadArray[ sIndex ] ) {
			copyWidgetConfig( sIndex, i, pixnames, brdnames );
		}
		else
			qDebug ( "OThemeBase: Unable to identify source widget for %s\n", widgetEntries[ i ] );
		return ;
	}
	// special inheritance for disabled arrows (these are tri-state unlike
	// the rest of what we handle).
	for ( tmpVal = DisArrowUp; tmpVal <= DisArrowRight; ++tmpVal ) {
		if ( tmpVal == i ) {
			tmpStr = pixnames [ i ];
			if ( tmpStr.isEmpty() ) {
				copyWidgetConfig( ArrowUp + ( tmpVal - DisArrowUp ), i, pixnames,
				                  brdnames );
				return ;
			}
		}
	}

	// Pixmap
	int existing;
	// Scan for duplicate pixmaps(two identical pixmaps, tile scale, no blend,
	// no pixmapped border)
	if ( !pixnames [ i ].isEmpty() ) {
		for ( existing = 0; existing < i; ++existing ) {
			if ( pixnames[ i ] == pixnames[ existing ] && scaleHints[ i ] == TileScale &&
			        scaleHints[ existing ] == TileScale && blends[ existing ] == 0.0 &&
			        blends[ i ] == 0.0 ) {
				pixmaps[ i ] = pixmaps[ existing ];
				duplicate[ i ] = true;
				break;
			}
		}
	}
	// load
	if ( !duplicate[ i ] && !pixnames[ i ].isEmpty() ) {
		pixmaps[ i ] = loadPixmap( pixnames[ i ] );
		// load and save images for scaled/blended widgets for speed.
		if ( scaleHints[ i ] == TileScale && blends[ i ] == 0.0 )
			images[ i ] = NULL;
		else
			images[ i ] = loadImage( pixnames[ i ] );
	}

	// Pixmap border
	if ( !brdnames [ i ]. isEmpty () ) {
		// duplicate check
		for ( existing = 0; existing < i; ++existing ) {
			if ( brdnames [i] == brdnames[ existing ] ) {
				pbPixmaps[ i ] = pbPixmaps[ existing ];
				pbDuplicate[ i ] = true;
				break;
			}
		}
	}
	// load
	if ( !pbDuplicate[ i ] && !brdnames[ i ].isEmpty() )
		pbPixmaps[ i ] = loadPixmap( brdnames[ i ] );

	if ( pbPixmaps[ i ] && !pbDuplicate[ i ] )
		generateBorderPix( i );

	loadArray[ i ] = true;
}


OThemePixmap::OThemePixmap( bool timer )
		: QPixmap()
{
	if(timer){
	   t = new QTime;
	    t->start();
	}
	else
	    t = NULL;
	int i;
	for ( i = 0; i < 8; ++i )
		b[ i ] = NULL;
}

OThemePixmap::OThemePixmap( const OThemePixmap &p )
		: QPixmap( p )
{
	if(p.t){
	    t = new QTime;
	    t->start();
	}
	else
	    t = NULL;
	int i;
	for ( i = 0; i < 8; ++i )
		if ( p.b[ i ] )
			b[ i ] = new QPixmap( *p.b[ i ] );
		else
			b[ i ] = NULL;
}



OThemePixmap::~OThemePixmap()
{
	if(t)
	    delete t;
	int i;
	for ( i = 0; i < 8; ++i )
		if ( b[ i ] )
			delete b[ i ];
}

OThemeCache::OThemeCache( int maxSize, QObject *parent, const char *name )
		: QObject( parent, name )
{
	cache.setMaxCost( maxSize * 1024 );
	cache.setAutoDelete( true );
	flushTimer.start(300000); // 5 minutes
	connect(&flushTimer, SIGNAL(timeout()), SLOT(flushTimeout()));
}

void OThemeCache::flushTimeout()
{
	QIntCacheIterator<OThemePixmap> it( cache );
	while ( it.current() ) {
		if ( it.current() ->isOld() )
			cache.remove( it.currentKey() );
		else
			++it;
	}
}

OThemePixmap* OThemeCache::pixmap( int w, int h, int widgetID, bool border,
                                   bool mask )
{

	kthemeKey key;
	key.cacheKey = 0; // shut up, gcc
	key.data.id = widgetID;
	key.data.width = w;
	key.data.height = h;
	key.data.border = border;
	key.data.mask = mask;

	OThemePixmap *pix = cache.find( ( unsigned long ) key.cacheKey );
	if ( pix )
		pix->updateAccessed();
	return ( pix );
}

OThemePixmap* OThemeCache::horizontalPixmap( int w, int widgetID )
{
	kthemeKey key;
	key.cacheKey = 0; // shut up, gcc
	key.data.id = widgetID;
	key.data.width = w;
	key.data.height = 0;
	key.data.border = false;
	key.data.mask = false;
	OThemePixmap *pix = cache.find( ( unsigned long ) key.cacheKey );
	if ( pix )
		pix->updateAccessed();
	return ( pix );
}

OThemePixmap* OThemeCache::verticalPixmap( int h, int widgetID )
{
	kthemeKey key;
	key.cacheKey = 0; // shut up, gcc
	key.data.id = widgetID;
	key.data.width = 0;
	key.data.height = h;
	key.data.border = false;
	key.data.mask = false;
	OThemePixmap *pix = cache.find( ( unsigned long ) key.cacheKey );
	if ( pix )
		pix->updateAccessed();
	return ( pix );
}

bool OThemeCache::insert( OThemePixmap *pixmap, ScaleHint scale, int widgetID,
                          bool border, bool mask )
{
	kthemeKey key;
	key.cacheKey = 0; // shut up, gcc
	key.data.id = widgetID;
	key.data.width = ( scale == FullScale || scale == HorizontalScale ) ?
	                 pixmap->width() : 0;
	key.data.height = ( scale == FullScale || scale == VerticalScale ) ?
	                  pixmap->height() : 0;
	key.data.border = border;
	key.data.mask = mask;

	if ( cache.find( ( unsigned long ) key.cacheKey, true ) != NULL ) {
		return ( true ); // a pixmap of this scale is already in there
	}
	return ( cache.insert( ( unsigned long ) key.cacheKey, pixmap,
	                       pixmap->width() * pixmap->height() * pixmap->depth() / 8 ) );
}

//#include "kthemebase.moc"
