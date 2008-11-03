#ifndef OPIE_DATE_BOOK_MANAGER_H
#define OPIE_DATE_BOOK_MANAGER_H

#include <qlist.h>

#include <opie2/opimrecord.h>
#include <opie2/odatebookaccess.h>
#include <opie2/opimoccurrence.h>

#include "holiday.h"

namespace Opie {
namespace Datebook {
    /**
     * book manager is responsible for managing the
     * access
     * All access to the underlying API will be done
     * through this class
     */
    class BookManager {
    public:
        BookManager();
        ~BookManager();

        bool isLoaded()const;
        bool load();
        void reload();
        bool save();

        OPimEvent event( int uid );
        OPimOccurrence::List list( const QDate& from,
                                         const QDate& to );
        ODateBookAccess::List allRecords()const;

        void add( const OPimEvent& );
        void add( const OPimRecord& );
        void update( const OPimEvent& );
        void remove( int uid );
        void remove( const QArray<int>& );

        QList<OPimRecord> records( const QDate& from,
                                      const QDate& to );

        DateBookHoliday *holiday();

    protected:
        void addAlarms( const OPimEvent &ev );
        void removeAlarms( const OPimEvent &ev );

    private:
        ODateBookAccess* m_db;
        DateBookHoliday* m_holiday;
    };
}
}

#endif
