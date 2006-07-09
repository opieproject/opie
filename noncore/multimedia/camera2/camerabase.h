/****************************************************************************
** Form interface generated from reading ui file 'camerabase.ui'
**
** Created: Mon Jul 10 04:21:22 2006
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef CAMERA_H
#define CAMERA_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QFrame;
class QPushButton;
class ThumbButton;
class VideoCaptureView;

class CameraBase : public QWidget
{ 
    Q_OBJECT

public:
    CameraBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CameraBase();

    QFrame* Frame3;
    VideoCaptureView* videocaptureview;
    QPushButton* photo;
    QPushButton* video;
    QFrame* thumbs;
    ThumbButton* thumb1;
    ThumbButton* thumb2;
    ThumbButton* thumb3;
    ThumbButton* thumb4;
    ThumbButton* thumb5;

protected:
    QGridLayout* cameraLayout;
    QVBoxLayout* Frame3Layout;
    QHBoxLayout* thumbsLayout;
};

#endif // CAMERA_H
