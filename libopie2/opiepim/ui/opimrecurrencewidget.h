/*
 * GPL and based on the widget from TT
 */

#ifndef OPIE_PIM_RECURRENCE_WIDGET_H
#define OPIE_PIM_RECURRENCE_WIDGET_H

#include <opie2/opimrecurrence.h>

#include <qpe/datebookmonth.h>

#include <qlist.h>
#include <qtoolbutton.h>
#include <qcheckbox.h>
#include <qdatetime.h>
#include <qbuttongroup.h>

#include "opimrecurrencebase.h"

/**
 * A widget to let the user select rules for recurrences.
 * This widget can take care of weekly, monthly, daily and yearly recurrence
 * It is used inside todolist and datebook.
 *
 *
 * @short Widget of selecting Recurrance
 * @author Trolltech, Holger Freyther
 * @version 0.9
 */
class OPimRecurrenceWidget : public OPimRecurrenceBase {
    Q_OBJECT
public:
    OPimRecurrenceWidget( bool startOnMonday,
                       const QDate& start, QWidget* parent = 0,
                       const char* name = 0,  bool modal = TRUE,
                       WFlags fl = 0 );
    OPimRecurrenceWidget( bool startOnMonday,
                          const Opie::OPimRecurrence& rp, const QDate& start,
                       QWidget* parent = 0, const char* name =0,
                       bool modal = TRUE, WFlags = 0 );
    ~OPimRecurrenceWidget();
    Opie::OPimRecurrence recurrence()const;
    QDate endDate()const;

public slots:
    void slotSetRType( int );
    /**
     * set the new end date
     */
    void endDateChanged( int, int, int );
    /**
     * enable/disable end date
     */
    void slotNoEnd( bool unused );
    void setStartDate( const QDate& );
    void setRecurrence( const Opie::OPimRecurrence& recur, const QDate& start );
    void setRecurrence( const Opie::OPimRecurrence& recur );

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

    class Private;
    Private *d;

};

#endif
