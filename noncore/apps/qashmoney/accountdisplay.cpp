#include <qmessagebox.h>
#include <qheader.h>

#include "accountdisplay.h"
#include "newaccount.h"
#include "transaction.h"
#include "transferdialog.h"
#include "transfer.h"

extern Account *account;
extern Transaction *transaction;
extern Transfer *transfer;
extern Preferences *preferences;

AccountDisplay::AccountDisplay ( QWidget *parent ) : QWidget ( parent )
  {
    cleared = 0;

    firstline = new QHBox ( this );
    firstline->setSpacing ( 2 );

    newaccount = new QPushButton ( firstline );
    newaccount->setPixmap ( QPixmap ("/opt/QtPalmtop/pics/new.png") );
    connect ( newaccount, SIGNAL ( released() ), this, SLOT ( addAccount() ) );

    editaccount = new QPushButton ( firstline );
    editaccount->setPixmap ( QPixmap ("/opt/QtPalmtop/pics/edit.png") );
    connect ( editaccount, SIGNAL ( released() ), this, SLOT ( editAccount() ) );

    deleteaccount = new QPushButton ( firstline );
    deleteaccount->setPixmap( QPixmap ( "/opt/QtPalmtop/pics/delete.png") );
    connect ( deleteaccount, SIGNAL ( released() ), this, SLOT ( deleteAccount() ) );

    transferbutton = new QPushButton ( firstline );
    transferbutton->setPixmap( QPixmap ( "/opt/QtPalmtop/pics/transfer.png") );
    transferbutton->setToggleButton ( TRUE );
    connect ( transferbutton, SIGNAL ( toggled(bool) ), this, SLOT ( accountTransfer(bool) ) );

    listview = new QListView ( this );
    listview->setAllColumnsShowFocus ( TRUE );
    listview->setShowSortIndicator ( TRUE );
    listview->setRootIsDecorated ( TRUE );
    listview->setMultiSelection ( FALSE );
    connect ( listview, SIGNAL ( expanded(QListViewItem*) ), this, SLOT ( setAccountExpanded(QListViewItem*) ) );
    connect ( listview, SIGNAL ( collapsed(QListViewItem*) ), this, SLOT ( setAccountCollapsed(QListViewItem*) ) );
    
    listview->header()->setTracking ( FALSE );
    connect ( listview->header(), SIGNAL ( sizeChange(int,int,int) ), this, SLOT ( saveColumnSize(int,int,int) ) );
    connect ( listview->header(), SIGNAL ( clicked(int) ), this, SLOT ( saveSortingPreference(int) ) );
    
    layout = new QVBoxLayout ( this, 2, 5 );
    layout->addWidget ( firstline );
    layout->addWidget ( listview );
  }

void AccountDisplay::setTabs ( QWidget *newtab2, QTabWidget *newtabs )
  {
    tab2 = newtab2;
    maintabs = newtabs;
  }

