/****************************************************************************
** MainView meta object code from reading C++ file 'mainview.h'
**
** Created: Thu Jul 25 10:24:03 2002
**      by: The Qt MOC ($Id: moc_mainview.cpp,v 1.3 2002-07-25 16:38:22 spiralman Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "mainview.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *MainView::className() const
{
    return "MainView";
}

QMetaObject *MainView::metaObj = 0;

void MainView::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QMainWindow::className(), "QMainWindow") != 0 )
	badSuperclassWarning("MainView","QMainWindow");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString MainView::tr(const char* s)
{
    return qApp->translate( "MainView", s, 0 );
}

QString MainView::tr(const char* s, const char * c)
{
    return qApp->translate( "MainView", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* MainView::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QMainWindow::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (MainView::*m1_t0)();
    typedef void (QObject::*om1_t0)();
    typedef void (MainView::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    m1_t0 v1_0 = &MainView::goClicked;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &MainView::textChanged;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    QMetaData *slot_tbl = QMetaObject::new_metadata(2);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(2);
    slot_tbl[0].name = "goClicked()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Public;
    slot_tbl[1].name = "textChanged()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Public;
    metaObj = QMetaObject::new_metaobject(
	"MainView", "QMainWindow",
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
