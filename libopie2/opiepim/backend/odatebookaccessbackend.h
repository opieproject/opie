#ifndef OPIE_DATE_BOOK_ACCESS_BACKEND_H
#define OPIE_DATE_BOOK_ACCESS_BACKEND_H

#include <qarray.h>

#include "opimaccessbackend.h"
#include "oevent.h"

class ODateBookAccessBackend : public OPimAccessBackend<OEvent> {
public:
    typedef int UID;
    ODateBookAccessBackend();
    ~ODateBookAccessBackend();

    virtual QArray<UID> rawEvents()const = 0;
    virtual QArray<UID> rawRepeats()const = 0;
    virtual QArray<UID> nonRepeats() const = 0;

    /**
     * these two methods are used if you do not implement
     * effectiveEvents...
     */
    virtual OEvent::ValueList directNonRepeats() = 0;
    virtual OEvent::ValueList directRawRepeats() = 0;

    /* is implemented by default but you can reimplement it*/
    virtual OEffectiveEvent::ValueList effecticeEvents( const QDate& from, const QDate& to );
    virtual OEffectiveEvent::ValueList effecticeEvents( const QDateTime& start );

};

#endif
