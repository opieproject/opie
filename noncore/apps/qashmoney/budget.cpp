#include "budget.h"
#include "transaction.h"
#include <stdlib.h>
#include <iostream.h>

extern Transaction *transaction;

Budget::Budget ()
  {
    bdb = sqlite_open ( "qmbudgets.db", 0, NULL );
  }

Budget::~Budget ()
  {
    sqlite_close ( bdb );
  }

int Budget::addBudget ( QString name, int type, QString description, QString currency, int startday, int startmonth, int startyear, int endday, int endmonth, int endyear, int defaultview )
  {
    sqlite_exec_printf ( bdb, "insert into budgets values ( '%q', %i, '%q', '%q', %i, %i, %i, %i, %i, %i, %i, NULL );", 0, 0, 0, ( const char * ) name, type, ( const char * ) description, ( const char * ) currency, startday, startmonth, startyear, endday, endmonth, endyear, defaultview );
    char **results;
    sqlite_get_table ( bdb, "select last_insert_rowid() from budgets;", &results, NULL, NULL, NULL );
    QString tablename = "table";
    tablename.append ( results [ 1 ] );
    sqlite_exec_printf ( bdb, "create table '%q' ( name, lineitemamount, type, lineitemid integer primary key );", 0, 0, 0, ( const char* ) tablename );
    return atoi ( results [ 1 ] );
  }

void Budget::updateBudget ( QString name, QString description, QString currency, int budgetid )
  {
    sqlite_exec_printf ( bdb, "update budgets set name = '%q', description = '%q', currency = '%q' where budgetid = %i;", 0, 0, 0, ( const char * ) name, ( const char * ) description, ( const char * ) currency, budgetid );
  }

void Budget::deleteBudget ( int budgetid )
  {
    if ( getNumberOfBudgets() != 0 )
      {
        QString tablename = "table";
        tablename.append ( QString::number ( budgetid ) );
        sqlite_exec_printf ( bdb, "delete from budgets where budgetid = %i;", 0, 0, 0, budgetid );
        sqlite_exec_printf ( bdb, "drop table '%q';", 0, 0, 0, ( const char* ) tablename );
      }
  }

int Budget::getNumberOfBudgets ()
  {
    char **results;
    sqlite_get_table ( bdb, "select count() from budgets;", &results, NULL, NULL, NULL );
    return atoi ( results [ 1 ] );
  }

int Budget::getNumberOfLineItems ( int budgetid )
  {
    QString tablename = "table";
    tablename.append ( QString::number ( budgetid ) );
    char **results;
    sqlite_get_table_printf ( bdb, "select count() from '%q';", &results, NULL, NULL, NULL, ( const char * ) tablename );
    return atoi ( results [ 1 ] );
  }

QStringList* Budget::getBudgetNames ()
  {
    QStringList *names = new QStringList ();
    char **results;
    int rows, counter;
    sqlite_get_table ( bdb, "select name from budgets;", &results, &rows, NULL, NULL );
    names->append ( "None" );
    for ( counter = 0; counter < rows; counter++ )
      names->append ( results [ counter+1 ] );
    return names;
  }

QString Budget::getBudgetName ( int budgetid )
  {
    char **results;
    sqlite_get_table_printf ( bdb, "select name from budgets where budgetid= %i;", &results, NULL, NULL, NULL, budgetid );
    return ( QString ) results [ 1 ];
  }

QString Budget::getBudgetDescription ( int budgetid )
  {
    char **results;
    sqlite_get_table_printf ( bdb, "select description from budgets where budgetid= %i;", &results, NULL, NULL, NULL, budgetid );
    return ( QString ) results [ 1 ];
  }

QString Budget::getCurrency ( int budgetid )
  {
    char **results;
    sqlite_get_table_printf ( bdb, "select currency from budgets where budgetid= %i;", &results, NULL, NULL, NULL, budgetid );
    return ( QString ) results [ 1 ];
  }

QStringList* Budget::getBudgetIDs ()
  {
    QStringList *ids = new QStringList ();
    char **results;
    int rows, counter;
    sqlite_get_table ( bdb, "select budgetid from budgets;", &results, &rows, NULL, NULL );
    for ( counter = 0; counter < rows; counter++ )
      ids->append ( results [ counter+1 ] );
    return ids;
  }

int Budget::addLineItem ( int budgetid, QString lineitemname, float lineitemamount, int lineitemtype )
  {
    QString tablename = "table";
    tablename.append ( QString::number ( budgetid ) );
    sqlite_exec_printf ( bdb, "insert into '%q' values ( '%q', %.2f, %i, NULL );", 0, 0, 0, ( const char* ) tablename, ( const char* ) lineitemname, lineitemamount, lineitemtype );
    char **results;
    sqlite_get_table_printf ( bdb, "select last_insert_rowid() from '%q';", &results, NULL, NULL, NULL, ( const char* ) tablename );
    return atoi ( results [ 1 ] );
  }

