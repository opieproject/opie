
#define _GNU_SOURCE
//#define private public
//#define protected public

#include <qwsdecoration_qws.h>
#include <qcommonstyle.h>
//#include <qapplication.h>
//#include <qfont.h>

//class QStyle;
class QApplication;
class QFont;
//class QWSDecoration;

#include <dlfcn.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


typedef void ( *qapp_setstyle_t ) ( QStyle * );
typedef void ( *qapp_setdeco_t ) ( QApplication *, QWSDecoration * ); 
typedef void ( *qapp_setfont_t ) ( const QFont &, bool, const char * ); 


static bool *opie_block_style_p = 0;

extern "C" {

static void *resolve_symbol ( const char *sym )
{
	void *adr = ::dlsym ( RTLD_NEXT, sym );

	if ( !adr )
		::fprintf ( stderr, "PANIC: Could not resolve symbol \"%s\"\n", sym );
		
	return adr;
}

extern void setStyle__12QApplicationP6QStyle ( QStyle *style )
{
	static qapp_setstyle_t qsetstyle = 0;

	if ( !qsetstyle )
		qsetstyle = (qapp_setstyle_t) resolve_symbol ( "setStyle__12QApplicationP6QStyle" );

	if ( !opie_block_style_p )
		opie_block_style_p = (bool *) resolve_symbol ( "opie_block_style" );
	
	if ( !qsetstyle || ( opie_block_style_p && *opie_block_style_p )) {
		delete style;
		return;
	}
	else
		( *qsetstyle ) ( style );
}

extern void setFont__12QApplicationRC5QFontbPCc ( const QFont &fnt, bool informWidgets, const char * className )
{
	static qapp_setfont_t qsetfont = 0;

	if ( !qsetfont )
		qsetfont = (qapp_setfont_t) resolve_symbol ( "setFont__12QApplicationRC5QFontbPCc" );

	if ( !opie_block_style_p )
		opie_block_style_p = (bool *) resolve_symbol ( "opie_block_style" );
	
	if ( qsetfont && !( opie_block_style_p && *opie_block_style_p ))
		( *qsetfont ) ( fnt, informWidgets, className );
}
	

extern void qwsSetDecoration__12QApplicationP13QWSDecoration ( QApplication *app, QWSDecoration *deco )
{
	static qapp_setdeco_t qsetdeco = 0;

	if ( !qsetdeco )
		qsetdeco = (qapp_setdeco_t) resolve_symbol ( "qwsSetDecoration__12QApplicationP13QWSDecoration" );

	if ( !opie_block_style_p )
		opie_block_style_p = (bool *) resolve_symbol ( "opie_block_style" );
	
	if ( !qsetdeco || ( opie_block_style_p && *opie_block_style_p )) {
		delete deco;
		return;
	}
	else
		( *qsetdeco ) ( app, deco );
}

}
