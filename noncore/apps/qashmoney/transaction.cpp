// RESERVEDONE COLUMN NAME REPRESENTS THE LINEITEMID AND SHOULD BE CHANGED IN
// FUTURE VERSIONS OF QASHMONEY

// RESERVEDTWO REPRESENTS THE TRANSACTION DESCRIPTION

#include "transaction.h"
#include "account.h"
#include "transactiondisplay.h"

#include <stdlib.h>

extern Account *account;
extern Preferences *preferences;

Transaction::Transaction ()
  {
    tdb = sqlite_open ( "qmtransactions.db", 0, NULL );
  }

Transaction::~Transaction ()
  {
    sqlite_close ( tdb );
  }

void Transaction::addTransaction ( QString description, QString payee, int accountid, int parentid, int number, int day, int month, int year, float amount, int cleared, int budgetid, int lineitemid )
  {
    sqlite_exec_printf ( tdb, "insert into transactions values ( '%q', %i, %i, %i, %i, %i, %i, %.2f, %i, %i, 0, 0, 0, 0, 0, 0, %i, '%q', 0,
    0, 0, 0, NULL );", 0, 0, 0, ( const char * ) payee, accountid, parentid, number, day, month, year, amount, cleared, budgetid, lineitemid, ( const char * ) description );
  }

void Transaction::updateTransaction ( QString description, QString payee, int number, int day, int month, int year, float amount, int cleared, int budgetid, int lineitemid, int transactionid )
  {
    sqlite_exec_printf ( tdb, "update transactions set reservedtwo = '%q', payee = '%q', number = %i, day = %i, month = %i, year = %i, amount = %.2f,
      cleared = %i, budgetid = %i, reservedone = %i where transid = %i;", 0, 0, 0, ( const char * ) description, ( const char * ) payee, number, day, month, year,
      amount, cleared, budgetid, lineitemid, transactionid );
  }

void Transaction::deleteTransaction ( int transid )
  {
    sqlite_exec_printf ( tdb, "delete from transactions where transid = %i;", 0, 0, 0, transid );
  }

void Transaction::deleteAllTransactions ( int accountid )
  {
    sqlite_exec_printf ( tdb, "delete from transactions where accountid = %i;", 0, 0, 0, accountid );
  }

int Transaction::getAccountID ( int id )
  {
    char **results;
    sqlite_get_table_printf ( tdb, "select accountid from transactions where transid= %i;", &results, NULL, NULL, NULL, id );
    return atol ( results [ 1 ] );
  }

int Transaction::getNumberOfTransactions ()
  {
    char **results;
    sqlite_get_table ( tdb, "select count() from transactions;", &results, NULL, NULL, NULL );
    return atoi ( results [ 1 ] );
  }

int Transaction::getNumberOfTransactions ( int accountid )
  {
    char **results;
    sqlite_get_table_printf ( tdb, "select count() from transactions where accountid = %i;", &results, NULL, NULL, NULL, accountid );
    return atol ( results [ 1 ] );
  }

QString Transaction::getPayee ( int id )
  {
    char **results;
    sqlite_get_table_printf ( tdb, "select payee from transactions where transid= %i;", &results, NULL, NULL, NULL, id );
    return results [ 1 ];
  }

QString Transaction::getTransactionDescription ( int id )
  {
    char **results;
    sqlite_get_table_printf ( tdb, "select reservedtwo from transactions where transid= %i;", &results, NULL, NULL, NULL, id );
    return results [ 1 ];
  }

QString Transaction::getNumber ( int id )
  {
    char **results;
    sqlite_get_table_printf ( tdb, "select number from transactions where transid= %i;", &results, NULL, NULL, NULL, id );
    return results [ 1 ];
  }

QString Transaction::getAmount ( int id )
  {
    char **results;
    sqlite_get_table_printf ( tdb, "select amount from transactions where transid= %i;", &results, NULL, NULL, NULL, id );
    return results [ 1 ];
  }

QString Transaction::getAbsoluteAmount ( int id )
  {
    char **results;
    sqlite_get_table_printf ( tdb, "select abs ( amount ) from transactions where transid= %i;", &results, NULL, NULL, NULL, id );
    return results [ 1 ];
  }

int Transaction::getCleared ( int id )
  {
    char **results;
    sqlite_get_table_printf ( tdb, "select cleared from transactions where transid= %i;", &results, NULL, NULL, NULL, id );
    QString cleared = results [ 1 ];
    return cleared.toInt();
  }

void Transaction::setCleared ( int id, int cleared )
  {
    sqlite_exec_printf ( tdb, "update transactions set cleared = %i where transid = %i;", 0, 0, 0, cleared, id );
  }

int Transaction::getBudgetID ( int id )
  {
    char **results;
    sqlite_get_table_printf ( tdb, "select budgetid from transactions where transid = %i;", &results, NULL, NULL, NULL, id );
    QString budgetid = results [ 1 ];
    return budgetid.toInt();
  }

int Transaction::getLineItemID ( int id )
  {
    char **results;
    sqlite_get_table_printf ( tdb, "select reservedone from transactions where transid = %i;", &results, NULL, NULL, NULL, id );
    QString lineitemid = results [ 1 ];
    return lineitemid.toInt();
  }

int Transaction::getDay ( int id )
  {
    char **results;
    sqlite_get_table_printf ( tdb, "select day from transactions where transid= %i;", &results, NULL, NULL, NULL, id );
    QString daystring = results [ 1 ];
    return daystring.toInt();
  }

int Transaction::getMonth ( int id )
  {
    char **results;
    sqlite_get_table_printf ( tdb, "select month from transactions where transid= %i;", &results, NULL, NULL, NULL, id );
    QString monthstring = results [ 1 ];
    return monthstring.toInt();
  }

