#ifndef NEWACCOUNT_H
#define NEWACCOUNT_H

#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qcheckbox.h>

#include "account.h"
#include "preferences.h"
#include "currency.h"

class NewAccount : public QDialog
  {
    Q_OBJECT

  public:
    NewAccount ( QWidget *parent = 0, const char *name = 0, bool modal = TRUE );
    ~NewAccount();

    QLabel *namelabel;
    QHBox *accountbox;
    QLineEdit* accountname;
    QPushButton* descriptionbutton;

    QHBox *datebox;
    QLabel *datelabel;
    QLineEdit* startdate;
    QPushButton* datebutton;

    QCheckBox* childcheckbox;

    QLabel *childlabel;
    QComboBox *childbox;

    QLabel *balancelabel;
    QHBox *balancebox;
    QLineEdit* accountbalance;
    QPushButton* balancecalculator;

    QLabel *creditlimitlabel;
    QHBox *creditlimitbox;
    QLineEdit* creditlimit;
    QPushButton* creditlimitcalculator;

    Currency *currencybox;

    QLabel *typelabel;
    QComboBox* accounttype;

    QGridLayout *layout;

    int getDay ();
    int getMonth ();
    int getYear ();
    QString getDescription ();
    void setDescription ( QString );

    // When a user edits an account and edits the date, this flag
    // is set to TRUE adding the new date to the account.  Otherwise,
    // the old date is used for the edited account.  This prevents a date
    // from reverting to 0/0/0 if the date is not edited
    bool getDateEdited ();

  public slots:
    void showChildPulldownMenu();
    void hideChildPulldownMenu();
    void showCalculator();
    void showCreditLimitCalculator ();
    void activateCreditLimit ( int );
    void showCalendar ();
    void addAccountDescription ();

  private:
    int index, year, month, day, currencypreference;
    QString accountdescription;
    bool dateedited;
};

#endif

