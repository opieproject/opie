// (c) by zecke LGPL what else for a lib I care about ;)

#ifndef opietodoresource_h
#define opietodoresource_h

#include <qarray.h>

#include "todoevent.h"
#include "tododb.h"

namespace Opie {
    /**
     * A default ToDoResource
     * it will be kept similiar to the one we created
     * for ocontactdb + I introduce a new neat thingie
     */
    class ToDoResource {
    public:
        ToDoResource(const QString& appName );
        virtual ~ToDoResource();
        virtual bool load() = 0;
        virtual bool save() = 0;
        virtual bool reload() = 0;
        virtual bool wasChangedExternally()const;
        virtual ToDoEvent findEvent(int uid,
                                   bool *ok ) = 0;
        virtual QArray<int> rawToDos() = 0;
        virtual QArray<int> queryByExample( const ToDoEvent&,
                                            uint querysettings ) = 0;
        virtual QArray<int> effectiveToDos( const QDate& start,
                                         const QDate& end,
                                         bool includeNoDates ) = 0;
        virtual QArray<int> overDue() = 0;
        virtual bool addEvent( const ToDoEvent& event ) = 0;
        virtual bool replaceEvent( const ToDoEvent& ) = 0;
        virtual bool removeEvent( const ToDoEvent& ) = 0;
        virtual void removeAll() = 0;
    protected:
        QString appName()const;

    private:
        QString m_appName;
        class ToDoResourcePrivate;
        ToDoResourcePrivate *d;
    };
};

#endif
