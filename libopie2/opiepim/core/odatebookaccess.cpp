#include "obackendfactory.h"
#include "odatebookaccess.h"

/**
 * Simple constructor
 * It takes a ODateBookAccessBackend as parent. If it is 0 the default implementation
 * will be used!
 * @param back The backend to be used or 0 for the default backend
 * @param ac What kind of access is intended
 */
ODateBookAccess::ODateBookAccess( ODateBookAccessBackend* back, enum Access ac )
    : OPimAccessTemplate<OEvent>( back )
{
    if (!back )
        back = OBackendFactory<ODateBookAccessBackend>::Default("datebook", QString::null );

    m_backEnd = back;
    setBackEnd( m_backEnd );
}
ODateBookAccess::~ODateBookAccess() {
}

/**
 * @return all events available
 */
ODateBookAccess::List ODateBookAccess::rawEvents()const {
    QArray<int> ints = m_backEnd->rawEvents();

    List lis( ints, this );
    return lis;
}

/**
 * @return all repeating events
 */
ODateBookAccess::List ODateBookAccess::rawRepeats()const {
    QArray<int> ints = m_backEnd->rawRepeats();

    List lis( ints, this );
    return lis;
}

/**
 * @return all non repeating events
 */
ODateBookAccess::List ODateBookAccess::nonRepeats()const {
    QArray<int> ints = m_backEnd->nonRepeats();

    List lis( ints, this );
    return lis;
}

/**
 * @return dates in the time span between from and to
 * @param from Include all events from...
 * @param to Include all events to...
 */
OEffectiveEvent::ValueList ODateBookAccess::effectiveEvents( const QDate& from, const QDate& to ) {
    return m_backEnd->effecticeEvents( from, to );
}
/**
 * @return all events at a given datetime
 */
OEffectiveEvent::ValueList ODateBookAccess::effectiveEvents( const QDateTime& start ) {
    return m_backEnd->effecticeEvents( start );
}
