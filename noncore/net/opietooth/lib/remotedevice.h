
#ifndef OpieToothRemoteDevice
#define OpieToothRemoteDevice

#include <qvaluelist.h>

namespace OpieTooth{
  class RemoteDevice {
  public:
    typedef QValueList<RemoteDevice> ValueList;
    RemoteDevice();
    RemoteDevice(const RemoteDevice& );
    RemoteDevice(const QString &mac, const QString &name );
    ~RemoteDevice();
//    friend bool operator==(const RemoteDevice&, const RemoteDevice&);
    RemoteDevice &operator=(const RemoteDevice& );
    bool isEmpty()const;
    QString mac()const;
    void setMac(const QString& mac );
    QString name()const;
    void setName( const QString& name );
    bool equals( const RemoteDevice& )const;
  private:
    QString m_name;
    QString m_mac;
  };
    bool operator==( const RemoteDevice&, const RemoteDevice& );
};

#endif
