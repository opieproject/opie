/****************************************************************************
** todayconfig meta object code from reading C++ file 'todayconfig.h'
**
** Created: Fri Feb 15 02:54:53 2002
**      by: The Qt MOC ($Id: moc_todayconfig.cpp,v 1.1 2002-02-15 16:25:32 harlekin Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "todayconfig.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *todayconfig::className() const
{
    return "todayconfig";
}

QMetaObject *todayconfig::metaObj = 0;

void todayconfig::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QDialog::className(), "QDialog") != 0 )
	badSuperclassWarning("todayconfig","QDialog");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString todayconfig::tr(const char* s)
{
    return qApp->translate( "todayconfig", s, 0 );
}

QString todayconfig::tr(const char* s, const char * c)
{
    return qApp->translate( "todayconfig", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* todayconfig::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QDialog::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    QMetaData::Access *slot_tbl_access = 0;
    metaObj = QMetaObject::new_metaobject(
	"todayconfig", "QDialog",
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
