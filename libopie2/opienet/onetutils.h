/*
                             This file is part of the Opie Project
                             (C) 2003-2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
              =.
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

#ifndef ONETUTILS_H
#define ONETUTILS_H

#include <qdict.h>
#include <qmap.h>
#include <qstring.h>
#include <qhostaddress.h>
#include <qobject.h>

#include <sys/types.h>

struct ifreq;

namespace Opie {
namespace Net  {

class OWirelessNetworkInterface;

/*======================================================================================
 * OMacAddress
 *======================================================================================*/

class OMacAddress
{
  public:
    // QString c'tor? -zecke
    OMacAddress();
    OMacAddress( unsigned char* );
    OMacAddress( const unsigned char* );
    OMacAddress( struct ifreq& );
    ~OMacAddress();

    QString manufacturer() const;
    QString toString( bool substitute = false ) const;
    const unsigned char* native() const;

    // no c'tor but this one why not make it a c'tor. it could also replace the others or is this the problem?
    static OMacAddress fromString( const QString& );

  public:
    static const OMacAddress& broadcast; // ff:ff:ff:ff:ff:ff
    static const OMacAddress& unknown;   // 44:44:44:44:44:44

  private:
    unsigned char _bytes[6];

  friend bool operator==( const OMacAddress &m1, const OMacAddress &m2 );
  class Private;
  Private *d;

};

bool operator==( const OMacAddress &m1, const OMacAddress &m2 );


/*======================================================================================
 * OHostAddress
 *======================================================================================*/

class OHostAddress : public QHostAddress
{
  /*public:
    OHostAddress();
    ~OHostAddress();
  */
  private:
  class Private;
  Private *d;
};


/*======================================================================================
 * ONetworkInterfaceDriverInfo
 *======================================================================================*/

class ONetworkInterfaceDriverInfo
{
  public:
    ONetworkInterfaceDriverInfo( const QString& name = "<unknown>",
                                 const QString& version = "<unknown>",
                                 const QString& firmware = "<unknown>",
                                 const QString& bus = "<unknown>" ) :
                                 _name( name ), _version( version ), _firmware( firmware ), _bus( bus ) { };
    ~ONetworkInterfaceDriverInfo() { };

    QString name() const { return _name; };
    QString version() const { return _version; };
    QString firmware() const { return _firmware; };
    QString bus() const { return _bus; };

  private:
    const QString _name;
    const QString _version;
    const QString _firmware;
    const QString _bus;
};

/*======================================================================================
 * OPrivateIOCTL
 *======================================================================================*/

class OPrivateIOCTL : public QObject
{
  public:
    OPrivateIOCTL( QObject* parent, const char* name, int cmd, int getargs, int setargs );
    ~OPrivateIOCTL();

    int numberGetArgs() const;
    int typeGetArgs() const;
    int numberSetArgs() const;
    int typeSetArgs() const;

    // FIXME return int? as ::ioctl does? -zecke
    void invoke() const;
    void setParameter( int, u_int32_t );

  private:
    u_int32_t _ioctl;
    u_int16_t _getargs;
    u_int16_t _setargs;

    class Private;
    Private *d;
};

 /*======================================================================================
 * Miscellaneous
 *======================================================================================*/

namespace Internal {
void dumpBytes( const unsigned char* data, int num );
QString modeToString( int );
int stringToMode( const QString& );
}
}
}

#define IW_PRIV_TYPE_MASK       0x7000
#define IW_PRIV_TYPE_NONE       0x0000
#define IW_PRIV_TYPE_BYTE       0x1000
#define IW_PRIV_TYPE_CHAR       0x2000
#define IW_PRIV_TYPE_INT        0x4000
#define IW_PRIV_TYPE_FLOAT      0x5000
#define IW_PRIV_TYPE_ADDR       0x6000
#define IW_PRIV_SIZE_FIXED      0x0800
#define IW_PRIV_SIZE_MASK       0x07FF

#define IW_HEADER_TYPE_NULL     0       /* Not available */
#define IW_HEADER_TYPE_CHAR     2       /* char [IFNAMSIZ] */
#define IW_HEADER_TYPE_UINT     4       /* __u32 */
#define IW_HEADER_TYPE_FREQ     5       /* struct iw_freq */
#define IW_HEADER_TYPE_ADDR     6       /* struct sockaddr */
#define IW_HEADER_TYPE_POINT    8       /* struct iw_point */
#define IW_HEADER_TYPE_PARAM    9       /* struct iw_param */
#define IW_HEADER_TYPE_QUAL     10      /* struct iw_quality */

#define IW_EV_POINT_OFF (((char *) &(((struct iw_point *) NULL)->length)) - \
                                  (char *) NULL)

#ifndef ARPHRD_IEEE80211
#define ARPHRD_IEEE80211 801
#endif
#ifndef ARPHRD_IEEE80211_PRISM
#define ARPHRD_IEEE80211_PRISM 802
#endif

/* Wireless Extension Scanning Stuff */
struct iw_stream_descr
{
    char *        end;            /* End of the stream */
    char *        current;        /* Current event in stream of events */
    char *        value;          /* Current value in event */
};


/* Network to host order macros */

#ifdef LBL_ALIGN
#define EXTRACT_16BITS(p) \
        ((u_int16_t)((u_int16_t)*((const u_int8_t *)(p) + 0) << 8 | \
                     (u_int16_t)*((const u_int8_t *)(p) + 1)))
#define EXTRACT_32BITS(p) \
        ((u_int32_t)((u_int32_t)*((const u_int8_t *)(p) + 0) << 24 | \
                     (u_int32_t)*((const u_int8_t *)(p) + 1) << 16 | \
                     (u_int32_t)*((const u_int8_t *)(p) + 2) << 8 | \
                     (u_int32_t)*((const u_int8_t *)(p) + 3)))
#else
#define EXTRACT_16BITS(p) \
        ((u_int16_t)ntohs(*(const u_int16_t *)(p)))
#define EXTRACT_32BITS(p) \
        ((u_int32_t)ntohl(*(const u_int32_t *)(p)))
#endif

#define EXTRACT_24BITS(p) \
        ((u_int32_t)((u_int32_t)*((const u_int8_t *)(p) + 0) << 16 | \
                     (u_int32_t)*((const u_int8_t *)(p) + 1) << 8 | \
                     (u_int32_t)*((const u_int8_t *)(p) + 2)))

/* Little endian protocol host order macros */
#define EXTRACT_LE_8BITS(p) (*(p))
#define EXTRACT_LE_16BITS(p) \
        ((u_int16_t)((u_int16_t)*((const u_int8_t *)(p) + 1) << 8 | \
                     (u_int16_t)*((const u_int8_t *)(p) + 0)))
#define EXTRACT_LE_32BITS(p) \
        ((u_int32_t)((u_int32_t)*((const u_int8_t *)(p) + 3) << 24 | \
                     (u_int32_t)*((const u_int8_t *)(p) + 2) << 16 | \
                     (u_int32_t)*((const u_int8_t *)(p) + 1) << 8 | \
                     (u_int32_t)*((const u_int8_t *)(p) + 0)))

#endif // ONETUTILS_H

