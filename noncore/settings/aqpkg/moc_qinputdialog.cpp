/****************************************************************************
** QInputDialog meta object code from reading C++ file 'qinputdialog.h'
**
** Created: Sun Sep 8 17:28:15 2002
**      by: The Qt MOC ($Id: moc_qinputdialog.cpp,v 1.1 2002-09-28 23:22:41 andyq Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "qinputdialog.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *QInputDialog::className() const
{
    return "QInputDialog";
}

QMetaObject *QInputDialog::metaObj = 0;

void QInputDialog::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QDialog::className(), "QDialog") != 0 )
	badSuperclassWarning("QInputDialog","QDialog");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString QInputDialog::tr(const char* s)
{
    return qApp->translate( "QInputDialog", s, 0 );
}

QString QInputDialog::tr(const char* s, const char * c)
{
    return qApp->translate( "QInputDialog", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* QInputDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QDialog::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (QInputDialog::*m1_t0)(const QString&);
    typedef void (QObject::*om1_t0)(const QString&);
    typedef void (QInputDialog::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    m1_t0 v1_0 = &QInputDialog::textChanged;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &QInputDialog::tryAccept;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    QMetaData *slot_tbl = QMetaObject::new_metadata(2);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(2);
    slot_tbl[0].name = "textChanged(const QString&)";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Private;
    slot_tbl[1].name = "tryAccept()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Private;
    metaObj = QMetaObject::new_metaobject(
	"QInputDialog", "QDialog",
	slot_tbl, 2,
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
