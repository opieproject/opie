#ifndef OPIE_DATE_BOOK_ACCESS_BACKEND_SQL__H
#define OPIE_DATE_BOOK_ACCESS_BACKEND_SQL__H

#include <qmap.h>
#include <opie2/osqlresult.h>

#include "odatebookaccessbackend.h"

class OSQLDriver;

/**
 * This is the default SQL implementation for DateBoook SQL storage
 * It fully implements the interface
 * @see ODateBookAccessBackend
 * @see OPimAccessBackend
 */
class ODateBookAccessBackend_SQL : public ODateBookAccessBackend {
public:
    ODateBookAccessBackend_SQL( const QString& appName,
                                const QString& fileName = QString::null);
    ~ODateBookAccessBackend_SQL();

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
    bool loadFile();
    QString m_fileName;
    QArray<int> m_uids;

    QMap<int, QString> m_fieldMap;
    QMap<QString, int> m_reverseFieldMap;

    OSQLDriver* m_driver;

    class Private;
    Private *d;

    void initFields();
    void update();
    QArray<int> extractUids( OSQLResult& res ) const;

};

#endif
