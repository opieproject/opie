#ifndef NEWTRANSACTION_H
#define NEWTRANSACTION_H

#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qstringlist.h>
#include <qhbox.h>

#include "transaction.h"
#include "preferences.h"

class NewTransaction : public QDialog
{
    Q_OBJECT

  public:

    int year, month, day;

    NewTransaction ( QWidget* parent );
    ~NewTransaction();

    QLabel *namelabel;
    QHBox *transactionnamebox;
    QComboBox* transactionname;
    QPushButton* descriptionbutton;

    QLabel *amountlabel;
    QHBox *transactionamountbox;
    QLineEdit* transactionamount;
    QPushButton* calculatorbutton;

    QLabel *datelabel;
    QHBox *transactiondatebox;
    QLineEdit* transactiondate;
    QPushButton* datebutton;

    QCheckBox *clearedcheckbox;

    QLabel *numberlabel;
    QLineEdit *transactionnumber;

    QLabel *budgetlabel;
    QComboBox *budgetbox;

    QLabel *lineitemlabel;
    QComboBox *lineitembox;

    QCheckBox *depositbox;

    QGridLayout *layout;

    int getDay ();
    int getMonth ();
    int getYear ();
    QString getDescription ();
    void setDescription ( QString );
    void setComboBoxes ( int, int );

    // When a user edits an transaction and edits the date, this flag
    // is set to TRUE adding the new date to the transaction.  Otherwise,
    // the old date is used for the edited transaction.  This prevents a date
    // from reverting to 0/0/0 if the date is not edited
    bool getDateEdited ();

  public slots:

    void showCalculator();
    void showCalendar ();
    void addTransactionDescription ();
    int getNameIndex ( QString name );
    int getCurrentBudget ();
    int getCurrentLineItem ();
    int getBudgetIndex ( int budgetid );
    int getLineItemIndex ( int lineitemid );
    void setLineItems ();

  private slots:
    void setCurrentBudget ( int index );
    void setCurrentLineItem ( int index );

  private:
    bool dateedited;
    int index, currentbudget, currentlineitem;
    QString transactiondescription;
    QStringList *budgetnameslist, *budgetidslist;
    QStringList lineitemslist, lineitemidslist;

};

#endif
