
#ifndef OpieToothServices_H
#define OpieToothServices_H

#include <qvaluelist.h>

namespace OpieTooth {
  class Services {
    class ProtocolDescriptor {
    public:
      typedef QValueList<ProtocolDescriptor> ValueList;
      ProtocolDescriptor(const QString& );

    };
  public:
    typedef QValueList<Services> ValueList;
    Services();
    Services(const  Services& service );
    ~Services();

    QString serviceName()const;
    void setServiceName( const QString& service );

    int recHandle()const;
    void setRecHandle( int );

    QString classIdList()const;
    void setClassIdList( const QString& );
    int classIdListInt()const;
    void setClassIdList(int );




  };
};
#endif
