/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
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

// defined in numberh.cpp
extern void applyNumberHeuristic( MetaTranslator *tor, bool verbose );
// defined in sametexth.cpp
extern void applySameTextHeuristic( MetaTranslator *tor, bool verbose );

typedef QValueList<MetaTranslatorMessage> TML;

/*
  Merges two MetaTranslator objects into the first one. The first one
  is a set of source texts and translations for a previous version of
  the internationalized program; the second one is a set of fresh
  source texts newly extracted from the source code, without any
  translation yet.
*/

void merge( MetaTranslator *tor, const MetaTranslator *virginTor, bool verbose )
{
    int known = 0;
    int neww = 0;
    int obsoleted = 0;
    TML all = tor->messages();
    TML::Iterator it;

    /*
      The types of all the messages from the vernacular translator
      are updated according to the virgin translator.
    */
    for ( it = all.begin(); it != all.end(); ++it ) {
	MetaTranslatorMessage::Type newType;
	MetaTranslatorMessage m = *it;

	// skip context comment
	if ( !QCString((*it).sourceText()).isEmpty() ) {
	    if ( !virginTor->contains((*it).context(), (*it).sourceText(),
				      (*it).comment()) ) {
		newType = MetaTranslatorMessage::Obsolete;
		if ( m.type() != MetaTranslatorMessage::Obsolete )
		    obsoleted++;
	    } else {
		switch ( m.type() ) {
		case MetaTranslatorMessage::Finished:
		    newType = MetaTranslatorMessage::Finished;
		    known++;
		    break;
		case MetaTranslatorMessage::Unfinished:
		    newType = MetaTranslatorMessage::Unfinished;
		    known++;
		    break;
		case MetaTranslatorMessage::Obsolete:
		    newType = MetaTranslatorMessage::Unfinished;
		    neww++;
		}
	    }

	    if ( newType != m.type() ) {
		m.setType( newType );
		tor->insert( m );
	    }
	}
    }

    /*
      Messages found only in the virgin translator are added to the
      vernacular translator. Among these are all the context comments.
    */
    all = virginTor->messages();

    for ( it = all.begin(); it != all.end(); ++it ) {
	if ( !tor->contains((*it).context(), (*it).sourceText(),
			    (*it).comment()) ) {
	    tor->insert( *it );
	    if ( !QCString((*it).sourceText()).isEmpty() )
		neww++;
	}
    }

    /*
      The same-text heuristic handles cases where a message has an
      obsolete counterpart with a different context or comment.
    */
    applySameTextHeuristic( tor, verbose );

    /*
      The number heuristic handles cases where a message has an
      obsolete counterpart with mostly numbers differing in the
      source text.
    */
    applyNumberHeuristic( tor, verbose );

    if ( verbose )
	fprintf( stderr, " %d known, %d new and %d obsoleted messages\n", known,
		 neww, obsoleted );
}
