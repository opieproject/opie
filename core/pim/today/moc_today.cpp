/****************************************************************************
** Today meta object code from reading C++ file 'today.h'
**
** Created: Fri Feb 15 03:08:29 2002
**      by: The Qt MOC ($Id: moc_today.cpp,v 1.1 2002-02-15 16:25:32 harlekin Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "today.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *Today::className() const
{
    return "Today";
}

QMetaObject *Today::metaObj = 0;

void Today::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(TodayBase::className(), "TodayBase") != 0 )
	badSuperclassWarning("Today","TodayBase");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString Today::tr(const char* s)
{
    return qApp->translate( "Today", s, 0 );
}

QString Today::tr(const char* s, const char * c)
{
    return qApp->translate( "Today", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* Today::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) TodayBase::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (Today::*m1_t0)();
    typedef void (QObject::*om1_t0)();
    typedef void (Today::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    typedef void (Today::*m1_t2)();
    typedef void (QObject::*om1_t2)();
    typedef void (Today::*m1_t3)();
    typedef void (QObject::*om1_t3)();
    m1_t0 v1_0 = &Today::startConfig;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &Today::goodBye;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &Today::startTodo;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    m1_t3 v1_3 = &Today::startDatebook;
    om1_t3 ov1_3 = (om1_t3)v1_3;
    QMetaData *slot_tbl = QMetaObject::new_metadata(4);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(4);
    slot_tbl[0].name = "startConfig()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Private;
    slot_tbl[1].name = "goodBye()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Private;
    slot_tbl[2].name = "startTodo()";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Private;
    slot_tbl[3].name = "startDatebook()";
    slot_tbl[3].ptr = (QMember)ov1_3;
    slot_tbl_access[3] = QMetaData::Private;
    metaObj = QMetaObject::new_metaobject(
	"Today", "TodayBase",
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
