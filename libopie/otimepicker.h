#ifndef OTIMEPICKER_H
#define OTIMEPICKER_H

#include <qwidget.h>
#include <qvaluelist.h>
#include <qdatetime.h>
#include <qdialog.h>

#include <opie/oclickablelabel.h>
#include "otimepickerbase.h"

class OTimePicker: public QWidget {
  Q_OBJECT

 public:
    OTimePicker(QWidget* parent = 0, const char* name = 0, 
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

class OTimePickerDialog: public OTimePickerDialogBase {
  Q_OBJECT

 public:
	OTimePickerDialog ( QWidget* parent = 0, const char* name = NULL, WFlags fl = 0 );
	~OTimePickerDialog() { };

	QTime& time();

 public slots:
	void setTime( const QTime& time );
        void setHour( const QString& hour );
        void setMinute( const QString& minute );

 private:
	QTime m_time;
};
#endif
