#ifndef OPIE_DATE_BOOK_ACCESS_H
#define OPIE_DATE_BOOK_ACCESS_H

#include "odatebookaccessbackend.h"
#include "opimaccesstemplate.h"

#include "oevent.h"

/**
 * This is the object orientated datebook database. It'll use OBackendFactory
 * to query for a backend.
 * All access to the datebook should be done via this class.
 * Make sure to load and save the datebook this is not part of
 * destructing and creating the object
 *
 * @author Holger Freyther, Stefan Eilers
 */
class ODateBookAccess : public OPimAccessTemplate<OEvent> {
public:
    ODateBookAccess( ODateBookAccessBackend* = 0l, enum Access acc = Random );
    ~ODateBookAccess();

    /* return all events */
    List rawEvents()const;

    /* return repeating events */
    List rawRepeats()const;

    /* return non repeating events */
    List nonRepeats()const;

    /* return non repeating events (from,to) */
    OEffectiveEvent::ValueList effectiveEvents( const QDate& from, const QDate& to ) const;
    OEffectiveEvent::ValueList effectiveEvents( const QDateTime& start ) const;
    OEffectiveEvent::ValueList effectiveNonRepeatingEvents( const QDate& from, const QDate& to ) const;
    OEffectiveEvent::ValueList effectiveNonRepeatingEvents( const QDateTime& start ) const;

private:
    ODateBookAccessBackend* m_backEnd;
    class Private;
    Private* d;
};

#endif
