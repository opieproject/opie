#ifndef _DATEBOOKWEEKLISTVIEW_H
#define _DATEBOOKWEEKLISTVIEW_H

#include <qpe/event.h>

#include <qwidget.h>
#include <qvaluelist.h>
#include <qstring.h>
#include <qdatetime.h>

class QKeyEvent;

class DateBookWeekLstView: public QWidget
{
    Q_OBJECT
public:
    DateBookWeekLstView(QValueList<EffectiveEvent> &ev, const QDate &d, bool onM,
            QWidget* parent = 0, const char* name = 0,
            WFlags fl = 0 );
    ~DateBookWeekLstView();
signals:
    void editEvent(const Event &e);
    void duplicateEvent(const Event &e);
    void removeEvent(const Event &e);
    void beamEvent(const Event &e);
    void redraw();
    void showDate(int y, int m, int d);
    void addEvent(const QDateTime &start, const QDateTime &stop,
    const QString &str, const QString &location);
private:
    bool bStartOnMonday;
protected slots:
    void keyPressEvent(QKeyEvent *);
};

#endif
