#include <qapplication.h>
#include <qcopchannel_qws.h>

#include <qpe/qcopenvelope_qws.h>

#include "opimresolver.h"
#include "opimmainwindow.h"

OPimMainWindow::OPimMainWindow( const QString& service, QWidget* parent,
                                const char* name, WFlags flag )
    : QMainWindow( parent, name, flag ), m_rtti(-1), m_service( service ), m_fallBack(0l) {

    /*
     * let's generate our QCopChannel
     */
    m_str = QString("QPE/"+m_service).local8Bit();
    m_channel= new QCopChannel(m_str, this );
    connect(m_channel, SIGNAL(received(const QCString&, const QByteArray& ) ),
            this, SLOT( appMessage( const QCString&, const QByteArray& ) ) );

    /* connect flush and reload */
    connect(qApp, SIGNAL(flush() ),
            this, SLOT(flush() ) );
    connect(qApp, SIGNAL(reload() ),
            this, SLOT(reload() ) );
}
OPimMainWindow::~OPimMainWindow() {
    delete m_channel;
}
QCopChannel* OPimMainWindow::channel() {
    return m_channel;
}
void OPimMainWindow::doSetDocument( const QString&  ) {

}
void OPimMainWindow::appMessage( const QCString& cmd, const QByteArray& array ) {
    /*
     * create demands to create
     * a new record...
     */
    QDataStream stream(array, IO_ReadOnly);
    if ( cmd == "create()" ) {
        int uid = create();
        QCopEnvelope e(m_str, "created(int)" );
        e << uid;
    }else if ( cmd == "remove(int)" ) {
        int uid;
        stream >> uid;
        bool rem = remove( uid );
        QCopEnvelope e(m_str, "removed(bool)" );
        e << rem;
    }else if ( cmd == "beam(int,int)" ) {
        int uid, trans;
        stream >> uid;
        stream >> trans;
        beam( uid, trans );
    }else if ( cmd == "show(int)" ) {
        int uid;
        stream >> uid;
        show( uid );
    }else if ( cmd == "edit(int)" ) {
        int uid;
        stream >> uid;
        edit( uid );
    }else if ( cmd == "add(int,QByteArray)" ) {
        int rtti;
        QByteArray array;
        stream >> rtti;
        stream >> array;
        m_fallBack = record(rtti, array );
        if (!m_fallBack) return;
        add( *m_fallBack );
        delete m_fallBack;
    }
}
/* implement the url scripting here */
void OPimMainWindow::setDocument( const QString& str) {
    doSetDocument( str );
}
/*
 * we now try to get the array demarshalled
 * check if the rtti matches this one
 */
OPimRecord* OPimMainWindow::record( int rtti,  const QByteArray& array ) {
    if ( service() != rtti )
        return 0l;

    OPimRecord* record = OPimResolver::self()->record( rtti );
    QDataStream str(array, IO_ReadOnly );
    if ( !record || !record->loadFromStream(str) ) {
        delete record;
        record = 0l;
    }

    return record;
}
/*
 * get the rtti for the service
 */
int OPimMainWindow::service() {
    if ( m_rtti == -1 )
        m_rtti  =  OPimResolver::self()->serviceId( m_service );

    return m_rtti;
}