void AccountDisplay::addAccount ()
  {
    // initialize local variables
    int parentid = 0;
    type = 0;
    QString parentlist [ listview->childCount() + 1 ] [ 3 ] ;

    // create new account window for entering data
    NewAccount *newaccount = new NewAccount ( this );
    int width = this->width();
    newaccount->accountbox->setMaximumWidth ( ( int ) ( width * 0.5 ) );
    newaccount->datebox->setMaximumWidth ( ( int ) ( width * 0.4 ) );
    newaccount->childbox->setMaximumWidth ( ( int ) ( width * 0.5 ) );
    newaccount->balancebox->setMaximumWidth ( ( int ) ( width * 0.4 ) );
    newaccount->creditlimitbox->setMaximumWidth ( ( int ) ( width * 0.4 ) );

    // if there are no accounts, disable the child check box
    if ( account->getNumberOfAccounts () == 0 )
      newaccount->childcheckbox->setEnabled ( FALSE );

    // if there are accounts, fill up the pulldown menu for
    // selecting a parent account.  We should only add those parents without transactions
    else
      {
        int c = 0;
        QListViewItemIterator it ( listview );
        for ( ; it.current(); ++it )
          {
            int id = it.current()->text ( getIDColumn() ).toInt();
            // iterate through accountdisplay listview and add parents with no transactions
            // add this item to the list box only if it is a parent and has no transactions
            if ( transfer->getNumberOfTransfers ( id ) == 0 && transaction->getNumberOfTransactions ( id ) == 0 && it.current()->parent() == 0  )
              {
                newaccount->childbox->insertItem ( it.current()->text ( 0 ) );
                parentlist [ c ] [ 0 ] = it.current()->text ( 0 );
                parentlist [ c ] [ 1 ] = it.current()->text ( getIDColumn() );
                parentlist [ c ] [ 2 ] = QString::number ( c );
                c++;
              }
          }
      }

    if ( preferences->getPreference ( 4 ) == 0 )
      newaccount->currencybox->setEnabled ( FALSE );

    // enter today's date in the date box as default
    QDate today = QDate::currentDate ();
    int defaultday = today.day();
    int defaultmonth = today.month();
    int defaultyear  = today.year();
    newaccount->startdate->setText ( preferences->getDate ( defaultyear, defaultmonth, defaultday )  );

    //add account information if user pushes OK button
    if ( newaccount->exec() == QDialog::Accepted )
      {
        if ( newaccount->childcheckbox->isChecked () == TRUE ) // set a parent id and type for a child account
          {
            // go through the parentlist we created and determine the parent accountid
            // we can't use the name of the account because there may be two accounts
            // with the same name.  This function does it all by accountid
            int counter;
            for ( counter = 0; counter < listview->childCount() + 1; counter++ )
              if ( ( parentlist [ counter ] [ 2 ].toInt() ) == newaccount->childbox->currentItem() )
                {
                  parentid = parentlist [ counter ] [ 1 ].toInt();
                  break;
                }
            type = ( newaccount->accounttype->currentItem() ) + 6;  // sets account ids for child accounts.  See accountdisplay.h for types
          }
        else
          {
            parentid = -1;
            type = newaccount->accounttype->currentItem(); // sets account ids for parent accounts
          }

        // add the new account
	if ( newaccount->getDateEdited () == TRUE )
          account->addAccount ( newaccount->accountname->text(), parentid, newaccount->accountbalance->text().toFloat(), type,
          newaccount->getDescription(), newaccount->creditlimit->text().toFloat(), newaccount->getYear(),
          newaccount->getMonth(), newaccount->getDay(), newaccount->accountbalance->text().toFloat(), newaccount->currencybox->currencybox->currentText() );
	else
          account->addAccount ( newaccount->accountname->text (), parentid, newaccount->accountbalance->text().toFloat(), type,
          newaccount->getDescription(), newaccount->creditlimit->text().toFloat(), defaultyear,
          defaultmonth, defaultday, newaccount->accountbalance->text().toFloat(), newaccount->currencybox->currencybox->currentText() );

        if ( parentid != -1 )
          account->changeParentAccountBalance ( parentid );

        // redisplay accounts
        // this function clears the account display first
        account->displayAccounts ( listview );
        setToggleButton();
      }
     maintabs->setTabEnabled ( tab2, FALSE );
  }

void AccountDisplay::deleteAccount ()
  {
    if ( listview->selectedItem() == 0 )
      QMessageBox::warning ( this, "QashMoney", "Please select an account\nto delete.");
    else if ( listview->selectedItem()->parent() == 0 && listview->selectedItem()->childCount() != 0 )
      QMessageBox::warning ( this, "QashMoney", "Can't delete parent accounts\nwith children");
    else
    {
      QMessageBox mb ( "Delete Account", "This will delete all transactions\nand transfers for this account.", QMessageBox::Information, QMessageBox::Ok, QMessageBox::Cancel, QMessageBox::NoButton );
      if ( mb.exec() == QMessageBox::Ok )
        {
          int accountid = listview->selectedItem()->text ( getIDColumn() ).toInt ();
          int parentid = account->getParentAccountID ( accountid );

          // delete all the transactions and transfers for the account
          transaction->deleteAllTransactions ( accountid );
          transfer->deleteAllTransfers ( accountid );

          // delete the account
          account->deleteAccount ( accountid );

          // update account balances
          if ( parentid != -1 )
            account->changeParentAccountBalance ( parentid );

          //redisplay accounts
          account->displayAccounts ( listview );

          //remove all the columns from the accountdisplay if there are not any accounts
          if ( account->getNumberOfAccounts() == 0 )
            {
              int columns = listview->columns();
              int counter;
              for ( counter = 0; counter <= columns; counter++ )
                listview->removeColumn ( 0 );
            }

          setToggleButton();
        }
    }
    maintabs->setTabEnabled ( tab2, FALSE );
  }

