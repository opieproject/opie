#include "transfer.h"
#include "account.h"
#include "transactiondisplay.h"
#include <stdlib.h>
#include <iostream.h>

extern Account *account;
extern Preferences *preferences;

Transfer::Transfer ()
  {
    db = sqlite_open ( "qmtransfers.db", 0, 0 );
  }

Transfer::~Transfer ()
  {
    sqlite_close ( db );
  }

void Transfer::addTransfer ( int fromaccount, int fromparent, int toaccount, int toparent, int day, int month, int year, float amount, int cleared )
  {
    int nextrowid = -1;
    char **results;
    sqlite_get_table ( db, "select count() from transfers;", &results, 0, 0, 0 );
    if ( atoi ( results [ 1 ] ) != 0 )
      {
        char **results;
        sqlite_get_table ( db, "select min ( rowid ) from transfers;", &results, 0, 0, 0 );
        nextrowid = ( atoi ( results [ 1 ] ) ) - 1;
      }
    sqlite_exec_printf ( db, "insert into transfers values ( %i, %i, %i, %i, %i, %i, %i, 0, 0, %.2f, %i, 0, 0, 0, 0, 0, %i );", 0, 0, 0, fromaccount, fromparent, toaccount, toparent, day, month, year, amount, cleared, nextrowid );
  }

void Transfer::updateTransfer ( int fromaccount, int fromparent, int toaccount, int toparent, int day, int month, int year, float amount, int cleared, int transferid )
  {
    sqlite_exec_printf ( db, "update transfers set fromaccount = %i, fromparent = %i, toaccount = %i, toparent = %i, day = %i, month = %i, year = %i,"
     "amount = %.2f, cleared = %i where transferid = %i;", 0, 0, 0, fromaccount, fromparent, toaccount, toparent, day, month, year, amount, cleared, transferid );
  }

void Transfer::deleteTransfer ( int transferid )
  {
    sqlite_exec_printf ( db, "delete from transfers where transferid = %i;", 0, 0, 0, transferid );
  }

void Transfer::deleteAllTransfers ( int accountid )
  {
    sqlite_exec_printf ( db, "delete from transfers where fromaccount = %i;", 0, 0, 0, accountid );
    sqlite_exec_printf ( db, "delete from transfers where toaccount = %i;", 0, 0, 0, accountid );
  }

int Transfer::getNumberOfTransfers ()
  {
    char **results;
    sqlite_get_table ( db, "select count() from transfers;", &results, 0, 0, 0 );
    return atoi ( results [ 1 ] );
  }

int Transfer::getNumberOfTransfers ( int accountid )
  {
    char **results;
    sqlite_get_table_printf ( db, "select count() from transfers where fromaccount = %i;", &results, 0, 0, 0, accountid );
    int transfers = atoi ( results [ 1 ] );
    sqlite_get_table_printf ( db, "select count() from transfers where toaccount = %i;", &results, 0, 0, 0, accountid );
    transfers = transfers + atoi ( results [ 1 ] );
    return transfers;
  }

