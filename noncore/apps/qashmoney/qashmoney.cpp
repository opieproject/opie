#include "qashmoney.h"
#include "preferencedialogs.h"
#include "memorydialog.h"

#include <qheader.h>
#include <iostream.h>

Budget *budget = new Budget ();
Preferences *preferences = new Preferences ();
Account *account = new Account ();
Transaction *transaction = new Transaction ();
Transfer *transfer = new Transfer ();
Memory *memory = new Memory ();

QashMoney::QashMoney () : QWidget ()
  {
    preferences->addPreferences();
    preferences->initializeColumnPreferences ();

    // set the text in the upper part of the frame
    setCaption ( tr ( "QashMoney" ) );

    // Create new menubar for our mainwindow
    // and add menu items
    mainmenu = new QPEMenuBar ( this );
    mainmenu->setFrameStyle ( QFrame::PopupPanel | QFrame::Raised );
    preferencesmenu = new QPopupMenu ( this );
    utilitiesmenu = new QPopupMenu ( this );
    mainmenu->insertItem ( "Preferences", preferencesmenu );
    mainmenu->insertItem ( "Utilities", utilitiesmenu );
    preferencesmenu->insertItem ( "Date", this, SLOT ( displayDatePreferencesDialog () ) );
    preferencesmenu->insertItem ( "Account", this, SLOT ( displayAccountPreferencesDialog () ) );
    preferencesmenu->insertItem ( "Transaction", this, SLOT ( displayTransactionPreferencesDialog () ) );
    utilitiesmenu->insertItem ( "Memory", this, SLOT ( displayMemoryDialog () ) );

    // create the main tabwidget for displaying accounts and transactions
    maintabs = new QTabWidget ( this );
    tab = new QWidget ( this );
    tab_2 = new QWidget ( this );
    tab_3 = new QWidget ( this );
    maintabs->addTab ( tab, "Accounts" );
    maintabs->addTab ( tab_2, "Transactions" );
    maintabs->addTab ( tab_3, "Budgets" );
    tabheight = tab->height();
    maintabs->setTabEnabled ( tab_2, FALSE );

    // create a new account display object
    accountdisplay = new AccountDisplay ( maintabs );
    accountdisplay->setTabs ( tab_2, maintabs );
    connect ( accountdisplay->listview, SIGNAL ( selectionChanged () ), this, SLOT ( setTransactionTab () ) );

    // set the connection to disable the one touch account viewing if we are transfering money
    connect ( accountdisplay->transferbutton, SIGNAL ( toggled ( bool ) ), this, SLOT ( toggleOneTouchViewing ( bool ) ) );

    // create a new transactiondisplay object
    transactiondisplay = new TransactionDisplay ( maintabs );
    transactiondisplay->hide();

    // create new budgetdisplay object
    budgetdisplay = new BudgetDisplay ( maintabs );
    budgetdisplay->hide();

    tabslayout = new QVBoxLayout ( maintabs, 4, 2 );
    tabslayout->addSpacing ( tabheight );
    tabslayout->addWidget ( accountdisplay );
    tabslayout->addWidget ( transactiondisplay );
    tabslayout->addWidget ( budgetdisplay );

    // connect a change in the maintabs with changing the tab display
    connect ( maintabs, SIGNAL ( currentChanged ( QWidget * ) ), this, SLOT ( changeTabDisplay () ) );

    // create layout that will contain the menubar and the maintabs
    layout = new QVBoxLayout ( this, 2, 2 );
    layout->setMenuBar ( mainmenu );
    layout->addWidget ( maintabs );
  }

QashMoney::~QashMoney ()
  {
    delete budget;
    delete preferences;
    delete account;
    delete transaction;
    delete transfer;
    delete memory;
  }

