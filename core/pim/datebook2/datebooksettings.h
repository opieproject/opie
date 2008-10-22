/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@handhelds.org>
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

#ifndef DATEBOOKSETTINGS_H
#define DATEBOOKSETTINGS_H
#include "datebooksettingsbase.h"
#include <qpe/categoryselect.h>
#include <qvaluelist.h>

namespace Opie {
namespace Core {
    class OPluginManager;
    class OGenericPluginLoader;
    class OPluginLoader;
}
namespace Ui {
    class OPluginConfigWidget;
}
namespace Datebook {
    class HolidayPluginConfigWidget;
}
}

class DateBookSettings : public DateBookSettingsBase
{
    Q_OBJECT
public:
    DateBookSettings( bool whichClock, QWidget *parent = 0,
                      const char *name = 0, bool modal = TRUE, WFlags = 0 );
    ~DateBookSettings();
    void setStartTime( int newStartViewTime );
    int startTime() const;
    void setAlarmPreset( bool bAlarm, int presetTime );
    bool alarmPreset() const;
    int presetTime() const;
    void setAlarmType( int alarmType );
    int alarmType() const;

    void setJumpToCurTime( bool bJump );
    bool jumpToCurTime() const;
    void setRowStyle( int style );
    int rowStyle() const;

    void setPluginList(Opie::Core::OPluginManager*,Opie::Core::OPluginLoader*);
    void savePlugins();
private slots:
    void slot12Hour( int );
    void slotChangeClock( bool );
protected slots:
    virtual void pluginItemClicked(QListViewItem *);

protected:
    void init();
    bool ampm;
    int oldtime;
    Opie::Core::OPluginManager*m_manager;
    Opie::Core::OPluginLoader*m_loader;
    QValueList<Opie::Datebook::HolidayPluginConfigWidget*> m_cfgWidgets;
};
#endif