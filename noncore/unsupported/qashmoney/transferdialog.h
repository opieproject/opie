#ifndef TRANSFERDIALOG_H
#define TRANSFERDIALOG_H

#include <qdialog.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qhbox.h>
#include <qlabel.h>

#include "preferences.h"
#include "account.h"

class TransferDialog : public QDialog
{
    Q_OBJECT

public:
    TransferDialog ( QWidget *parent, int fromaccountid, int toaccountid );

    QLabel *fromaccountlabel;
    QLabel *fromaccount;

    QLabel *toaccountlabel;
    QLabel *toaccount;

    QLabel *datelabel;

    QHBox *datebox;
    QLineEdit *date;
    QPushButton *datebutton;

    QLabel *amounttlabel;

    QHBox *amountbox;
    QLineEdit *amount;
    QPushButton *calculatorbutton;

    QCheckBox *clearedcheckbox;

    QBoxLayout *layout;

    bool getDateEdited ();

public slots:

    void showCalendar ();
    void showCalculator ();
    int getDay ();
    int getMonth ();
    int getYear ();

private:

    int fromaccountid;
    int toaccountid;
    int year, month, day;
    bool dateedited;
};

#endif
