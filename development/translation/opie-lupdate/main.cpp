/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
** Copyright (C) 2003 zecke
**
** This file is part of Qt Linguist.
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

#include <metatranslator.h>
#include <proparser.h>
#include <opie.h>

#include <qfile.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include <errno.h>
#include <string.h>

// defined in fetchtr.cpp
extern void fetchtr_cpp( const char *fileName, MetaTranslator *tor,
			 const char *defaultContext, bool mustExist );
extern void fetchtr_ui( const char *fileName, MetaTranslator *tor,
			const char *defaultContext, bool mustExist );

// defined in merge.cpp
extern void merge( MetaTranslator *tor, const MetaTranslator *virginTor,
		   bool verbose );

typedef QValueList<MetaTranslatorMessage> TML;

static const char* LUPDATE_VERSION = "0.1";

static void printUsage()
{
    fprintf( stderr, "Usage:\n"
	     "    opie-lupdate [options] project-file\n"
	     "    opie-lupdate [options] source-files -ts ts-files\n"
	     "Options:\n"
	     "    -opie  The OPIE base dir if not supplied $OPIEDIR will be taken\n"
	     "    -help  Display this information and exit\n"
	     "    -noobsolete\n"
	     "           Drop all obsolete strings\n"
	     "    -verbose\n"
	     "           Explain what is being done\n"
	     "    -version\n"
	     "           Display the version of lupdate and exit\n" );
}

static void updateTsFiles( const MetaTranslator& fetchedTor,
                           const QString& opiedir,
                           const QStringList& languages,
                           const QString& basename,
			   const QString& codec,
			   bool noObsolete, bool verbose )
{
    QStringList::ConstIterator it = languages.begin();
    for ( ; it != languages.end(); ++it ) {
        QString fileName = opiedir + "/i18n/" + (*it) + "/" + basename;
	MetaTranslator tor;
	tor.load( fileName );
	if ( !codec.isEmpty() )
	    tor.setCodec( codec );
	if ( verbose )
	    fprintf( stderr, "Updating '%s'...\n", fileName.latin1() );
	merge( &tor, &fetchedTor, verbose );
	if ( noObsolete )
	    tor.stripObsoleteMessages();
	tor.stripEmptyContexts();
	if ( !tor.save(fileName) )
	    fprintf( stderr, "lupdate error: Cannot save '%s': %s\n",
		     fileName.latin1(), strerror(errno) );
    }
}

int main( int argc, char **argv )
{
    QString defaultContext = "@default";
    MetaTranslator fetchedTor;
    QCString codec;
    QStringList tsFileNames;
    QString opiedir;
    QString translationBase;
    QString target;

    bool verbose = FALSE;
    bool noObsolete = FALSE;
    bool metSomething = FALSE;
    bool isLib = FALSE;
    int numFiles = 0;

    int i;

    QStringList languageList = OPIE::self()->languageList(opiedir);

    for ( i = 1; i < argc; i++ ) {
	if ( qstrcmp(argv[i], "-help") == 0 ) {
	    printUsage();
	    return 0;
	} else if ( qstrcmp(argv[i], "-noobsolete") == 0 ) {
	    noObsolete = TRUE;
	    continue;
	} else if ( qstrcmp(argv[i], "-verbose") == 0 ) {
	    verbose = TRUE;
	    continue;
	} else if ( qstrcmp(argv[i], "-version") == 0 ) {
	    fprintf( stderr, "lupdate version %s\n", LUPDATE_VERSION );
	    return 0;
	} else if ( qstrcmp(argv[i], "-opie") == 0 ) {
	    if( i+1 < argc ) {
        	opiedir = argv[i+1];
                languageList = OPIE::self()->languageList(opiedir);
            }
	    i++; // UGLY but we want to skip the next argument
	    continue;
	}

	numFiles++;

	QString fullText;

        QFile f( argv[i] );
        if ( !f.open(IO_ReadOnly) ) {
            fprintf( stderr, "lupdate error: Cannot open file '%s': %s\n",
                     argv[i], strerror(errno) );
            return 1;
        }

        QTextStream t( &f );
        fullText = t.read();
        f.close();

        fetchedTor = MetaTranslator();
        codec.truncate( 0 );
        tsFileNames.clear();
        isLib = FALSE;

        QMap<QString, QString> tagMap = proFileTagMap( fullText );
        QMap<QString, QString>::Iterator it;

        for ( it = tagMap.begin(); it != tagMap.end(); ++it ) {
            QStringList toks = QStringList::split( ' ', it.data() );
            QStringList::Iterator t;

            for ( t = toks.begin(); t != toks.end(); ++t ) {
                if ( it.key() == "HEADERS" || it.key() == "SOURCES" ) {
                    fetchtr_cpp( *t, &fetchedTor, defaultContext, TRUE );
                    metSomething = TRUE;
                } else if ( it.key() == "INTERFACES" ||
                            it.key() == "FORMS" ) {
                    fetchtr_ui( *t, &fetchedTor, defaultContext, TRUE );
                    fetchtr_cpp( *t + ".h", &fetchedTor, defaultContext,
                                 FALSE );
                    metSomething = TRUE;
                } else if ( it.key() == "TRANSLATIONS" ) {
		    // we do not care for that attribute anymore
                    //tsFileNames.append( *t );
                    metSomething = TRUE;
                } else if ( it.key() == "CODEC" ) {
                    codec = (*t).latin1();
                } else if ( it.key() == "TARGET" ) {
                    target = *t;
                    metSomething = TRUE;
                } else if ( it.key() == "TEMPLATE" ) {
                    if ( (*t).stripWhiteSpace().lower() == "lib" )
                        isLib = true;
                }
            }
        }
        /**
         * We know the $OPIEDIR or have opiedir
         * we've a list of languages (de,en,gb,foo,bar)
         * we've got the TARGET and we no it's the lib
         * so let's do that
         * $OPIEDIR/language[i]/ifLibAppendLib$TARGET.ts
         */
        qWarning("TARGET %s IsLib:%d",  target.latin1(), isLib );
        qWarning("LANGS %s", languageList.join(";").latin1() );
        qWarning("OPIEDIR %s",  OPIE::self()->opieDir(opiedir).latin1() );
        if (isLib )
            target.prepend("lib");
        target += ".ts";
        updateTsFiles( fetchedTor,  OPIE::self()->opieDir(opiedir),
                       languageList, target, codec, noObsolete, verbose );

        if ( !metSomething ) {
            fprintf( stderr,
                     "lupdate warning: File '%s' does not look like a"
                     " project file\n",
                     argv[i] );
        }

    }

    if ( numFiles == 0 ) {
	printUsage();
	return 1;
    }
    return 0;
}
