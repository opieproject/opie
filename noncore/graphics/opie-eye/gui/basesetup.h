#ifndef _BASESETUP_H
#define _BASESETUP_H

#include <opie2/oconfig.h>

#include <qframe.h>

class QVBoxLayout;
class QGridLayout;
class QSpinBox;
class QLabel;
class QCheckBox;
class QSpacerItem;

class BaseSetup:public QFrame
{
    Q_OBJECT
public:
    BaseSetup(Opie::Core::OConfig *a_cfg,QWidget * parent=0, const char * name=0, WFlags f=0);
    virtual ~BaseSetup();

public slots:
    virtual void save_values();

protected:
    Opie::Core::OConfig *m_cfg;
    QVBoxLayout * m_MainLayout;
    QGridLayout * m_SlidetimeLayout;
    QSpinBox * m_SlideShowTime;
    QLabel * m_SlidetimeLabel;
    QCheckBox *m_SaveStateAuto;
    QSpacerItem *spacer1;
};

#endif
