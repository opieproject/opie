#ifndef _DATEBOOKWEEKLSTHEADER_H
#define _DATEBOOKWEEKLSTHEADER_H

#include "datebookweeklstheaderbase.h"

#include <qdatetime.h>

class DateBookWeekLstHeader: public DateBookWeekLstHeaderBase
{
    Q_OBJECT
public:
    DateBookWeekLstHeader(bool onM, QWidget* parent = 0, const char* name = 0,
              WFlags fl = 0 );
    ~DateBookWeekLstHeader();
    void setDate(const QDate &d);

public slots:
    void nextWeek();
    void prevWeek();
    void nextMonth();
    void prevMonth();
    void pickDate();
    void setDate(int y, int m, int d);
signals:
    void dateChanged(QDate &newdate);
    void setDbl(bool on);
protected:
    QDate date;
//  bool onMonday;
    bool bStartOnMonday;

};

#endif
