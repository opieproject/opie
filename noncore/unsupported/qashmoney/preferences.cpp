#include "preferences.h"
#include <stdlib.h>

Preferences::Preferences ()
  {
    db = sqlite_open ( "qmpreferences.db", 0, NULL );
  }

Preferences::~Preferences ()
  {
    sqlite_close ( db );
  }

void Preferences::addPreferences ()
  {
    // This function checks the preferences database for existing preferences and adds
    // them if they are not there.  First we set up variables.  Preferences are always set
    // to non-zero numbers because when we check to see if a preference has been
    // added to the database, the result is zero if it hasn't
    int rows = 0;
    int columns = 0;
    char **results;

    sqlite_get_table ( db, "select count() from preferences;", &results, 0, 0, 0 );
    if ( atoi ( results [ 1 ] ) != 7 )
    {
      // dateformat preference 1 = yyyymmdd 2 = yymmdd 3 = mmddyyyy 4 = mmddyy
      // 5 = yyyyddmm 6 = yyddmm 7 = ddmmyyyy 8 = ddmmyy
      sqlite_get_table ( db, "select preference from preferences where id = 1;", &results, &rows, &columns, 0 );
      if ( rows == 0 )
        sqlite_exec ( db, "insert into preferences values ( 4, 'dateformat', 0, 0, 0, NULL );", 0, 0, 0 );

      // dateseparator preference 1 = / ( forward slash ) 2 = - ( dash ) 3 = . ( period )
      rows = 0;
      sqlite_get_table ( db, "select preference from preferences where id = 2;", &results, &rows, &columns, 0 );
      if ( rows == 0 )
        sqlite_exec ( db, "insert into preferences values ( 1, 'dateseparator', 0, 0, 0, NULL );", 0, 0, 0 );

      // showclearedtransactions preference 0 = no 1 = yes
      rows = 0;
      sqlite_get_table ( db, "select preference from preferences where id = 3;", &results, &rows, &columns, 0 );
      if ( rows == 0 )
        sqlite_exec ( db, "insert into preferences values ( 0, 'showclearedtransactions', 0, 0, 0, NULL );", 0, 0, 0 );

      // enable currency support preference 0 = no 1 = yes
      rows = 0;
      sqlite_get_table ( db, "select preference from preferences where id = 4;", &results, &rows, &columns, 0 );
      if ( rows == 0 )
        sqlite_exec ( db, "insert into preferences values ( 0, 'enablecurrencysupport', 0, 0, 0, NULL );", 0, 0, 0 );

      // one touch account viewing preference 0 = no 1 = yes
      rows = 0;
      sqlite_get_table ( db, "select preference from preferences where id = 5;", &results, &rows, &columns, 0 );
      if ( rows == 0 )
        sqlite_exec ( db, "insert into preferences values ( 0, 'onetouchviewing', 0, 0, 0, NULL );", 0, 0, 0 );

      // exclude transfers from limit view 0 = no 1 = yes
      rows = 0;
      sqlite_get_table ( db, "select preference from preferences where id = 6;", &results, &rows, &columns, 0 );
      if ( rows == 0 )
        sqlite_exec ( db, "insert into preferences values ( 0, 'excludetransfersfromlimit', 0, 0, 0, NULL );", 0, 0, 0 );

      // limit number of transactions to 0 = 14 days 1 = 30 days, 2 = 90 days, 3 = 180 days, 4 = 365 days 5 = all
      rows = 0;
      sqlite_get_table ( db, "select preference from preferences where id = 7;", &results, &rows, &columns, 0 );
      if ( rows == 0 )
        sqlite_exec ( db, "insert into preferences values ( 0, 'limittransactions', 0, 0, 0, NULL );", 0, 0, 0 );
    }
  }

