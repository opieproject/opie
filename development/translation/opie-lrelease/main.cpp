/**********************************************************************
** Copyright (C) 2000 Trolltech AS.All rights reserved.
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
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include <errno.h>

typedef QValueList<MetaTranslatorMessage> TML;

static void printUsage()
{
fprintf( stderr, "Usage:\n"
	"lrelease [options] project-file\n"
	"lrelease [options] ts-files\n"
	"Options:\n"
	"-opieOPIE dir overrides $OPIEDIR\n"
	"-helpDisplay this information and exit\n"
	"-verbose\n"
	" Explain what is being done\n"
	"-version\n"
	" Display the version of lrelease and exit\n" );
}
static void releaseQmFile( const QString& tsFileName, bool verbose )
{
MetaTranslator tor;
QString qmFileName = tsFileName;
qmFileName.replace( QRegExp("\\.ts$"), "" );
qmFileName += ".qm";

if ( tor.load(tsFileName) ) {
	if ( verbose )
		fprintf( stderr, "Updating '%s'...\n", qmFileName.latin1() );
	if ( !tor.release(qmFileName, verbose) )
		fprintf( stderr,
			 "lrelease warning: For some reason, I cannot save '%s'\n",
			 qmFileName.latin1() );
	} else {
		fprintf( stderr,
			 "lrelease warning: For some reason, I cannot load '%s'\n",
			 tsFileName.latin1() );
	}
}

static void metaQmFile( const QString &opiedir,
const QStringList& lang,
const QString& basename,
bool isLib, bool verb ) {
QString target = basename + ".ts";

if ( isLib ) target.prepend("lib");

for ( QStringList::ConstIterator it = lang.begin(); it != lang.end(); ++it )
{
	QString fileName = opiedir + "/i18n/" + (*it) + "/" + target;
	qWarning("Target is %s", fileName.latin1() );
	releaseQmFile( fileName, verb );
}
}

int main( int argc, char **argv )
{
bool verbose = FALSE;
// bool metTranslations = FALSE;
int numFiles = 0;
QString opiedir;
QStringList languageList = OPIE::self()->languageList( opiedir );

for ( int i = 1; i < argc; i++ )
{
	if ( qstrcmp(argv[i], "-help") == 0 )
	{
		printUsage();
		return 0;
	}
	else if ( qstrcmp(argv[i], "-verbose") == 0 )
	{
		verbose = TRUE;
		continue;
	}
	else if ( qstrcmp(argv[i], "-version") == 0 )
	{
		fprintf( stderr, "lrelease version %s\n", QT_VERSION_STR );
		return 0;
	}
	else if ( qstrcmp(argv[i], "-opie") == 0 )
	{
		i++;
		if ( i < argc )
		{
			opiedir = argv[i];
			languageList = OPIE::self()->languageList(opiedir);
			continue;
		}
//		else
//		{
			fprintf( stderr, "lrelease error: -opie option need a parameter\n" );
			printUsage();
			return 1;
//		}
	}

	numFiles++;
	QFile f( argv[i] );
	if ( !f.open(IO_ReadOnly) )
	{
		fprintf( stderr,
			 "lrelease error: Cannot open file '%s': %s\n",
			 argv[i],
			 strerror(errno) );
		return 1;
	}

	QTextStream t( &f );
	QString fullText = t.read();
	f.close();

	if ( fullText.find(QString("<!DOCTYPE TS>")) >= 0 )
	{
		releaseQmFile( argv[i], verbose );
	}
	else
	{
		QString target;
		bool isLib = FALSE;
		QMap<QString, QString> tagMap = proFileTagMap( fullText );
		QMap<QString, QString>::Iterator it;

		for ( it = tagMap.begin(); it != tagMap.end(); ++it )
		{
			QStringList toks = QStringList::split( ' ', it.data() );
			QStringList::Iterator t;

			for ( t = toks.begin(); t != toks.end(); ++t )
			{
				if ( it.key() == "TARGET" )
				{
					target = *t;
					qWarning("%s %s", it.key().latin1(), (*t).latin1() );
				}
				else if ( it.key() == "TEMPLATE" )
				{
				if ( (*t).stripWhiteSpace().lower() == "lib" )
					isLib = TRUE;
				}
			}
		}
		qWarning("%s", target.latin1() );
		metaQmFile( OPIE::self()->opieDir(opiedir),
		languageList,target, isLib, verbose );
	}
}

if ( numFiles == 0 )
{
	printUsage();
	return 1;
}
return 0;
}
