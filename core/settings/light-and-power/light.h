/*
                             This file is part of the OPIE Project
               =.            Copyright (c)  2002 Maximilian Reiss <harlekin@handhelds.org>
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:       
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/
#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "lightsettingsbase.h"

#include <qstringlist.h>
#include <qlistbox.h>

class QTimer;

class LightSettings : public LightSettingsBase
{
    Q_OBJECT

public:
    LightSettings( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~LightSettings();
    static QString appName() { return QString::fromLatin1("light-and-power"); }

protected:
    virtual void accept();
    virtual void done ( int r );

protected slots:
    virtual void calibrateSensor ( );
    virtual void calibrateSensorAC ( );
    void setBacklight ( int );
    void setContrast ( int );
    void setFrequency ( int );
    void setCloseHingeAction ( int );
    void resetBacklight ( );

private:
    int m_bres;
    int m_cres;
    int m_oldcontrast;
    int m_oldfreq;
    QTimer *m_resettimer;
    QStringList m_sensordata;
    QStringList m_sensordata_ac;
};


#endif

