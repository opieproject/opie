#ifndef OPIE_DATE_BOOK_ACCESS_H
#define OPIE_DATE_BOOK_ACCESS_H

#include "odatebookaccessbackend.h"
#include "opimaccesstemplate.h"

#include "oevent.h"

class ODateBookAccess : public OPimAccessTemplate<OEvent> {
public:
    ODateBookAccess( ODateBookAccessBackend* = 0l, enum Access acc = Random );
    ~ODateBookAccess();

    /** return all events */
    List rawEvents()const;

    /** return repeating events */
    List rawRepeats()const;

    /** return non repeating events */
    List nonRepeats()const;

    OEffectiveEvent::ValueList effectiveEvents( const QDate& from, const QDate& to );
    OEffectiveEvent::ValueList effectiveEvents( const QDateTime& start );

private:
    ODateBookAccessBackend* m_backEnd;
    class Private;
    Private* d;
};

#endif
