/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Linguist.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <metatranslator.h>

#include <qcstring.h>
#include <qmap.h>

typedef QMap<QCString, MetaTranslatorMessage> TMM;
typedef QValueList<MetaTranslatorMessage> TML;

/*
  Augments a MetaTranslator with trivially derived translations.

  For example, if "Enabled:" is consistendly translated as "Eingeschaltet:" no
  matter the context or the comment, "Eingeschaltet:" is added as the
  translation of any untranslated "Enabled:" text and is marked Unfinished.
*/

void applySameTextHeuristic( MetaTranslator *tor, bool verbose )
{
    TMM translated;
    TMM avoid;
    TMM::Iterator t;
    TML untranslated;
    TML::Iterator u;
    TML all = tor->messages();
    TML::Iterator it;
    int inserted = 0;

    for ( it = all.begin(); it != all.end(); ++it ) {
	if ( (*it).type() == MetaTranslatorMessage::Unfinished ) {
	    if ( (*it).translation().isEmpty() )
		untranslated.append( *it );
	} else {
	    QCString key = (*it).sourceText();
	    t = translated.find( key );
	    if ( t != translated.end() ) {
		/*
		  The same source text is translated at least two
		  different ways. Do nothing then.
		*/
		if ( (*t).translation() != (*it).translation() ) {
		    translated.remove( key );
		    avoid.insert( key, *it );
		}
	    } else if ( !avoid.contains(key) &&
			!(*it).translation().isEmpty() ) {
		translated.insert( key, *it );
	    }
	}
    }

    for ( u = untranslated.begin(); u != untranslated.end(); ++u ) {
	QCString key = (*u).sourceText();
	t = translated.find( key );
	if ( t != translated.end() ) {
	    MetaTranslatorMessage m( *u );
	    m.setTranslation( (*t).translation() );
	    tor->insert( m );
	    inserted++;
	}
    }
    if ( verbose && inserted != 0 )
	fprintf( stderr, " same-text heuristic provided %d translation%s\n",
		 inserted, inserted == 1 ? "" : "s" );
}
