/*
                             This file is part of the Opie Project

              =.             Copyright (C) The Opie Team <opie-devel@lists.sourceforge.net>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/


#ifndef OBLUETOOTH_SERVICES_H
#define OBLUETOOTH_SERVICES_H

#include <qmap.h>
#include <qvaluelist.h>

namespace Opie {
namespace Bluez {
    /**
     * OBluetoothServices lets shows you all available services
     * on a remote device
     */
    class OBluetoothServices {
        public:
            /** The profile descriptor
             *
             */
            class ProfileDescriptor {
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
                    //friend bool operator==(const ProfileDescriptor&, const ProfileDescriptor& );
                private:
                    QString m_id;
                    int m_idInt;
                    int m_version;
            };
            public:
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
                        //const ProtocolDescriptor& );
                    private:
                        QString m_name;
                        int m_number;
                        int m_channel;
                };

                public:
                    typedef QValueList<OBluetoothServices> ValueList;
                    OBluetoothServices();
                    OBluetoothServices(const  OBluetoothServices& service );
                    ~OBluetoothServices();

                    OBluetoothServices &operator=( const OBluetoothServices& );
                    friend bool operator==(const OBluetoothServices&, const OBluetoothServices& );
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
}
}
#endif
