/****************************************************************************
** Form interface generated from reading ui file 'KHCWidgetBase.ui'
**
** Created: Mon Feb 28 09:54:07 2005
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef KHCWIDGETBASE_H
#define KHCWIDGETBASE_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QLabel;
class QLineEdit;
class QMultiLineEdit;
class QPushButton;

class KHCWidgetBase : public QWidget
{ 
    Q_OBJECT

public:
    KHCWidgetBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KHCWidgetBase();

    QPushButton* btnCtrl;
    QPushButton* btnGen;
    QPushButton* btnCopy;
    QLabel* lblOrgK;
    QLabel* lblMapK;
    QLabel* lblMapKeycode;
    QLineEdit* txtMapKey;
    QLineEdit* txtOrgKey;
    QLabel* lblOriginal;
    QLabel* lblMapping;
    QMultiLineEdit* mleDefine;
    QLabel* lblOrgKeycode;
    QLabel* lblOrgU;
    QLabel* lblOrgUnicode;
    QLabel* lblMapU;
    QLabel* lblMapUnicode;
    QCheckBox* chkMapShift;
    QCheckBox* chkMapCtrl;
    QCheckBox* chkMapAlt;
    QCheckBox* chkOrgShift;
    QCheckBox* chkOrgCtrl;
    QCheckBox* chkOrgAlt;

};

#endif // KHCWIDGETBASE_H