void AccountDisplay::setToggleButton ()
  {
    // iterate through account display and determine how many "transferable" accounts we have
    // if there are less than two, disable the transfer button
    QListViewItemIterator it ( listview );
    int counter = 0;
    for ( ; it.current(); ++it )
      {
        // add one to counter if we find a transferable account
        if ( it.current()->parent() != 0 || ( it.current()->childCount() ) == 0 )
          counter++;
      }
    if ( counter > 1 )
      transferbutton->show();
    else
      transferbutton->hide();
  }

void AccountDisplay::accountTransfer ( bool state )
  {
    if ( state == TRUE )
      {
        firstaccountid = -1;
        secondaccountid = -1;
        listview->clearSelection ();
        listview->setMultiSelection ( TRUE );
        disableParentsWithChildren ();
        connect ( listview, SIGNAL ( clicked(QListViewItem*) ), this, SLOT ( getTransferAccounts(QListViewItem*) ) );
      }
    else
      {
        firstaccountid = -1;
        secondaccountid = -1;
        listview->clearSelection ();
	listview->setMultiSelection ( FALSE );
        enableAccounts ();
        disconnect ( listview, SIGNAL ( clicked(QListViewItem*) ), this, SLOT ( getTransferAccounts(QListViewItem*) ) );
      }
  }

void AccountDisplay::getTransferAccounts ( QListViewItem * item )
  {
    if ( item->parent() != 0 || item->childCount() == 0 ) // only set an account for transfer if its a child or parent with no children
      {
        if ( firstaccountid == -1 )
          firstaccountid = item->text ( getIDColumn() ).toInt(); // set first account if we've selected a valid account
        else
          if ( item->text ( getIDColumn() ).toInt() != firstaccountid ) // set the second account if its not equal to the first
            secondaccountid = item->text ( getIDColumn() ).toInt();
      }

    // open transfer window if both accounts are set
    if ( firstaccountid != -1 && secondaccountid != -1 )
      {
        // construct the transferdialog window
        TransferDialog *td = new TransferDialog ( this, firstaccountid, secondaccountid );

        // enter today's date in the date box as default
        QDate today = QDate::currentDate ();
        int defaultday = today.day();
        int defaultmonth = today.month();
        int defaultyear  = today.year();
        td->date->setText ( preferences->getDate ( defaultyear, defaultmonth, defaultday ) );

        if ( td->exec() == QDialog::Accepted )
          {
            // set the cleared integer if the checkbox is checked
            if ( td->clearedcheckbox->isChecked() == TRUE )
              cleared = 1;
            qDebug("Year from transferdialog = %i",td->getYear());
            // add the transfer with a new date if its been edited or use the default date
            if ( td->getDateEdited () == TRUE )
              transfer->addTransfer ( firstaccountid, account->getParentAccountID ( firstaccountid ), secondaccountid, account->getParentAccountID ( secondaccountid ), td->getDay(), td->getMonth(), td->getYear(), td->amount->text().toFloat(), cleared );
            else
              transfer->addTransfer ( firstaccountid, account->getParentAccountID ( firstaccountid ), secondaccountid, account->getParentAccountID ( secondaccountid ), defaultday, defaultmonth, defaultyear, td->amount->text().toFloat(), cleared );

            // update account balances of both accounts and parents if necessary
            account->updateAccountBalance ( firstaccountid );
             if ( account->getParentAccountID ( firstaccountid ) != -1 )
               account->changeParentAccountBalance ( account->getParentAccountID ( firstaccountid ) );
            account->updateAccountBalance ( secondaccountid );
            if ( account->getParentAccountID ( secondaccountid ) != -1 )
               account->changeParentAccountBalance ( account->getParentAccountID ( secondaccountid ) );

            // redisplay accounts
            account->displayAccounts ( listview );
          }
        else
          {
            firstaccountid = -1;
            secondaccountid = -1;
            listview->clearSelection ();
	    listview->setMultiSelection ( FALSE );
            disconnect ( listview, SIGNAL ( clicked(QListViewItem*) ), this, SLOT ( getTransferAccounts(QListViewItem*) ) );
          }

       // reset the accounts display window
       transferbutton->toggle(); // toggling this button with clear the window as well

       // reenable all the accounts so the transaction tab will be properly set
       enableAccounts ();
      }
  }

