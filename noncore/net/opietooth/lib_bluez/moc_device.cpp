/****************************************************************************
** OpieTooth::Device meta object code from reading C++ file 'device.h'
**
** Created: Sat Apr 19 13:59:24 2003
**      by: The Qt MOC ($Id: moc_device.cpp,v 1.1.2.1 2003-05-11 17:38:22 eilers Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "device.h"
#include <qmetaobject.h>
#include <qapplication.h>
#ifdef QWS
#include <qobjectdict.h>
#endif



const char *OpieTooth::Device::className() const
{
    return "OpieTooth::Device";
}

QMetaObject *OpieTooth::Device::metaObj = 0;

#ifdef QWS
static class OpieTooth__Device_metaObj_Unloader {
public:
    ~OpieTooth__Device_metaObj_Unloader()
    {
         if ( objectDict )
             objectDict->remove( "OpieTooth::Device" );
    }
} OpieTooth__Device_metaObj_unloader;
#endif

void OpieTooth::Device::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QObject::className(), "QObject") != 0 )
	badSuperclassWarning("OpieTooth::Device","QObject");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString OpieTooth::Device::tr(const char* s)
{
    return qApp->translate( "OpieTooth::Device", s, 0 );
}

QString OpieTooth::Device::tr(const char* s, const char * c)
{
    return qApp->translate( "OpieTooth::Device", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* OpieTooth::Device::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QObject::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (OpieTooth::Device::*m1_t0)(OProcess*);
    typedef void (QObject::*om1_t0)(OProcess*);
    typedef void (OpieTooth::Device::*m1_t1)(OProcess*,char*,int);
    typedef void (QObject::*om1_t1)(OProcess*,char*,int);
    typedef void (OpieTooth::Device::*m1_t2)(OProcess*,char*,int);
    typedef void (QObject::*om1_t2)(OProcess*,char*,int);
    m1_t0 v1_0 = &OpieTooth::Device::slotExited;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &OpieTooth::Device::slotStdOut;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &OpieTooth::Device::slotStdErr;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    QMetaData *slot_tbl = QMetaObject::new_metadata(3);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(3);
    slot_tbl[0].name = "slotExited(OProcess*)";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Private;
    slot_tbl[1].name = "slotStdOut(OProcess*,char*,int)";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Private;
    slot_tbl[2].name = "slotStdErr(OProcess*,char*,int)";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Private;
    typedef void (OpieTooth::Device::*m2_t0)(const QString&,bool);
    typedef void (QObject::*om2_t0)(const QString&,bool);
    m2_t0 v2_0 = &OpieTooth::Device::device;
    om2_t0 ov2_0 = (om2_t0)v2_0;
    QMetaData *signal_tbl = QMetaObject::new_metadata(1);
    signal_tbl[0].name = "device(const QString&,bool)";
    signal_tbl[0].ptr = (QMember)ov2_0;
    metaObj = QMetaObject::new_metaobject(
	"OpieTooth::Device", "QObject",
	slot_tbl, 3,
	signal_tbl, 1,
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

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL device
void OpieTooth::Device::device( const QString& t0, bool t1 )
{
    // No builtin function for signal parameter type const QString&,bool
    QConnectionList *clist = receivers("device(const QString&,bool)");
    if ( !clist || signalsBlocked() )
	return;
    typedef void (QObject::*RT0)();
    typedef void (QObject::*RT1)(const QString&);
    typedef void (QObject::*RT2)(const QString&,bool);
    RT0 r0;
    RT1 r1;
    RT2 r2;
    QConnectionListIt it(*clist);
    QConnection   *c;
    QSenderObject *object;
    while ( (c=it.current()) ) {
	++it;
	object = (QSenderObject*)c->object();
	object->setSender( this );
	switch ( c->numArgs() ) {
	    case 0:
#ifdef Q_FP_CCAST_BROKEN
		r0 = reinterpret_cast<RT0>(*(c->member()));
#else
		r0 = (RT0)*(c->member());
#endif
		(object->*r0)();
		break;
	    case 1:
#ifdef Q_FP_CCAST_BROKEN
		r1 = reinterpret_cast<RT1>(*(c->member()));
#else
		r1 = (RT1)*(c->member());
#endif
		(object->*r1)(t0);
		break;
	    case 2:
#ifdef Q_FP_CCAST_BROKEN
		r2 = reinterpret_cast<RT2>(*(c->member()));
#else
		r2 = (RT2)*(c->member());
#endif
		(object->*r2)(t0, t1);
		break;
	}
    }
}
