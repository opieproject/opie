/****************************************************************************
** Form interface generated from reading ui file 'interfaceedit.ui'
**
** Created: Mon Sep 23 12:18:55 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef INTERFACECONFIGURATION_H
#define INTERFACECONFIGURATION_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QComboBox;
class QFrame;
class QGroupBox;
class QLabel;
class QLineEdit;
class QSpinBox;

class InterfaceConfiguration : public QWidget
{ 
    Q_OBJECT

public:
    InterfaceConfiguration( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~InterfaceConfiguration();

    QComboBox* profile;
    QLabel* TextLabel1;
    QFrame* Line1;
    QCheckBox* CheckBox3;
    QCheckBox* dhcpCheckBox;
    QLabel* TextLabel3_3_2;
    QSpinBox* SpinBox1_2;
    QLabel* TextLabel4;
    QLineEdit* ipAddressEdit;
    QLabel* TextLabel5;
    QLineEdit* firstDNSLineEdit;
    QLabel* TextLabel3;
    QLineEdit* subnetMaskEdit;
    QLineEdit* gatewayEdit;
    QLabel* TextLabel7;
    QLabel* TextLabel2;
    QLineEdit* secondDNSLineEdit;
    QGroupBox* GroupBox2;

protected:
    QGridLayout* InterfaceConfigurationLayout;
};

#endif // INTERFACECONFIGURATION_H
