/****************************************************************************
** Form interface generated from reading ui file 'datebookdayheader.ui'
**
** Created: Mon Mar 10 20:49:12 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef DATEBOOKDAYHEADERBASE_H
#define DATEBOOKDAYHEADERBASE_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QToolButton;

class DateBookDayHeaderBase : public QWidget
{ 
    Q_OBJECT

public:
    DateBookDayHeaderBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DateBookDayHeaderBase();

    QToolButton* backweek;
    QToolButton* back;
    QToolButton* date;
    QButtonGroup* grpDays;
    QToolButton* cmdDay1;
    QToolButton* cmdDay2;
    QToolButton* cmdDay3;
    QToolButton* cmdDay4;
    QToolButton* cmdDay5;
    QToolButton* cmdDay6;
    QToolButton* cmdDay7;
    QToolButton* forward;
    QToolButton* forwardweek;

public slots:
    virtual void goBackWeek();
    virtual void goBack();
    virtual void goForward();
    virtual void goForwardWeek();
    virtual void setDate( int, int, int );
    virtual void setDay( int );

protected:
    QHBoxLayout* DateBookDayHeaderBaseLayout;
    QHBoxLayout* grpDaysLayout;
    bool event( QEvent* );
};

#endif // DATEBOOKDAYHEADERBASE_H
