
#ifndef OpieToothServices_H
#define OpieToothServices_H

#include <qvaluelist.h>

namespace OpieTooth {
  /**
   * Services lets shows you all available services
   * on a remote device
   */
  class Services {

    /** The profile descriptor
     *
     */
    class ProfileDescriptor{
    public:
      /** typedef */
      typedef QValueList<ProfileDescriptor> ValueList;
      /** c'tor for QValueList */
      ProfileDescriptor();
      /**
       *  c'tor
       *  @param id The id or name ("Lan Access Using PPP")
       *  @param idInt The id as uint ( 0x1102 )
       *  @param version Version of the Profile ( 1 )
       */
      ProfileDescriptor(const QString &id, uint idInt, uint version );
      /**
       * copy c'tor
       */
      ProfileDescriptor(const ProfileDescriptor& );
      /**
       * returns the id
       */
      QString id()const;
      /**
       * sets the id
       */
      void setId(const QString& id);

      /**
       * sets the uint id
       */
      void setId(uint );
      /**
       * reutns the id as int
       */
      uint idInt()const;
      /**
       * returns the version
       */
      uint version()const;
      /**
       * sets the Version
       */
      void setVersion(uint version );
      /**
       * copy operator
       */
      ProfileDescriptor &operator=( const ProfileDescriptor& );
      /**
       * operator==
       */
      friend bool operator==(const ProfileDescriptor&, const ProfileDescriptor& );
    private:
      QString m_id;
      uint m_idInt;
      uint m_version;
    };
    /**
     * Protocol Descriptor
     */
    class ProtocolDescriptor {
    public:
      typedef QValueList<ProtocolDescriptor> ValueList;
      /**
       * c'tor
       */
      ProtocolDescriptor();
      /**
       * name
       * number
       * channel/port
       */
      ProtocolDescriptor(const QString&, uint, uint channel ); // Q_UINT8 ?
      ProtocolDescriptor(const ProtocolDescriptor& );
      ~ProtocolDescriptor();
      QString name()const;
      void setName(const QString& );
      uint id()const;
      void setId(uint );
      uint port()const;
      void setPort(uint );
      ProtocolDescriptor &operator=( const ProtocolDescriptor& );
      friend bool operator==( const ProtocolDescriptor&,
			      const ProtocolDescriptor& );
    private:
      QString m_name;
      uint m_number;
      uint m_channel;
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
