#ifndef OPIE_DATE_BOOK_ACCESS_BACKEND_XML__H
#define OPIE_DATE_BOOK_ACCESS_BACKEND_XML__H

#include <qmap.h>

#include "odatebookaccessbackend.h"

/**
 * This is the default XML implementation for DateBoook XML storage
 * It fully implements the interface
 * @see ODateBookAccessBackend
 * @see OPimAccessBackend
 */
class ODateBookAccessBackend_XML : public ODateBookAccessBackend {
public:
    ODateBookAccessBackend_XML( const QString& appName,
                                const QString& fileName = QString::null);
    ~ODateBookAccessBackend_XML();

    bool load();
    bool reload();
    bool save();

    QArray<int> allRecords()const;
    QArray<int> matchRegexp(const QRegExp &r) const;
    QArray<int> queryByExample( const OEvent&, int, const QDateTime& d = QDateTime() );
    OEvent find( int uid )const;
    void clear();
    bool add( const OEvent& ev );
    bool remove( int uid );
    bool replace( const OEvent& ev );

    QArray<UID> rawEvents()const;
    QArray<UID> rawRepeats()const;
    QArray<UID> nonRepeats()const;

    OEvent::ValueList directNonRepeats();
    OEvent::ValueList directRawRepeats();

private:
    bool m_changed :1 ;
    bool loadFile();
    inline void finalizeRecord( OEvent& ev );
    inline void setField( OEvent&, int field, const QString& val );
    QString m_name;
    QMap<int, OEvent> m_raw;
    QMap<int, OEvent> m_rep;

    struct Data;
    Data* data;
    class Private;
    Private *d;
};

#endif
