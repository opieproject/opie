/****************************************************************************
** Form interface generated from reading ui file 'soundsettingsbase.ui'
**
** Created: Thu May 23 11:23:38 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef SOUNDSETTINGSBASE_H
#define SOUNDSETTINGSBASE_H

#include <qvariant.h>
#include <qdialog.h>
#include <qmainwindow.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QSlider;

class SoundSettingsBase : public QMainWindow
{ 
    Q_OBJECT

public:
    SoundSettingsBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SoundSettingsBase();

    QLabel* PixmapLabel1_2;
    QLabel* TextLabel1_2;
/*     QLabel* volLabel; */
/*     QLabel* micLabel; */
/*     QSlider* volume; */
/*     QSlider* mic; */
/*     QLabel* PixmapLabel2_2; */
/*     QLabel* TextLabel2_2; */
/*     QGroupBox* GroupBox1; */
    QLabel* sampleRateLabel;
    QComboBox* sampleRate;
    QCheckBox* stereoCheckBox;
    QCheckBox* sixteenBitCheckBox;
    QCheckBox* AlertCheckBox;
    QLabel* TextLabel1;
    QComboBox* LocationComboBox;
    QComboBox* keyComboBox;
    QComboBox* timeLimitComboBox;
    bool keyReset;
protected:
    QGridLayout* SoundSettingsBaseLayout;
protected slots:
        
};

#endif // SOUNDSETTINGSBASE_H
