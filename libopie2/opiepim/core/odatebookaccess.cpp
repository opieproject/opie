#include "obackendfactory.h"
#include "odatebookaccess.h"

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
ODateBookAccess::List ODateBookAccess::rawEvents()const {
    QArray<int> ints = m_backEnd->rawEvents();

    List lis( ints, this );
    return lis;
}
ODateBookAccess::List ODateBookAccess::rawRepeats()const {
    QArray<int> ints = m_backEnd->rawRepeats();

    List lis( ints, this );
    return lis;
}
ODateBookAccess::List ODateBookAccess::nonRepeats()const {
    QArray<int> ints = m_backEnd->nonRepeats();

    List lis( ints, this );
    return lis;
}
OEffectiveEvent::ValueList ODateBookAccess::effectiveEvents( const QDate& from, const QDate& to ) {
    return m_backEnd->effecticeEvents( from, to );
}
OEffectiveEvent::ValueList ODateBookAccess::effectiveEvents( const QDateTime& start ) {
    return m_backEnd->effecticeEvents( start );
}
