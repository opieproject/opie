// RESERVEDONE COLUMN NAME REPRESENTS THE LINEITEMID AND SHOULD BE CHANGED IN
// FUTURE VERSIONS OF QASHMONEY

// RESERVEDTWO REPRESENTS THE TRANSACTION DESCRIPTION

#include "transaction.h"
#include "account.h"
#include "transactiondisplay.h"

#include <stdlib.h>
#include <iostream.h>

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
    sqlite_exec_printf ( tdb, "insert into transactions values ( '%q', %i, %i, %i, %i, %i, %i, %.2f, %i, %i, 0, 0, 0, 0, 0, 0, %i, '%q', 0, "
    "0, 0, 0, NULL );", 0, 0, 0, ( const char * ) payee, accountid, parentid, number, day, month, year, amount, cleared, budgetid, lineitemid, ( const char * ) description );
  }

void Transaction::updateTransaction ( QString description, QString payee, int number, int day, int month, int year, float amount, int cleared, int budgetid, int lineitemid, int transactionid )
  {
    sqlite_exec_printf ( tdb, "update transactions set reservedtwo = '%q', payee = '%q', number = %i, day = %i, month = %i, year = %i, amount = %.2f,"
      "cleared = %i, budgetid = %i, reservedone = %i where transid = %i;", 0, 0, 0, ( const char * ) description, ( const char * ) payee, number, day, month, year,
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

char ** Transaction::selectAllTransactions ( QDate fromdate, bool children, const char *limit, int id )
  {
    // initialize variables
    char **results;
    int showcleared = preferences->getPreference ( 3 );
    QDate today = QDate::currentDate();
    int fromyear = fromdate.year();
    int toyear = today.year();
    int frommonth = fromdate.month();
    int tomonth = today.month();
    int fromday = fromdate.day();

    // construct the first part of the string
    QString query = "select day, month, year, payee, amount, transid, accountid from transactions where";

    if ( frommonth == tomonth && fromyear == toyear ) // our dates cross neither a month nor a year
      {
        query.append ( " year = " );
        query.append ( QString::number ( toyear ) );
        query.append ( " and month = " );
        query.append ( QString::number ( tomonth ) );
        query.append ( " and day >= " );
        query.append ( QString::number ( fromday ) );
        query.append ( " and" );
      }
    else if ( frommonth != tomonth && fromyear == toyear ) // our dates cross a month within the same year
      {
        query.append ( " year = " );
        query.append ( QString::number ( toyear ) );
        query.append ( " and ( ( month <= " );
        query.append ( QString::number ( tomonth ) );
        query.append ( " and month > " );
        query.append ( QString::number ( frommonth ) );
        query.append ( " ) or ( month = " );
        query.append ( QString::number ( frommonth ) );
        query.append ( " and day >= " );
        query.append ( QString::number ( fromday ) );
        query.append ( " ) ) and " );
      }
    else if ( fromyear != toyear && fromyear != 1900 ) // here we are showing transactions from an entire year
      {
        // divide this taks into two parts - get the transactions from the prior and then the current year
        // current year part
        int tmpfrommonth = 1; // set temporary from months and days to Jan. 1
        int tmpfromday = 1;
        query.append ( " ( year >= " );
        query.append ( QString::number ( fromyear ) );
        query.append ( " and ( month <= " );
        query.append ( QString::number ( tomonth ) );
        query.append ( " and month > " );
        query.append ( QString::number ( tmpfrommonth ) );
        query.append ( " ) or ( month = " );
        query.append ( QString::number ( tmpfrommonth ) );
        query.append ( " and day >= " );
        query.append ( QString::number ( tmpfromday ) );
        query.append ( " ) ) or" );

        // prior year part
        int tmptomonth = 12;
        query.append ( " ( year = " );
        query.append ( QString::number ( fromyear ) );
        query.append ( " and ( ( month <= " );
        query.append ( QString::number ( tmptomonth ) );
        query.append ( " and month > " );
        query.append ( QString::number ( frommonth ) );
        query.append ( " ) or ( month = " );
        query.append ( QString::number ( frommonth ) );
        query.append ( " and day >= " );
        query.append ( QString::number ( fromday ) );
        query.append ( " ) ) ) and " );
      }

    if ( account->getParentAccountID ( id ) == -1 && children == TRUE )
      query.append ( " parentid = %i and payee like '%q';" );
    else
      query.append ( " accountid = %i and payee like '%q';" );

    sqlite_get_table_printf ( tdb, query, &results, &rows, &columns, NULL, id, limit );
    return results;
  }

char ** Transaction::selectNonClearedTransactions ( QDate fromdate, bool children, const char *limit, int id )
  {
    char **results;
    if ( account->getParentAccountID ( id ) == -1 && children == TRUE )
     sqlite_get_table_printf ( tdb, "select day, month, year, payee, amount, transid, accountid from transactions where cleared = 0 and parentid = %i and payee like '%q';", &results, &rows, &columns, NULL, id, limit );
    else
      sqlite_get_table_printf ( tdb, "select day, month, year, payee, amount, transid, accountid from transactions where cleared = 0 and accountid = %i and payee like '%q';", &results, &rows, &columns, NULL, id, limit );
    return results;
  }

void Transaction::displayTransactions ( QListView *listview, int id, bool children, const char *limit, QDate displaydate )
  {
    int showcleared = preferences->getPreference ( 3 );

    char **results;
    if ( showcleared == 0 )
      results = selectNonClearedTransactions ( displaydate, children, limit, id );
    else
      results = selectAllTransactions ( displaydate, children, limit, id );

    // iterate through the result list and display each item
    int counter = 7;
    while ( counter < ( ( rows + 1 ) * columns ) )
      {
        //QDate testdate ( atoi ( results [ counter + 2 ] ), atoi ( results [ counter + 1 ] ), atoi ( results [ counter ] ) );
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
	     ColorListItem *item = new ColorListItem ( listview, date, payee, amount, transferid );
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

