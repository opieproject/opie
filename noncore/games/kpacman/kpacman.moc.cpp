/****************************************************************************
** Kpacman meta object code from reading C++ file 'kpacman.h'
**
** Created: Sat Jan 19 13:52:36 2002
**      by: The Qt MOC ($Id: kpacman.moc.cpp,v 1.1 2002-04-15 22:40:28 leseb Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "kpacman.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *Kpacman::className() const
{
    return "Kpacman";
}

QMetaObject *Kpacman::metaObj = 0;

void Kpacman::initMetaObject()
{
    if ( metaObj )
        return;
    if ( qstrcmp(QWidget::className(), "QWidget") != 0 )
        badSuperclassWarning("Kpacman","QWidget");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString Kpacman::tr(const char* s)
{
    return qApp->translate( "Kpacman", s, 0 );
}

QString Kpacman::tr(const char* s, const char * c)
{
    return qApp->translate( "Kpacman", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* Kpacman::staticMetaObject()
{
    if ( metaObj )
        return metaObj;
    (void) QWidget::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    QMetaData::Access *slot_tbl_access = 0;
    metaObj = QMetaObject::new_metaobject(
        "Kpacman", "QWidget",
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
