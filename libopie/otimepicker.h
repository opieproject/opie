#ifndef OTIMEPICKER_H
#define OTIMEPICKER_H

#include <qwidget.h>
#include <qvaluelist.h>
#include <qdatetime.h>
#include <qdialog.h>

#include <opie/oclickablelabel.h>
#include "otimepickerbase.h"

/**
 * A class to pick time. It uses clickable labels
 * internally to allow a quick selection of a time.
 * A time can be selected by two clicks of a user
 *
 * @short A widget to quickly pick a QTime
 * @version 1.0
 * @see QWidget
 * @see QTime
 * @author Hakan Ardo, Stefan Eilers
 */
class OTimePicker: public QWidget {
  Q_OBJECT

 public:
    OTimePicker(QWidget* parent = 0, const char* name = 0,
	       WFlags fl = 0);
 
 public slots:	      
    void setHour(int h);
    void setMinute(int m);
    void setTime( const QTime& );
    void setTime( int h,  int m );
    
 public:
    QTime time()const;

 private:
    QValueList<OClickableLabel *> hourLst;
    QValueList<OClickableLabel *> minuteLst;
    QTime tm;
    struct Private;
    Private *d;

 private slots:
    void slotHour(bool b);
    void slotMinute(bool b);

 signals:
    /**
     * gets emitted when the time got changed by the user
     */
    void timeChanged(const QTime &);
};

/**
 *
 * @short
 * @version 1.0
 * @author Stefan Eilers
 */

class OTimePickerDialog: public OTimePickerDialogBase {
  Q_OBJECT

 public:
	OTimePickerDialog ( QWidget* parent = 0, const char* name = NULL, WFlags fl = 0 );
	~OTimePickerDialog() { };

	QTime time()const;

 public slots:
	void setTime( const QTime& time );
        void setHour( const QString& hour );
        void setMinute( const QString& minute );

 private:
	QTime m_time;
};
#endif