void Preferences::initializeColumnPreferences ()
  {
    int rows = 0;
    int columns = 0;
    char **results;

    // initialize accountname column width
    sqlite_get_table ( db, "select width from columns where id = 1;", &results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into columns values ( 'accountname', 90, 0, 0, 0, NULL );", 0, 0, 0 );

    // initialize accountbalance column width
    sqlite_get_table ( db, "select width from columns where id = 2;", &results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into columns values ( 'accountbalance', 90, 0, 0, 0, NULL );", 0, 0, 0 );

    // initialize transactiondate column width
    sqlite_get_table ( db, "select width from columns where id = 3;", &results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into columns values ( 'normaltransactiondate', 50, 0, 0, 0, NULL );", 0, 0, 0 );

    // initialize transactionname column width
    sqlite_get_table ( db, "select width from columns where id = 4;", &results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into columns values ( 'normaltransactionname', 75, 0, 0, 0, NULL );", 0, 0, 0 );

    // initialize transactionamount column width
    sqlite_get_table ( db, "select width from columns where id = 5;", &results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into columns values ( 'normaltransactionamount', 50, 0, 0, 0, NULL );", 0, 0, 0 );

    // initialize transactiondate column width
    sqlite_get_table ( db, "select width from columns where id = 6;", &results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into columns values ( 'extendedtransactiondate', 50, 0, 0, 0, NULL );", 0, 0, 0 );

    // initialize transactionname column width
    sqlite_get_table ( db, "select width from columns where id = 7;", &results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into columns values ( 'extendedtransactionname', 75, 0, 0, 0, NULL );", 0, 0, 0 );

    // initialize transactionamount column width
    sqlite_get_table ( db, "select width from columns where id = 8;", &results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into columns values ( 'extendedtransactionamount', 50, 0, 0, 0, NULL );", 0, 0, 0 );

    // initialize transactionaccount column width
    sqlite_get_table ( db, "select width from columns where id = 9;", &results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into columns values ( 'transactionaccount', 50, 0, 0, 0, NULL );", 0, 0, 0 );

    // initialize account name with currency column width
    sqlite_get_table ( db, "select width from columns where id = 10;", &results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into columns values ( 'accountnamewithcurrency', 100, 0, 0, 0, NULL );", 0, 0, 0 );

    // initialize account currency column width
    sqlite_get_table ( db, "select width from columns where id = 11;", &results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into columns values ( 'currencycolumn', 10, 0, 0, 0, NULL );", 0, 0, 0 );

    // initialize account balance with currency column width
    sqlite_get_table ( db, "select width from columns where id = 12;", &results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into columns values ( 'accountbalancewithcurrency', 50, 0, 0, 0, NULL );", 0, 0, 0 );

    // initialize budget line item column
    sqlite_get_table ( db, "select width from columns where id = 13;", &results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into columns values ( 'budgetlineitem', 50, 0, 0, 0, NULL );", 0, 0, 0 );

    // initialize budget budget column
    sqlite_get_table ( db, "select width from columns where id = 14;", &results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into columns values ( 'budgetbudget', 50, 0, 0, 0, NULL );", 0, 0, 0 );

    // initialize budget actual column
    sqlite_get_table ( db, "select width from columns where id = 15;", &results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into columns values ( 'budgetactual', 50, 0, 0, 0, NULL );", 0, 0, 0 );
  }

void Preferences::initializeSortingPreferences ()
  {
    int rows = 0;
    int columns = 0;
    char **results;

    if ( sqlite_get_table ( db, "select count() from sorting;", 0, 0, 0, 0 ) != 0 )
      sqlite_exec ( db, "create table sorting ( listbox, column, direction, id integer primary key );", 0, 0, 0 );

    // initialize account listbox sorting. Set direction = 1 here so ascending sort is default.
    sqlite_get_table ( db, "select column, direction from sorting where id = 1;", &results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into sorting values ( 'accounts', 0, 1, NULL );", 0, 0, 0 );

    // initialize transaction listbox sorting
    sqlite_get_table ( db, "select column, direction from sorting where id = 2;", &results, &rows, &columns, 0);
    if ( rows == 0 )
      sqlite_exec ( db, "insert into sorting values ( 'transactions', 0, 1, NULL );", 0, 0, 0 );

    // initialize budgets listbox sorting
    sqlite_get_table ( db, "select column, direction from sorting where id = 3;", & results, &rows, &columns, 0 );
    if ( rows == 0 )
      sqlite_exec ( db, "insert into sorting values ( 'budgets', 0, 1, NULL );", 0, 0, 0 );
  }

void Preferences::changeColumnPreference ( int id, int width )
  {
    sqlite_exec_printf ( db, "update columns set width = %i where id = %i;", 0, 0, 0, width, id );
  }

int Preferences::getColumnPreference ( int id )
  {
    char **results;
    sqlite_get_table_printf ( db, "select width from columns where id = %i;", &results, NULL, NULL, NULL, id );
    return atoi ( results [ 1 ] );
  }

void Preferences::changeSortingPreference ( int id, int column )
  {
    int pColumn = 0;    // column setting coming from the prefs object
    int pDirection = 0; // direction setting coming from the prefs object

    // because there appears to be no way to query the QT header object directly for it's current sort settings, we have
    // to maintain track of them ourselves.  So start by pulling the current saved setting for this view.
    getSortingPreference ( id, &pColumn, &pDirection );

    // if the current saved column == the new column, then the user wants to toggle the sort order.
    // otherwise we behave like QT does by default, which is to select the new column and default to an ascending sort.
    if ( column == pColumn )
      sqlite_exec_printf ( db, "update sorting set direction = %i where id = %i;", 0, 0, 0, !pDirection, id );
    else
      sqlite_exec_printf ( db, "update sorting set column = %i, direction = 1 where id = %i;", 0, 0, 0, column, id );
  }

void Preferences::getSortingPreference ( int id, int *column, int *direction )
  {
    char **results;
    sqlite_get_table_printf ( db, "select column, direction from sorting where id = %i;", &results, NULL, NULL, NULL, id );
    *column = atoi ( results [ 2 ] );
    *direction = atoi ( results [ 3 ] );
  }

int Preferences::getPreference ( int id )
  {
    char **results;
    sqlite_get_table_printf ( db, "select preference from preferences where id = %i;", &results, NULL, NULL, NULL, id );
    return atoi ( results [ 1 ] );
  }

QString Preferences::getSeparator ( )
  {
    int s = getPreference ( 2 );
    if ( s == 1 )
      return "/";
    if ( s ==2 )
      return "-";
    else
      return ".";
  }

void Preferences::changePreference ( int id, int newpreference )
  {
    sqlite_exec_printf ( db, "update preferences set preference = %i where id = %i;", 0, 0, 0, newpreference, id );
  }

void Preferences::setDefaultDatePreferences ()
  {
    sqlite_exec ( db, "update preferences set preference = 4 where id = 1;", 0, 0, 0 );
    sqlite_exec ( db, "update preferences set preference = 1 where id = 2;", 0, 0, 0 );
  }

QString Preferences::getDate ( int y, int m, int d )
  {
    QString date;
    int format = getPreference ( 1 );
    QString separator = getSeparator();

    // Convert all date integers to QStrings
    QString year = QString::number ( y );
    QString month = QString::number ( m );
    if ( m < 10 )
      month.prepend ( "0" );
    QString day = QString::number ( d );
    if ( d < 10 )
      day.prepend ( "0" );

    // Truncate four digit year if necessary
    if ( format == 2 || format == 4 || format == 6 || format == 8 )
      year.remove ( 0, 2 );

    // Concatenate dates as necessary
     if ( format == 1 || format == 2 )
      {
        date = year;
	date.append ( separator );
	date.append ( month );
	date.append ( separator );
	date.append ( day );
	return date;
      }
    if ( format == 3 || format == 4 )
      {
        date = month;
	date.append ( separator );
	date.append ( day );
	date.append ( separator );
	date.append ( year );
	return date;
      }
    if ( format == 5 || format == 6 )
      {
        date = year;
	date.append ( separator );
	date.append ( day );
	date.append ( separator );
	date.append ( month );
	return date;
      }
    if ( format == 7 || format == 8 )
      {
        date = day;
	date.append ( separator );
	date.append ( month );
	date.append ( separator );
	date.append ( year );
	return date;
      }
  }

QString Preferences::getDate ( int y, int m )
  {
    QString date;
    int format = getPreference ( 1 );
    QString separator = getSeparator();

    // Convert all date integers to QStrings
    QString year = QString::number ( y );
    QString month = QString::number ( m );
    if ( m < 10 )
      month.prepend ( "0" );

    // Truncate four digit year if necessary
    if ( format == 2 || format == 4 || format == 6 || format == 8 )
      year.remove ( 0, 2 );

    // Concatenate dates as necessary
     if ( format == 1 || format == 2 )
      {
        date = year;
	date.append ( separator );
	date.append ( month );
	return date;
      }
    if ( format == 3 || format == 4 )
      {
        date = month;
	date.append ( separator );
	date.append ( year );
	return date;
      }
    if ( format == 5 || format == 6 )
      {
        date = year;
	date.append ( separator );
	date.append ( month );
	return date;
      }
    if ( format == 7 || format == 8 )
      {
	date.append ( month );
	date.append ( separator );
	date.append ( year );
	return date;
      }
  }





