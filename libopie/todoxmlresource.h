
#ifndef XMLOPIETODO_RESOURCE_H
#define XMLOPIETODO_RESOURCE_H

#include <qarray.h>
#include <qasciidict.h>
#include <qmap.h>

#include <opie/todoevent.h>
#include <opie/todoresource.h>

namespace Opie {
    /**
     * Is the standard XML resource for the todoapplication
     */
    class ToDoXMLResource : public ToDoResource {
    public:
        ToDoXMLResource(const QString& appName,
                        const QString& fileName = QString::null);
        virtual ~ToDoXMLResource();
        bool load();
        bool save();
        bool reload();
        bool wasChangedExternally()const;
        ToDoEvent findEvent( int uid,
                             bool *ok );
        QArray<int> rawToDos();
        QArray<int> queryByExample( const ToDoEvent&,
                                    uint querysettings );
        QArray<int> effectiveToDos( const QDate& start,
                                    const QDate& end,
                                    bool includeNoDates );
        QArray<int> overDue();
        bool addEvent( const ToDoEvent& event );
        bool replaceEvent( const ToDoEvent& event );
        bool removeEvent( const ToDoEvent& event );
    private:
        ToDoEvent todo( QAsciiDict<int>*, XMLElement* )const;
        QString toString( const ToDoEvent& ev )const;
        QString toString( const QArray<int>& ints )const;
        QMap<int, ToDoEvent> m_events;
        QString m_file;
    };
}
#endif
