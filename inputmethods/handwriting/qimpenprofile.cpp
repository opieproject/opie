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

#include "qimpencombining.h"
#include "qimpenprofile.h"

#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/global.h>


QIMPenProfile::QIMPenProfile( const QString &fn )
    : filename( fn )
{
    sets.setAutoDelete( true );

    Config config( filename, Config::File );
    config.setGroup( "Handwriting" );

    pname = config.readEntry( "Name" );
    pdesc = config.readEntry( "Description" );

    tstyle = config.readBoolEntry( "CanSelectStyle", false );

    wordMatch = config.readBoolEntry( "MatchWords", true );

    config.setGroup( "Settings" );

    pstyle = BothCases;
    QString s = config.readEntry( "Style", "BothCases" );
    if ( s == "ToggleCases" )
	pstyle = ToggleCases;

    msTimeout = config.readNumEntry( "MultiTimeout", 500 );

    // Read user configuration
    Config usrConfig( userConfig() );
    usrConfig.setGroup( "Settings" );
    msTimeout = usrConfig.readNumEntry( "MultiTimeout", msTimeout );

    if ( tstyle && usrConfig.hasKey( "Style" ) ) {
	pstyle = BothCases;
	QString s = usrConfig.readEntry( "Style", "BothCases" );
	if ( s == "ToggleCases" )
	    pstyle = ToggleCases;
    }
}

void QIMPenProfile::setStyle( Style s )
{
    if ( tstyle && s != pstyle ) {
	pstyle = s;
	Config config( userConfig() );
	config.setGroup( "Settings" );
	QString s = pstyle == ToggleCases ? "ToggleCases" : "BothCases";
	config.writeEntry( "Style", s );
    }
}

void QIMPenProfile::setMultiStrokeTimeout( int t )
{
    if ( t != msTimeout ) {
	msTimeout = t;
	Config config( userConfig() );
	config.setGroup( "Settings" );
	config.writeEntry( "MultiTimeout", msTimeout );
    }
}

QString QIMPenProfile::userConfig()
{
    QString un = filename;
    int pos = un.findRev( '/' );
    if ( pos >= 0 )
	un = un.mid( pos + 1 );
    pos = un.find( '.' );
    if ( pos > 0 )
	un.truncate( pos );

    un = "handwriting-" + un;

    return un;
}

void QIMPenProfile::loadData()
{
    Config config( filename, Config::File );
    config.setGroup( "CharSets" );

    QString baseDir = QPEApplication::qpeDir();
    baseDir += "/etc/";
    // accents
    QIMPenCombining *combining = 0;
    QString s = config.readEntry( "Combining" );
    if ( !s.isEmpty() ) {
	combining = new QIMPenCombining( baseDir + "qimpen/" + s );
	if ( combining->isEmpty() ) {
	    delete combining;
	    combining = 0;
	}
    }
    // uppercase latin1
    QIMPenCharSet *cs = 0;
    s = config.readEntry( "Uppercase" );
    if ( !s.isEmpty() ) {
	cs = new QIMPenCharSet( baseDir + "qimpen/" + s );
	cs->load( Global::applicationFileName("qimpen",s), QIMPenCharSet::User );
	if ( !cs->isEmpty() ) {
	    if ( combining )
		combining->addCombined( cs );
	    sets.append( cs );
	} else {
	    delete cs;
	}
    }
    // lowercase latin1
    s = config.readEntry( "Lowercase" );
    if ( !s.isEmpty() ) {
	cs = new QIMPenCharSet( baseDir + "qimpen/" + s );
	cs->load( Global::applicationFileName("qimpen",s), QIMPenCharSet::User );
	if ( !cs->isEmpty() ) {
	    if ( combining )
		combining->addCombined( cs );
	    sets.append( cs );
	} else {
	    delete cs;
	}
    }
    // numeric (may comtain punctuation and symbols)
    s = config.readEntry( "Numeric" );
    if ( !s.isEmpty() ) {
	cs = new QIMPenCharSet( baseDir + "qimpen/" + s );
	cs->load( Global::applicationFileName("qimpen",s), QIMPenCharSet::User );
	if ( !cs->isEmpty() ) {
	    sets.append( cs );
	} else {
	    delete cs;
	}
    }
    // punctuation
    s = config.readEntry( "Punctuation" );
    if ( !s.isEmpty() ) {
	cs = new QIMPenCharSet( baseDir + "qimpen/" + s );
	cs->load( Global::applicationFileName("qimpen",s), QIMPenCharSet::User );
	if ( !cs->isEmpty() ) {
	    sets.append( cs );
	} else {
	    delete cs;
	}
    }
    // symbol
    s = config.readEntry( "Symbol" );
    if ( !s.isEmpty() ) {
	cs = new QIMPenCharSet( baseDir + "qimpen/" + s );
	cs->load( Global::applicationFileName("qimpen",s), QIMPenCharSet::User );
	if ( !cs->isEmpty() ) {
	    sets.append( cs );
	} else {
	    delete cs;
	}
    }
    // shortcut
    s = config.readEntry( "Shortcut" );
    if ( !s.isEmpty() ) {
	cs = new QIMPenCharSet( baseDir + "qimpen/" + s );
	cs->load( Global::applicationFileName("qimpen",s), QIMPenCharSet::User );
	if ( !cs->isEmpty() ) {
	    sets.append( cs );
	} else {
	    delete cs;
	}
    }

    if ( combining )
	delete combining;
}

QIMPenCharSet *QIMPenProfile::uppercase()
{
    return find( QIMPenCharSet::Upper );
}

QIMPenCharSet *QIMPenProfile::lowercase()
{
    return find( QIMPenCharSet::Lower );
}

QIMPenCharSet *QIMPenProfile::numeric()
{
    return find( QIMPenCharSet::Numeric );
}

QIMPenCharSet *QIMPenProfile::punctuation()
{
    return find( QIMPenCharSet::Punctuation );
}

QIMPenCharSet *QIMPenProfile::symbol()
{
    return find( QIMPenCharSet::Symbol );
}

QIMPenCharSet *QIMPenProfile::shortcut()
{
    return find( QIMPenCharSet::Shortcut );
}

QIMPenCharSetList &QIMPenProfile::charSets()
{
    if ( sets.isEmpty() )
	loadData();
    return sets;
}

QIMPenCharSet *QIMPenProfile::find( QIMPenCharSet::Type t )
{
    if ( sets.isEmpty() )
	loadData();
    QIMPenCharSetIterator it( sets );
    for ( ; it.current(); ++it ) {
	if ( it.current()->type() == t )
	    return it.current();
    }

    return 0;
}


