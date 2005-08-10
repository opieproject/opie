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

#ifndef ODEVICE_H_
#define ODEVICE_H_

/* OPIE */
#include <opie2/odevicebutton.h>
#include <qpe/qpeapplication.h> /* for Transformation enum.. */

/* QT */
#include <qnamespace.h>
#include <qobject.h>
#include <qstring.h>


#include <qstrlist.h>
#include <qwindowsystem_qws.h>


class Sound;

namespace Opie{
namespace Core{

class ODeviceData;
/**
 * The available devices
 */
enum OModel {
    Model_Unknown, // = 0

    Model_Series_Mask   = 0xffff0000,

    Model_iPAQ          = ( 1 << 16 ),

    Model_iPAQ_All      = ( Model_iPAQ | 0xffff ),
    Model_iPAQ_H31xx    = ( Model_iPAQ | 0x0001 ),
    Model_iPAQ_H36xx    = ( Model_iPAQ | 0x0002 ),
    Model_iPAQ_H37xx    = ( Model_iPAQ | 0x0003 ),
    Model_iPAQ_H38xx    = ( Model_iPAQ | 0x0004 ),
    Model_iPAQ_H39xx    = ( Model_iPAQ | 0x0005 ),
    Model_iPAQ_H5xxx    = ( Model_iPAQ | 0x0006 ),
    Model_iPAQ_H22xx    = ( Model_iPAQ | 0x0007 ),
    Model_iPAQ_H191x    = ( Model_iPAQ | 0x0008 ),

    Model_Jornada       = ( 6 << 16 ),
    Model_Jornada_56x   = ( Model_Jornada | 0x0001 ),
    Model_Jornada_720   = ( Model_Jornada | 0x0002 ),

    Model_Zaurus        = ( 2 << 16 ),

    Model_Zaurus_SL5000 = ( Model_Zaurus | 0x0001 ),
    Model_Zaurus_SL5500 = ( Model_Zaurus | 0x0002 ),
    Model_Zaurus_SLA300 = ( Model_Zaurus | 0x0003 ),
    Model_Zaurus_SLB600 = ( Model_Zaurus | 0x0004 ),
    Model_Zaurus_SLC7x0 = ( Model_Zaurus | 0x0005 ),
    Model_Zaurus_SL6000 = ( Model_Zaurus | 0x0006 ),
    Model_Zaurus_SLC3000 = ( Model_Zaurus | 0x0007 ),
    Model_Zaurus_SLC1000 = ( Model_Zaurus | 0x0008 ),

    Model_SIMpad        = ( 3 << 16 ),

    Model_SIMpad_All    = ( Model_SIMpad | 0xffff ),
    Model_SIMpad_CL4    = ( Model_SIMpad | 0x0001 ),
    Model_SIMpad_SL4    = ( Model_SIMpad | 0x0002 ),
    Model_SIMpad_SLC    = ( Model_SIMpad | 0x0003 ),
    Model_SIMpad_TSinus = ( Model_SIMpad | 0x0004 ),

    Model_Ramses        = ( 4 << 16 ),

    Model_Ramses_All    = ( Model_Ramses | 0xffff ),
    Model_Ramses_MNCI   = ( Model_Ramses | 0x0001 ),
    Model_Ramses_MNCIRX = ( Model_Ramses | 0x0002 ),

    Model_Yopy          = ( 5 << 16 ),

    Model_Yopy_All      = ( Model_Yopy | 0xffff ),
    Model_Yopy_3000     = ( Model_Yopy | 0x0001 ),
    Model_Yopy_3500     = ( Model_Yopy | 0x0002 ),
    Model_Yopy_3700     = ( Model_Yopy | 0x0003 ),

    Model_Beagle        = ( 6 << 16 ),

    Model_Beagle_All    = ( Model_Beagle | 0xffff ),
    Model_Beagle_PA100  = ( Model_Beagle | 0x0001 ),

    Model_GenuineIntel  = ( 7 << 16 ),

    Model_MyPal         = ( 8 << 16 ),

    Model_MyPal_All     = ( Model_MyPal | 0xffff ),
    Model_MyPal_620     = ( Model_MyPal | 0x0001 ),
    Model_MyPal_716     = ( Model_MyPal | 0x0002 ),
    Model_MyPal_730     = ( Model_MyPal | 0x0003 )
};

/**
 * The vendor of the device
 */
enum OVendor {
    Vendor_Unknown,

    Vendor_HP,
    Vendor_Sharp,
    Vendor_SIEMENS,
    Vendor_MundN,
    Vendor_GMate,
    Vendor_MasterIA,
    Vendor_GenuineIntel,
    Vendor_Asus
};

/**
 * The System used
 */
enum OSystem {
    System_Unknown,

