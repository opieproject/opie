#ifndef DATEBOOKDAYALLDAYBASE_H
#define DATEBOOKDAYALLDAYBASE_H

#include <qvariant.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlist.h>
#include <qpe/event.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class DatebookAlldayDisp;
class DatebookEventDesc;
class DateBookDB;

class DatebookdayAllday : public QWidget
{
    Q_OBJECT

public:
    DatebookdayAllday(DateBookDB* db,
            QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~DatebookdayAllday();
    DatebookAlldayDisp* addEvent(const EffectiveEvent&e);
    const unsigned int items()const{return item_count;}

public slots:
    void removeAllEvents();

protected:
    QVBoxLayout* datebookdayalldayLayout;
    DatebookEventDesc * lblDesc;
    unsigned int item_count;
    QList<DatebookAlldayDisp> subWidgets;
    DateBookDB *dateBook;
};

class DatebookAlldayDisp : public QLabel
{
    Q_OBJECT

public:
    DatebookAlldayDisp(DateBookDB* db,const EffectiveEvent& e,
                       QWidget* parent=0,const char* name = 0, WFlags fl=0);
    virtual ~DatebookAlldayDisp();

signals:
    void deleteMe( const Event &e );
    void duplicateMe( const Event &e );
    void editMe( const Event &e );
    void beamMe( const Event &e );
    void displayMe(const Event &e);

public slots:

protected:
    EffectiveEvent m_Ev;
    DateBookDB* dateBook;
    void mousePressEvent( QMouseEvent *e );
    void beam_single_event();
};

class DatebookEventDesc: public QLabel
{
    Q_OBJECT

public:
    DatebookEventDesc(QWidget* Parent=0,const char* name = 0);
    virtual ~DatebookEventDesc();

public slots:
    void disp_event(const Event&e);

protected:
    void mousePressEvent(QMouseEvent*e);
    QTimer* m_Timer;
};

#endif // DATEBOOKDAYALLDAYBASE_H
