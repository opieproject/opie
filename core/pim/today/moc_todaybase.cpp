/****************************************************************************
** TodayBase meta object code from reading C++ file 'todaybase.h'
**
** Created: Fri Feb 15 04:02:12 2002
**      by: The Qt MOC ($Id: moc_todaybase.cpp,v 1.1 2002-02-15 16:25:32 harlekin Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "todaybase.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *TodayBase::className() const
{
    return "TodayBase";
}

QMetaObject *TodayBase::metaObj = 0;

void TodayBase::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QWidget::className(), "QWidget") != 0 )
	badSuperclassWarning("TodayBase","QWidget");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString TodayBase::tr(const char* s)
{
    return qApp->translate( "TodayBase", s, 0 );
}

QString TodayBase::tr(const char* s, const char * c)
{
    return qApp->translate( "TodayBase", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* TodayBase::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QWidget::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    QMetaData::Access *slot_tbl_access = 0;
    metaObj = QMetaObject::new_metaobject(
	"TodayBase", "QWidget",
	0, 0,
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
