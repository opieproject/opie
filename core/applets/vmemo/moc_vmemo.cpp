/****************************************************************************
** VMemo meta object code from reading C++ file 'vmemo.h'
**
** Created: Wed Feb 13 17:52:12 2002
**      by: The Qt MOC ($Id: moc_vmemo.cpp,v 1.1 2002-02-14 01:39:21 llornkcor Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "vmemo.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *VMemo::className() const
{
    return "VMemo";
}

QMetaObject *VMemo::metaObj = 0;

void VMemo::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QWidget::className(), "QWidget") != 0 )
	badSuperclassWarning("VMemo","QWidget");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString VMemo::tr(const char* s)
{
    return qApp->translate( "VMemo", s, 0 );
}

QString VMemo::tr(const char* s, const char * c)
{
    return qApp->translate( "VMemo", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* VMemo::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QWidget::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (VMemo::*m1_t0)();
    typedef void (QObject::*om1_t0)();
    typedef void (VMemo::*m1_t1)(QMouseEvent*);
    typedef void (QObject::*om1_t1)(QMouseEvent*);
    typedef void (VMemo::*m1_t2)(QMouseEvent*);
    typedef void (QObject::*om1_t2)(QMouseEvent*);
    m1_t0 v1_0 = &VMemo::record;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &VMemo::mousePressEvent;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &VMemo::mouseReleaseEvent;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    QMetaData *slot_tbl = QMetaObject::new_metadata(3);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(3);
    slot_tbl[0].name = "record()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "mousePressEvent(QMouseEvent*)";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Public;
    slot_tbl[2].name = "mouseReleaseEvent(QMouseEvent*)";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"VMemo", "QWidget",
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
