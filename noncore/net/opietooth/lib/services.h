
#ifndef OpieToothServices_H
#define OpieToothServices_H

#include <qmap.h>
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
      ProfileDescriptor(const QString &id, int idInt, int version );
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
       * sets the int id
       */
      void setId(int );
      /**
       * reutns the id as int
       */
      int idInt()const;
      /**
       * returns the version
       */
      int version()const;
      /**
       * sets the Version
       */
      void setVersion(int version );
      /**
       * copy operator
       */
      ProfileDescriptor &operator=( const ProfileDescriptor& );
      /**
       * operator==
       */
//      friend bool operator==(const ProfileDescriptor&, const ProfileDescriptor& );
    private:
      QString m_id;
      int m_idInt;
      int m_version;
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
      ProtocolDescriptor(const QString&, int, int port = -1 ); // Q_UINT8 ?
      ProtocolDescriptor(const ProtocolDescriptor& );
      ~ProtocolDescriptor();
      QString name()const;
      void setName(const QString& );
      int id()const;
      void setId(int );
      int port()const;
      void setPort(int );
      ProtocolDescriptor &operator=( const ProtocolDescriptor& );
        //friend bool operator==( const ProtocolDescriptor&,
        //	      const ProtocolDescriptor& );
    private:
      QString m_name;
      int m_number;
      int m_channel;
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


    QMap<int, QString> classIdList()const;
    void insertClassId( int id, const QString& className );
    void removeClassId( int id );
    void clearClassId();

    void insertProtocolDescriptor(const ProtocolDescriptor& );
    void clearProtocolDescriptorList();
    void removeProtocolDescriptor( const ProtocolDescriptor& );
    ProtocolDescriptor::ValueList protocolDescriptorList()const;

    void insertProfileDescriptor( const ProfileDescriptor& );
    void clearProfileDescriptorList();
    void removeProfileDescriptor(const ProfileDescriptor& );
    ProfileDescriptor::ValueList profileDescriptor()const;

  private:
      QMap<int, QString> m_classIds;
      QString m_name;
      int m_recHandle;
      QValueList<ProfileDescriptor> m_profiles;
      QValueList<ProtocolDescriptor> m_protocols;
  };
};
#endif