void Transfer::displayTransfers ( QListView *listview, int accountid, bool children, QDate displaydate )
  {
    int showcleared = preferences->getPreference ( 3 );

    // select the from transfers to display
    char **results;
    int rows, columns;
    if ( account->getParentAccountID ( accountid ) == -1 && children == TRUE )
      {
        if ( showcleared == 0 )
          sqlite_get_table_printf ( db, "select day, month, year, amount, transferid, fromaccount, toaccount from transfers where cleared = 0 and toparent = %i;", &results, &rows, &columns, 0, accountid );
        else
          sqlite_get_table_printf ( db, "select day, month, year, amount, transferid, fromaccount, toaccount from transfers where toparent = %i;", &results, &rows, &columns, 0, accountid );
      }
    else
      {
        if ( showcleared == 0 )
          sqlite_get_table_printf ( db, "select day, month, year, amount, transferid, fromaccount, toaccount from transfers where cleared = 0 and toaccount = %i;", &results, &rows, &columns, 0, accountid );
        else
          sqlite_get_table_printf ( db, "select day, month, year, amount, transferid, fromaccount, toaccount from transfers where toaccount = %i;", &results, &rows, &columns, 0, accountid );
      }

    // iterate through the list and display the from items
    int counter = 7;
    int position = 0;
    while ( counter < ( ( rows + 1 ) * columns ) )
      {
        // construct the date
	QString daystring = results [ counter ];
	int day = daystring.toInt ();
	QString monthstring = results [ counter + 1 ];
	int month = monthstring.toInt ();
	QString yearstring = results [ counter + 2 ];
	int year = yearstring.toInt ();
	QString date = preferences->getDate ( year, month, day );
        QDate testdate ( year, month, day );

        //construct the amount and id strings
        QString amount = results [ counter + 3 ];
        QString id = results [ counter + 4 ];

        // construct the transaction name
        QString transactionname = "FROM: ";
        QString temp1 = results [ counter + 5 ];
        transactionname.append ( account->getAccountName ( temp1.toInt() ) );

        QString toaccount = account->getAccountName ( atol ( results [ counter + 6 ] ) );

        if ( testdate >= displaydate || showcleared == 0 )
          {
            // display this transfer
            if ( account->getParentAccountID ( accountid ) == -1 )
              {
                if ( showcleared == 1 && getCleared ( id.toInt() ) == 1 )
                  ColorListItem *item = new ColorListItem ( listview, date, transactionname, amount, id, toaccount );
                else
                  QListViewItem *item = new QListViewItem ( listview, date, transactionname, amount, id, toaccount );
              }
            else
              {
                if ( showcleared == 1 && getCleared ( id.toInt() ) == 1 )
                  ColorListItem *item = new ColorListItem ( listview, date, transactionname, amount, id );
                else
                  QListViewItem *item = new QListViewItem ( listview, date, transactionname, amount, id );
              }
          }
        counter = counter + 7;
      }

    // select the to transfers to display
    char **toresults;
    rows = 0;
    columns = 0;
    if ( account->getParentAccountID ( accountid ) == -1 && children == TRUE )
      {
        if ( showcleared == 0 )
          sqlite_get_table_printf ( db, "select day, month, year, amount, transferid, fromaccount, toaccount from transfers where cleared = 0 and fromparent = %i;", &toresults, &rows, &columns, 0, accountid );
        else
          sqlite_get_table_printf ( db, "select day, month, year, amount, transferid, fromaccount, toaccount from transfers where fromparent = %i;", &toresults, &rows, &columns, 0, accountid );
      }
    else
      {
        if ( showcleared == 0 )
          sqlite_get_table_printf ( db, "select day, month, year, amount, transferid, fromaccount, toaccount from transfers where cleared = 0 and fromaccount = %i;", &toresults, &rows, &columns, 0, accountid );
        else
          sqlite_get_table_printf ( db, "select day, month, year, amount, transferid, fromaccount, toaccount from transfers where fromaccount = %i;", &toresults, &rows, &columns, 0, accountid );
      }

    // iterate through the list and display the from items
    counter = 7;
    position = 0;
    while ( counter < ( ( rows + 1 ) * columns ) )
      {
        // construct the date
	QString daystring = toresults [ counter ];
	int day = daystring.toInt ();
	QString monthstring = toresults [ counter + 1 ];
	int month = monthstring.toInt ();
	QString yearstring = toresults [ counter + 2 ];
	int year = yearstring.toInt ();
	QString date = preferences->getDate ( year, month, day );
        QDate testdate ( year, month, day );

        //construct the amount and id strings
        QString amount = toresults [ counter + 3 ];
        amount.prepend ( "-" );
        QString id = toresults [ counter + 4 ];

        // construct the transaction name
        QString transactionname = "TO: ";
        QString temp1 = toresults [ counter + 6 ];
        transactionname.append ( account->getAccountName ( temp1.toInt() ) );

        QString fromaccount = account->getAccountName ( atol ( toresults [ counter + 5 ] ) );

         if ( testdate >= displaydate || showcleared == 0 )
           {
             // display this transfer
             if ( account->getParentAccountID ( accountid ) == -1 )
              {
                if ( showcleared == 1 && getCleared ( id.toInt() ) == 1 )
                  ColorListItem *item = new ColorListItem ( listview, date, transactionname, amount, id, fromaccount );
                else
                  QListViewItem *item = new QListViewItem ( listview, date, transactionname, amount, id, fromaccount );
              }
            else
              {
                if ( showcleared == 1 && getCleared ( id.toInt() ) == 1 )
                  ColorListItem *item = new ColorListItem ( listview, date, transactionname, amount, id );
                else
                  QListViewItem *item = new QListViewItem ( listview, date, transactionname, amount, id );
              }
          }

        counter = counter + 7;
      }
  }

int Transfer::getCleared ( int id )
  {
    char **results;
    sqlite_get_table_printf ( db, "select cleared from transfers where transferid= %i;", &results, 0, 0, 0, id );
    return atoi ( results [ 1 ] );
  }

void Transfer::setCleared ( int id, int cleared )
  {
    sqlite_exec_printf ( db, "update transfers set cleared = %i where transferid = %i;", 0, 0, 0, cleared, id );
  }

int Transfer::getFromAccountID ( int id )
  {
    char **results;
    sqlite_get_table_printf ( db, "select fromaccount from transfers where transferid= %i;", &results, 0, 0, 0, id );
    return atoi ( results [ 1 ] );
  }

int Transfer::getToAccountID ( int id )
  {
    char **results;
    sqlite_get_table_printf ( db, "select toaccount from transfers where transferid= %i;", &results, 0, 0, 0, id );
    return atoi ( results [ 1 ] );
  }

int Transfer::getDay ( int id )
  {
    char **results;
    sqlite_get_table_printf ( db, "select day from transfers where transferid= %i;", &results, 0, 0, 0, id );
    return atoi ( results [ 1 ] );
  }

int Transfer::getMonth ( int id )
  {
    char **results;
    sqlite_get_table_printf ( db, "select month from transfers where transferid= %i;", &results, 0, 0, 0, id );
    return atoi ( results [ 1 ] );
  }

int Transfer::getYear ( int id )
  {
    char **results;
    sqlite_get_table_printf ( db, "select year from transfers where transferid= %i;", &results, 0, 0, 0, id );
    return atoi ( results [ 1 ] );
  }

QString Transfer::getAmount ( int id )
  {
    char **results;
    sqlite_get_table_printf ( db, "select amount from transfers where transferid= %i;", &results, 0, 0, 0, id );
    return results [ 1 ];
  }


