#include <qlabel.h>
#include <qlayout.h>
#include <qcombobox.h>

#include "iolayerbase.h"
#include "irdaconfigwidget.h"

namespace {
    void setCurrent( const QString& str, QComboBox* bo ) {
        uint b = bo->count();
        for (uint i = 0; i < bo->count(); i++ ) {
            if ( bo->text(i) == str ) {
                bo->setCurrentItem( i );
                return;
            }
        }
        bo->insertItem( str );
        bo->setCurrentItem( b );
    }


}

IrdaConfigWidget::IrdaConfigWidget( const QString& name,
                                        QWidget* parent,
                                        const char* na )
    : ProfileDialogConnectionWidget( name, parent, na ) {

    m_lay = new QVBoxLayout(this );
    m_device = new QLabel(tr("Device"), this );
    m_deviceCmb = new QComboBox(this );
    m_deviceCmb->setEditable( TRUE );

    m_base = new IOLayerBase(this, "base");

    m_lay->addWidget( m_device );
    m_lay->addWidget( m_deviceCmb );
    m_lay->addWidget( m_base );

    m_deviceCmb->insertItem( "/dev/ircomm0" );
    m_deviceCmb->insertItem( "/dev/ircomm1" );
}

IrdaConfigWidget::~IrdaConfigWidget() {

}
void IrdaConfigWidget::load( const Profile& prof ) {
    int rad_flow = prof.readNumEntry("Flow");
    int rad_parity = prof.readNumEntry("Parity");
    int speed = prof.readNumEntry("Speed");

    if (rad_flow == 1) {
        m_base->setFlow( IOLayerBase::Hardware );
    } else if (rad_flow == 2) {
        m_base->setFlow( IOLayerBase::Software );
    } else if (rad_flow == 0) {
         m_base->setFlow( IOLayerBase::None );
    }

    if (rad_parity == 1) {
        m_base->setParity( IOLayerBase::Even );
    } else {
        m_base->setParity( IOLayerBase::Odd );
    }

    switch( speed ) {
    case 115200:
        m_base->setSpeed(IOLayerBase::Baud_115200 );
        break;
    case 57600:
        m_base->setSpeed( IOLayerBase::Baud_57600  );
        break;
    case 38400:
        m_base->setSpeed(IOLayerBase::Baud_38400  );
        break;
    case 19200:
        m_base->setSpeed( IOLayerBase::Baud_19200  );
        break;
    case 9600:
    default:
        m_base->setSpeed(IOLayerBase::Baud_9600 );
        break;
    }

    if ( prof.readEntry("Device").isEmpty() ) return;
    setCurrent( prof.readEntry("Device"), m_deviceCmb );

}
/*
 * save speed,
 * flow,
 * parity
 */
void IrdaConfigWidget::save( Profile& prof ) {
    int flow, parity, speed;
    prof.writeEntry("Device", m_deviceCmb->currentText() );

    switch( m_base->flow() ) {
    case IOLayerBase::None:
        flow = 0;
        break;
    case IOLayerBase::Software:
        flow = 2;
        break;
    case IOLayerBase::Hardware:
        flow = 1;
        break;
    }


    switch( m_base->parity() ) {
    case IOLayerBase::Odd:
        parity = 2;
        break;
    case IOLayerBase::Even:
        parity = 1;
        break;
    }

    switch( m_base->speed() ) {
    case IOLayerBase::Baud_115200:
        speed = 115200;
        break;
    case IOLayerBase::Baud_57600:
        speed = 57600;
        break;
    case IOLayerBase::Baud_38400:
        speed = 38400;
        break;
    case IOLayerBase::Baud_19200:
        speed = 19200;
        break;
    case IOLayerBase::Baud_9600:
        speed = 9600;
        break;
    }

    prof.writeEntry("Flow",  flow);
    prof.writeEntry("Parity", parity);
    prof.writeEntry("Speed",  speed);
}
