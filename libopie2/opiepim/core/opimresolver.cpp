#include <qcopchannel_qws.h>

#include <qpe/qcopenvelope_qws.h>

#include "otodoaccess.h"
#include "ocontactaccess.h"

//#include "opimfactory.h"
#include "opimresolver.h"

OPimResolver* OPimResolver::m_self = 0l;

OPimResolver::OPimResolver() {
    /* the built in channels */
    m_builtIns << "Todolist" << "Addressbook" << "Datebook";
}
OPimResolver* OPimResolver::self() {
    if (!m_self)
        m_self = new OPimResolver();

    return m_self;
}

/*
 * FIXME use a cache here too
 */
OPimRecord* OPimResolver::record( const QString& service, int uid ) {
    OPimRecord* rec = 0l;
    OPimBase* base = backend( service );

    if ( base )
        rec = base->record( uid );
    delete base;

    return rec;
}
OPimRecord* OPimResolver::record( const QString& service ) {
    return record( serviceId( service ) );
}
OPimRecord* OPimResolver::record( int rtti ) {
    OPimRecord* rec = 0l;
    switch( rtti ) {
    case 1: /* todolist */
        rec = new OTodo();
    case 2: /* contact  */
        rec = new OContact();
    default:
        break;
    }
    /*
     * FIXME resolve externally
     */
    if (!rec ) {
        ;
    }
    return 0l;
}
bool OPimResolver::isBuiltIn( const QString& str) const{
    return m_builtIns.contains( str );
}
QCString OPimResolver::qcopChannel( enum BuiltIn& built)const {
    QCString str("QPE/");
    switch( built ) {
    case TodoList:
        str += "Todolist";
        break;
    case DateBook:
        str += "Datebook";
        break;
    case AddressBook:
        str += "Addressbook";
        break;
    default:
        break;
    }

    return str;
}
QCString OPimResolver::qcopChannel( const QString& service )const {
    QCString str("QPE/");
    str += service.latin1();
    return str;
}
/*
 * Implement services!!
 * FIXME
 */
QCString OPimResolver::applicationChannel( enum BuiltIn& built)const {
    QCString str("QPE/Application/");
    switch( built ) {
    case TodoList:
        str += "todolist";
        break;
    case DateBook:
        str += "datebook";
        break;
    case AddressBook:
        str += "addressbook";
        break;
    }

    return str;
}
QCString OPimResolver::applicationChannel( const QString& service )const {
    QCString str("QPE/Application/");

    if ( isBuiltIn( service ) ) {
        if ( service == "Todolist" )
            str += "todolist";
        else if ( service == "Datebook" )
            str += "datebook";
        else if ( service == "Addressbook" )
            str += "addressbook";
    }else
        ; // FIXME for additional stuff

    return str;
}
QStringList OPimResolver::services()const {
    return m_builtIns;
}
QString OPimResolver::serviceName( int rtti ) const{
    QString str;
    switch ( rtti ) {
    case TodoList:
        str = "Todolist";
        break;
    case DateBook:
        str = "Datebook";
        break;
    case AddressBook:
        str = "Addressbook";
        break;
    default:
        break;
    }
    return str;
    // FIXME me for 3rd party
}
int OPimResolver::serviceId( const QString& service ) {
    int rtti = 0;
    if ( service == "Todolist" )
        rtti = TodoList;
    else if ( service == "Datebook" )
        rtti = DateBook;
    else if ( service == "Addressbook" )
        rtti = AddressBook;

    return rtti;
}
/**
 * check if the 'service' is registered and if so we'll
 */
bool OPimResolver::add( const QString& service,  const OPimRecord& rec) {
    if ( QCopChannel::isRegistered( applicationChannel( service ) ) ) {
        QByteArray data;
        QDataStream arg(data, IO_WriteOnly );
        if ( rec.saveToStream( arg ) ) {
            QCopEnvelope env( applicationChannel( service ), "add(int,QByteArray)" );
            env << rec.rtti();
            env << data;
        }else
            return false;
    }else{
        OPimBase* base = backend( service );
        if (!base ) return false;

        base->load();
        base->add( rec );
        base->save();
        delete base;
    }

    return true;
}
OPimBase* OPimResolver::backend( const QString& service ) {
    return backend( serviceId( service ) );
}
OPimBase* OPimResolver::backend( int rtti ) {
    OPimBase* base = 0l;
    switch( rtti ) {
    case TodoList:
        base = new OTodoAccess();
        break;
    case DateBook:
        break;
    case AddressBook:
        base = new OContactAccess("Resolver");
        break;
    default:
        break;
    }
    // FIXME for 3rd party
    if (!base )
        ;

    return base;
}
