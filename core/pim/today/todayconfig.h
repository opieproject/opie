/****************************************************************************
** Form interface generated from reading ui file 'todayconfig.ui'
**
** Created: Thu Feb 14 15:04:33 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef TODAYCONFIG_H
#define TODAYCONFIG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QFrame;
class QLabel;
class QSpinBox;
class QTabWidget;
class QWidget;

class todayconfig : public QDialog
{ 
    Q_OBJECT

public:
    todayconfig( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~todayconfig();

    QTabWidget* TabWidget3;
    QWidget* tab;
    QFrame* Frame8;
    QLabel* TextLabel4;
    QLabel* TextLabel5;
    QCheckBox* CheckBox3;
    QCheckBox* CheckBox2;
    QCheckBox* CheckBox1;
    QSpinBox* SpinBox1;
    QLabel* TextLabel3;
    QWidget* tab_2;
    QFrame* Frame9;
    QLabel* TextLabel6;
    QSpinBox* SpinBox2;
    QWidget* tab_3;
    QFrame* Frame14;
    QLabel* TextLabel1;
    QSpinBox* SpinBox7;

};

#endif // TODAYCONFIG_H
