
#ifndef OpieToothRemoteDevice
#define OpieToothRemoteDevice

#include <qvaluelist.h>

namespace OpieTooth{
  class RemoteDevices {
  public:
    typedef QValueList<RemoteDevices> ValueList;
    RemoteDevices();
    RemoteDevices(const RemoteDevices& );
    RemoteDevices(const QString &mac, const QString &name );
    ~RemoteDevices();
    friend bool operator==(const RemoteDevices&, const RemoteDevices&);
    RemoteDevies &operator=(const RemoteDevices& );
    QString mac()const;
    void setMac(const QString& mac );
    QString name()const;
    void setName( const QString& name );
  };
};

#endif
