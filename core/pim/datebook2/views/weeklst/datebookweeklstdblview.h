#ifndef _DATEBOOKWEEKLSTDBLVIEW_H
#define _DATEBOOKWEEKLSTDBLVIEW_H

#include <opie2/opimoccurrence.h>

#include <qwidget.h>
#include <qvaluelist.h>
#include <qdatetime.h>
#include <qstring.h>

#include "weeklstview.h"

class DateBookWeekLstView;
class QHBoxLayout;

class DateBookWeekLstDblView: public QWidget {
        Q_OBJECT
public:
    DateBookWeekLstDblView( Opie::Datebook::WeekLstView *view,
                Opie::OPimOccurrence::List &ev1,
                Opie::OPimOccurrence::List &ev2,
                QDate &d, bool onM, bool showAmPm,
                QWidget* parent = 0, const char* name = 0,
                WFlags fl = 0 );
    DateBookWeekLstDblView( Opie::Datebook::WeekLstView *view,
                Opie::OPimOccurrence::List &ev1,
                QDate &d, bool onM, bool showAmPm,
                QWidget* parent = 0, const char* name = 0,
                WFlags fl = 0 );
    virtual ~DateBookWeekLstDblView();
    void setEvents(Opie::OPimOccurrence::List &ev1,Opie::OPimOccurrence::List &ev2,QDate &d, bool onM);
    void setEvents(Opie::OPimOccurrence::List &ev1,QDate &d, bool onM);
    void setRightEvents(Opie::OPimOccurrence::List &ev1,QDate &d, bool onM);

    bool toggleDoubleView(bool how);

signals:
    void redraw();
    void showDate(int y, int m, int d);
    void addEvent(const QDateTime &start, const QDateTime &stop);

protected:
    QHBoxLayout*m_MainLayout;
    Opie::Datebook::WeekLstView *weekLstView;
    DateBookWeekLstView *leftView,*rightView;
    bool ampm;
};

#endif
