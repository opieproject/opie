/****************************************************************************
** Form interface generated from reading ui file 'qsmbbase.ui'
**
** Created: Thu Aug 11 19:30:41 2005
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef FORMQPESMBBASE_H
#define FORMQPESMBBASE_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QListView;
class QListViewItem;
class QPushButton;
class QTabWidget;
class QTextView;

class FormQPESMBBase : public QWidget
{ 
    Q_OBJECT

public:
    FormQPESMBBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FormQPESMBBase();

    QTabWidget* TabWidget2;
    QWidget* tab;
    QComboBox* CBHost;
    QComboBox* mountpt;
    QLabel* LScan;
    QLabel* textLabel1;
    QListView* ListViewScan;
    QPushButton* BtnScan;
    QPushButton* BtnClear;
    QCheckBox* onbootBtn;
    QPushButton* DoItBtn;
    QLabel* pwdlabel;
    QLineEdit* password;
    QLabel* usrlabel;
    QLineEdit* username;
    QWidget* tab_2;
    QTextView* TextViewOutput;

protected:
    QGridLayout* FormQPESMBBaseLayout;
    QGridLayout* tabLayout;
    QGridLayout* Layout6;
    QHBoxLayout* Layout2;
    QHBoxLayout* Layout5;
    QGridLayout* tabLayout_2;
};

#endif // FORMQPESMBBASE_H
