#ifndef DATEBOOKWEEKLST
#define DATEBOOKWEEKLST

#include <qwidget.h>
#include <qdatetime.h> 
#include <qpe/event.h>
#include <qlabel.h> 
#include <qscrollview.h>

#include "datebookweeklstheader.h"
#include "datebookweeklstdayhdr.h"

#include "clickablelabel.h"

class QDateTime;
class DateBookDB;

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
    void pickDate();
    void setDate(int y, int m, int d);
signals:
    void dateChanged(int y, int w);
private:
    QDate date;
    bool onMonday;
};

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
		  const QString &str);
private:
    QDate date;
};

class DateBookWeekLstEvent: public ClickableLabel 
{
  Q_OBJECT
public:
    DateBookWeekLstEvent(const EffectiveEvent &ev, 
			 QWidget* parent = 0, const char* name = 0, 
			 WFlags fl = 0);
signals:
    void editEvent(const Event &e);
private slots:
    void editMe();
private:
    const EffectiveEvent event;
};

class DateBookWeekLstView: public QWidget
{
    Q_OBJECT
public:
    DateBookWeekLstView(QValueList<EffectiveEvent> &ev, QDate &d, bool onM,
			QWidget* parent = 0, const char* name = 0, 
			WFlags fl = 0 );
    ~DateBookWeekLstView();
signals:
    void editEvent(const Event &e);
    void showDate(int y, int m, int d);
    void addEvent(const QDateTime &start, const QDateTime &stop,
		  const QString &str);
private:
    bool onMonday;
protected slots:
    void keyPressEvent(QKeyEvent *);
};

class DateBookWeekLst : public QWidget 
{
    Q_OBJECT

public:
    DateBookWeekLst( bool ampm, bool onM, DateBookDB *newDB, 
		     QWidget *parent = 0, 
		     const char *name = 0 );
    void setDate( int y, int w );
    void setDate(const QDate &d );
    int week() const { return _week; };
    QDate date() const;
    
public slots:
    void redraw();
    void dateChanged(int y, int w);
protected slots:
    void keyPressEvent(QKeyEvent *);

signals:
    void showDate(int y, int m, int d);
    void addEvent(const QDateTime &start, const QDateTime &stop, 
		  const QString &str);
    void editEvent(const Event &e);

private:
  DateBookDB *db;
  int startTime;
  bool ampm;
  bool onMonday;
  int year, _week;
  DateBookWeekLstHeader *header;
  DateBookWeekLstView *view;
  QVBoxLayout *layout;
  QScrollView *scroll;
  
  void getEvents();
};

#endif