void QashMoney::changeTabDisplay ()
  {
    // if the user pressed the transactions tab, hide the account display
    // object and create a new transaction display
    if ( maintabs->currentPageIndex() == 1 )
      {
        // initialize variables
        bool children = FALSE;

        // hide the account display and define accountid
        int accountid = accountdisplay->listview->selectedItem()->text ( accountdisplay->getIDColumn() ).toInt();

        //remove all the columns from the transactiondisplay
        int columns = transactiondisplay->listview->columns();
        int counter;
        for ( counter = 0; counter <= columns; counter++ )
          transactiondisplay->listview->removeColumn ( 0 );

        // set the account name and account balance
        QString name = account->getAccountName ( accountid );
        QString balance = account->getAccountBalance ( accountid );
        transactiondisplay->name->setText ( name );
        transactiondisplay->balance->setText ( balance );

        // clear the limitbox
        transactiondisplay->limitbox->clear();

        // get parent account id
        int parentaccountid = account->getParentAccountID ( accountid );

        // add columns based on which account is selected
        // this first if determines if we selected a parent with no children or a child
        // in these cases, we add standard three columns for date, transaction, amount
        transactiondisplay->listview->addColumn ( "Date", 0 );
        transactiondisplay->listview->addColumn ( "Transaction", 0 );
        transactiondisplay->listview->addColumn ( "Amt", 0);
        transactiondisplay->listview->setColumnAlignment ( 2, Qt::AlignRight );
        transactiondisplay->listview->addColumn ( "", 0 );

        if ( accountdisplay->listview->selectedItem()->parent() == 0 && accountdisplay->listview->selectedItem()->childCount() != 0 ) // we selected a parent with children
          {
            // add an extra column for the account name for eac child transaction
            transactiondisplay->listview->addColumn ( "Acct", 0 );
            children = TRUE;

            // hide the new transaction button
            transactiondisplay->newtransaction->setEnabled ( FALSE );
          }
        else //we selected a parent without children or a child
          transactiondisplay->newtransaction->setEnabled ( TRUE );

        // disable the transactionid column so it can't be red
        transactiondisplay->listview->header()->setResizeEnabled ( FALSE, 3 );

        // set the accountid and children variables
        transactiondisplay->setChildren ( children );
        transactiondisplay->setAccountID ( accountid );

        // display transactions
        transactiondisplay->listview->clear();
        QString displaytext = "%";
        displaytext.prepend ( transactiondisplay->limitbox->text() );
        if ( transaction->getNumberOfTransactions() > 0 )
          transaction->displayTransactions ( transactiondisplay->listview, accountid, children, displaytext );

        // display transfers
        transfer->displayTransfers ( transactiondisplay->listview, accountid, children );

        // open a new preferences object and resize the transaction display columns
        // each column will have a different size based on whether we are looking at a child
        // account or children through a parent
        if ( parentaccountid != -1 || accountdisplay->listview->selectedItem()->childCount() == 0 ) // a parent with no children or a child - three columns
          {
            transactiondisplay->listview->setColumnWidth ( 0, preferences->getColumnPreference ( 3 ) ); // normal transaction date width
            transactiondisplay->listview->setColumnWidthMode ( 0, QListView::Manual );
            transactiondisplay->listview->setColumnWidth ( 1, preferences->getColumnPreference ( 4 ) ); // normal transaction name width
            transactiondisplay->listview->setColumnWidthMode ( 1, QListView::Manual );
            transactiondisplay->listview->setColumnWidth ( 2, preferences->getColumnPreference ( 5 ) ); // normal transaction amount width
            transactiondisplay->listview->setColumnWidthMode ( 2, QListView::Manual );
          }
        else
          {
            transactiondisplay->listview->setColumnWidth ( 0, preferences->getColumnPreference ( 6 ) ); // extended transaction date width
            transactiondisplay->listview->setColumnWidthMode ( 0, QListView::Manual );
            transactiondisplay->listview->setColumnWidth ( 1, preferences->getColumnPreference ( 7 ) ); // extended transaction name width
            transactiondisplay->listview->setColumnWidthMode ( 1, QListView::Manual );
            transactiondisplay->listview->setColumnWidth ( 2, preferences->getColumnPreference ( 8 ) ); // extended transaction amount width
            transactiondisplay->listview->setColumnWidthMode ( 2, QListView::Manual );
            transactiondisplay->listview->setColumnWidth ( 4, preferences->getColumnPreference ( 9 ) ); // transaction account width
            transactiondisplay->listview->setColumnWidthMode ( 4, QListView::Manual );
          }

        // show the window
        transactiondisplay->show();
        // hide the account display and define accountid
        accountdisplay->hide();
        // hide the budget display
        budgetdisplay->hide();
      }
    else if ( maintabs->currentPageIndex() == 0 )
      {
        disableOneTouchViewing();

        // clear the account display selection
        accountdisplay->listview->clearSelection();

        // resize the account display columns
        accountdisplay->listview->setColumnWidth ( 0, preferences->getColumnPreference ( 1 ) );
        accountdisplay->listview->setColumnWidth ( 1, preferences->getColumnPreference ( 2 ) );

        // display the accounts
        if ( account->getNumberOfAccounts() != 0 )
          account->displayAccounts ( accountdisplay->listview );
        maintabs->setTabEnabled ( tab_2, FALSE );

        // set the toggle button
        accountdisplay->setToggleButton ();

        // show the account display
        accountdisplay->show();

        // hide the transaction display
        transactiondisplay->hide();

        // hide the budget display
        budgetdisplay->hide();


        enableOneTouchViewing ();
      }
    else
      {
        budgetdisplay->displayLineItems();
        budgetdisplay->show();
        transactiondisplay->hide();
        accountdisplay->hide();
      }
  }

