
#ifndef OpieToothServices_H
#define OpieToothServices_H

#include <qvaluelist.h>

namespace OpieTooth {
  class Services {
    class ProfileDesriptor{
    public:
      typedef QValueList<ProfileDescriptor> ValueList;
      ProfileDescriptor();
      ProfileDescriptor(const QString &id, uint, uint version );
      ProfileDescriptor(const ProfileDescriptor& );
      QString id()const;
      void setId(const QString& id);
      void setId(uint );
      uint idInt()const;
      uint version()const;
      void setVersion(uint version );
      ProfileDescriptor &operator=( const ProfileDescriptor );
      friend bool operator==(const ProfileDescriptor&, const ProfileDescriptor& );
    };
    class ProtocolDescriptor {
    public:
      typedef QValueList<ProtocolDescriptor> ValueList;
      ProtocolDescriptor();
      ProtocolDescriptor(const QString&, uint, uint channel ); // Q_UINT8 ?
      ProtocolDescriptot(const ProtocolDescriptor& );
      ~ProtocolDescriptor();
      QString name()const;
      void setName(const QString& );
      uint id()const;
      void setId(uint );
      uint port()const;
      void setPort(uint );
      ProtocolDescriptor &operator=( const ProtocolDescriptor& );
      friend bool operator==( const ProtocolDescription&,
			      const ProtocolDescription& );
    };
  public:
    typedef QValueList<Services> ValueList;
    Services();
    Services(const  Services& service );
    ~Services();

    Services &operator=( const Services& );
    friend bool operator==(const Services&, const Services& );
    QString serviceName()const;
    void setServiceName( const QString& service );

    int recHandle()const;
    void setRecHandle( int );

    QString classIdList()const;
    void setClassIdList( const QString& );
    int classIdListInt()const;
    void setClassIdList(int );

    void insertProtocolDescriptor(const ProtocolDescriptor& );
    void clearProtocolDescriptorList();
    void removeProtocolDescriptor( const ProtocolDescriptor& );
    ProtocolDescriptor::ValueList protocolDescriptorList()const;

    void insertProfileDescriptor( const ProfileDescriptor& );
    void clearProfileDescriptorList();
    void removeProfileDescriptor(const ProfileDescriptor& );
    ProfileDescriptor::ValueList profileDescriptor()const;



  };
};
#endif
