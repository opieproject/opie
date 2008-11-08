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
    OPimAlarmDlg( int uid, const QDateTime &alarmTime, const QString &title, const QString &desc, int defaultSnooze, int defaultSnoozeUnits, bool viewEnabled = TRUE, QWidget * parent=0, bool modal=FALSE );
    ~OPimAlarmDlg();
    bool snooze();
    QDateTime snoozeDateTime();

signals:
    void viewItem( int uid );

protected slots:
    void viewClicked();
    void snoozeClicked();

protected:
    int m_uid;
    bool m_snooze;
    QDateTime m_alarmTime;
    static QDateTime snoozeDateTime( int snooze, int snoozeUnits );

private:

    class Private;
    Private *d;

};

#endif