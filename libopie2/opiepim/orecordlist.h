
#ifndef OPIE_RECORD_LIST_H
#define OPIE_RECORD_LIST_H

#include <opie/opimaccesstemplate.h>
#include <opie/opimrecord.h>

template <class T = OPimRecord >
class ORecordList {
public:
    class Iterator {
        friend class ORecordList;
    public:
        Iterator() {}
        ~Iterator() {}
        Iterator(const Iterator& ) {}
        Iterator &operator=(const Iterator& );
        T &operator*() {}
        Iterator &operator++();

        bool operator==( const Iterator& it );
        bool operator!=( const Iterator& it );

    }
    ORecordList( const QArray<int>& ids,
                 OPimAccessTemplate<T>* acc )
        : m_ids(ids ), m_acc( acc ) {

    }
    ~ORecordList() {

    }
    Iterator begin();
    Iterator end();
    /*
      ConstIterator begin()const;
      ConstIterator end()const;
    */
private:
    QArray<int> ids;
    OPimAccessTemplate<T>* m_acc;
};

#endif
