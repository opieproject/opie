#include <qlabel.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qregexp.h>

#include "modemconfigwidget.h"
#include "dialdialog.h"

namespace {
    void setCurrent( const QString& str, QComboBox* bo ) {
        uint b = bo->count();
        for (int i = 0; i < bo->count(); i++ ) {
            if ( bo->text(i) == str ) {
                bo->setCurrentItem( i );
                return;
            }
        }
        bo->insertItem( str );
        bo->setCurrentItem( b );
    }
}

ModemConfigWidget::ModemConfigWidget( const QString& name, QWidget* parent,
                                const char* na )
    : ProfileDialogConnectionWidget( name, parent, na ) {

    m_lay = new QVBoxLayout( this );
    m_device = new QLabel(tr( "Modem is attached to:" ), this );
    m_deviceCmb = new QComboBox(this );
    m_deviceCmb->setEditable( TRUE );

    QLabel* telLabel = new QLabel( this );
    telLabel->setText( tr( "Enter telefon number here:" ) );
    QHBox *buttonBox = new QHBox( this );
    m_telNumber = new QLineEdit( buttonBox );
    QPushButton *atButton = new QPushButton( buttonBox );
    atButton->setText( tr( "AT commands" ) );
    connect( atButton, SIGNAL( clicked() ), this, SLOT( slotAT() ) );

    QPushButton *dialButton = new QPushButton( buttonBox );
    dialButton->setText( tr( "Enter number" ) );
    connect( dialButton, SIGNAL( clicked() ), this, SLOT( slotDial() ) );

    m_base = new IOLayerBase( this, "base" );

    m_lay->addWidget( m_device );
    m_lay->addWidget( m_deviceCmb );
    m_lay->addWidget( telLabel );
    m_lay->addWidget( buttonBox );
    m_lay->addWidget( m_base );
    m_lay->addStretch( 0 );

    m_deviceCmb->insertItem( "/dev/ttyS0" );
    m_deviceCmb->insertItem( "/dev/ttyS1" );
    m_deviceCmb->insertItem( "/dev/ttyS2" );

    atConf = new  ATConfigDialog( this, "ATConfig", true );
}

ModemConfigWidget::~ModemConfigWidget() {

}
void ModemConfigWidget::load( const Profile& prof ) {

    int rad_flow = prof.readNumEntry( "Flow" );
    int rad_parity = prof.readNumEntry( "Parity" );
    int speed = prof.readNumEntry( "Speed" );
    int dbits = prof.readNumEntry("DataBits");
    int sbits = prof.readNumEntry("StopBits");
    QString number = prof.readEntry( "Number" );

    if ( !number.isEmpty() ) {
        m_telNumber->setText( number );
    }

    if ( rad_flow == 1 ) {
        m_base->setFlow( IOLayerBase::Hardware );
    } else if (rad_flow == 2) {
        m_base->setFlow( IOLayerBase::Software );
    } else if (rad_flow == 0) {
         m_base->setFlow( IOLayerBase::None );
    }


    if ( rad_parity == 1 ) {
        m_base->setParity( IOLayerBase::Even );
    } else if ( rad_parity == 2 ){
        m_base->setParity( IOLayerBase::Odd );
    } else {
        m_base->setParity( IOLayerBase::NonePar );
    }

    switch( speed ) {
    case 115200:
        m_base->setSpeed( IOLayerBase::Baud_115200 );
        break;
    case 57600:
        m_base->setSpeed( IOLayerBase::Baud_57600 );
        break;
    case 38400:
        m_base->setSpeed( IOLayerBase::Baud_38400 );
        break;
    case 19200:
        m_base->setSpeed( IOLayerBase::Baud_19200 );
        break;
    case 9600:
    default:
        m_base->setSpeed( IOLayerBase::Baud_9600 );
        break;
    }


    if ( dbits == 5) {
        m_base->setData( IOLayerBase::Data_Five );
    } else if (rad_flow == 6) {
        m_base->setData( IOLayerBase::Data_Six );
    } else if (rad_flow == 7) {
        m_base->setData( IOLayerBase::Data_Seven );
    } else {
        m_base->setData( IOLayerBase::Data_Eight );
    }

    if ( sbits == 2) {
        m_base->setStop( IOLayerBase::Stop_Two );
    } else if ( sbits == 15 ) {
        m_base->setStop( IOLayerBase::Stop_OnePointFive );
    } else {
        m_base->setStop( IOLayerBase::Stop_One );
    }



    if ( prof.readEntry( "Device" ).isEmpty() ) {
        return;
    }
    setCurrent( prof.readEntry( "Device" ), m_deviceCmb );

    atConf->readConfig( prof );
}

/*
 * save speed,
 * flow,
 * parity
 */
void ModemConfigWidget::save( Profile& prof ) {
    int flow, parity, speed, data, stop;
    flow = parity = speed = data = stop = 0;
    prof.writeEntry( "Device", m_deviceCmb->currentText() );


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
    case IOLayerBase::NonePar:
        parity = 0;
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

    switch( m_base->data() ) {
    case IOLayerBase::Data_Five:
        data = 5;
        break;
    case IOLayerBase::Data_Six:
        data = 6;
        break;
    case IOLayerBase::Data_Seven:
        data = 7;
        break;
    case IOLayerBase::Data_Eight:
        data = 8;
        break;
    }

    switch( m_base->stop() ) {
    case IOLayerBase::Stop_One:
        stop = 1;
        break;
    case IOLayerBase::Stop_OnePointFive:
        stop = 15;
        break;
    case IOLayerBase::Stop_Two:
        stop = 2;
        break;
    }

    prof.writeEntry( "Flow", flow );
    prof.writeEntry( "Parity", parity );
    prof.writeEntry( "Speed", speed );
    prof.writeEntry("DataBits", data);
    prof.writeEntry("StopBits", stop);
    prof.writeEntry( "Number", m_telNumber->text() );


    atConf->writeConfig(prof);
}

void ModemConfigWidget::slotAT() {
    //  ATConfigDialog conf( this, "ATConfig", true );
    atConf->showMaximized();
    if ( atConf->exec() != QDialog::Accepted ) {
        // reload old settings
    }
}

void ModemConfigWidget::slotDial() {
    DialDialog dial( this, "DialConfig", true );
    if(!m_telNumber->text().isEmpty()) {
        dial.setNumber(m_telNumber->text().replace(QRegExp("[\\-\\/\\ \\.\\,]"), ""));
    }
    dial.showMaximized();
    if ( dial.exec() == QDialog::Accepted ) {
        m_telNumber->setText( dial.number() );
    }
}
