/*
                             This file is part of the Opie Project
                             Copyright (C) The Main Author <main-author@whereever.org>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/
#ifndef OPIE_PIM_NOTIFY_MANAGER_H
#define OPIE_PIM_NOTIFY_MANAGER_H

#include <qvaluelist.h>

#include <opie2/opimnotify.h>

namespace Opie {
/**
 * The notify manager keeps track of the Notifiers....
 */
class OPimNotifyManager {
public:
    typedef QValueList<OPimReminder> Reminders;
    typedef QValueList<OPimAlarm>    Alarms;
    OPimNotifyManager( const Reminders& rems = Reminders(), const Alarms& alarms = Alarms() );
    ~OPimNotifyManager();

    /* we will cast it for you ;) */
    void add( const OPimNotify& );
    void remove( const OPimNotify& );
    /* replaces all with this one! */
    void replace( const OPimNotify& );

    Reminders reminders()const;

    /**
     * Return 
     */
    Alarms    alarms()const;

    /**
     * Return alarm at DateTime "when". If more than one is registered at this
     * DateTime, the first one is returned.
     * If none was found, an empty Alarm is returned.
     * @param when The date and time of the returned alarm
     * @param found Returns true if anything was found.
     * @return Returns the found alarm at given DateTime. It is empty if found is false 
     * (nothing could be found at given date and time)
     */
    OPimAlarm alarmAtDateTime( const QDateTime& when, bool& found ) const;

    void setAlarms( const Alarms& );
    void setReminders( const Reminders& );

    /* register is a Ansi C keyword... */
    /**
     * This function will register the Notify to the Alarm Server
     * or datebook depending on the type of the notify
     */
    void registerNotify( const OPimNotify& );

    /**
     * this will do the opposite..
     */
    void deregister( const OPimNotify& );

    bool isEmpty()const;

    /** 
     * Return all alarms as string
     */
    QString alarmsToString() const;
    /** 
     * Return all notifiers as string
     */
    QString remindersToString() const;

    /**
     * Convert string to alarms 
     * @param str String created by alarmsToString()
     */
    void alarmsFromString( const QString& str );

    /**
     * Convert string to reminders
     * @param str String created by remindersToString()
     */
    void remindersFromString( const QString& str );



private:
    Reminders m_rem;
    Alarms m_al;

    class Private;
    Private *d;

};

}

#endif
