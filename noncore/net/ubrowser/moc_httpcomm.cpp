/****************************************************************************
** HttpComm meta object code from reading C++ file 'httpcomm.h'
**
** Created: Fri Jul 19 21:08:51 2002
**      by: The Qt MOC ($Id: moc_httpcomm.cpp,v 1.2 2002-07-20 03:10:26 llornkcor Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "httpcomm.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *HttpComm::className() const
{
    return "HttpComm";
}

QMetaObject *HttpComm::metaObj = 0;

void HttpComm::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QObject::className(), "QObject") != 0 )
	badSuperclassWarning("HttpComm","QObject");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString HttpComm::tr(const char* s)
{
    return qApp->translate( "HttpComm", s, 0 );
}

QString HttpComm::tr(const char* s, const char * c)
{
    return qApp->translate( "HttpComm", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* HttpComm::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QObject::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (HttpComm::*m1_t0)();
    typedef void (QObject::*om1_t0)();
    typedef void (HttpComm::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    typedef void (HttpComm::*m1_t2)();
    typedef void (QObject::*om1_t2)();
    typedef void (HttpComm::*m1_t3)();
    typedef void (QObject::*om1_t3)();
    m1_t0 v1_0 = &HttpComm::hostFound;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &HttpComm::connected;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &HttpComm::connectionClosed;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    m1_t3 v1_3 = &HttpComm::incoming;
    om1_t3 ov1_3 = (om1_t3)v1_3;
    QMetaData *slot_tbl = QMetaObject::new_metadata(4);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(4);
    slot_tbl[0].name = "hostFound()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "connected()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "connectionClosed()";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Public;
    slot_tbl[3].name = "incoming()";
    slot_tbl[3].ptr = (QMember)ov1_3;
    slot_tbl_access[3] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"HttpComm", "QObject",
	slot_tbl, 4,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    metaObj->set_slot_access( slot_tbl_access );
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    return metaObj;
}
