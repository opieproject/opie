#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <qstring.h>
#include <qlistview.h>
#include <sqlite.h>

#include "preferences.h"

class Transaction
  {
    public:

      Transaction ();
      ~Transaction ();

      // This function adds a new transaction to the database.  It takes the  payee, accountid,
      // budgetid, number, day, month, year, amount, cleared
      void addTransaction ( QString description, QString payee, int accountid, int parentid, int number, int day, int month, int year, float amount, int cleared, int budgetid, int lineitemid );

      // This updates an already existing transaction
      void updateTransaction ( QString, QString, int, int, int, int, float, int, int, int, int );

      // Deletes a transaction.  Takes the transid as its parameter
      void deleteTransaction ( int );

      // Returns the number of transactions
      int getNumberOfTransactions ();

      // this is an overloaded function that returns the number of
      // transactions for an account
      int getNumberOfTransactions ( int );

      // deletes all transactions for the provided accountid
      void deleteAllTransactions ( int accountid );

      QString getBudgetTotal ( int budgetid, int lineitemid, int year, int month, int viewtype );
      QString getActualTotal ( int budgetid, int year, int month, int viewtype );

      // These two functions clear budget ids is either a line item or an entire budget is deleted
      void clearBudgetIDs ( int, int );
      void clearBudgetIDs ( int );

    public slots:

      void displayTransactions ( QListView *, int, bool, const char * );
      QString getPayee ( int );
      QString getTransactionDescription ( int );
      QString getNumber ( int );
      QString getAmount ( int );
      QString getAbsoluteAmount ( int );
      int getCleared ( int );
      void setCleared ( int id, int cleared );
      int getBudgetID ( int id );
      int getLineItemID ( int id );
      int getDay ( int );
      int getMonth ( int );
      int getYear ( int );
      int getAccountID ( int id );

    private:

      sqlite *tdb;
  };

#endif