void QashMoney::setTransactionTab ()
  {
    if ( accountdisplay->listview->selectedItem() == 0 )
      maintabs->setTabEnabled ( tab_2, FALSE );
    else
      maintabs->setTabEnabled ( tab_2, TRUE );
  }

void QashMoney::displayDatePreferencesDialog ()
  {
    // this shows a dialog to set preferences for formatting the date
    DatePreferences *pd = new DatePreferences ( this );
    pd->exec ();
    if ( transactiondisplay->isVisible() )
      {
        // set the account id
        int accountid = accountdisplay->listview->selectedItem()->text ( accountdisplay->getIDColumn() ).toInt();

        // set children so we can let displayTransfers know if there are children for the selected account
        bool children;
        if ( accountdisplay->listview->selectedItem()->parent() == 0 && accountdisplay->listview->selectedItem()->childCount() != 0 )
          children = TRUE;
        else
          children = FALSE;

        // redisplay transactions if they are visible incorporating
        // any changes to the date format
        transactiondisplay->listview->clear();
        QString displaytext = "%";
        displaytext.prepend ( transactiondisplay->limitbox->text() );
        if ( transaction->getNumberOfTransactions() > 0 )
          transaction->displayTransactions ( transactiondisplay->listview, accountid, children, displaytext );

        if ( transfer->getNumberOfTransfers() != 0 )
          transfer->displayTransfers ( transactiondisplay->listview, accountid, children );
      }
    else if ( accountdisplay->isVisible() )
      {
        accountdisplay->listview->clearSelection();
        maintabs->setTabEnabled ( tab_2, FALSE );
      }
    else
      budgetdisplay->updateBudgetInformation();
  }

void QashMoney::displayTransactionPreferencesDialog ()
  {
    // display a dialog for setting preferences for transactions
    TransactionPreferences *td = new TransactionPreferences ( this );
    td->exec ();
    if ( transactiondisplay->isVisible() )
      {
        // set the account id
        int accountid = accountdisplay->listview->selectedItem()->text ( accountdisplay->getIDColumn() ).toInt();

        // set children so we can let displayTransfers know if there are children for the selected account
        bool children;
        if ( accountdisplay->listview->selectedItem()->parent() == 0 && accountdisplay->listview->selectedItem()->childCount() != 0 )
          children = TRUE;
        else
          children = FALSE;

        // redisplay transactions incorporating any transaction preference changes
        transactiondisplay->listview->clear();
        QString displaytext = "%";
        displaytext.prepend ( transactiondisplay->limitbox->text() );
        if ( transaction->getNumberOfTransactions() > 0 )
          transaction->displayTransactions ( transactiondisplay->listview, accountid, children, displaytext );

        if ( transfer->getNumberOfTransfers() != 0 )
          transfer->displayTransfers ( transactiondisplay->listview, accountid, children );
      }
    else
      {
        accountdisplay->listview->clearSelection();
        maintabs->setTabEnabled ( tab_2, FALSE );
      }
  }

void QashMoney::displayAccountPreferencesDialog ()
  {
    // display a dialog for setting preferences for accounts
    AccountPreferences *ap = new AccountPreferences ( this );
    ap->exec ();

    if ( accountdisplay->isVisible() && account->getNumberOfAccounts() != 0 )
      {
        accountdisplay->listview->clear();
        account->displayAccounts ( accountdisplay->listview );
        accountdisplay->listview->clearSelection();
        maintabs->setTabEnabled ( tab_2, FALSE );
      }
    changeTabDisplay();
  }

void QashMoney::displayMemoryDialog ()
  {
    // opens a dialog to add, edit and delete memory items
    MemoryDialog *md = new MemoryDialog ();
    md->exec();
  }

void QashMoney::showTransactions ()
  {
    maintabs->setCurrentPage ( 1 );
  }

void QashMoney::enableOneTouchViewing ()
  {
    if ( preferences->getPreference ( 5 ) == 1 )
      connect ( accountdisplay->listview, SIGNAL ( selectionChanged () ), this, SLOT ( showTransactions () ) );
    else
      disconnect ( accountdisplay->listview, SIGNAL ( selectionChanged () ), this, SLOT ( showTransactions () ) );
  }

void QashMoney::disableOneTouchViewing ()
  {
    disconnect ( accountdisplay->listview, SIGNAL ( selectionChanged () ), this, SLOT ( showTransactions () ) );
  }

void QashMoney::toggleOneTouchViewing ( bool state )
  {
    if ( state == TRUE )
      disableOneTouchViewing();
    else
      enableOneTouchViewing();
  }



