#include <qcombobox.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qgroupbox.h>
#include <qhbuttongroup.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qhbox.h>

#ifndef DATEPREFERENCES_H
#define DATEPREFERENCES_H

class DatePreferences : public QDialog
  {
    Q_OBJECT

  public:
    DatePreferences ( QWidget * parent );
    ~DatePreferences();

    QPushButton *defaults;
    QComboBox *dateformat;
    QComboBox *dateseparator;
    QBoxLayout *layout;

  public slots:
    void changeDateFormat ( int );
    void changeDateSeparator ( int );
    void setDefaultDatePreferences ();
};

#endif

#ifndef TRANSACTIONPREFERENCES_H
#define TRANSACTIONPREFERENCES_H

class TransactionPreferences : public QDialog
  {
    Q_OBJECT

  public:
    TransactionPreferences ( QWidget * parent );
    ~TransactionPreferences();

    QCheckBox *showclearedtransactions;
    QCheckBox *excludetransfers;
    QHBox *limittransactionsbox;
    QLabel *limittransactionslabel;
    QComboBox *limittransactions;
    QPushButton *defaults;
    QBoxLayout *layout;

  public slots:
    void changeShowClearedPreference ( bool );
    void changeExcludeTranfersPreference ( bool );
    void setDefaultTransactionPreferences ();
    void changeLimitTransactionsPreference ( int );
};

#endif

#ifndef ACCOUNTPREFERENCES_H
#define ACCOUNTPREFERENCES_H

class AccountPreferences : public QDialog
  {
    Q_OBJECT

  public:
    AccountPreferences ( QWidget * parent );
    ~AccountPreferences();

    QCheckBox *currencysupport;
    QCheckBox *onetouch;
    QPushButton *defaults;
    QBoxLayout *layout;

  public slots:
    void changeCurrencySupport ( bool );
    void changeOneTouchViewing ( bool );
    void setDefaultAccountPreferences ();
};

#endif




