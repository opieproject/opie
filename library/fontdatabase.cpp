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

#include "qpeapplication.h"
#include "fontfactoryinterface.h"
#include "fontdatabase.h"

#include <qpe/qlibrary.h>

#include <qfontmanager_qws.h>
#include <qdir.h>
#include <qdict.h>
#include <stdio.h>
#include <stdlib.h>

static QString fontDir()
{
    QString qtdir = getenv("QTDIR");
    if ( qtdir.isEmpty() ) qtdir = "/usr/local/qt-embedded";
    return qtdir+"/lib/fonts/";
}

#ifdef QT_NO_FONTDATABASE
static QString fontFamily( const QString& key )
{
    int u0 = key.find('_');
    int u1 = key.find('_',u0+1);
    int u2 = key.find('_',u1+1);
    QString family = key.left(u0);
    //int pointSize = key.mid(u0+1,u1-u0-1).toInt();
    //int weight = key.mid(u1+1,u2-u1-1).toInt();
    //bool italic = key.mid(u2-1,1) == "i";
    // #### ignores _t and _I fields
    return family;
}
#endif

QValueList<FontFactory> *FontDatabase::factoryList = 0;

/*!
  \class FontDatabase fontdatabase.h
  \brief The FontDatabase class provides information about available fonts.

  Provides information about available fonts. 

  FontDatabase provides information about the available fonts of the
  underlying window system. 

  Most often you will simply want to query the database for all font
  families(), and their respective pointSizes(), styles() and charSets().

  Use FontDatabase rather than QFontDatabase when you may need access to
  fonts not normally available.  For example, if the freetype library and
  Qtopia freetype plugin are installed TrueType fonts will be available
  to your application.  Font renderer plugins have greater resource
  requirements than the system fonts so they should be used only when
  necessary.
*/

/*!
  Constructs the FontDatabase class.
*/
FontDatabase::FontDatabase()
#ifndef QT_NO_FONTDATABASE
    : QFontDatabase()
#endif
{
    if ( !factoryList )
	loadRenderers();
}

/*!
  Returns a list of names of all available font families.
*/
QStringList FontDatabase::families() const
{
#ifndef QT_NO_FONTDATABASE
    return QFontDatabase::families();
#else
    QStringList list;
    QDict<void> familyDict;
    QDiskFont *qdf;
    for ( qdf=qt_fontmanager->diskfonts.first(); qdf!=0;
            qdf=qt_fontmanager->diskfonts.next()) {
	QString familyname = qdf->name;
	if ( !familyDict.find( familyname ) ) {
	    familyDict.insert( familyname, (void *)1 );
	    list.append( familyname );
	}
    }

    QDir dir(fontDir(),"*.qpf");
    for (int i=0; i<(int)dir.count(); i++) {
	QString familyname = fontFamily(dir[i]);
	if ( !familyDict.find( familyname ) ) {
	    familyDict.insert( familyname, (void *)1 );
	    list.append( familyname );
	}
    }

    return list;
#endif
}

#ifdef QT_NO_FONTDATABASE
/*!
  Returns a list of standard fontsizes.
*/
QValueList<int> FontDatabase::standardSizes()
{
    static int s[]={ 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28,
		     36, 48, 72, 0 };
    static bool first = TRUE;
    static QValueList<int> sList;
    if ( first ) {
	first = FALSE;
	int i = 0;
	while( s[i] )
	    sList.append( s[i++] );
    }
    return sList;
}

#endif

/*!
  Load any font renderer plugins that are available and make fonts that
  the plugins can read available.
*/
void FontDatabase::loadRenderers()
{
#ifndef QT_NO_COMPONENT
    if ( !factoryList )
	factoryList = new QValueList<FontFactory>;

    QValueList<FontFactory>::Iterator mit;
    for ( mit = factoryList->begin(); mit != factoryList->end(); ++mit ) {
	qt_fontmanager->factories.setAutoDelete( false );
	qt_fontmanager->factories.removeRef( (*mit).factory );
	qt_fontmanager->factories.setAutoDelete( true );
	(*mit).interface->release();
	(*mit).library->unload();
	delete (*mit).library;
    }
    factoryList->clear();

    QString path = QPEApplication::qpeDir() + "/plugins/fontfactories";
    QDir dir( path, "lib*.so" );
    QStringList list = dir.entryList();
    QStringList::Iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
	FontFactoryInterface *iface = 0;
	QLibrary *lib = new QLibrary( path + "/" + *it );
	if ( lib->queryInterface( IID_FontFactory, (QUnknownInterface**)&iface ) == QS_OK ) {
	    FontFactory factory;
	    factory.library = lib;
	    factory.interface = iface;
	    factory.factory = factory.interface->fontFactory();
	    factoryList->append( factory );
	    qt_fontmanager->factories.append( factory.factory );
	    readFonts( factory.factory );
	} else {
	    delete lib;
	}
    }
#endif
}

/*!
  \internal
*/
void FontDatabase::readFonts( QFontFactory *factory )
{

    // Load in font definition file
    QString fn = fontDir() + "fontdir";
    FILE* fontdef=fopen(fn.local8Bit(),"r");
    if(!fontdef) {
	QCString temp=fn.local8Bit();
	qWarning("Cannot find font definition file %s - is $QTDIR set correctly?",
	       temp.data());
	return;
    }
    char buf[200]="";
    char name[200]="";
    char render[200]="";
    char file[200]="";
    char flags[200]="";
    char isitalic[10]="";
    fgets(buf,200,fontdef);
    while(!feof(fontdef)) {
	if ( buf[0] != '#' ) {
	    int weight=50;
	    int size=0;
	    flags[0]=0;
	    sscanf(buf,"%s %s %s %s %d %d %s",name,file,render,isitalic,&weight,&size,flags);
	    QString filename;
	    if ( file[0] != '/' )
		filename = fontDir();
	    filename += file;
	    if ( QFile::exists(filename) ) {
		if( factory->name() == render ) {
		    QDiskFont * qdf=new QDiskFont(factory,name,isitalic[0]=='y',
						  weight,size,flags,filename);
		    qt_fontmanager->diskfonts.append(qdf);
#ifndef QT_NO_FONTDATABASE
#if QT_VERSION >= 232
		    QFontDatabase::qwsAddDiskFont( qdf );
#endif
#endif
		}
	    }
	}
	fgets(buf,200,fontdef);
    }
    fclose(fontdef);
}

