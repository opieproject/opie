#include <qlabel.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qhbuttongroup.h>
#include <qradiobutton.h>

#include "iolayerbase.h"

namespace {
    enum ParityIds {
        id_parity_none,
        id_parity_odd,
        id_parity_even
    };

    enum FlowIds {
        id_flow_hw,
        id_flow_sw,
        id_flow_none,
    };

    enum SpeedIds {
        id_baud_115200,
        id_baud_57600,
        id_baud_38400,
        id_baud_19200,
        id_baud_9600
    };

}


IOLayerBase::IOLayerBase( QWidget* par,  const char* name )
    : QWidget( par, name )
{
    m_speedLabel = new QLabel(tr("Speed"), this );
    m_speedBox = new QComboBox(this );

    m_groupFlow = new QButtonGroup(tr("Flow control"),this );
    m_flowHw = new QRadioButton(tr("Hardware"), m_groupFlow );
    m_flowSw = new QRadioButton(tr("Software"), m_groupFlow );
    m_flowNone = new QRadioButton( tr("None"), m_groupFlow );

    m_groupParity = new QButtonGroup(tr("Parity"), this );
    m_parityNone = new QRadioButton(tr("None"), m_groupParity );
    m_parityOdd = new QRadioButton(tr("Odd"), m_groupParity );
    m_parityEven = new QRadioButton(tr("Even"), m_groupParity );

    m_lroot = new QVBoxLayout( this );
    m_lroot->add(m_speedLabel );
    m_lroot->add(m_speedBox );
    m_lroot->setStretchFactor(m_speedLabel, 1);
    m_lroot->setStretchFactor(m_speedBox, 1 );

    m_hbox = new QHBoxLayout(m_groupFlow, 2 );
    m_hbox->add(m_flowHw );
    m_hbox->add(m_flowSw );
    m_hbox->add(m_flowNone );
    m_lroot->add(m_groupFlow );
    m_lroot->setStretchFactor(m_groupFlow, 2 );

    m_hboxPar = new QHBoxLayout( m_groupParity, 2 );
    m_hboxPar->add(m_parityOdd );
    m_hboxPar->add(m_parityEven );
    m_hboxPar->add(m_parityNone );
    m_lroot->add(m_groupParity );
    m_lroot->setStretchFactor(m_groupParity, 2 );
    m_lroot->addStretch(2);

    // profiles
    m_speedBox->insertItem(tr("115200 baud"), id_baud_115200 );
    m_speedBox->insertItem(tr("57600 baud"), id_baud_57600   );
    m_speedBox->insertItem(tr("38400 baud"), id_baud_38400   );
    m_speedBox->insertItem(tr("19200 baud"), id_baud_19200   );
    m_speedBox->insertItem(tr("9600 baud"), id_baud_9600     );

};
IOLayerBase::~IOLayerBase() {

}
void IOLayerBase::setFlow( Flow flo ) {
    switch ( flo ) {
    case Software:
        m_flowSw->setChecked( true );
        break;
    case Hardware:
        m_flowHw->setChecked( true );
        break;
    case None:
        m_flowNone->setChecked( true );
        break;
    }
}

void IOLayerBase::setParity( Parity par ) {
    switch( par ) {
    case NonePar:
        m_parityNone->setChecked( true );
        break;
    case Odd:
        m_parityOdd->setChecked( true );
        break;
    case Even:
        m_parityEven->setChecked( true );
        break;
    }
}
void IOLayerBase::setSpeed( Speed sp ) {
    int index;
    switch( sp ) {
    case Baud_115200:
        index = id_baud_115200;
        break;
    case Baud_57600:
        index = id_baud_57600;
        break;
    case Baud_38400:
        index = id_baud_38400;
        break;
    case Baud_19200:
        index = id_baud_19200;
        break;
    case Baud_9600:
        index = id_baud_9600;
        break;
    }
    m_speedBox->setCurrentItem(index );
}
IOLayerBase::Flow IOLayerBase::flow()const {
    if (m_flowHw->isChecked() ) {
        return Hardware;
    }else if( m_flowSw->isChecked() ) {
        return Software;
    } else {
        return None;
    }
}
IOLayerBase::Parity IOLayerBase::parity()const {
    if ( m_parityOdd->isChecked() ) {
        return Odd;
    } else if  ( m_parityEven->isChecked() ) {
        return Even;
    } else {
        return NonePar;
    }

}
IOLayerBase::Speed IOLayerBase::speed()const{
    switch( m_speedBox->currentItem() ) {
    case id_baud_115200:
        return Baud_115200;
        break;
    case id_baud_57600:
        return Baud_57600;
        break;
    case id_baud_38400:
        return Baud_38400;
        break;
    case id_baud_19200:
        return Baud_19200;
        break;
    case id_baud_9600:
        return Baud_9600;
        break;
    }
}