    System_Familiar,
    System_Zaurus,
    System_OpenZaurus,
    System_Linupy,
    System_OpenEmbedded,
    System_PC
};

typedef struct {
    OSystem system;
    char* sysstr;
    char* sysvfile;
} ODistribution;

extern ODistribution distributions[];


enum OLedState {
    Led_Off,
    Led_On,
    Led_BlinkSlow,
    Led_BlinkFast
};

enum OLed {
    Led_Mail,
    Led_Power,
    Led_BlueTooth
};

enum OHardKey {
    HardKey_Datebook  = Qt::Key_F9,
    HardKey_Contacts  = Qt::Key_F10,
    HardKey_Menu      = Qt::Key_F11,
    HardKey_Home      = Qt::Key_F12,
    HardKey_Mail      = Qt::Key_F13,
    HardKey_Record    = Qt::Key_F24,
    HardKey_Suspend   = Qt::Key_F34,
    HardKey_Backlight = Qt::Key_F35,
    HardKey_Action    = Qt::Key_F10,
    HardKey_OK        = Qt::Key_F11,
    HardKey_End       = Qt::Key_F12,
};

enum ODirection {
    CW   = 0,
    CCW  = 1,
    Flip = 2,
};

enum OHingeStatus {
    CASE_CLOSED = 3,
    CASE_PORTRAIT = 2,
    CASE_LANDSCAPE = 0,
    CASE_UNKNOWN = 1,
};

/* default button for qvfb or such
 * see odevice.cpp for details.
 * hint: manage a user defined button for qvfb?
 * alwin
 */
struct default_button {
    Qt::Key code;
    char *utext;
    char *pix;
    char *fpressedservice;
    char *fpressedaction;
    char *fheldservice;
    char *fheldaction;
};

/**
 * A singleton which gives informations about device specefic option
 * like the Hardware used, LEDs, the Base Distribution and
 * hardware key mappings.
 *
 * @short A small class for device specefic options
 * @see QObject
 * @author Robert Griebl
 * @version 1.0
 */
class ODevice : public QObject
{
    Q_OBJECT

private:
    /* disable copy */
    ODevice ( const ODevice & );

protected:
    ODevice();
    virtual void init(const QString&);
    virtual void initButtons();
    static void sendSuspendmsg();

    ODeviceData *d;

public:
    // sandman do we want to allow destructions? -zecke?
    virtual ~ODevice();
    static ODevice *inst();

    // information
    QString modelString() const;
    OModel model() const;
    inline OModel series() const { return (OModel) ( model() & Model_Series_Mask ); }

    QString vendorString() const;
    OVendor vendor() const;

    QString systemString() const;
    OSystem system() const;

    QString systemVersionString() const;

    virtual Transformation rotation() const;
    virtual ODirection direction() const;

    QString qteDriver() const;

    // system
    virtual bool suspend();

    virtual bool setDisplayStatus ( bool on );
    virtual bool setDisplayBrightness ( int brightness );
    virtual int displayBrightnessResolution() const;
    virtual bool setDisplayContrast ( int contrast );
    virtual int displayContrastResolution() const;

    // don't add new virtual methods, use this:
    //  /*virtual */ void boo(int i ) { return virtual_hook(1,&i); };
    // and in your subclass do overwrite
    //  protected virtual int virtual_hook(int, void *)
    // which is defined below

    // input / output
    virtual void playAlarmSound();
    virtual void playKeySound();
    virtual void playTouchSound();

    virtual QValueList <OLed> ledList() const;
    virtual QValueList <OLedState> ledStateList ( OLed led ) const;
    virtual OLedState ledState ( OLed led ) const;
    virtual bool setLedState ( OLed led, OLedState st );

    virtual bool hasLightSensor() const;
    virtual int readLightSensor();
    virtual int lightSensorResolution() const;

    virtual bool hasHingeSensor() const;
    virtual OHingeStatus readHingeSensor()const;

    const QStrList &allowedCpuFrequencies() const;
    bool setCurrentCpuFrequency(uint index);

    /**
     * Returns the available buttons on this device.  The number and location
     * of buttons will vary depending on the device.  Button numbers will be assigned
     * by the device manufacturer and will be from most preferred button to least preffered
     * button.  Note that this list only contains "user mappable" buttons.
     *
     * @todo Make method const and take care of calling initButtons or make that const too
     *
     */
    const QValueList<ODeviceButton> &buttons();

    /**
     * Returns the DeviceButton for the \a keyCode.  If \a keyCode is not found, it
     * returns 0L
     */
    const ODeviceButton *buttonForKeycode ( ushort keyCode );

    /**
     * Reassigns the pressed action for \a button.  To return to the factory
     * default pass an empty string as \a qcopMessage.
     */
    void remapPressedAction ( int button, const OQCopMessage &qcopMessage );

    /**
     * Reassigns the held action for \a button.  To return to the factory
     * default pass an empty string as \a qcopMessage.
     */
    void remapHeldAction ( int button, const OQCopMessage &qcopMessage );

    /**
     * How long (in ms) you have to press a button for a "hold" action
     */
    uint buttonHoldTime() const;

signals:
    void buttonMappingChanged();

private slots:
    void systemMessage ( const QCString &, const QByteArray & );
    void playingStopped();

protected:
    void addPreHandler(QWSServer::KeyboardFilter*aFilter);
    void remPreHandler(QWSServer::KeyboardFilter*aFilter);
    void reloadButtonMapping();
    void changeMixerForAlarm( int mixer, const char* file, Sound *snd);

    /*  ugly virtual hook */
    virtual void virtual_hook( int id, void* data );
};

class ODeviceData {

  public:
    QString m_vendorstr;
    OVendor m_vendor;

    QString m_modelstr;
    OModel m_model;

    QString m_systemstr;
    OSystem m_system;

    QString m_sysverstr;

    Transformation m_rotation;
    ODirection m_direction;

    QString m_qteDriver;

    QValueList <ODeviceButton> *m_buttons;
    uint                        m_holdtime;
    QStrList                   *m_cpu_frequencies;
    bool    m_initializedButtonQcop : 1;

    /* values for changeMixerForAlarm */
    int m_sound, m_vol, m_mixer;
};

extern bool isQWS();
extern QCString makeChannel ( const char *str );
}
}




#endif

