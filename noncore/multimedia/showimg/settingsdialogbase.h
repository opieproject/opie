/****************************************************************************
** Form interface generated from reading ui file 'settingsdialogbase.ui'
**
** Created: Sun Nov 3 07:29:03 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef SETTINGSDIALOGBASE_H
#define SETTINGSDIALOGBASE_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QGroupBox;
class QLabel;
class QSlider;

class SettingsDialogBase : public QDialog
{ 
    Q_OBJECT

public:
    SettingsDialogBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SettingsDialogBase();

    QGroupBox* GroupBox1;
    QLabel* TextLabel1;
    QSlider* delaySlider;
    QLabel* delayText;
    QCheckBox* repeatCheck;
    QCheckBox* reverseCheck;
    QCheckBox* rotateCheck;
    QCheckBox* fastLoadCheck;

protected:
    QVBoxLayout* SettingsDialogBaseLayout;
    QVBoxLayout* GroupBox1Layout;
    QGridLayout* Layout3;
};

#endif // SETTINGSDIALOGBASE_H
