#ifndef OPIE_STRING_DATEBOOK_MANAGER_H
#define OPIE_STRING_DATEBOOK_MANAGER_H

#include <qstring.h>

#include "managertemplate.h"

class QListView;
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
        StringManager(const QString& baseName = QString::null);
        ~StringManager();

        /**
         * override the add implementation
         * cause we do not have a value :)
         */
        void add( const QString& );
        bool load();
        bool save();
        QString baseName()const;
    private:
        QString m_base;
    };

    /**
     * A Generic Editor for StringManager
     */
    class StringManagerDialog : public QDialog {
        Q_OBJECT
    public:
        StringManagerDialog( const StringManager&);
        ~StringManagerDialog();

        StringManager manager()const;
    private slots:
        void init( const StringManager&  );
        void slotAdd();
        void slotRemove();
        void slotRename();
    private:
        QListView* m_view;
        QString m_base;
    };
}

#endif
