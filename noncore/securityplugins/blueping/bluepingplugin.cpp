#include "bluepingplugin.h"

#include <opie2/oapplication.h>
#include <opie2/odebug.h>
#include <opie2/odevice.h>

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
BluepingPlugin::BluepingPlugin() : MultiauthPluginObject(), m_ping(0), m_bluepingW(0), bluetoothWasOff(false) {
}

/// cleans m_ping and m_bluepingW if we need to
BluepingPlugin::~BluepingPlugin() {
    odebug << "closing Blueping plugin..." << oendl;
    if (m_ping != 0)
        delete m_ping;
    if (m_bluepingW != 0)
        delete m_bluepingW;
    killBluetoothIfNecessary();
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

/// If Bluetooth was off before the plugin ran, we switch it off again
void BluepingPlugin::killBluetoothIfNecessary() {
    if (bluetoothWasOff) {
        OProcess killB;
        killB << "killall" << "hciattach";
        odebug << "killing Bluetooth... (since it was up only for Blueping)" << oendl;
        if ( !killB.start(OProcess::Block) ) {
            oerr << "could not kill bluetooth" << oendl;
        }
    } else {
        odebug << "keeping Bluetooth on" << oendl;
    }
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


        
        /* let's start Bluetooth if it's not running
         */
        OProcess checkB;
        checkB << "pidof" << "hciattach";
        odebug << "checking if Bluetooth is running..." << oendl;
        // now we start bluetooth *only* if the previous command works, exits normally, and
        // it returns a non-null exit code (which means hciattach is not running)
        if ( checkB.start(OProcess::Block) && checkB.normalExit() && (checkB.exitStatus() != 0) )
        {
            // remember to switch off Bluetooth once we're finished...
            bluetoothWasOff = true;
            odebug << "Bluetooth is not running, we must start it now" << oendl;

            OProcess startB;
            switch ( ODevice::inst()->model() ) {
                case Model_iPAQ_H39xx:
                    startB << "/sbin/hciattach" << "/dev/tts/1" << "bcsp" << "921600";
                    break;

                case Model_iPAQ_H5xxx:
                    startB << "/sbin/hciattach" << "/dev/tts/1" << "any" << "921600";
                    break;

                case Model_MyPal_716:
                    startB << "/sbin/hciattach" <<  "/dev/ttyS1" << "bcsp" << "921600";
                    break;

                default:
                    startB << "/sbin/hciattach" << "/dev/ttySB0" << "bcsp" << "230400";
                    break;
            } // end switch on device models

            if ( !startB.start(OProcess::Block) ) {
                oerr << "could not start Bluetooth" << oendl;
                return MultiauthPluginObject::Skip;
            }
            else
            {
                if ( (startB.normalExit()) && (startB.exitStatus() == 0) )
                {
                    odebug << "hciattach exited normally, Bluetooth is probably on now, let's wait 500 ms and ping" << oendl;
                    // 500 ms timer, so l2ping won't try to find a route before bluetooth has \em really started
                    QTimer::singleShot( 500, this, SLOT(ping()) );
                }
                else
                {
                    owarn << "hciattach exited anormally (normalExit: " << startB.normalExit() << ", exit status: " << startB.exitStatus() << ")" << oendl;
                    return MultiauthPluginObject::Skip;
                } // end if startB exited normaly
            } // end if startBluetooth started
        }
        else
        {
            // we don't need to wait, since bluetooth has been started long enough ago
            odebug << "Bluetooth is already running, we can try to ping now" << oendl;
            ping();
        } // end if Bluetooth was off


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
