#ifndef _DATEBOOKWEEKLSTDBLVIEW_H
#define _DATEBOOKWEEKLSTDBLVIEW_H

#include <qpe/event.h>

#include <qwidget.h>
#include <qvaluelist.h>
#include <qdatetime.h>
#include <qstring.h>

class DateBookWeekLstDblView: public QWidget {
        Q_OBJECT
public:
    DateBookWeekLstDblView(QValueList<EffectiveEvent> &ev1,
               QValueList<EffectiveEvent> &ev2,
               QDate &d, bool onM,
               QWidget* parent = 0, const char* name = 0,
               WFlags fl = 0 );
    virtual ~DateBookWeekLstDblView();
signals:
    void editEvent(const Event &e);
    void duplicateEvent(const Event &e);
    void removeEvent(const Event &e);
    void beamEvent(const Event &e);
    void redraw();
    void showDate(int y, int m, int d);
    void addEvent(const QDateTime &start, const QDateTime &stop,
          const QString &str, const QString &location);
};

#endif
