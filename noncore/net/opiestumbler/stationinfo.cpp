#include <qlayout.h>
#include <qgroupbox.h>


#include "stationinfo.h"


StationInfo::StationInfo( const QString &ssid, const QString &type, const QString &channel,
        const QString &maxrate, const QString &level, const QString &encryption, const QString &address,
        const QString &vendor, const QString &time,
        QWidget *parent, const char *name, bool modal, WFlags f)
    : QDialog( parent, name, modal, f)
{
    setCaption(tr("Station Details"));

    QGridLayout *grid = new QGridLayout( this, 1, 2, 5);
    QVBoxLayout *layLeft = new QVBoxLayout(0, 0, 6);
    QVBoxLayout *layRight = new QVBoxLayout(0, 0, 6);


    m_ssidLabel = new QLabel( tr("SSID:"), this );
    layLeft->addWidget(m_ssidLabel);
    m_typeLabel = new QLabel( tr("Type:"), this );
    layLeft->addWidget(m_typeLabel);
    m_channelLabel = new QLabel( tr("Channel:"), this );
    layLeft->addWidget(m_channelLabel);
    m_maxRateLabel = new QLabel( tr("Max Rate"), this );
    layLeft->addWidget(m_maxRateLabel);
    m_levelLabel = new QLabel( tr("Max Signal:"), this );
    layLeft->addWidget(m_levelLabel);
    m_encLabel = new QLabel( tr("Encryption:"), this );
    layLeft->addWidget(m_encLabel);
    m_addrLabel = new QLabel( tr("Address:"), this );
    layLeft->addWidget(m_addrLabel);
    m_vendorLabel = new QLabel( tr("Vendor:"), this );
    layLeft->addWidget(m_vendorLabel);
    m_timeLabel = new QLabel( tr("Last Time Seen:"), this );
    layLeft->addWidget(m_timeLabel);


    m_ssid = new QLabel( ssid, this);
    layRight->addWidget(m_ssid);
    m_type = new QLabel( type, this);
    layRight->addWidget(m_type);
    m_channel = new QLabel( channel, this);
    layRight->addWidget(m_channel);
    m_maxRate = new QLabel( maxrate + " " + tr("Mb/s"), this );
    layRight->addWidget( m_maxRate );
    m_level = new QLabel( level, this);
    layRight->addWidget(m_level);
    m_encryption = new QLabel( encryption, this);
    layRight->addWidget(m_encryption);
    m_address = new QLabel( address, this);
    layRight->addWidget(m_address);
    m_vendor = new QLabel( vendor, this);
    layRight->addWidget(m_vendor);
    m_lastTime = new QLabel( time, this);
    layRight->addWidget(m_lastTime);


    grid->addLayout( layRight, 0, 1);
    grid->addLayout( layLeft, 0, 0 );
}
