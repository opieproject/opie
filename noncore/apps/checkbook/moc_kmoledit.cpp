/****************************************************************************
** KMolEdit meta object code from reading C++ file 'kmoledit.h'
**
** Created: Sun Sep 15 15:21:49 2002
**      by: The Qt MOC ($Id: moc_kmoledit.cpp,v 1.1 2002-09-15 14:02:04 cniehaus Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "kmoledit.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *KMolEdit::className() const
{
    return "KMolEdit";
}

QMetaObject *KMolEdit::metaObj = 0;

void KMolEdit::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QDialog::className(), "QDialog") != 0 )
	badSuperclassWarning("KMolEdit","QDialog");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString KMolEdit::tr(const char* s)
{
    return qApp->translate( "KMolEdit", s, 0 );
}

QString KMolEdit::tr(const char* s, const char * c)
{
    return qApp->translate( "KMolEdit", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* KMolEdit::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QDialog::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef bool (KMolEdit::*m1_t0)();
    typedef bool (QObject::*om1_t0)();
    typedef void (KMolEdit::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    typedef void (KMolEdit::*m1_t2)();
    typedef void (QObject::*om1_t2)();
    m1_t0 v1_0 = &KMolEdit::edit;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &KMolEdit::save;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &KMolEdit::undo;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    QMetaData *slot_tbl = QMetaObject::new_metadata(3);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(3);
    slot_tbl[0].name = "edit()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Private;
    slot_tbl[1].name = "save()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Private;
    slot_tbl[2].name = "undo()";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Private;
    metaObj = QMetaObject::new_metaobject(
	"KMolEdit", "QDialog",
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
