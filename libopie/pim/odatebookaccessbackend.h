#ifndef OPIE_DATE_BOOK_ACCESS_BACKEND_H
#define OPIE_DATE_BOOK_ACCESS_BACKEND_H

#include <qarray.h>

#include "opimaccessbackend.h"
#include "oevent.h"

/**
 * This class is the interface to the storage of Events.
 * @see OPimAccessBackend
 *
 */
class ODateBookAccessBackend : public OPimAccessBackend<OEvent> {
public:
    typedef int UID;

    /**
     * c'tor without parameter
     */
    ODateBookAccessBackend();
    ~ODateBookAccessBackend();

    /**
     * This method should return a list of UIDs containing
     * all events. No filter should be applied
     * @return list of events
     */
    virtual QArray<UID> rawEvents()const = 0;

    /**
     * This method should return a list of UIDs containing
     * all repeating events. No filter should be applied
     * @return list of repeating events
     */
    virtual QArray<UID> rawRepeats()const = 0;

    /**
     * This mthod should return a list of UIDs containing all non
     * repeating events. No filter should be applied
     * @return list of nonrepeating events
     */
    virtual QArray<UID> nonRepeats() const = 0;

    /**
     * If you do not want to implement the effectiveEvents methods below
     * you need to supply it with directNonRepeats.
     * This method can return empty lists if effectiveEvents is implememted
     */
    virtual OEvent::ValueList directNonRepeats() = 0;

    /**
     * Same as above but return raw repeats!
     */
    virtual OEvent::ValueList directRawRepeats() = 0;

    /* is implemented by default but you can reimplement it*/
    /**
     * Effective Events are special event occuring during a time frame. This method does calcualte
     * EffectiveEvents bases on the directNonRepeats and directRawRepeats. You may implement this method
     * yourself
     */
    virtual OEffectiveEvent::ValueList effecticeEvents( const QDate& from, const QDate& to );

    /**
     * this is an overloaded member function
     * @see effecticeEvents
     */
    virtual OEffectiveEvent::ValueList effecticeEvents( const QDateTime& start );

};

#endif
