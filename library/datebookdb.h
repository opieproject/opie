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

#ifndef DATEBOOKDB_H
#define DATEBOOKDB_H

#include <qdatetime.h>
#include <qfile.h>
#include <qvaluelist.h>
#include <qpe/event.h>

// journal actions...
enum journal_action { ACTION_ADD, ACTION_REMOVE, ACTION_REPLACE };

class DateBookDBPrivate;
class DateBookDB
{
public:
    DateBookDB();
    ~DateBookDB();

    // very depreciated now!!!
    QValueList<Event> getEvents( const QDate &from, const QDate &to );
    QValueList<Event> getEvents( const QDateTime &start );

    // USE THESE!!!
    QValueList<EffectiveEvent> getEffectiveEvents( const QDate &from,
						   const QDate &to );
    QValueList<EffectiveEvent> getEffectiveEvents( const QDateTime &start );

    QValueList<Event> getRawRepeats() const;
    QValueList<Event> getNonRepeatingEvents( const QDate &from,
					     const QDate &to ) const;

    // Use these when dealing with adding removing events...
    void addEvent( const Event &ev, bool doalarm=TRUE );
    void removeEvent( const Event &ev );
    void editEvent( const Event &old, Event &ev );
    // add/remove event without journaling ( these ended up in public by accident, never
    // use them unless you know what you are doing...),
    // please put them in private if we ever can change the class...
    void addJFEvent( const Event &ev, bool doalarm=TRUE );
    void removeJFEvent( const Event &ev );

    bool save();
    void reload();
private:
    //find the real repeat...
    bool origRepeat( const Event &ev, Event &orig ) const;
    bool removeRepeat( const Event &ev );
    void init();
    void loadFile( const QString &strFile );
    // depreciated...
    void saveJournalEntry( const Event &ev, journal_action action );
    // new version, uncomment the "= -1" when we remove the above
    // function..
    bool saveJournalEntry( const Event &ev, journal_action action,
			   int key/* = -1*/, bool origHadRepeat = false );

    QValueList<Event> eventList; // non-repeating events...
    QValueList<Event> repeatEvents; // the repeating events...
    DateBookDBPrivate *d;
    QFile journalFile;

    int	recordIdMax;  // ADDITION
};

/* helper functions, also useful to other apps. */
bool nextOccurance( const Event &e, const QDate &from, QDateTime &next);
#endif
