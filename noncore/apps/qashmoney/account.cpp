#include "account.h"
#include "transaction.h"
#include "transfer.h"
#include "preferences.h"

#include <qpixmap.h>
#include <stdlib.h>
#include <iostream.h>

extern Preferences *preferences;

Account::Account ()
  {
    adb = sqlite_open ( "qmaccounts.db", 0, NULL );
  }

Account::~Account ()
  {
    sqlite_close ( adb );
  }

void Account::addAccount ( QString name, int parentid, float balance, int type, QString description, float creditlimit,
					int statementyear, int statementmonth, int statementday, float statementbalance, const char *currency  )
  {
    sqlite_exec_printf ( adb, "insert into accounts2 values ( '%q', %i, %.2f, %i, '%q', %.2f, %i, %i, %i, %.2f, '%q', 0, 0, 0, 0, 0, NULL );", 0, 0, 0,
      (const char *) name, parentid, balance, type, (const char *) description, creditlimit, statementyear, statementmonth, statementday, statementbalance, currency );
  }

void Account::updateAccount ( QString name, QString description, QString currencycode, int accountid )
  {
    sqlite_exec_printf ( adb, "update accounts2 set name = '%q', description = '%q', currency = '%q' where accountid = %i;", 0, 0, 0, ( const char * ) name, ( const char * ) description, ( const char * ) currencycode, accountid );
  }

void Account::deleteAccount ( int accountid )
  {
    sqlite_exec_printf ( adb, "delete from accounts2 where accountid = %i;", 0, 0, 0, accountid );
  }

void Account::setAccountExpanded ( int expanded, int accountid )
  {
    sqlite_exec_printf ( adb, "update accounts2 set r1 = %i where accountid = %i;", 0, 0, 0, expanded, accountid );
  }

int Account::getAccountExpanded ( int id )
  {
    char **results;
    sqlite_get_table_printf ( adb, "select r1 from accounts2 where accountid = %i;", &results, 0, 0, 0, id );
    if ( strlen ( results [1] ) == 0 )
      return 0;
    else
      return atoi ( results [ 1 ] );
  }

int Account::getNumberOfAccounts ()
  {
    char **results;
    sqlite_get_table ( adb, "select count() from accounts2;", &results, NULL, NULL, NULL );
    return atoi ( results [ 1 ] );
  }

int Account::getNumberOfChildAccounts ( int id )
  {
    char **results;
    sqlite_get_table_printf ( adb, "select count() from accounts2 where parent = %i;", &results, NULL, NULL, NULL, id );
    return atoi ( results [ 1 ] );
  }

void Account::updateAccountBalance ( int accountid )
  {
    // Here, we'll get a balance for the transactions in an account
    sqlite *tdb = sqlite_open ( "qmtransactions.db", 0, NULL );
    int rows, columns;
    char **results;
    sqlite_get_table_printf ( tdb, "select sum (amount) from transactions where accountid= %i;", &results, &rows, &columns, NULL, accountid );
    float transactionsbalance = strtod ( results [ 1 ], 0 );
    sqlite_close ( tdb );

    // next, we'll get a balance for all the transfers from the account
    sqlite *trdb = sqlite_open ( "qmtransfers.db", 0, NULL );
    rows = 0;
    columns = 0;
    char **results2;
    sqlite_get_table_printf ( trdb, "select sum (amount) from transfers where fromaccount = %i;", &results2, &rows, &columns, NULL, accountid );
    float fromtransfersbalance = ( strtod ( results2 [ 1 ], 0 ) * -1 );

    // finally, we'll get a balance for all the transfers into the account
    rows = 0;
    columns= 0;
    char **results3;
    sqlite_get_table_printf ( trdb, "select sum (amount) from transfers where toaccount = %i;", &results3, &rows, &columns, NULL, accountid );
    float totransfersbalance = strtod ( results3 [ 1 ], 0 );

    sqlite_close ( trdb );

    // calculate and update new balance
    sqlite_exec_printf ( adb, "update accounts2 set balance = %.2f where accountid = %i;", 0, 0, 0,
      ( transactionsbalance + fromtransfersbalance + totransfersbalance + getStatementBalance ( accountid ) ), accountid );
  }

void Account::changeParentAccountBalance ( int parentid )
  {
    // select all child balances that share the parent of the current child account
    char **results;
    int rows;
    sqlite_get_table_printf ( adb, "select sum ( balance ) from accounts2 where parent = %i;", &results, &rows, NULL, NULL, parentid );
    sqlite_exec_printf ( adb, "update accounts2 set balance = %.2f where accountid = %i;", 0, 0, 0, strtod ( results[ 1 ], NULL ), parentid );
  }

int Account::getParentAccountID ( int id )
  {
    char **results;
    sqlite_get_table_printf ( adb, "select parent from accounts2 where accountid = %i;", &results, NULL, NULL, NULL, id );
    return atoi ( results [ 1 ] );
  }

