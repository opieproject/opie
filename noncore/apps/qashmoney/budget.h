#ifndef BUDGET_H
#define BUDGET_H

#include <qstring.h>
#include <qstringlist.h>
#include <qlistview.h>
#include <sqlite.h>

class Budget
  {
    public:

      Budget ();
      ~Budget ();

      int addBudget ( QString name, int type, QString description, QString currency, int startday, int startmonth, int startyear, int endday, int endmonth, int endyear, int defaultview );
      void updateBudget ( QString name, QString description, QString currency, int budgetid );
      void deleteBudget ( int budgetid );
      int getNumberOfBudgets ();
      int getNumberOfLineItems ( int budgetid );

      QStringList* getBudgetNames ();
      QStringList* getBudgetIDs ();
      QStringList getLineItems ( int budgetid );
      QStringList getLineItemIDs ( int budgetid );
      QString getBudgetName ( int budgetid );
      QString getBudgetDescription ( int budgetid );
      QString getCurrency ( int budgetid );
      QString getBudgetTotal ( int budgetid, int viewtype );

      int getLastAdded ();

      int addLineItem ( int budgetid, QString lineitemname, float lineitemamount, int lineitemtype );
      void updateLineItem ( QString lineitemname, float lineitemamount, int lineitemtype, int budgetid, int lineitemid );
      void displayLineItems ( int budgetid, QListView *listview, int month, int year, int viewtype );
      void deleteLineItem ( int budgetid, int lineitemid );
      int getLineItemTime ( int budgetid, int lineitemid );
      float getLineItemAmount ( int budgetid, int lineitemid );

    private:
      sqlite *bdb;
  };

#endif

