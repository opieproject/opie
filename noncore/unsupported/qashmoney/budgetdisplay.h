#ifndef BUDGETDISPLAY_H
#define BUDGETDISPLAY_H

#include <qlistview.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qhbox.h>

#include "currency.h"

class BudgetDisplay : public QWidget
  {
    Q_OBJECT

    public:
      BudgetDisplay ( QWidget *parent );

      QMenuBar *menu;
      QPopupMenu *budgetmenu;
      QPopupMenu *lineitemsmenu;
      QPopupMenu *datemenu;

      QHBox *firstline;
      QHBox *secondline;

      QLabel *budgeted;
      QLabel *actual;
      QLabel *date;

      QLineEdit *budgetname;
      QLineEdit *description;
      Currency *currencybox;

      QLineEdit *lineitemname;
      QLineEdit *lineitemamount;
      QComboBox *lineitemtime;

      QListView *listview;
      QComboBox *budgetbox;
      QComboBox *budgetview;

      QBoxLayout *layout;

      int getIDColumn ();

    public slots:
      void displayBudgetNames ();
      void displayLineItems ();
      void updateBudgetInformation ();

    private slots:
      void saveColumnSize ( int column, int oldsize, int newsize );
      void newBudget ();
      void deleteBudget ();
      void setCurrentBudget ( int );
      void setCurrentView ( int );
      void showCalendar ();
      void newLineItem ();
      void deleteLineItem ();
      void checkBudgets ();
      void editBudget ();
      void editLineItem ();
      void constructBudgetWindow ();
      void constructLineItemWindow ();
      void saveSortingPreference ( int column );

    private:
      QStringList *names;
      QStringList *ids;
      int currentbudget, year, month, day;
      QDate newDate;
      QString totalbudget, totalactual;
      QString datelabel;
      QDialog *nb;
      QDialog *newlineitem;
};

#endif // BUDGETDISPLAY_H