int Account::getParentAccountID ( QString accountname )
  {
    char **results;
    sqlite_get_table_printf ( adb, "select parent from accounts2 where name= '%q';", &results, NULL, NULL, NULL, ( const char * ) accountname );
    return atoi ( results [ 1 ] );
  }

void Account::displayAccounts ( QListView *listview )
  {
    char **results;
    int rows, columns;
    sqlite_get_table ( adb, "select name, parent, balance, accountid, currency from accounts2;", &results, &rows, &columns, 0 );

    // determine if we are using currency support
    int currency = preferences->getPreference ( 4 );

    // remove all columns from the account display
    int counter;
    for ( counter = 0; counter <= columns; counter++ )
      listview->removeColumn ( 0 );

    // add columns to the account display
    listview->addColumn ( "Account", 0 );
    int columntoalign = 1;
    if ( preferences->getPreference ( 4 ) == 1 ) // add the currency column if the user wants it
      {
        listview->addColumn ( "C", 0 );
        columntoalign = 2;
      }
    listview->addColumn ( "Balance", 0 );
    listview->addColumn ( "", 0 );
    listview->setColumnAlignment ( columntoalign, Qt::AlignRight );
    counter = 5;
    int total = ( rows + 1 ) * columns;
    while ( counter < total )
      {
        int accountid = atoi ( results [ counter + 3 ] );
        if ( atoi ( results [ counter + 1 ]  ) == -1 )
          {
            QListViewItem *parent = new QListViewItem ( listview );
            parent->setText ( 0, results [ counter ] );
            if ( currency == 0 )
              {
                parent->setText ( 1, results [ counter + 2 ] );
                parent->setText ( 2, results [ counter + 3 ] );
              }
            else
              {
                if ( getNumberOfChildAccounts ( accountid ) == 0 ) // add the currency flag if this is a parent with no children
                  {
                    // create the string we'll use to set the currency pixmap
                    QString filename = "/opt/QtPalmtop/pics/flags/";
                    QString flag = results [ counter + 4 ];
                    filename.append ( flag );
                    filename.append ( ".png" );
                    parent->setPixmap ( 1, QPixmap ( filename ) );
                    parent->setText ( 1, flag );
                  }
                parent->setText ( 2, results [ counter + 2 ] );
                parent->setText ( 3, results [ counter + 3 ] );
              }

            if ( getAccountExpanded ( accountid ) == 1 )
              parent->setOpen ( TRUE );

            //Start display child accounts for this parent
            int childcounter = 5;
            while ( childcounter < total )
              {
                if ( atoi ( results [ childcounter + 1 ] ) == accountid )
                  {
                    if ( currency == 0 )
                      QListViewItem *child = new QListViewItem ( parent, results [ childcounter ], results [ childcounter + 2 ], results [ childcounter + 3 ] );
                    else
                      {
                        // create the string we'll use to set the currency pixmap
                        QString filename = "/opt/QtPalmtop/pics/flags/";
                        QString flag = results [ childcounter + 4 ];
                        filename.append ( flag );
                        filename.append ( ".png" );
                        QListViewItem *child = new QListViewItem ( parent, results [ childcounter ], "", results [ childcounter + 2 ], results [ childcounter + 3 ] );
                        child->setPixmap ( 1, QPixmap ( filename ) );
                        child->setText ( 1, flag );
                      }
                  }
                childcounter = childcounter + 5;
              }
            //End display child accounts
          }
        counter = counter + 5;
      }

    // resize all columns appropriately
    if ( preferences->getPreference ( 4 ) == 0 )
      {
        listview->setColumnWidth ( 0, preferences->getColumnPreference ( 1 ) );
        listview->setColumnWidthMode ( 0, QListView::Manual );
        listview->setColumnWidth ( 1, preferences->getColumnPreference ( 2 ) );
        listview->setColumnWidthMode ( 1, QListView::Manual );
        listview->setColumnWidthMode ( 2, QListView::Manual );
      }
    else
      {
        listview->setColumnWidth ( 0, preferences->getColumnPreference ( 10 ) );
        listview->setColumnWidthMode ( 0, QListView::Manual );
        listview->setColumnWidth ( 1, preferences->getColumnPreference ( 11 ) );
        listview->setColumnWidthMode ( 1, QListView::Manual );
        listview->setColumnWidth ( 2, preferences->getColumnPreference ( 12 ) );
        listview->setColumnWidthMode ( 2, QListView::Manual );
        listview->setColumnWidthMode ( 3, QListView::Manual );
      }

    // Now reset the column sorting to user preference
    int column = 0;
    int direction = 0;
    preferences->getSortingPreference ( 1, &column, &direction );
    listview->setSorting ( column, direction );
  }

