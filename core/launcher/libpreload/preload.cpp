
#define _GNU_SOURCE
//#define private public
//#define protected public

#include <qapplication.h>
#include <qwsdecoration_qws.h>
#include <qcommonstyle.h>

//#include <qapplication.h>
//#include <qfont.h>

//class QStyle;
//class QApplication;
class QFont;
//class QWSDecoration;
class QFontDatabase;

#include <dlfcn.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


typedef void ( *qapp_setstyle_t ) ( QStyle * );
typedef void ( *qapp_setdeco_t ) ( QWSDecoration * ); 
typedef void ( *qapp_setfont_t ) ( const QFont &, bool, const char * ); 
typedef void ( *qapp_polish_t ) ( QApplication *, QWidget * );
typedef QValueList <int> ( *qfdb_pointsizes_t ) ( QFontDatabase *, QString const &, QString const &, QString const & );


static int *opie_block_style_p = 0;

extern "C" {

extern void __gmon_start ( ) __attribute(( weak ));

extern void __gmon_start__ ( ) 
{
}

static void *resolve_symbol ( const char *sym )
{
	void *adr = ::dlsym ( RTLD_NEXT, sym );

	if ( !adr )
		::fprintf ( stderr, "PANIC: Could not resolve symbol \"%s\"\n", sym );
		
	return adr;
}

void polish__12QApplicationP7QWidget ( QApplication *that, QWidget *w )
{
	static qapp_polish_t qpolish = 0;

	if ( !qpolish )
		qpolish = (qapp_polish_t) resolve_symbol ( "polish__12QApplicationP7QWidget" );

	if ( qpolish )
		( *qpolish ) ( that, w );

	if (( ::strcmp ( qApp-> argv ( ) [0], "HancomMobileWord" ) == 0 ) && ( w-> inherits ( "HTextEdit" ))) {
		QPalette pal = w-> palette ( );
		pal. setColor ( QColorGroup::Background, pal. color ( QPalette::Active, QColorGroup::Base ));
		w-> setPalette ( pal );
		// w-> setPalette ( w-> palette ( ). active ( ). color ( QColorGroup::Base ));
	}
}

extern QValueList <int> pointSizes__13QFontDatabaseRC7QStringN21 ( QFontDatabase *that, QString const &family, QString const &style, QString const &charset )
{
	static qfdb_pointsizes_t qpointsizes = 0;

	if ( !qpointsizes )
		qpointsizes = (qfdb_pointsizes_t) resolve_symbol ( "pointSizes__13QFontDatabaseRC7QStringN21" );

	QValueList <int> sl;

	if ( qpointsizes ) {
		sl = ( *qpointsizes ) ( that, family, style, charset );
		
		if ( ::strcmp ( qApp-> argv ( ) [0], "HancomMobileWord" ) == 0 ) {
			for ( QValueList <int>::Iterator it = sl. begin ( ); it != sl. end ( ); ++it ) 
				*it *= 10;	
		}		
	}
	
	return sl;
}

extern void setStyle__12QApplicationP6QStyle ( QStyle *style )
{
	static qapp_setstyle_t qsetstyle = 0;

	if ( !qsetstyle )
		qsetstyle = (qapp_setstyle_t) resolve_symbol ( "setStyle__12QApplicationP6QStyle" );

	if ( !opie_block_style_p )
		opie_block_style_p = (int *) resolve_symbol ( "opie_block_style" );
	
	if ( !qsetstyle || ( opie_block_style_p && ( *opie_block_style_p & 0x01 )))
		delete style;
	else
		( *qsetstyle ) ( style );
}

extern void setFont__12QApplicationRC5QFontbPCc ( const QFont &fnt, bool informWidgets, const char * className )
{
	static qapp_setfont_t qsetfont = 0;

	if ( !qsetfont )
		qsetfont = (qapp_setfont_t) resolve_symbol ( "setFont__12QApplicationRC5QFontbPCc" );

	if ( !opie_block_style_p )
		opie_block_style_p = (int *) resolve_symbol ( "opie_block_style" );
	
	if ( qsetfont && !( opie_block_style_p && ( *opie_block_style_p & 0x02 )))
		( *qsetfont ) ( fnt, informWidgets, className );
}
	

extern void qwsSetDecoration__12QApplicationP13QWSDecoration ( QWSDecoration *deco )
{
	static qapp_setdeco_t qsetdeco = 0;

	if ( !qsetdeco )
		qsetdeco = (qapp_setdeco_t) resolve_symbol ( "qwsSetDecoration__12QApplicationP13QWSDecoration" );

	if ( !opie_block_style_p )
		opie_block_style_p = (int *) resolve_symbol ( "opie_block_style" );
	
	if ( !qsetdeco || ( opie_block_style_p && ( *opie_block_style_p & 0x04 )))
		delete deco;
	else
		( *qsetdeco ) ( deco );
}

}
