/****************************************************************************
** Form interface generated from reading ui file 'otimepickerbase.ui'
**
** Created: Tue Aug 20 10:04:21 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef OTIMEPICKERDIALOGBASE_H
#define OTIMEPICKERDIALOGBASE_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class OTimePicker;
class QFrame;
class QGroupBox;
class QLabel;
class QLineEdit;

class OTimePickerDialogBase : public QDialog
{ 
    Q_OBJECT

public:
    OTimePickerDialogBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~OTimePickerDialogBase();

    QFrame* Frame10;
    QFrame* Frame4;
    QLabel* TextLabel1;
    QLineEdit* hourField;
    QLabel* TextLabel1_2;
    QLineEdit* minuteField;
    QGroupBox* GroupBox1;
    OTimePicker* m_timePicker;

protected:
    QVBoxLayout* OTimePickerDialogBaseLayout;
    QHBoxLayout* Frame10Layout;
    QHBoxLayout* Frame4Layout;
    bool event( QEvent* );
};

#endif // OTIMEPICKERDIALOGBASE_H
