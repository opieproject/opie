#ifndef _DATEBOOKWEEKLSTDBLVIEW_H
#define _DATEBOOKWEEKLSTDBLVIEW_H

#include <qpe/event.h>

#include <qwidget.h>
#include <qvaluelist.h>
#include <qdatetime.h>
#include <qstring.h>

class DateBookWeekLstView;
class QHBoxLayout;

class DateBookWeekLstDblView: public QWidget {
        Q_OBJECT
public:
    DateBookWeekLstDblView(QValueList<EffectiveEvent> &ev1,
               QValueList<EffectiveEvent> &ev2,
               QDate &d, bool onM, bool showAmPm,
               QWidget* parent = 0, const char* name = 0,
               WFlags fl = 0 );
    DateBookWeekLstDblView(QValueList<EffectiveEvent> &ev1,
               QDate &d, bool onM, bool showAmPm,
               QWidget* parent = 0, const char* name = 0,
               WFlags fl = 0 );
    virtual ~DateBookWeekLstDblView();
    void setEvents(QValueList<EffectiveEvent> &ev1,QValueList<EffectiveEvent> &ev2,QDate &d, bool onM);
    void setEvents(QValueList<EffectiveEvent> &ev1,QDate &d, bool onM);
    void setRightEvents(QValueList<EffectiveEvent> &ev1,QDate &d, bool onM);

    bool toggleDoubleView(bool how);

signals:
    void editEvent(const Event &e);
    void duplicateEvent(const Event &e);
    void removeEvent(const Event &e);
    void beamEvent(const Event &e);
    void redraw();
    void showDate(int y, int m, int d);
    void addEvent(const QDateTime &start, const QDateTime &stop,
          const QString &str, const QString &location);

protected:
    QHBoxLayout*m_MainLayout;
    DateBookWeekLstView *leftView,*rightView;
    bool ampm;
};

#endif
