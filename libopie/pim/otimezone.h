#ifndef OPIE_TIME_ZONE_H
#define OPIE_TIME_ZONE_H

#include <time.h>
#include <qdatetime.h>

/**
 * A very primitive class to convert time
 * from one timezone to another
 * and to localtime
 * and time_t
 */
class OTimeZone {
 public:
    typedef QString ZoneName;
    OTimeZone( const ZoneName&  = ZoneName::null );
    virtual ~OTimeZone(); // just in case.

    bool isValid()const;

    /**
     * converts the QDateTime to a DateTime
     * in the local timezone
     * if QDateTime is 25th Jan and takes place in Europe/Berlin at 12h
     * and the current timezone is Europe/London the returned
     * time will be 11h.
     */
    QDateTime toLocalDateTime( const QDateTime& dt );

    /**
     * converts the QDateTime to UTC time
     */
    QDateTime toUTCDateTime( const QDateTime& dt );

    /**
     * reads the time_t into a QDateTime using UTC as timezone!
     */
    QDateTime fromUTCDateTime( time_t );

    /**
     * converts the time_t to the time in the timezone
     */
    QDateTime toDateTime( time_t );

    /**
     * converts the QDateTime from one timezone to this timeZone
     */
    QDateTime toDateTime( const QDateTime&, const OTimeZone& timeZone );

    /**
     * converts the date time into a time_t. It takes the timezone into account
     */
    time_t fromDateTime( const QDateTime& );

    /**
     * converts the datetime with timezone UTC
     */
    time_t fromUTCDateTime( const QDateTime& );

    static OTimeZone current();
    static OTimeZone utc();

    QString timeZone()const;
 private:
    ZoneName m_name;
    class Private;
    Private* d;
};


#endif
