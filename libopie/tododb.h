
#ifndef tododb_h
#define tododb_h

#include <qobject.h>
#include <qvaluelist.h>

#include <opie/todoevent.h>

namespace Opie {

    class ToDoResource;
    class ToDoDB : public QObject
    {
        Q_OBJECT
        /**
         * A special ToDoDB Iterator for iterating
         * through the lists.
         * The Implementation varies though
         */
        class Iterator {
        public:
            Iterator();
            Iterator( const Iterator& );
            ~Iterator();

            Iterator &operator=( const Iterator& );
            //const ToDoEvent &operator*()const;
            ToDoEvent &operator*();
            Iterator &operator++();

            bool operator==( const Iterator& it );
            bool operator!=( const Iterator& it );


            struct IteratorPrivate;
            IteratorPrivate *d;
        };

        /**
         * same as above but ConstIterator
         */
        class ConstIterator {
        public:
            ConstIterator();
            ConstIterator( const ConstIterator& );
        ~ConstIterator();

            ConstIterator &operator=( const ConstIterator& );
            const ToDoEvent &operator=( const ConstIterator& )const;
            ConstIterator &operator++();
            ConstIterator &operator--();
            bool operator==( const ConstIterator & );
            bool operator!=( const ConstIterator & );

            struct ConstIteratorPrivate;
            ConstIteratorPrivate *d;
        };


    public:
        /**
         * Query Possibilities. They should be self explaining
         */
        enum Query { WildCards = 0, IgnoreCase = 1,
                     RegExp = 2,  ExactMatch = 4 };

        /**
         * c'tor
         * @param resource The Resource to use if 0 the \
         * default resource will be generated
         *
         */
        ToDoDB(ToDoResource* resource= 0 );

        /**
         * d'tor
         */
        ~ToDoDB();


        /**
         * Return the Iterator to the effectiveToDos.
         * @param from The date which should be included
         * @param to The end date which should be included
         * @param includeNoDates whether or not to include
         *         ToDoEvents
         * @return a Iterator to the class
         */
        Iterator effectiveToDos(const QDate &from,
                                const QDate &to,
                                bool includeNoDates = true);

        /**
         * Return all todos from start to now
         */
        Iterator effectiveToDos(const QDate &start,
                                bool includeNoDates = true );

        /**
         * @return a Iterator to all available todos
         */
        Iterator rawToDos(); // all events

        /**
         * @return a Iterator to all overDue todos
         */
        Iterator overDue(); // make it const? -zecke

        Iterator end();

        /** adds a Event to the TodoDB */
        void addEvent(const ToDoEvent &event );

        /** removes an Event from the TodoDB */
        void removeEvent(const ToDoEvent &event);

        /** removes the current Event from the
         * ToDoDB and updates the Iterator
         */
        Iterator remove( const Iterator& );

        /**
         * removeAll removes all Todos
         */
        void removeAll();

        /**
         * Replace event with another Event
         * with the same uid
         */
        void replaceEvent(const ToDoEvent &event );

        /**
         * merge with a set of ToDoEvents
         */
        void mergeWith(const QValueList<ToDoEvent>& );

        /** reload a tododb */
        bool reload();

        /** load */
        bool load();

        /** saves it  This depends on the backend*/
        bool save();
        ToDoResource *resource();
        void setResource(ToDoResource* res);

        /**
         * find a Event by uid
         * @param foundEvent the found Event
         * @param uid The UID to search for
         * @return whether or not the event was found
         */
        ToDoEvent findEvent( int uid, bool *ok = 0 );

        /**
         * "Query by Example"
         */
        Iterator queryByExample( const ToDoEvent& query,
                                 int query = WildCards );


    signals:
        /** emitted if was changed externally */
        void signalChanged( const ToDoDB* which );
    private:
        void addEventAlarm( const ToDoEvent& event );
        void delEventAlarm( const ToDoEvent& event );

        class ToDoDBPrivate;
        ToDoDBPrivate *d;
        ToDoResource *m_res;
    private slots:
        void copMessage( const QCString& msg,
                         const QByteArray& );

    };
};

#endif
