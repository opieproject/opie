#ifndef _DATEBOOKWEEKLSTDAYHDR_H
#define _DATEBOOKWEEKLSTDAYHDR_H

#include "datebookweeklstdayhdrbase.h"

#include <qdatetime.h>

class DateBookWeekLstDayHdr: public DateBookWeekLstDayHdrBase
{
    Q_OBJECT
public:
    DateBookWeekLstDayHdr(const QDate &d, bool onM,
              QWidget* parent = 0, const char* name = 0,
              WFlags fl = 0 );
public slots:
    void showDay();
    void newEvent();
signals:
    void showDate(int y, int m, int d);
    void addEvent(const QDateTime &start, const QDateTime &stop,
          const QString &str, const QString &location);
private:
    QDate date;
};

#endif