int Account::displayParentAccountNames ( QComboBox *combobox, QString indexstring )
  {
    char **results;
    int rows, columns, index;
    index = 0;
    sqlite_get_table ( adb, "select name from accounts2 order by name asc;", &results, &rows, &columns, NULL );
    int counter = 1;
    int indexcounter = 1;
    int total = ( rows + 1 ) * columns;
    while ( counter < total )
      {
        if ( getParentAccountID ( results [ counter ] ) == -1 )
          {
            combobox->insertItem ( results [ counter ], -1 );
            if ( strcmp ( results [ counter ], indexstring ) == 0 )
              index = indexcounter;
            indexcounter++;
          }
        counter ++;
      }
    return index;
  }

int Account::getAccountType ( int accountid )
  {
    char **results;
    sqlite_get_table_printf ( adb, "select type from accounts2 where accountid= %i;", &results, NULL, NULL, NULL, accountid );
    return atoi ( results [ 1 ] );
  }

int Account::getStatementDay ( int accountid )
  {
    char **results;
    sqlite_get_table_printf ( adb, "select statementday from accounts2 where accountid= %i;", &results, NULL, NULL, NULL, accountid );
    return atoi ( results [ 1 ] );
  }

int Account::getStatementMonth ( int accountid )
  {
    char **results;
    sqlite_get_table_printf ( adb, "select statementmonth from accounts2 where accountid= %i;", &results, NULL, NULL, NULL, accountid );
    return atoi ( results [ 1 ] );
  }

int Account::getStatementYear ( int accountid )
  {
    char **results;
    sqlite_get_table_printf ( adb, "select statementyear from accounts2 where accountid= %i;", &results, NULL, NULL, NULL, accountid );
    return atoi ( results [ 1 ] );
  }

QString Account::getAccountDescription ( int accountid )
  {
    char **results;
    sqlite_get_table_printf ( adb, "select description from accounts2 where accountid= %i;", &results, NULL, NULL, NULL, accountid );
    return ( QString ) results [ 1 ];
  }

QString Account::getCurrencyCode ( int accountid )
  {
    char **results;
    sqlite_get_table_printf ( adb, "select currency from accounts2 where accountid= %i;", &results, NULL, NULL, NULL, accountid );
    return ( QString ) results [ 1 ];
  }

QString Account::getAccountName ( int accountid )
  {
    char **results;
    sqlite_get_table_printf ( adb, "select name from accounts2 where accountid= %i;", &results, NULL, NULL, NULL, accountid );
    return ( QString ) results [ 1 ];
  }

QString Account::getAccountBalance ( int accountid )
  {
    char **results;
    sqlite_get_table_printf ( adb, "select balance from accounts2 where accountid= %i;", &results, NULL, NULL, NULL, accountid );
    return ( QString ) results [ 1 ];
  }

float Account::getAccountCreditLimit ( int accountid )
  {
    char **results;
    sqlite_get_table_printf ( adb, "select creditlimit from accounts2 where accountid = %i;", &results, NULL, NULL, NULL, accountid );
    return strtod ( results [ 1 ], NULL );
  }

float Account::getStatementBalance ( int accountid )
  {
    char **results;
    sqlite_get_table_printf ( adb, "select statementbalance from accounts2 where accountid = %i;", &results, NULL, NULL, NULL, accountid );
    return strtod ( results [ 1 ], NULL );
  }

GreyBackgroundItem::GreyBackgroundItem ( QListView *parent )
  :  QListViewItem ( parent )
  {
  }

GreyBackgroundItem::GreyBackgroundItem ( QListView *parent, QString label1, QString label2, QString label3 )
  :  QListViewItem ( parent, label1, label2, label3 )
  {
  }

GreyBackgroundItem::GreyBackgroundItem ( QListView *parent, QString label1, QString label2, QString label3, QString label4 )
  :  QListViewItem ( parent, label1, label2, label3, label4 )
  {
  }

GreyBackgroundItem::GreyBackgroundItem ( QListView *parent, QString label1, QString label2, QString label3, QString label4, QString label5 )
  :  QListViewItem ( parent, label1, label2, label3, label4, label5 )
  {
  }

void GreyBackgroundItem::paintCell ( QPainter *p, const QColorGroup &cg, int column, int width, int alignment )
  {
    QColorGroup _cg ( cg );
    _cg.setColor ( QColorGroup::Base, Qt::lightGray );
    QListViewItem::paintCell ( p, _cg, column, width, alignment );
  }

QStringList Account::getAccountNames ()
  {
    QStringList accountnames;
    char **results;
    int rows, counter;
    sqlite_get_table ( adb, "select name from accounts2;", &results, &rows, 0, 0 );
    for ( counter = 0; counter < rows; counter++ )
      accountnames.append ( results [ counter+1 ] );
    return accountnames;
  }

QStringList Account::getAccountIDs ()
  {
    QStringList accountids;
    char **results;
    int rows, counter;
    sqlite_get_table ( adb, "select accountid from accounts2;", &results, &rows, 0, 0 );
    for ( counter = 0; counter < rows; counter++ )
      accountids.append ( results [ counter+1 ] );
    return accountids;
  }


