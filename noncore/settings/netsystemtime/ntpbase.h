/****************************************************************************
** Form interface generated from reading ui file 'ntpbase.ui'
**
** Created: Mon Oct 21 21:32:43 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef NTPBASE_H
#define NTPBASE_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QComboBox;
class QFrame;
class QLabel;
class QMultiLineEdit;
class QPushButton;
class QSpinBox;
class QTabWidget;
class QTable;

class NtpBase : public QWidget
{ 
    Q_OBJECT

public:
    NtpBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~NtpBase();

    QTabWidget* TabWidgetMain;
    QWidget* tabMain;
    QWidget* FrameSystemTime;
    QWidget* tabNtp;
    QPushButton* runNtp;
    QWidget* FrameNtp;
    QLabel* TextLabel1;
    QLabel* TextLabelStartTime;
    QLabel* TextLabel3;
    QLabel* TextLabelTimeShift;
    QLabel* TextLabel5;
    QLabel* TextLabelNewTime;
    QMultiLineEdit* MultiLineEditntpOutPut;
    QWidget* tabPredict;
    QTable* TableLookups;
    QLabel* TextLabelShift;
    QLabel* TextLabel4;
    QLabel* TextLabelEstimatedShift;
    QLabel* TextLabel3_2;
    QLabel* Mean_shift_label;
    QLabel* TextLabelPredTime;
    QPushButton* PushButtonSetPredTime;
    QPushButton* PushButtonPredict;
    QWidget* TabSettings;
    QWidget* FrameSettings;
    QLabel* TextLabel7_2;
    QLabel* TextLabel2_2;
    QLabel* TextLabel1_3;
    QComboBox* ComboNtpSrv;
    QCheckBox* CheckBoxAdvSettings;
    QFrame* Line1;
    QLabel* TextLabel1_2_2;
    QLabel* TextLabel2_3;
    QLabel* TextLabel3_3_2;
    QSpinBox* SpinBoxNtpDelay;
    QLabel* TextLabel1_2;
    QLabel* TextLabel2;
    QLabel* TextLabel3_3;
    QSpinBox* SpinBoxMinLookupDelay;
    QWidget* tabManualSetTime;
    QWidget* FrameSetTime;
    QPushButton* PushButtonSetManualTime;

protected:
    QGridLayout* NtpBaseLayout;
    QGridLayout* tabMainLayout;
    QGridLayout* tabNtpLayout;
    QGridLayout* FrameNtpLayout;
    QVBoxLayout* Layout4;
    QGridLayout* tabPredictLayout;
    QGridLayout* Layout9;
    QHBoxLayout* Layout11;
    QGridLayout* TabSettingsLayout;
    QVBoxLayout* FrameSettingsLayout;
    QGridLayout* Layout6;
    QGridLayout* Layout7_2;
    QGridLayout* Layout7;
    QVBoxLayout* tabManualSetTimeLayout;
    bool event( QEvent* );
};

#endif // NTPBASE_H
