/*
                             This file is part of the Opie Project
                             Copyright (C) The Opie Team <opie-devel@handhelds.org>
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

#include "odevice.h"

/* QT */
#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qwindowsystem_qws.h>

/* OPIE */
#include <qpe/config.h>
#include <qpe/resource.h>
#include <qpe/sound.h>
#include <qpe/qcopenvelope_qws.h>

/* STD */
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
#ifndef QT_NO_SOUND
#include <linux/soundcard.h>
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

// _IO and friends are only defined in kernel headers ...

#define OD_IOC(dir,type,number,size)    (( dir << 30 ) | ( type << 8 ) | ( number ) | ( size << 16 ))

#define OD_IO(type,number)              OD_IOC(0,type,number,0)
#define OD_IOW(type,number,size)        OD_IOC(1,type,number,sizeof(size))
#define OD_IOR(type,number,size)        OD_IOC(2,type,number,sizeof(size))
#define OD_IORW(type,number,size)       OD_IOC(3,type,number,sizeof(size))

using namespace Opie;

class Yopy : public ODevice
{
  protected:

    virtual void init();
    virtual void initButtons();

  public:
    virtual bool suspend();

    virtual bool setDisplayBrightness ( int b );
    virtual int displayBrightnessResolution() const;

    static bool isYopy();
};

struct yopy_button {
    Qt::Key code;
    char *utext;
    char *pix;
    char *fpressedservice;
    char *fpressedaction;
    char *fheldservice;
    char *fheldaction;
} yopy_buttons [] = {
{ Qt::Key_F10, QT_TRANSLATE_NOOP("Button", "Action Button"),
    "devicebuttons/yopy_action",
    "datebook", "nextView()",
    "today", "raise()" },
{ Qt::Key_F11, QT_TRANSLATE_NOOP("Button", "OK Button"),
    "devicebuttons/yopy_ok",
    "addressbook", "raise()",
    "addressbook", "beamBusinessCard()" },
{ Qt::Key_F12, QT_TRANSLATE_NOOP("Button", "End Button"),
    "devicebuttons/yopy_end",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
};

bool Yopy::isYopy()
{
QFile f( "/proc/cpuinfo" );
if ( f. open ( IO_ReadOnly ) ) {
    QTextStream ts ( &f );
    QString line;
    while( line = ts. readLine() ) {
    if ( line. left ( 8 ) == "Hardware" ) {
    int loc = line. find ( ":" );
    if ( loc != -1 ) {
    QString model =
        line. mid ( loc + 2 ). simplifyWhiteSpace( );
    return ( model == "Yopy" );
    }
    }
    }
}
return false;
}

void Yopy::init()
{
d->m_vendorstr = "G.Mate";
d->m_vendor = Vendor_GMate;
d->m_modelstr = "Yopy3700";
d->m_model = Model_Yopy_3700;
d->m_rotation = Rot0;

d->m_systemstr = "Linupy";
d->m_system = System_Linupy;

QFile f ( "/etc/issue" );
if ( f. open ( IO_ReadOnly )) {
    QTextStream ts ( &f );
    ts.readLine();
    d->m_sysverstr = ts. readLine();
    f. close();
}
}

void Yopy::initButtons()
{
if ( d->m_buttons )
    return;

d->m_buttons = new QValueList <ODeviceButton>;

for (uint i = 0; i < ( sizeof( yopy_buttons ) / sizeof(yopy_button)); i++) {

    yopy_button *ib = yopy_buttons + i;

    ODeviceButton b;

    b. setKeycode ( ib->code );
    b. setUserText ( QObject::tr ( "Button", ib->utext ));
    b. setPixmap ( Resource::loadPixmap ( ib->pix ));
    b. setFactoryPresetPressedAction
    (OQCopMessage(makeChannel(ib->fpressedservice), ib->fpressedaction));
    b. setFactoryPresetHeldAction
    (OQCopMessage(makeChannel(ib->fheldservice), ib->fheldaction));

    d->m_buttons->append ( b );
}
reloadButtonMapping();

QCopChannel *sysch = new QCopChannel("QPE/System", this);
connect(sysch, SIGNAL(received(const QCString &, const QByteArray & )), 
    this, SLOT(systemMessage(const QCString &, const QByteArray & )));
}

bool Yopy::suspend()
{
/* Opie for Yopy does not implement its own power management at the 
    moment.  The public version runs parallel to X, and relies on the 
    existing power management features. */
return false;
}

bool Yopy::setDisplayBrightness(int bright)
{
/* The code here works, but is disabled as the current version runs
    parallel to X, and relies on the existing backlight demon. */
#if 0
if ( QFile::exists("/proc/sys/pm/light") ) {
    int fd = ::open("/proc/sys/pm/light", O_WRONLY);
    if (fd >= 0 ) {
    if (bright)
    ::write(fd, "1\n", 2);
    else
    ::write(fd, "0\n", 2);
    ::close(fd);
    return true;
    }
}
#endif
return false;
}

int Yopy::displayBrightnessResolution() const
{
    return 2;
}