void AccountDisplay::disableParentsWithChildren ()
  {
     // iterate through accountdisplay listview and disable all the parents that have children
     QListViewItemIterator it ( listview );
     for ( ; it.current(); ++it )
       {
         if ( it.current()->parent() == 0 && it.current()->childCount() != 0 )
           it.current()->setSelectable ( FALSE );
       }
  }

void AccountDisplay::enableAccounts ()
  {
     // iterate through accountdisplay listview and enable all accounts
     QListViewItemIterator it ( listview );
     for ( ; it.current(); ++it )
       it.current()->setSelectable ( TRUE );
  }

void AccountDisplay::saveColumnSize ( int column, int oldsize, int newsize )
  {
    switch ( column )
      {
        case 0:
          if ( listview->columns() == 3 )
            preferences->changeColumnPreference ( 1, newsize );
          else
            preferences->changeColumnPreference ( 10, newsize );
          break;
        case 1:
          if ( listview->columns() == 3 )
            preferences->changeColumnPreference ( 2, newsize );
          else
            preferences->changeColumnPreference ( 11, newsize );
          break;
        case 2:
            preferences->changeColumnPreference ( 12, newsize );
          break;
      }

  }

void AccountDisplay::saveSortingPreference ( int column )
  {
    preferences->changeSortingPreference ( 1, column );
  }

int AccountDisplay::getIDColumn ()
  {
    int counter;
    int columns = listview->columns();
    for ( counter = 0; counter <= columns; counter++ )
      if ( listview->header()->label ( counter ).length() == 0 )
        return counter;
  }

void AccountDisplay::editAccount ()
  {
    if ( listview->selectedItem() == 0 )
      QMessageBox::warning ( this, "QashMoney", "Please select an account\nto edit.");
    else
      {
        // set the accountid
        int accountid = listview->selectedItem()->text ( getIDColumn() ).toInt();

        //construct new dialog box
        QDialog *editaccountwindow = new QDialog ( this, 0, TRUE );
        editaccountwindow->setCaption ( "Edit Account" );

        // construct the items which will go in the dialog bix
        QLabel *namelabel = new QLabel ( "Account Name", editaccountwindow );
        QLineEdit *accountname = new QLineEdit ( editaccountwindow );
        QLabel *descriptionlabel = new QLabel ( "Account Description", editaccountwindow );
        QLineEdit *accountdescription = new QLineEdit ( editaccountwindow );
        Currency *currencybox = new Currency ( editaccountwindow );

        QVBoxLayout *layout = new QVBoxLayout ( editaccountwindow, 5, 2 );
        layout->addWidget ( namelabel );
        layout->addWidget ( accountname );
        layout->addWidget ( descriptionlabel );
        layout->addWidget ( accountdescription );
        layout->addWidget ( currencybox );

        //set the account name
        accountname->setText ( listview->selectedItem()->text ( 0 ) );

        //set the account description
        accountdescription->setText ( account->getAccountDescription ( accountid ) );

        if ( preferences->getPreference ( 4 ) == 1 )
          {
            // get currency code for this account then iterate through the currency box
            // to find the one we want
            int count = currencybox->currencybox->count();
            QString code = account->getCurrencyCode ( accountid );
            for ( int counter = 0; count - 1; counter++ )
              {
                if ( QString::compare ( currencybox->currencybox->text ( counter ), code ) == 0 )
                  {
                    currencybox->currencybox->setCurrentItem ( counter );
                    break;
                  }
              }
            }
          else
            currencybox->setEnabled ( FALSE );

        //execute the dialog box
        int response = editaccountwindow->exec();
        if ( response == 1 )
         {
           account->updateAccount ( accountname->text(), accountdescription->text(), currencybox->currencybox->currentText(), accountid );
           account->displayAccounts ( listview );

           // Try and select the same account that was just edited
           QListViewItemIterator it ( listview );
           for ( ; it.current(); ++it )
             {
               if ( it.current()->text ( 0 ) == accountname->text() )
                 {
                   listview->setSelected ( it.current(), TRUE );
                   return;
                 }
             }
           maintabs->setTabEnabled ( tab2, FALSE );
         }
      }
  }

void AccountDisplay::setAccountExpanded ( QListViewItem *item )
  {
    int accountid = item->text ( getIDColumn() ).toInt();
    account->setAccountExpanded ( 1, accountid );
  }

void AccountDisplay::setAccountCollapsed ( QListViewItem *item )
  {
    int accountid = item->text ( getIDColumn() ).toInt();
    account->setAccountExpanded ( 0, accountid );
  }