int Transaction::getYear ( int id )
  {
    char **results;
    sqlite_get_table_printf ( tdb, "select year from transactions where transid= %i;", &results, NULL, NULL, NULL, id );
    QString yearstring = results [ 1 ];
    return yearstring.toInt();
  }

void Transaction::displayTransactions ( QListView *listview, int id, bool children, const char *limit )
  {
    int showcleared = preferences->getPreference ( 3 );

    // select the transactions to display
    // two different statements are used based on
    // whether we are showing cleared transactions
    char **results;
    int rows, columns;
    if ( showcleared == 0 )
      {
        if ( account->getParentAccountID ( id ) == -1 && children == TRUE )
          sqlite_get_table_printf ( tdb, "select day, month, year, payee, amount, transid, accountid from transactions where cleared = 0 and parentid = %i and payee like '%q';", &results, &rows, &columns, NULL, id, limit );
        else
          sqlite_get_table_printf ( tdb, "select day, month, year, payee, amount, transid, accountid from transactions where cleared = 0 and accountid = %i and payee like '%q';", &results, &rows, &columns, NULL, id, limit );
      }
    else
      {
      if ( account->getParentAccountID ( id ) == -1 && children == TRUE )
          sqlite_get_table_printf ( tdb, "select day, month, year, payee, amount, transid, accountid from transactions where parentid = %i and payee like '%q';", &results, &rows, &columns, NULL, id, limit );
      else
        sqlite_get_table_printf ( tdb, "select day, month, year, payee, amount, transid, accountid from transactions where accountid = %i and payee like '%q';", &results, &rows, &columns, NULL, id, limit );
      }

    // iterate through the result list and display each item
    int counter = 7;
    while ( counter < ( ( rows + 1 ) * columns ) )
      {
        // construct the date
	//QString daystring = results [ counter ];
	//int day = results [ counter ].toInt ();
	//QString monthstring = results [ counter + 1 ];
	//int month = results [ counter + 1 ].toInt ();
	//QString yearstring = results [ counter + 2 ];
	//int year = results [ counter + 2 ].toInt ();
	QString date = preferences->getDate ( atoi ( results [ counter + 2 ] ), atoi ( results [ counter + 1 ] ), atoi ( results [ counter ] ) );

	// construct transaction name, amount, id
	QString payee = results [ counter + 3 ];
	QString amount = results [ counter + 4 ];
	QString transferid = results [ counter + 5 ];

        //determine the account name of the child accounts that we're displaying
        QString accountname = account->getAccountName ( atoi ( results [ counter + 6 ] ) );

	// fill in values
        if ( account->getParentAccountID ( id ) != -1 ) // use these constructors if we're showing a child account
          {
	    if ( showcleared == 1 && getCleared ( transferid.toInt() ) == 1 )
	      ColorListItem *item = new ColorListItem ( listview, date, payee, amount, transferid);
	    else
	      QListViewItem *item = new QListViewItem ( listview, date, payee, amount, transferid );
          }
       else
         {
	    if ( showcleared == 1 && getCleared ( transferid.toInt() ) == 1 )
	      ColorListItem *item = new ColorListItem ( listview, date, payee, amount, transferid, accountname );
	    else
	      QListViewItem *item = new QListViewItem ( listview, date, payee, amount, transferid, accountname );
         }

	// advance counter
	counter = counter + 7;
      }
  }

QString Transaction::getBudgetTotal ( int budgetid, int lineitemid, int year, int month, int viewtype )
  {
    // determine if we are viewing a years, months, or days budget
    // we have to pick a different sum for each
    char **results;
    switch ( viewtype )
      {
        case 1: // we are viewing a year
        sqlite_get_table_printf ( tdb, "select abs ( sum ( amount ) ) from transactions where year = %i and amount < 0 and budgetid = %i and reservedone = %i;", &results, NULL, NULL, NULL, year, budgetid, lineitemid );
        break;

        case 0: // we are viewing a month
        sqlite_get_table_printf ( tdb, "select abs ( sum ( amount ) ) from transactions where year = %i and month = %i and amount < 0 and budgetid = %i and reservedone = %i;", &results, NULL, NULL, NULL, year, month, budgetid, lineitemid );
        break;
      }
    QString amount = results [ 1 ];
    float total  = amount.toFloat();
    amount.setNum ( total, 'f', 2 );
    return amount;
  }

QString Transaction::getActualTotal ( int budgetid, int year, int month, int viewtype )
  {
    // determine if we are viewing a years, months, or days budget
    // we have to pick a different sum for each
    char **results;
    switch ( viewtype )
      {
        case 1: // we are viewing a year
        sqlite_get_table_printf ( tdb, "select abs ( sum ( amount ) ) from transactions where year = %i and amount < 0 and budgetid = %i;", &results, NULL, NULL, NULL, year, budgetid );
        break;

        case 0: // we are viewing a month
        sqlite_get_table_printf ( tdb, "select abs ( sum ( amount ) ) from transactions where year = %i and month = %i and amount < 0 and budgetid = %i;", &results, NULL, NULL, NULL, year, month, budgetid );
        break;
      }
    QString amount = results [ 1 ];
    float total  = amount.toFloat();
    amount.setNum ( total, 'f', 2 );
    return amount;
  }

void Transaction::clearBudgetIDs ( int budgetid, int lineitemid )
  {
    sqlite_exec_printf ( tdb, "update transactions set budgetid = -1 where budgetid = %i and reservedone = %i;", 0, 0, 0, budgetid, lineitemid );
  }

void Transaction::clearBudgetIDs ( int budgetid )
  {
    sqlite_exec_printf ( tdb, "update transactions set budgetid = -1 where budgetid = %i;", 0, 0, 0, budgetid );
  }

