/*
                             This file is part of the Opie Project
                             Copyright (C) 2002-2005 The Opie Team <opie-devel@lists.sourceforge.net>
              =.             Copyright (C) 2002-2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#ifndef ODEVICE_HTC
#define ODEVICE_HTC

#include "odevice_abstractmobiledevice.h"

/* QT */
#include <qfile.h>
#include <qwindowsystem_qws.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

// Vesa Standard
#define FB_BLANK_UNBLANK            0
#define FB_BLANK_POWERDOWN          4

namespace Opie {
namespace Core {
namespace Internal {

class HTC : public OAbstractMobileDevice, public QWSServer::KeyboardFilter
{
  Q_OBJECT

  protected:
    virtual void init(const QString&);
    virtual void initButtons();
    void initHingeSensor();

  protected slots:
    void hingeSensorTriggered();
    void systemMessage( const QCString &msg, const QByteArray & );

  public:
    virtual bool setDisplayStatus( bool on );

    virtual bool hasHingeSensor() const;
    virtual OHingeStatus readHingeSensor() const;

    virtual Transformation rotation() const;
    virtual ODirection direction() const;
    virtual bool suspend();

  protected:
    virtual bool filter( int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat );

    QFile m_hinge;
};

struct htc_button {
    Qt::Key code;
    char *utext;
    char *pix;
    char *fpressedservice;
    char *fpressedaction;
    char *fheldservice;
    char *fheldaction;
};
}
}
}
#endif
