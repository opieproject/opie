#include "bluepingplugin.h"

#include <opie2/oapplication.h>
#include <opie2/odebug.h>

#include <qpe/qcopenvelope_qws.h>

#include <qdialog.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtimer.h>

using namespace Opie::Core;
using Opie::Security::MultiauthPluginObject;
using Opie::Security::MultiauthConfigWidget;


/// standard c'tor
BluepingPlugin::BluepingPlugin() : MultiauthPluginObject(), m_ping(0), m_bluepingW(0), m_waitingEnable(false) {
}

/// cleans m_ping and m_bluepingW if we need to
BluepingPlugin::~BluepingPlugin() {
    odebug << "closing Blueping plugin..." << oendl;
    if (m_ping != 0)
        delete m_ping;
    if (m_bluepingW != 0)
        delete m_bluepingW;

    QCopEnvelope e("QPE/Bluetooth", "disableBluetoothTemp()");
}

/// Simply return its name (Blueping plugin)
QString BluepingPlugin::pluginName() const {
    return "Blueping plugin";
}

/// returns a BluepingConfigWidget
MultiauthConfigWidget * BluepingPlugin::configWidget(QWidget * parent) {
    if (m_bluepingW == 0)
        m_bluepingW = new BluepingConfigWidget(parent, "Blueping configuration widget");
    return m_bluepingW;
}

QString BluepingPlugin::pixmapNameWidget() const {
    return "security/bluepingplugin";
}

QString BluepingPlugin::pixmapNameConfig() const {
    return "security/bluepingplugin";
}

/// Emit the MultiauthPluginObject::Success emitCode
void BluepingPlugin::success() {
    emit emitCode(MultiauthPluginObject::Success);
}

/// Emit the MultiauthPluginObject::Failure emitCode
void BluepingPlugin::failure() {
    emit emitCode(MultiauthPluginObject::Failure);
}

/// Emit the MultiauthPluginObject::Skip emitCode
void BluepingPlugin::skip() {
    emit emitCode(MultiauthPluginObject::Skip);
}

/// do the actual ping
void BluepingPlugin::ping() {
    m_ping = new OProcess();
    odebug << "pinging device: " << macToPing << oendl;
    *m_ping << "l2ping" << "-c 1" << macToPing;

    // starting to ping in the background
    /// \todo as soon as ping is launched, check RSSI (signal strength) and check
    /// it's high enough, meaning the device is close enough?
    /// \todo make it optionally pollable, so don't finish the ping and call
    /// Opie suspend if l2ping timeouts?
    if ( !m_ping->start() ) {
        oerr << "could not start l2ping" << oendl;
        this->skip();
    }
    QObject::connect(m_ping, SIGNAL(processExited(Opie::Core::OProcess*)),
                     this, SLOT(pingFinished(Opie::Core::OProcess*)) );
}

/// Deals with m_ping result
void BluepingPlugin::pingFinished(OProcess * ping) {
    if ( ping->normalExit() && (ping->exitStatus() == 0) )
    {
        odebug << "Successful Bluetooth ping!" << oendl;
        success();
    }
    else
    {
        odebug << "Failed Bluetooth ping... (normalExit: " << ping->normalExit() << ", exitStatus: " << ping->exitStatus() << ")" << oendl;
        failure();
    }
}

/// Make one authentication attempt with this plugin
/**
 * (very simple "success" / "failure" buttons in a dialog)
 * \return The outcome code of this  authentication
 */
int BluepingPlugin::authenticate() {

    Config cfg("Security");
    cfg.setGroup("BluepingPlugin");
    macToPing = cfg.readEntry("mac");
    if (!macToPing.isEmpty())
    {
        /* Standard, inescapable authentication dialog
        */
        QDialog bluepingDialog(0,
                               "Blueping dialog",
                               TRUE,
                               Qt::WStyle_NoBorder | Qt::WStyle_Customize | Qt::WStyle_StaysOnTop);

        QRect desk = oApp->desktop()->geometry();
        bluepingDialog.setGeometry( 0, 0, desk.width(), desk.height() );

        // Creation of the particular widgets of our Blueping user interface
        QVBoxLayout *layout = new QVBoxLayout(&bluepingDialog);
        layout->setSpacing(11);
        layout->setMargin(11);
        layout->setAlignment( Qt::AlignTop );

        QLabel title("<center><h1>\"Blueping\" <br />plugin</h1></center>", &bluepingDialog);
        QLabel subTitle("<center><h2>Trying to reach your configured bluetooth device...</h2></center>", &bluepingDialog);
        QLabel subTitle2("<center>You can skip this step and use another authentication way with the following button</center>", &bluepingDialog);
        QPushButton pbSkip("Skip", &bluepingDialog);
        layout->addWidget(&title);
        layout->addWidget(&subTitle);
        layout->addWidget(&subTitle2);
        layout->addWidget(&pbSkip, 0, Qt::AlignHCenter);

        // connect the skip button to the skip signal emitting function
        QObject::connect(&pbSkip, SIGNAL(clicked()), this, SLOT(skip()));
        // connect the signal emitting functions to the bluepingDialog done(int) finishing function
        QObject::connect(this, SIGNAL(emitCode(int)), &bluepingDialog, SLOT(done(int)));

        QCopChannel* chan = new QCopChannel("QPE/Bluetooth", this );
        connect(chan, SIGNAL(received(const QCString&,const QByteArray&) ),
                this, SLOT(slotMessage(const QCString&,const QByteArray&) ) );

        // Enable bluetooth if it isn't already enabled
        QTimer::singleShot( 500, this, SLOT(enableBluetooth()) );
        
        // start the dialog event loop, while the ping is starting (or will start soon) in the background
        return bluepingDialog.exec();
    }
    else
    {
        owarn << "No Bluetooth device has been set!" << oendl;
        owarn << "We will consider it as a successful authentication though." << oendl;
        return MultiauthPluginObject::Success;
    } // end if mac defined
}

void BluepingPlugin::enableBluetooth()
{
    m_waitingEnable = true;
    QCopEnvelope e("QPE/Bluetooth", "enableBluetoothTemp()");
    QCopEnvelope e2("QPE/Bluetooth", "sendStatus()"); // FIXME shouldn't be needed - enableBluetoothTemp should reply with something
}

// receiver for QCopChannel("QPE/Bluetooth") messages.
void BluepingPlugin::slotMessage( const QCString& str, const QByteArray& data )
{
    QDataStream stream ( data, IO_ReadOnly );
    if( str == "status(int)" || str == "statusChange(int)" ) {
        int status;
        stream >> status;
        if( m_waitingEnable && ((status & 2 ) == 2) ) { // FIXME constants!
            m_waitingEnable = false;
            ping();
        }
    }
}
