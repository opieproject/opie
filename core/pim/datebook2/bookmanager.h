#ifndef OPIE_DATE_BOOK_MANAGER_H
#define OPIE_DATE_BOOK_MANAGER_H

#include <qlist.h>

#include <opie/opimrecord.h>
#include <opie/odatebookaccess.h>

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
        bool save();

        OEvent event( int uid );
        OEffectiveEvent::ValueList list( const QDate& from,
                                         const QDate& to );
        ODateBookAccess::List allRecords()const;

        void add( const OEvent& );
        void update( const OEvent& );
        void remove( int uid );
        void remove( const QArray<int>& );

        QPtrList<OPimRecord> records( const QDate& from,
                                      const QDate& to );
    };
}

#endif
