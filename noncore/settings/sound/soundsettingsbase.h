/****************************************************************************
** Form interface generated from reading ui file 'soundsettingsbase.ui'
**
** Created: Sun May 19 12:11:23 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef SOUNDSETTINGSBASE_H
#define SOUNDSETTINGSBASE_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QSlider;

class SoundSettingsBase : public QDialog
{ 
    Q_OBJECT

public:
    SoundSettingsBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SoundSettingsBase();

    QGroupBox* GroupBox3;
    QLabel* PixmapLabel1_2;
    QLabel* TextLabel1_2;
    QLabel* volLabel;
    QLabel* micLabel;
    QSlider* volume;
    QSlider* mic;
    QLabel* PixmapLabel2_2;
    QLabel* TextLabel2_2;
    QGroupBox* GroupBox1;
    QLabel* sampleRateLabel;
    QComboBox* sampleRate;
    QCheckBox* stereoCheckBox;
    QCheckBox* sixteenBitCheckBox;
    QCheckBox* AlertCheckBox;
    QLabel* TextLabel1;
    QComboBox* LocationComboBox;
/*     QCheckBox* touchsound; */
/*     QCheckBox* keysound; */

protected:
    QGridLayout* SoundSettingsBaseLayout;
    QVBoxLayout* GroupBox3Layout;
    QVBoxLayout* Layout10;
    QHBoxLayout* Layout16;
    QHBoxLayout* Layout13;
    QHBoxLayout* Layout12;
    QHBoxLayout* Layout17;
    QVBoxLayout* Layout12_2;
    QVBoxLayout* Layout11;
};

#endif // SOUNDSETTINGSBASE_H