void Budget::updateLineItem ( QString lineitemname, float lineitemamount, int lineitemtype, int budgetid, int lineitemid )
  {
    QString tablename = "table";
    tablename.append ( QString::number ( budgetid ) );
    sqlite_exec_printf ( bdb, "update '%q' set name = '%q', lineitemamount = %f, type = %i where lineitemid = %i;", 0, 0, 0, ( const char* ) tablename, ( const char * ) lineitemname, lineitemamount, lineitemtype, lineitemid );
  }

void Budget::deleteLineItem ( int budgetid, int lineitemid )
  {
    QString tablename = "table";
    tablename.append ( QString::number ( budgetid ) );
   sqlite_exec_printf ( bdb, "delete from '%q' where lineitemid = %i;", 0, 0, 0, ( const char * ) tablename, lineitemid );
  }

void Budget::displayLineItems ( int budgetid, QListView *listview, int month, int year, int viewtype )
  {
    QString tablename = "table";
    tablename.append ( QString::number ( budgetid ) );
    char **results;
    int rows, columns, counter;
    sqlite_get_table_printf ( bdb, "select name, lineitemamount, lineitemid from '%q';", &results, &rows, &columns, NULL, ( const char * ) tablename );
    int total = ( ( rows + 1 ) * columns );
    for ( counter = 3; counter < total; counter = counter + 3 )
      {
        float amount = 0;
        if ( viewtype == 0 )
          {
            QString lineitemamount = results [ counter + 1 ];
            amount = lineitemamount.toFloat() / 12;
          }
        else
          {
            QString lineitemamount = results [ counter + 1 ];
            amount = lineitemamount.toFloat();
          }
        QListViewItem *item = new QListViewItem ( listview, results [ counter ], QString::number ( amount, 'f', 2 ), transaction->getBudgetTotal ( budgetid, atoi ( results [ counter + 2 ] ), year, month, viewtype ), results [ counter + 2 ] );
      }
  }

QStringList Budget::getLineItems ( int budgetid )
  {
    QString tablename = "table";
    tablename.append ( QString::number ( budgetid ) );
    QStringList lineitems;
    char **results;
    int rows, counter;
    sqlite_get_table_printf ( bdb, "select name from '%q';", &results, &rows, NULL, NULL, (const char*) tablename );
    for ( counter = 0; counter < rows; counter++ )
      lineitems.append ( results [ counter + 1 ] );
    return lineitems;
  }

QStringList Budget::getLineItemIDs ( int budgetid )
  {
    QString tablename = "table";
    tablename.append ( QString::number ( budgetid ) );
    QStringList lineitemids;
    char **results;
    int rows, counter;
    sqlite_get_table_printf ( bdb, "select lineitemid from '%q';", &results, &rows, NULL, NULL, (const char*) tablename );
    for ( counter = 0; counter < rows; counter++ )
      lineitemids.append ( results [ counter + 1 ] );
    return lineitemids;
  }

int Budget::getLineItemTime ( int budgetid, int lineitemid )
  {
    QString tablename = "table";
    tablename.append ( QString::number ( budgetid ) );
    char **results;
    sqlite_get_table_printf ( bdb, "select type from '%q' where lineitemid= %i;", &results, NULL, NULL, NULL, ( const char * ) tablename, lineitemid );
    return atoi ( results [ 1 ] );
  }

float Budget::getLineItemAmount ( int budgetid, int lineitemid )
  {
    QString tablename = "table";
    tablename.append ( QString::number ( budgetid ) );
    char **results;
    sqlite_get_table_printf ( bdb, "select lineitemamount from '%q' where lineitemid= %i;", &results, NULL, NULL, NULL, ( const char* ) tablename, lineitemid );
    return strtod ( results [ 1 ], 0 );
  }

QString Budget::getBudgetTotal ( int budgetid, int viewtype )
  {
    QString tablename = "table";
    tablename.append ( QString::number ( budgetid ) );
    // determine if we are viewing a years, months, or days budget
    // we have to pick a different sum for each
    char **results;
    sqlite_get_table_printf ( bdb, "select sum ( lineitemamount ) from '%q';", &results, NULL, NULL, NULL, ( const char * ) tablename );
    QString amount = results [ 1 ];
    float total  = amount.toFloat();
    if ( viewtype == 0 )
      total = total / 12;
    amount.setNum ( total, 'f', 2 );
    return amount;
  }

int Budget::getLastAdded ()
  {
    char **results;
    sqlite_get_table ( bdb, "select last_insert_rowid() from budgets;", &results, NULL, NULL, NULL );
    return atoi ( results [ 1 ] );
  }



