#include <qlabel.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qhbuttongroup.h>
#include <qradiobutton.h>
#include <qhbox.h>

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

    enum DataIds {
        id_data_5,
        id_data_6,
        id_data_7,
        id_data_8
    };

    enum StopIds {
        id_stop_1,
        id_stop_1_5,
        id_stop_2
    };

}


IOLayerBase::IOLayerBase( QWidget* par,  const char* name )
    : QWidget( par, name )
{
    m_speedLabel = new QLabel(tr("Speed"), this );
    m_speedBox = new QComboBox(this );

    m_groupFlow = new QHButtonGroup(tr("Flow control"),this );
    m_flowHw = new QRadioButton(tr("Hardware"), m_groupFlow );
    m_flowSw = new QRadioButton(tr("Software"), m_groupFlow );
    m_flowNone = new QRadioButton( tr("None"), m_groupFlow );

    m_groupParity = new QHButtonGroup(tr("Parity"), this );
    m_parityNone = new QRadioButton(tr("None"), m_groupParity );
    m_parityOdd = new QRadioButton(tr("Odd"), m_groupParity );
    m_parityEven = new QRadioButton(tr("Even"), m_groupParity );

    m_groupData = new QHButtonGroup(tr("Data Bits"), this);
    m_data5 = new QRadioButton(tr("5"), m_groupData );
    m_data6 = new QRadioButton(tr("6"), m_groupData );
    m_data7 = new QRadioButton(tr("7"), m_groupData );
    m_data8 = new QRadioButton(tr("8"), m_groupData );

    m_groupStop = new QHButtonGroup(tr("Stop Bits"), this );
    m_stop1 = new QRadioButton(tr("1"), m_groupStop );
    m_stop15 = new QRadioButton(tr("1.5"), m_groupStop );
    m_stop2 = new QRadioButton(tr("2"), m_groupStop );


    m_lroot = new QVBoxLayout( this );
    m_lroot->add(m_speedLabel );
    m_lroot->add(m_speedBox );
    m_lroot->add(m_groupFlow );
    m_lroot->add(m_groupParity );
    m_lroot->add( m_groupData );
    m_lroot->add( m_groupStop );
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
    int index = -1;
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
    default:
    case Baud_9600:
        index = id_baud_9600;
        break;
    }
    m_speedBox->setCurrentItem(index );
}

void IOLayerBase::setData( Data data ) {
    switch( data ) {
    case Data_Five:
        m_data5->setChecked( true );
        break;
    case Data_Six:
        m_data6->setChecked( true );
        break;
    case Data_Seven:
        m_data7->setChecked( true );
        break;
    default:
        m_data8->setChecked( true );
        break;
    }
}


void IOLayerBase::setStop( Stop stop  ) {
    switch( stop ) {
    case Stop_Two:
        m_stop2->setChecked( true );
        break;
    case Stop_OnePointFive:
        m_stop15->setChecked( true );
        break;
    default:
        m_stop1->setChecked( true );
        break;
    }
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
    default:
    case id_baud_9600:
        return Baud_9600;
        break;
    }
}
IOLayerBase::Data IOLayerBase::data()const {
    if ( m_data5->isChecked() ) {
        return Data_Five;
    } else if  ( m_data6->isChecked() ) {
        return Data_Six;
    } else if  ( m_data7->isChecked() ) {
        return Data_Seven;
    } else {
        return Data_Eight;
    }
}
IOLayerBase::Stop IOLayerBase::stop()const {
    if ( m_stop2->isChecked() ) {
        return Stop_Two;
    } else if  ( m_stop15->isChecked() ) {
        return Stop_OnePointFive;
    } else {
        return Stop_One;
    }
}

