/****************************************************************************
** Form interface generated from reading ui file 'wlanGUI.ui'
**
** Created: Tue Mar 30 02:42:55 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef WLANGUI_H
#define WLANGUI_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QProgressBar;
class QRadioButton;
class QSpinBox;
class QTabWidget;

class WLanGUI : public QWidget
{ 
    Q_OBJECT

public:
    WLanGUI( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~WLanGUI();

    QTabWidget* tabWidget;
    QWidget* ConfigPage;
    QLabel* essidLabel;
    QComboBox* essid;
    QLabel* modeLabel;
    QComboBox* mode;
    QCheckBox* specifyAp;
    QLabel* macLabel;
    QLineEdit* macEdit;
    QCheckBox* specifyChan;
    QSpinBox* networkChannel;
    QWidget* WepPage;
    QCheckBox* wepEnabled;
    QButtonGroup* KeyButtonGroup;
    QRadioButton* keyRadio0;
    QRadioButton* keyRadio3;
    QLineEdit* LineEdit6;
    QRadioButton* keyRadio1;
    QRadioButton* keyRadio2;
    QLineEdit* LineEdit6_2;
    QLineEdit* LineEdit6_4;
    QLineEdit* LineEdit6_3;
    QButtonGroup* NonEncButtonGroup;
    QRadioButton* acceptNonEnc;
    QRadioButton* rejectNonEnc;
    QWidget* tab;
    QLabel* TextLabel1_3;
    QLabel* Channel_LBL;
    QLabel* TextLabel4_3;
    QLabel* TextLabel3_3;
    QLabel* TextLabel2_3;
    QLabel* TextLabel9_2;
    QLabel* Station_LBL;
    QLabel* Rate_LBL;
    QLabel* TextLabel4_2_2;
    QLabel* AP_LBL;
    QLabel* essidLabel_3;
    QLabel* modeLabel_3;
    QGroupBox* GroupBox1;
    QLabel* TextLabel2_2_2;
    QLabel* TextLabel3_2_2;
    QProgressBar* Noise_PB;
    QProgressBar* Quality_PB;
    QLabel* TextLabel1_2_2;
    QProgressBar* Signal_PB;

protected:
    QVBoxLayout* WLanGUILayout;
    QVBoxLayout* ConfigPageLayout;
    QGridLayout* Layout6;
    QGridLayout* Layout8;
    QHBoxLayout* Layout7;
    QVBoxLayout* WepPageLayout;
    QGridLayout* KeyButtonGroupLayout;
    QGridLayout* NonEncButtonGroupLayout;
    QVBoxLayout* tabLayout;
    QGridLayout* Layout12;
    QGridLayout* GroupBox1Layout;
};

#endif // WLANGUI_H
