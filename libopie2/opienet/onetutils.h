/*
                             This file is part of the Opie Project

                             (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
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
class OWirelessNetworkInterface;

/*======================================================================================
 * OMacAddress
 *======================================================================================*/

class OMacAddress
{
  public:
    // QString c'tor? -zecke
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

};

bool operator==( const OMacAddress &m1, const OMacAddress &m2 );


/*======================================================================================
 * OHostAddress
 *======================================================================================*/

class OHostAddress : public QHostAddress
{
  public:
    OHostAddress();
    ~OHostAddress();
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

};

 /*======================================================================================
 * Miscellaneous
 *======================================================================================*/

/* dump bytes */

void dumpBytes( const unsigned char* data, int num );

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

