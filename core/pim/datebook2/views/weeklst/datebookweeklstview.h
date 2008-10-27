#ifndef _DATEBOOKWEEKLISTVIEW_H
#define _DATEBOOKWEEKLISTVIEW_H

#include <opie2/opimoccurrence.h>

#include <qwidget.h>
#include <qvaluelist.h>
#include <qstring.h>
#include <qdatetime.h>

#include "weeklstview.h"

class QKeyEvent;
class QVBoxLayout;

class DateBookWeekLstView: public QWidget
{
    Q_OBJECT
public:
    DateBookWeekLstView(Opie::OPimOccurrence::List &ev, 
                        Opie::Datebook::WeekLstView *view, const QDate &d,
                        bool onM, bool showAmPm, QWidget* parent = 0,
                        const char* name = 0,
            WFlags fl = 0 );
    ~DateBookWeekLstView();

    void setEvents(Opie::OPimOccurrence::List &ev, const QDate &d, bool onM);

signals:
    void redraw();
    void showDate(int y, int m, int d);
    void addEvent(const QDateTime &start, const QDateTime &stop);

protected:
    bool bStartOnMonday;
    bool ampm;
    QValueList<QObject*> childs;
    Opie::Datebook::WeekLstView *weekLstView;

    QVBoxLayout*m_MainLayout;

protected slots:
    void keyPressEvent(QKeyEvent *);
    void slotClockChanged( bool ap );
};

#endif
