/****************************************************************************
** ScQtFileEditDlg meta object code from reading C++ file 'scqtfileedit.h'
**
** $Id: scqtfileedit.moc.cpp,v 1.1 2003-07-22 19:23:20 zcarsten Exp $
**
** Created: Sun Jun 9 16:09:49 2002
**      by: The Qt MOC ($Id: scqtfileedit.moc.cpp,v 1.1 2003-07-22 19:23:20 zcarsten Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "scqtfileedit.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *ScQtFileEditDlg::className() const
{
    return "ScQtFileEditDlg";
}

QMetaObject *ScQtFileEditDlg::metaObj = 0;

void ScQtFileEditDlg::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(ScQtFileDlg::className(), "ScQtFileDlg") != 0 )
	badSuperclassWarning("ScQtFileEditDlg","ScQtFileDlg");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString ScQtFileEditDlg::tr(const char* s)
{
    return qApp->translate( "ScQtFileEditDlg", s, 0 );
}

QString ScQtFileEditDlg::tr(const char* s, const char * c)
{
    return qApp->translate( "ScQtFileEditDlg", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* ScQtFileEditDlg::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) ScQtFileDlg::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (ScQtFileEditDlg::*m1_t0)();
    typedef void (QObject::*om1_t0)();
    typedef void (ScQtFileEditDlg::*m1_t1)();
    typedef void (QObject::*om1_t1)();
    typedef void (ScQtFileEditDlg::*m1_t2)(QListViewItem*);
    typedef void (QObject::*om1_t2)(QListViewItem*);
    typedef void (ScQtFileEditDlg::*m1_t3)(QListViewItem*);
    typedef void (QObject::*om1_t3)(QListViewItem*);
    typedef void (ScQtFileEditDlg::*m1_t4)(int);
    typedef void (QObject::*om1_t4)(int);
    typedef void (ScQtFileEditDlg::*m1_t5)(int);
    typedef void (QObject::*om1_t5)(int);
    typedef void (ScQtFileEditDlg::*m1_t6)(const QString&);
    typedef void (QObject::*om1_t6)(const QString&);
    typedef void (ScQtFileEditDlg::*m1_t7)();
    typedef void (QObject::*om1_t7)();
    typedef void (ScQtFileEditDlg::*m1_t8)();
    typedef void (QObject::*om1_t8)();
    m1_t0 v1_0 = &ScQtFileEditDlg::slotOK;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    m1_t1 v1_1 = &ScQtFileEditDlg::slotCancel;
    om1_t1 ov1_1 = (om1_t1)v1_1;
    m1_t2 v1_2 = &ScQtFileEditDlg::slotDoubleClicked;
    om1_t2 ov1_2 = (om1_t2)v1_2;
    m1_t3 v1_3 = &ScQtFileEditDlg::slotSelectionChanged;
    om1_t3 ov1_3 = (om1_t3)v1_3;
    m1_t4 v1_4 = &ScQtFileEditDlg::slotDirComboBoxChanged;
    om1_t4 ov1_4 = (om1_t4)v1_4;
    m1_t5 v1_5 = &ScQtFileEditDlg::slotTypeComboBoxChanged;
    om1_t5 ov1_5 = (om1_t5)v1_5;
    m1_t6 v1_6 = &ScQtFileEditDlg::slotFileTextChanged;
    om1_t6 ov1_6 = (om1_t6)v1_6;
    m1_t7 v1_7 = &ScQtFileEditDlg::slotShowDir;
    om1_t7 ov1_7 = (om1_t7)v1_7;
    m1_t8 v1_8 = &ScQtFileEditDlg::slotMkDir;
    om1_t8 ov1_8 = (om1_t8)v1_8;
    QMetaData *slot_tbl = QMetaObject::new_metadata(9);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(9);
    slot_tbl[0].name = "slotOK()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Private;
    slot_tbl[1].name = "slotCancel()";
    slot_tbl[1].ptr = (QMember)ov1_1;
    slot_tbl_access[1] = QMetaData::Private;
    slot_tbl[2].name = "slotDoubleClicked(QListViewItem*)";
    slot_tbl[2].ptr = (QMember)ov1_2;
    slot_tbl_access[2] = QMetaData::Private;
    slot_tbl[3].name = "slotSelectionChanged(QListViewItem*)";
    slot_tbl[3].ptr = (QMember)ov1_3;
    slot_tbl_access[3] = QMetaData::Private;
    slot_tbl[4].name = "slotDirComboBoxChanged(int)";
    slot_tbl[4].ptr = (QMember)ov1_4;
    slot_tbl_access[4] = QMetaData::Private;
    slot_tbl[5].name = "slotTypeComboBoxChanged(int)";
    slot_tbl[5].ptr = (QMember)ov1_5;
    slot_tbl_access[5] = QMetaData::Private;
    slot_tbl[6].name = "slotFileTextChanged(const QString&)";
    slot_tbl[6].ptr = (QMember)ov1_6;
    slot_tbl_access[6] = QMetaData::Private;
    slot_tbl[7].name = "slotShowDir()";
    slot_tbl[7].ptr = (QMember)ov1_7;
    slot_tbl_access[7] = QMetaData::Private;
    slot_tbl[8].name = "slotMkDir()";
    slot_tbl[8].ptr = (QMember)ov1_8;
    slot_tbl_access[8] = QMetaData::Private;
    metaObj = QMetaObject::new_metaobject(
	"ScQtFileEditDlg", "ScQtFileDlg",
	slot_tbl, 9,
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


const char *ScQtFileEdit::className() const
{
    return "ScQtFileEdit";
}

QMetaObject *ScQtFileEdit::metaObj = 0;

void ScQtFileEdit::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QObject::className(), "QObject") != 0 )
	badSuperclassWarning("ScQtFileEdit","QObject");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString ScQtFileEdit::tr(const char* s)
{
    return qApp->translate( "ScQtFileEdit", s, 0 );
}

QString ScQtFileEdit::tr(const char* s, const char * c)
{
    return qApp->translate( "ScQtFileEdit", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* ScQtFileEdit::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QObject::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    QMetaData::Access *slot_tbl_access = 0;
    metaObj = QMetaObject::new_metaobject(
	"ScQtFileEdit", "QObject",
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
