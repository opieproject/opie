
#include <qpe/qpeapplication.h>
#include <qwsdecoration_qws.h>
#include <qcommonstyle.h>
#include <qfontdatabase.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "qt_override_p.h"



struct color_fix_t {
	char *m_app;
	char *m_class;
	char *m_name;
	QColorGroup::ColorRole m_set;
	QColorGroup::ColorRole m_get;
};



static const color_fix_t apps_that_need_special_colors [] = {
	{ "HancomMobileWord", "HTextEdit", 0, QColorGroup::Background, QColorGroup::Base },
	{ "neocal", "Display", 0, QColorGroup::Background, QColorGroup::Base },
	
	{ 0, 0, 0, QColorGroup::Base, QColorGroup::Base }
};

static const char * const apps_that_need_pointsizes_times_10 [] = {
	"HancomMobileWord",
	
	0
};




int Opie::force_appearance = 0;


// Return the *real* name of the binary - not just a quick guess
// by looking at argv [0] (which could be anything)

static void binaryNameFree ( )
{
	::free ((void *) Opie::binaryName ( )); // we need to cast away the const here
}

const char *Opie::binaryName ( )
{
	static const char *appname = 0;
	
	if ( !appname ) {
		char dst [PATH_MAX + 1];
		int l = ::readlink ( "/proc/self/exe", dst, PATH_MAX );
			
		if ( l <= 0 ) 
			l = 0;
			
		dst [l] = 0;
		const char *b = ::strrchr ( dst, '/' );
		appname = ::strdup ( b ? b + 1 : dst );
		
		::atexit ( binaryNameFree );
	}	
	return appname;
}


// Fix for a toolchain incompatibility (binaries compiled with 
// old tcs using shared libs compiled with newer tcs)

extern "C" {

extern void __gmon_start__ ( ) __attribute__(( weak ));

extern void __gmon_start__ ( ) 
{
}

}


// Fix for apps, that use QPainter::eraseRect() which doesn't work with styles
// that set a background pixmap (it would be easier to fix eraseRect(), but 
// TT made it an inline ...)

void QPEApplication::polish ( QWidget *w )
{
	qDebug ( "QPEApplication::polish()" );

	for ( const color_fix_t *ptr = apps_that_need_special_colors; ptr-> m_app; ptr++ ) {
		if (( ::strcmp ( Opie::binaryName ( ), ptr-> m_app ) == 0 ) &&
		    ( ptr-> m_class ? w-> inherits ( ptr-> m_class ) : true ) &&
		    ( ptr-> m_name ? ( ::strcmp ( w-> name ( ), ptr-> m_name ) == 0 ) : true )) {
			QPalette pal = w-> palette ( );
			pal. setColor ( ptr-> m_set, pal. color ( QPalette::Active, ptr-> m_get ));
			w-> setPalette ( pal );
		}
	}
	QApplication::polish ( w );
}


// Fix for the binary incompatibility that TT introduced in Qt/E 2.3.4 -- point sizes
// were multiplied by 10 (which was incorrect)

QValueList <int> QFontDatabase::pointSizes ( QString const &family, QString const &style, QString const &charset )
{
	qDebug ( "QFontDatabase::pointSizes()" );

	QValueList <int> sl = pointSizes_NonWeak ( family, style, charset );

	for ( const char * const *ptr = apps_that_need_pointsizes_times_10; *ptr; ptr++ ) {
		if ( ::strcmp ( Opie::binaryName ( ), *ptr ) == 0 ) {
			for ( QValueList <int>::Iterator it = sl. begin ( ); it != sl. end ( ); ++it ) 
				*it *= 10;	
		}
	}
	return sl;
}


// Various style/font/color related overrides for weak symbols in Qt/E,
// which allows us to force the usage of the global Opie appearance.

void QApplication::setStyle ( QStyle *style )
{
	qDebug ( "QApplication::setStyle()" );

	if ( Opie::force_appearance & Opie::Force_Style )
		delete style;
	else
		QApplication::setStyle_NonWeak ( style );
}

void QApplication::setPalette ( const QPalette &pal, bool informWidgets, const char *className )
{
	qDebug ( "QApplication::setPalette()" );

	if (!( Opie::force_appearance & Opie::Force_Style ))
		QApplication::setPalette_NonWeak ( pal, informWidgets, className );
}

void QApplication::setFont ( const QFont &fnt, bool informWidgets, const char *className )
{
	qDebug ( "QApplication::setFont()" );

	if (!( Opie::force_appearance & Opie::Force_Font ))
		QApplication::setFont_NonWeak ( fnt, informWidgets, className );
}
	

void QApplication::qwsSetDecoration ( QWSDecoration *deco )
{
	qDebug ( "QApplication::qwsSetDecoration()" );

	if ( Opie::force_appearance & Opie::Force_Decoration )
		delete deco;
	else
		QApplication::qwsSetDecoration_NonWeak ( deco );
}

