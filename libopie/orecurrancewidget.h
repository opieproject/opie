/*
 * GPL and based on the widget from TT
 */

#ifndef OPIE_RECURRANCE_WIDGET_H
#define OPIE_RECURRANCE_WIDGET_H

#include <qlist.h>
#include <qtoolbutton.h>
#include <qcheckbox.h>
#include <qdatetime.h>
#include <qbuttongroup.h>

#include <qpe/datebookmonth.h>

#include "orecurrancebase.h"
#include <opie/orecur.h>

class ORecurranceWidget : public ORecurranceBase {
    Q_OBJECT
public:
    ORecurranceWidget( bool startOnMonday,
                       const QDate& start, QWidget* parent = 0,
                       const char* name = 0,  bool modal = TRUE,
                       WFlags fl = 0 );
    ORecurranceWidget( bool startOnMonday,
                       const ORecur& rp, const QDate& start,
                       QWidget* parent = 0, const char* name =0,
                       bool modal = TRUE, WFlags = 0 );
    ~ORecurranceWidget();
    ORecur recurrance()const;
    QDate endDate()const;

public slots:
    void slotSetRType( int );
    void endDateChanged( int, int, int );
    void slotNoEnd( bool unused );

private slots:
    void setupRepeatLabel( const QString& );
    void setupRepeatLabel( int );
    void slotWeekLabel();
    void slotMonthLabel( int );
    void slotChangeStartOfWeek( bool onMonday );

private:
    void setupNone();
    void setupDaily();
    void setupWeekly();
    void setupMonthly();
    void setupYearly();

    enum repeatButtons { None, Day, Week, Month, Year };
    void init();
    void hideExtras();
    void showRepeatStuff();

    QList<QToolButton> listRTypeButtons;
    QList<QToolButton> listExtra;
    QDate start;    // only used in one spot...
    QDate end;
    repeatButtons currInterval;
    bool startWeekOnMonday : 1;
    DateBookMonth *repeatPicker;

};

#endif
