#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <qstring.h>
#include <qlistview.h>
#include <qcombobox.h>
#include <sqlite.h>

class Account
  {
    public:

      Account ();
      ~Account ();

      // This function adds a new account to the database.  It takes the account name, parent,
      // initial balance and the account type, description, credit limit, statementbalancedate
      // as three integers, and the statementbalance amount
      // The parent is an integer account id.  Its -1 if there is no parent
      // The account types (so far)  are 0=not defined 1=parent checking 2=child checking
      void addAccount ( QString, int, float, int, QString, float, int, int, int, float, const char * );

      // updates certain parts of an account
      void updateAccount ( QString name, QString description, QString currencycode, int accountid );

      void setAccountExpanded ( int expanded, int accountid );
      int getAccountExpanded ( int id );

      // Deletes an account
      // Takes the accountid as its parameter
      void deleteAccount ( int );

      // Returns the number of accounts
      int getNumberOfAccounts ();

      // returns number of child accounts for a given parent
      // this function is useless for child accounts.  It will
      // always return 0
      int getNumberOfChildAccounts ( int );

      // This function retrieves all transactions for an account and updates the
      // account balance based on the transactions
      void updateAccountBalance ( int accountid );
      //void changeAccountBalance ( int accountid, float amount );

      // updates a parent account
      void changeParentAccountBalance ( int parentid );

      // Returns the parent account ID for an account
      // Takes the account name as its parameter or the account id
      int getParentAccountID ( QString accountname );
      int getParentAccountID ( int id );

      // This takes a QListView and puts parents and children memorys
      // into the list view
      void displayAccounts ( QListView * );

      // This function displays a sorted list of account names in a combobox
      // Takes the combobox address for its parameter
      int displayParentAccountNames ( QComboBox *, QString );

      int getAccountType ( int );  // returns account type for given account id

      // The next three collectively return a date or balance
      // They take the accountid as their parameters
      int getStatementDay ( int );
      int getStatementMonth ( int );
      int getStatementYear ( int );
      float getStatementBalance ( int );

      // Returns account description and name
      QString getAccountDescription ( int accountid );
      QString getCurrencyCode ( int accountid );
      QString getAccountName ( int accountid );
      QStringList getAccountNames ();
      QStringList getAccountIDs ();
      QString getAccountBalance ( int accountid );

      // returns account credit limit
      float getAccountCreditLimit ( int );

      // The primary database that stores all our data
      sqlite *adb;
  };

class GreyBackgroundItem : public QListViewItem
  {
    public:

      GreyBackgroundItem ( QListView *parent );
      GreyBackgroundItem ( QListView *parent, QString label1, QString label2, QString label3 );
      GreyBackgroundItem ( QListView *parent, QString label1, QString label2, QString label3, QString label4 );
      GreyBackgroundItem ( QListView *parent, QString label1, QString label2, QString label3, QString label4, QString label5 );

      virtual void paintCell ( QPainter *p, const QColorGroup &cg, int column, int width, int alignment );

  };

#endif

