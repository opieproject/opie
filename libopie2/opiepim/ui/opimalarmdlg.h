#ifndef OPIMALARMDLG_H
#define OPIMALARMDLG_H

#include "opimalarmdlgbase.h"

#include <qdatetime.h>

/**
 * A dialog for displaying alarm events
 *
 *
 * @short Alarm dialog
 * @author Paul Eggleton
 */
class OPimAlarmDlg : public OPimAlarmDlgBase {
    Q_OBJECT
public:
    enum AlarmResponse { None, Snooze, View };

    OPimAlarmDlg( const QDateTime &alarmTime, const QString &title, const QString &desc, int defaultSnooze, int defaultSnoozeUnits, bool viewEnabled = TRUE, QWidget * parent=0, bool modal=FALSE );
    ~OPimAlarmDlg();
    AlarmResponse response();
    QDateTime snoozeDateTime();

protected slots:
    void viewClicked();
    void snoozeClicked();

protected:
    AlarmResponse m_response;
    QDateTime m_alarmTime;
    static QDateTime snoozeDateTime( int snooze, int snoozeUnits );

private:

    class Private;
    Private *d;

};

#endif