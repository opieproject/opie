/****************************************************************************
** Form interface generated from reading ui file 'pinDialogBase.ui'
**
** Created: Mon Jun 14 11:00:59 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef PINDIALOGBASE_H
#define PINDIALOGBASE_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QLineEdit;

class PinDialogBase : public QWidget
{ 
    Q_OBJECT

public:
    PinDialogBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~PinDialogBase();

    QLabel* button_4;
    QLabel* button_7;
    QLabel* prompt;
    QLineEdit* display;
    QLabel* button_1;
    QLabel* button_2;
    QLabel* button_5;
    QLabel* button_6;
    QLabel* button_3;
    QLabel* button_0;
    QLabel* button_8;
    QLabel* button_9;
    QLabel* button_OK;
    QLabel* button_Skip;

protected:
    bool event( QEvent* );
};

#endif // PINDIALOGBASE_H
