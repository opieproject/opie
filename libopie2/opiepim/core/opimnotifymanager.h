#ifndef OPIE_PIM_NOTIFY_MANAGER_H
#define OPIE_PIM_NOTIFY_MANAGER_H

#include <qvaluelist.h>

#include <opie/opimnotify.h>

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
    Alarms    alarms()const;

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

private:
    Reminders m_rem;
    Alarms m_al;

    class Private;
    Private *d;

};

#endif
