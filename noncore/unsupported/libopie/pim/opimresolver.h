#ifndef OPIE_PIM_RESOLVER
#define OPIE_PIM_RESOLVER

#include <qstring.h>
#include <qvaluelist.h>

#include <opie/otemplatebase.h>

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
class OPimResolver {
public:
    enum BuiltIn { TodoList = 0,
                   DateBook,
                   AddressBook
    };
    static OPimResolver* self();


    /**
     * return a record for a uid
     * and an service
     * You've THE OWNERSHIP NOW!
     */
    OPimRecord *record( const QString& service, int uid );

    /**
     * return the QCopChannel for service
     * When we will use Qtopia Services it will be used here
     */
    QCString qcopChannel( enum BuiltIn& )const;
    QCString qcopChannel( const QString& service )const;

    /**
     * The Application channel (QPE/Application/name)
     */
    QCString applicationChannel( enum BuiltIn& )const;
    QCString applicationChannel( const QString& service )const;

    /**
     * return a list of available services
     */
    QStringList services()const;
    inline QString serviceName(int rrti )const;
    int serviceId( const QString& Service);
    /**
     * add a record to a service... ;)
     */
    bool add( const QString& service, const OPimRecord& );


    /**
     *  record returns an empty record for a given service.
     *  Be sure to delete it!!!
     *
     */
    OPimRecord* record( const QString& service );
    OPimRecord* record( int rtti );

    /**
     * you can cast to your
     */
    OPimBase* backend( const QString& service );
    OPimBase* backend( int rtti );
private:
    OPimResolver();
    void loadData();
    inline bool isBuiltIn( const QString& )const;
    OPimRecord* recordExtern( const QString&, int );
    OPimRecord* recordExtern( const QString& );

    static OPimResolver* m_self;
    struct Data;
    class Private;

    Data* data;
    Private* d;
    QStringList m_builtIns;
};

#endif
