#include <stdlib.h>

#include <qapplication.h>
#include <qlistview.h>
#include <qpe/applnk.h>
#include <qpe/mimetype.h>
#include <qpe/resource.h>

#include "buttonutils.h"

using namespace Opie;

struct predef_qcop {
	const char *m_text;
	const char *m_pixmap;
	const char *m_channel;
	const char *m_function;
};

static const predef_qcop predef [] = {

	{ QT_TRANSLATE_NOOP( "ButtonSettings", "Beam VCard" ),       "beam",                  "QPE/Application/addressbook", "beamBusinessCard()" },
	{ QT_TRANSLATE_NOOP( "ButtonSettings", "Send eMail" ),       "buttonsettings/mail",   "QPE/Application/mail",        "newMail()"          },
        

	{ QT_TRANSLATE_NOOP( "ButtonSettings", "Toggle Menu" ),      "buttonsettings/menu",   "QPE/TaskBar",  "toggleMenu()"      },
	{ QT_TRANSLATE_NOOP( "ButtonSettings", "Toggle O-Menu" ),    "buttonsettings/omenu",  "QPE/TaskBar",  "toggleStartMenu()" },
	{ QT_TRANSLATE_NOOP( "ButtonSettings", "Show Desktop" ),     "home",                  "QPE/Launcher", "home()"            },
	{ QT_TRANSLATE_NOOP( "ButtonSettings", "Toggle Recording" ), "buttonsettings/record", "QPE/VMemo",    "toggleRecord()"    },
       
    { 0, 0, 0, 0 }                                                          
};




ButtonUtils *ButtonUtils::ButtonUtils::inst ( )
{
	static ButtonUtils *p = 0;
	
	if ( !p ) {
		p = new ButtonUtils ( );
		::atexit ( cleanup );
	}
	return p;
}

void ButtonUtils::cleanup ( )
{
	delete inst ( );
}

ButtonUtils::ButtonUtils ( )
{
	m_apps = new AppLnkSet( MimeType::appsFolderName ( ));
}

ButtonUtils::~ButtonUtils ( )
{
	delete m_apps;
}

qCopInfo ButtonUtils::messageToInfo ( const OQCopMessage &c )
{
	QCString ch = c. channel ( );
	QCString f  = c. message ( ); 

	if ( ch. isNull ( ))
		return qCopInfo ( qApp-> translate ( "ButtonSettings", "<nobr>Ignored</nobr>" ));
	
	for ( const predef_qcop *p = predef; p-> m_text; p++ ) {
		if (( ch == p-> m_channel ) && ( f == p-> m_function ))
			return qCopInfo ( qApp-> translate ( "ButtonSettings", p-> m_text ), Resource::loadPixmap ( p-> m_pixmap ));
	}	
	
	if ( ch. left ( 16 ) == "QPE/Application/" ) {
		QString app = ch. mid ( 16 );
		const AppLnk *applnk = m_apps-> findExec ( app );
		if ( applnk )
			app = applnk-> name ( );
			
		if (( f == "raise()" ) || ( f == "nextView()" ))
			return qCopInfo ( qApp-> translate ( "ButtonSettings", "<nobr>Show <b>%1</b></nobr>" ). arg ( app ), applnk ? applnk-> pixmap ( ) : QPixmap ( ));
		else
			return qCopInfo ( qApp-> translate ( "ButtonSettings", "<nobr>Call <b>%1</b>: <i>%2</i></nobr>" ). arg ( app ). arg ( f ), applnk ? applnk-> pixmap ( ) : QPixmap ( ));
	}
	else {
		return qCopInfo ( qApp-> translate ( "ButtonSettings", "<nobr>Call <b>%1</b> <i>%2</i></nobr>" ). arg (( ch. left ( 4 ) == "QPE/" ) ? ch. mid ( 4 ) : ch ). arg ( f ));
	}
}


void ButtonUtils::insertActions ( QListViewItem *here )
{
	for ( const predef_qcop *p = predef; p-> m_text; p++ ) {
		QListViewItem *item = new QListViewItem ( here, qApp-> translate ( "ButtonSettings", p-> m_text ), p-> m_channel, p-> m_function );
		item-> setPixmap ( 0, Resource::loadPixmap ( p-> m_pixmap ));
	}		
}


void ButtonUtils::insertAppLnks ( QListViewItem *here )
{
	QStringList types = m_apps-> types ( );   
	QListViewItem *typeitem [types. count ( )];
   
   	int i = 0;
	for ( QStringList::Iterator it = types. begin ( ); it != types. end ( ); ++it ) {
		QListViewItem *item = new QListViewItem ( here, m_apps-> typeName ( *it ));
		item-> setPixmap ( 0, m_apps-> typePixmap ( *it ));
	
		typeitem [i++] = item;	
	}	
	
	for ( QListIterator <AppLnk> appit ( m_apps-> children ( )); *appit; ++appit ) {
		AppLnk *l = *appit;
	
		int i = 0;
		for ( QStringList::Iterator it = types. begin ( ); it != types. end ( ); ++it ) {		
			if ( l-> type ( ) == *it ) {
				QListViewItem *sub = new QListViewItem ( typeitem [i], l-> name ( ), QString ( "QPE/Application/" ) + l-> exec ( ), "raise()" );
				sub-> setPixmap ( 0, l-> pixmap ( ));
			}
			i++;
		}	
	}                                  
}
