#ifndef STATIONINFO_H
#define STATIONINFO_H

#include <qdialog.h>
#include <qlabel.h>

class QWidget;
class QString;
class QGroupBox;

class StationInfo: public QDialog
{
    Q_OBJECT

public:
    StationInfo( const QString &essid, const QString &type, const QString &channel,
            const QString &maxrate, const QString &level, const QString &encryption, const QString &address,
            const QString &vendor, const QString &time,
            QWidget* parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0 );

protected:
    QLabel *m_ssidLabel;
    QLabel *m_typeLabel;
    QLabel *m_channelLabel;
    QLabel *m_maxRateLabel;
    QLabel *m_levelLabel;
    QLabel *m_encLabel;
    QLabel *m_addrLabel;
    QLabel *m_vendorLabel;
    QLabel *m_timeLabel;
    QLabel *m_ssid;
    QLabel *m_type;
    QLabel *m_channel;
    QLabel *m_maxRate;
    QLabel *m_level;
    QLabel *m_encryption;
    QLabel *m_address;
    QLabel *m_vendor;
    QLabel *m_lastTime;
};

#endif
