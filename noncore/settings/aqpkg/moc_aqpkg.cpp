/****************************************************************************
** Aqpkg meta object code from reading C++ file 'aqpkg.h'
**
** Created: Thu Aug 29 13:01:06 2002
**      by: The Qt MOC ($Id: moc_aqpkg.cpp,v 1.1 2002-09-28 23:22:41 andyq Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "aqpkg.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *Aqpkg::className() const
{
    return "Aqpkg";
}

QMetaObject *Aqpkg::metaObj = 0;

void Aqpkg::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QWidget::className(), "QWidget") != 0 )
	badSuperclassWarning("Aqpkg","QWidget");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString Aqpkg::tr(const char* s)
{
    return qApp->translate( "Aqpkg", s, 0 );
}

QString Aqpkg::tr(const char* s, const char * c)
{
    return qApp->translate( "Aqpkg", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* Aqpkg::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QWidget::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (Aqpkg::*m1_t0)(int);
    typedef void (QObject::*om1_t0)(int);
    typedef void (Aqpkg::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    typedef void (Aqpkg::*m1_t2)();
    typedef void (QObject::*om1_t2)();
    m1_t0 v1_0 = &Aqpkg::serverSelected;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &Aqpkg::applyChanges;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &Aqpkg::updateServer;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    QMetaData *slot_tbl = QMetaObject::new_metadata(3);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(3);
    slot_tbl[0].name = "serverSelected(int)";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "applyChanges()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "updateServer()";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"Aqpkg", "QWidget",
	slot_tbl, 3,
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
