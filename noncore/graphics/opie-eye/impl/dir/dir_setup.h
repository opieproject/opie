#ifndef _DIRSETUP_H
#define _DIRSETUP_H

#include <qframe.h>

class QCheckBox;
class QSpinBox;
class QLabel;

class DirImageWidget : public QFrame {
    Q_OBJECT
public:
    DirImageWidget();
    ~DirImageWidget();
    QCheckBox* chkbox,*recBox;
    QSpinBox * recDepth;
    QLabel* recDepthLabel,*recWarningLabel;
protected slots:
    void recBoxToggled(bool);
};

#endif
