#ifndef OPIE_STRING_DATEBOOK_MANAGER_H
#define OPIE_STRING_DATEBOOK_MANAGER_H

#include <qstring.h>

#include "managertemplate.h"

namespace Datebook {
    /**
     * StringManager is a generic manager
     * whick keeps track of strings
     * It'll be used with the Location
     * and the Description Manager
     */
    class StringManager : public ManagerTemplate<QString> {
    public:
        /**
         * baseName is used for storing
         */
        StringManager(const QString& baseName);
        ~StringManager();

        /**
         * override the add implementation
         * cause we do not have a value :)
         */
        void add( const QString& );
    private:
        void doLoad();
        void doSave();
    };

    /**
     * A Generic Editor for StringManager
     */
    class StringManagerDialog : public QDialog {
        Q_OBJECT
    public:
        StringManagerDialog( const StringManager& );
        ~StringManager();

        StringManager manager()const;
    };
}

#endif
