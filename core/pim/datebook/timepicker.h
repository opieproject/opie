#ifndef TIMEPICKER_H
#define TIMEPICKER_H

#include <qwidget.h>
#include <qvaluelist.h>
#include <opie/oclickablelabel.h>
#include <qdatetime.h>

class TimePicker: public QWidget {
  Q_OBJECT

 public:
    TimePicker(QWidget* parent = 0, const char* name = 0, 
	       WFlags fl = 0);
    void setHour(int h);
    void setMinute(int m);

 private:
    QValueList<OClickableLabel *> hourLst;
    QValueList<OClickableLabel *> minuteLst;
    QTime tm;

 private slots:
    void slotHour(bool b);
    void slotMinute(bool b);

 signals:
    void timeChanged(const QTime &);
};


#endif
