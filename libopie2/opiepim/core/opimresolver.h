#ifndef OPIE_PIM_RESOLVER
#define OPIE_PIM_RESOLVER

#include <qstring.h>
#include <qvaluelist.h>

/**
 * OPimResolver is a MetaClass to access
 * available backends read only.
 * It will be used to resolve uids + app names
 * to full informations
 * to traverse through a list of alarms, reminders
 * to get access to built in PIM functionality
 * and to more stuff
 * THE PERFORMANCE will depend on THE BACKEND
 * USING XML is a waste of memory!!!!!
 */
class OPimResolver : public QObject {
public:
    enum BuiltIn { TodoList = 0,
                   DateBook,
                   AddressBook
    };
    static OPimResolver* self();


    /*
     * return a record for a uid
     * and an app
     */
    OPimRecord &record( const QString& service, int uid );

    /**
     * return the QCopChannel for service
     * When we will use Qtopia Services it will be used here
     */
    QString qcopChannel( enum BuiltIn& )const;
    QString qcopChannel( const QString& service );

    /**
     * return a list of available services
     */
    QStringList services()const;

    /**
     * add a record to a service... ;)
     */
    bool add( const QString& service, const OPimRecord& );

private:
    OPimResolver();
    OPimRecord *m_last;

}:

#endif
