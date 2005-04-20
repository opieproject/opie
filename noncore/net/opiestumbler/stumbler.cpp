#include <qstring.h>
#include <qtimer.h>
#include <qapplication.h>

//#include <opie2/onetutils.h>
#include <opie2/onetwork.h>
#include <opie2/odebug.h>

#include "stumbler.h"

using namespace Opie::Net;

Stumbler::Stumbler(const QString &iface, QObject *parent, const char *name)
    :QObject(parent, name), m_interval(5000),
    m_wifaceName(iface),
    m_timer(new QTimer(this))
{
    m_wiface = static_cast<OWirelessNetworkInterface*>(ONetwork::instance()->interface(m_wifaceName));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotRefresh()));
}
   

void Stumbler::start()
{
    if (!m_wiface) {
        odebug << "Error, interface " << m_wifaceName << " does not exist" << oendl;
        return;
    }
    
    if (!ONetwork::instance()->isWirelessInterface(m_wifaceName.ascii())) {
        odebug << "Error, " << m_wifaceName << " is not a wireless interface" << oendl;
        //FIXME: Tell the user about this
        return;
    }
    
    if (!m_timer->isActive()) {
        odebug << "Starting stumbler" << oendl;
        m_wiface->setUp(FALSE);
        m_wiface->setSSID("any");
        m_wiface->setAssociatedAP( OMacAddress::broadcast );
        m_wiface->setUp(TRUE);
        m_timer->start(m_interval);
    }
}

void Stumbler::stop()
{
    if (m_timer->isActive()) {
        odebug << "Stoping stumbler" << oendl;
        m_timer->stop();
    }
}

void Stumbler::setInterval(int msec)
{
    m_interval = msec;
    if (m_timer->isActive()) {
        m_timer->stop();
        m_timer->start(m_interval);
    }
}

void Stumbler::setIface(const QString &iface)
{
    m_wifaceName = iface;
    m_wiface = static_cast<OWirelessNetworkInterface*>(ONetwork::instance()->interface(m_wifaceName));
}

void Stumbler::slotRefresh()
{
    m_stationList = m_wiface->scanNetwork();
    if ( qApp )
        qApp->processEvents();
    emit (newdata());
}

Opie::Net::OStationList * Stumbler::stations()
{
    return m_stationList;
}
