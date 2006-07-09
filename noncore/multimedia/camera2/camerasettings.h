/****************************************************************************
** Form interface generated from reading ui file 'camerasettings.ui'
**
** Created: Mon Jul 10 04:21:22 2006
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef CAMERASETTINGS_H
#define CAMERASETTINGS_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class LocationCombo;
class QComboBox;
class QGroupBox;
class QLabel;
class QSlider;

class CameraSettings : public QDialog
{ 
    Q_OBJECT

public:
    CameraSettings( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~CameraSettings();

    LocationCombo* location;
    QGroupBox* photo;
    QLabel* TextLabel1;
    QComboBox* photo_size;
    QLabel* TextLabel2;
    QSlider* photo_quality;
    QLabel* photo_quality_n;
    QGroupBox* video;
    QLabel* TextLabel1_2;
    QComboBox* video_size;
    QLabel* TextLabel2_2;
    QSlider* video_quality;
    QLabel* video_quality_n;
    QLabel* TextLabel4;
    QSlider* video_framerate;
    QLabel* TextLabel6;

protected:
    QVBoxLayout* CameraSettingsLayout;
    QVBoxLayout* photoLayout;
    QHBoxLayout* Layout1;
    QHBoxLayout* Layout2;
    QVBoxLayout* videoLayout;
    QHBoxLayout* Layout1_2;
    QHBoxLayout* Layout2_2;
    QHBoxLayout* Layout6;
};

#endif // CAMERASETTINGS_H
