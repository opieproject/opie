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

/// creates and initializes the m_config Config object
BluepingPlugin::BluepingPlugin() : MultiauthPluginObject(), m_ping(0) {
    m_config = new Config("Security");
    m_config->setGroup("BluepingPlugin");
    bluetoothAlreadyRestarted = false;
}

/// deletes the m_config Config object and noticeW if necessary
BluepingPlugin::~BluepingPlugin() {
    delete m_config;
    if (m_ping != 0)
        delete m_ping;
}

/// Simply return its name (Blueping plugin)
QString BluepingPlugin::pluginName() const {
    return "Blueping plugin";
}

/// no configuration widget for the moment
MultiauthConfigWidget * BluepingPlugin::configWidget(QWidget * parent) {
    return 0l;
}
QString BluepingPlugin::pixmapNameWidget() const {
    return "security/bluepingplugin";
}
QString BluepingPlugin::pixmapNameConfig() const {
    return 0l;
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
            odebug << "Failed Bluetooth ping..." << oendl;
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

        if (!bluetoothAlreadyRestarted)
        {
            // we have just started or resumed the device, so Bluetooth has to be (re)started
            OProcess killB;
            killB << "killall" << "hciattach";
            odebug << "killing Bluetooth..." << oendl;
            if ( !killB.start(OProcess::Block) ) {
                oerr << "could not kill bluetooth" << oendl;
            }

            OProcess startB;
            switch ( ODevice::inst()->model() ) {
                case Model_iPAQ_H39xx:
                    startB << "/sbin/hciattach" << "/dev/tts/1" << "bcsp" << "921600";
                    break;

                case Model_iPAQ_H5xxx:
                    startB << "/sbin/hciattach" << "/dev/tts/1" << "any" << "921600";
                    break;

                default:
                    startB << "/sbin/hciattach" << "/dev/ttySB0" << "bcsp" << "230400";
                    break;
            } // end switch on device models

            if ( !startB.start(OProcess::Block) ) {
                oerr << "could not (re)start bluetooth" << oendl;
                return MultiauthPluginObject::Skip;
            }
            else
            {
                if ( startB.normalExit() && (startB.exitStatus() == 0) )
                {
                    odebug << "hciattach exited normally."<< oendl;
                    bluetoothAlreadyRestarted = true;
                    // 500 ms timer, so l2ping won't try to find a route before bluetooth has \em really started
                    QTimer::singleShot( 500, this, SLOT(ping()) );
                }
                else
                {
                    owarn << "hciattach exited anormally (error code: " << startB.exitStatus() << ")" << oendl;
                } // end if startBluetooth exit status == 0
            } // end if startBluetooth started
        }
        else
        {
            // we don't need to wait, since bluetooth has been started long enough ago
            ping();
        } // end if bluetooth not restarted


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
