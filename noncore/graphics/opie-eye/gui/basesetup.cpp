#include "basesetup.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qcheckbox.h>

BaseSetup::BaseSetup(Opie::Core::OConfig *a_cfg,QWidget * parent, const char * name, WFlags f)
    :QFrame(parent,name,f)
{
    setFrameStyle(Box|Raised);
    m_cfg = a_cfg;
    m_MainLayout = new QVBoxLayout( this, 11, 6, "m_MainLayout");

    m_SlidetimeLayout = new QGridLayout( 0, 1, 1, 0, 6, "m_SlidetimeLayout");

    m_SlideShowTime = new QSpinBox( this, "m_SlideShowTime" );
    m_SlideShowTime->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
    m_SlideShowTime->setButtonSymbols( QSpinBox::PlusMinus );
    m_SlideShowTime->setMaxValue( 60 );
    m_SlideShowTime->setMinValue(0);
    m_SlideShowTime->setValue( 2 );
    m_SlideShowTime->setSuffix(tr(" seconds"));

    m_SlidetimeLayout->addWidget( m_SlideShowTime, 0, 1 );

    m_SlidetimeLabel = new QLabel( this, "m_SlidetimeLabel" );
    m_SlidetimeLabel->setText(tr("Slideshow timeout:"));
    m_SlidetimeLayout->addWidget( m_SlidetimeLabel, 0, 0 );
    m_MainLayout->addLayout( m_SlidetimeLayout );

#if 0
    m_ShowToolBar = new QCheckBox( this, "m_ShowToolBar" );
    m_ShowToolBar->setText(tr("Show toolbar on startup"));
    m_MainLayout->addWidget( m_ShowToolBar );
#endif
    m_SaveStateAuto = new QCheckBox( this, "m_SaveStateAuto" );
    m_SaveStateAuto->setText(tr("Save status of fullscreen/autorotate"));
    m_MainLayout->addWidget( m_SaveStateAuto );

    spacer1 = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
    m_MainLayout->addItem( spacer1 );

    int stime = m_cfg->readNumEntry("base_slideshowtimeout",2);
    if (stime<0) stime=2;
    if (stime>60) stime=60;
    m_SlideShowTime->setValue(stime);
    m_SaveStateAuto->setChecked(m_cfg->readBoolEntry("base_savestatus",false));
}

BaseSetup::~BaseSetup()
{
}

void BaseSetup::save_values()
{
    if (!m_cfg) return;
    m_cfg->writeEntry("base_slideshowtimeout",m_SlideShowTime->value());
    m_cfg->writeEntry("base_savestatus",m_SaveStateAuto->isChecked());
}
