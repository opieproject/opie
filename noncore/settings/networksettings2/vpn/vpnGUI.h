/****************************************************************************
** Form interface generated from reading ui file 'vpnGUI.ui'
**
** Created: Tue Mar 30 02:42:50 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef VPN_FRM_H
#define VPN_FRM_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QLabel;
class QLineEdit;
class QMultiLineEdit;

class VPNGUI : public QWidget
{ 
    Q_OBJECT

public:
    VPNGUI( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~VPNGUI();

    QLabel* TextLabel4;
    QLineEdit* Name_LE;
    QLabel* TextLabel3;
    QMultiLineEdit* Description_LE;
    QCheckBox* Automatic_CB;
    QCheckBox* Confirm_CB;

protected:
    QVBoxLayout* VPN_FRMLayout;
    QHBoxLayout* Layout4;
    QHBoxLayout* Layout5;
};

#endif // VPN_FRM_H
