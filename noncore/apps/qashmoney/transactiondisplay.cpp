#include "transactiondisplay.h"
#include "newtransaction.h"
#include "account.h"
#include "budget.h"
#include "memory.h"
#include "transfer.h"
#include "preferences.h"
#include "calculator.h"
#include "datepicker.h"

#include <qdatetime.h>
#include <qmessagebox.h>
#include <qheader.h>
#include <qmultilineedit.h>
#include <qdatetime.h>

extern Transaction *transaction;
extern Budget *budget;
extern Account *account;
extern Preferences *preferences;
extern Memory *memory;
extern Transfer *transfer;

TransactionDisplay::TransactionDisplay ( QWidget* parent ) : QWidget ( parent )
  {
    // set transactiondisplay variables;
    accountid = 0;
    children = TRUE;

    firstline = new QHBox ( this );
    firstline->setSpacing ( 2 );

    newtransaction = new QPushButton ( firstline );
    newtransaction->setPixmap ( QPixmap ("/opt/QtPalmtop/pics/new.png") );
    connect ( newtransaction, SIGNAL ( released () ), this, SLOT ( addTransaction () ) );

    edittransaction = new QPushButton ( firstline );
    edittransaction->setPixmap( QPixmap ("/opt/QtPalmtop/pics/edit.png") );
    connect ( edittransaction, SIGNAL ( released () ), this, SLOT ( checkListViewEdit () ) );

    deletetransaction = new QPushButton ( firstline );
    deletetransaction->setPixmap( QPixmap ( "/opt/QtPalmtop/pics/delete.png") );
    connect ( deletetransaction, SIGNAL ( released () ), this, SLOT ( checkListViewDelete () ) );

    toggletransaction = new QPushButton ( firstline );
    toggletransaction->setPixmap( QPixmap ( "/opt/QtPalmtop/pics/redo.png") );
    connect ( toggletransaction, SIGNAL ( released () ), this, SLOT ( checkListViewToggle () ) );

    viewtransactionnotes = new QPushButton ( firstline );
    viewtransactionnotes->setPixmap( QPixmap ( "/opt/QtPalmtop/pics/info.png") );
    connect ( viewtransactionnotes, SIGNAL ( released () ), this, SLOT ( showTransactionNotes () ) );

    secondline = new QHBox ( this );
    secondline->setSpacing ( 5 );

    name = new QLabel ( secondline );
    balance = new QLabel ( secondline );

    QLabel *limit = new QLabel ( "Limit", secondline );
    limitbox = new QLineEdit ( secondline );
    limitbox->setMinimumWidth ( ( int ) ( this->width() / 6 ) );
    connect ( limitbox, SIGNAL ( textChanged ( const QString & ) ), this, SLOT ( limitDisplay ( const QString & ) ) );

    listview = new QListView ( this );
    listview->setAllColumnsShowFocus ( TRUE );
    listview->setShowSortIndicator ( TRUE );
    listview->header()->setTracking ( FALSE );
    connect ( listview->header(), SIGNAL ( sizeChange ( int, int, int ) ), this, SLOT ( saveColumnSize ( int, int, int ) ) );
    connect ( listview->header(), SIGNAL ( clicked ( int ) ), this, SLOT ( saveSortingPreference ( int ) ) );

    layout = new QVBoxLayout ( this, 2, 2 );
    layout->addWidget ( firstline );
    layout->addWidget ( secondline );
    layout->addWidget ( listview );
  }

void TransactionDisplay::addTransaction ()
  {
    // create local variables
    int cleared = -1;

    // create new transaction window
    NewTransaction *newtransaction = new NewTransaction ( this );
    int width = this->size().width();
    newtransaction->transactionname->setMaximumWidth ( ( int ) ( width * 0.45 ) );
    newtransaction->transactionname->setMinimumWidth ( ( int ) ( width * 0.35 ) );
    newtransaction->lineitembox->setMaximumWidth ( ( int ) ( width * 0.45 ) );
    newtransaction->transactiondatebox->setMaximumWidth ( ( int ) ( width * 0.4 ) );
    newtransaction->transactionamountbox->setMaximumWidth ( ( int ) ( width * 0.4 ) );
    newtransaction->transactionnumber->setMaximumWidth ( ( int ) ( width * 0.25 ) );

    // enter today's date in the date box as defaul
    QDate today = QDate::currentDate ();
    int defaultday = today.day();
    int defaultmonth = today.month();
    int defaultyear  = today.year();
    newtransaction->transactiondate->setText ( preferences->getDate ( defaultyear, defaultmonth, defaultday ) );

    // add memory items to the transactionname combobox
    memory->displayMemoryItems ( newtransaction->transactionname );
    newtransaction->transactionname->insertItem ( "", 0 );

    if ( newtransaction->exec () == QDialog::Accepted )
      {
        if ( newtransaction->clearedcheckbox->isChecked () == TRUE ) // set a parent id and type for a child transaction
          cleared = 1;
        else
          cleared = 0;

        float amount = newtransaction->transactionamount->text().toFloat();
	if ( newtransaction->depositbox->isChecked() == FALSE )
	  amount = amount * -1;

       // add the transaction name to the memory items
       memory->addMemoryItem ( newtransaction->transactionname->currentText() );

        // add the transaction
        if ( newtransaction->getDateEdited () == TRUE )
          transaction->addTransaction ( newtransaction->getDescription(), newtransaction->transactionname->currentText(), accountid, account->getParentAccountID ( accountid ),
            newtransaction->transactionnumber->text().toInt(), newtransaction->getDay(), newtransaction->getMonth(), newtransaction->getYear(), amount, cleared, newtransaction->getCurrentBudget(),
            newtransaction->getCurrentLineItem() );
        else
          transaction->addTransaction ( newtransaction->getDescription(), newtransaction->transactionname->currentText(), accountid, account->getParentAccountID ( accountid ),
            newtransaction->transactionnumber->text().toInt(), defaultday, defaultmonth, defaultyear, amount, cleared, newtransaction->getCurrentBudget(), newtransaction->getCurrentLineItem() );

        // redisplay transactions
        listview->clear();
        QString displaytext = "%";
        displaytext.prepend ( limitbox->text() );
        setTransactionDisplayDate ();
        if ( transaction->getNumberOfTransactions() > 0 )
          transaction->displayTransactions ( listview, accountid, children, displaytext, displaydate );

        // redisplay transfers
        if ( transfer->getNumberOfTransfers() > 0 )
          transfer->displayTransfers ( listview, accountid, children, displaydate );

        // add the transaction amount to the account it's associated with
	// and update its parent account balance if necessary
        account->updateAccountBalance ( accountid );
        if ( account->getParentAccountID ( accountid ) != -1 )
          account->changeParentAccountBalance ( account->getParentAccountID ( accountid ) );

        // format then reset the account balance
        redisplayAccountBalance ();
      }
  }

void TransactionDisplay::checkListViewEdit ()
  {
    if ( listview->selectedItem() == 0 )
      QMessageBox::warning ( this, "QashMoney", "Please select a transaction\nto edit.");
    else if ( listview->currentItem()->text ( getIDColumn() ).toInt() < 0 )
      editTransfer ();
    else
      editTransaction();
  }

void TransactionDisplay::showCalculator ()
  {
    Calculator *calculator = new Calculator ( this );
    if ( calculator->exec () == QDialog::Accepted )
      amount->setText ( calculator->display->text() );
  }

void TransactionDisplay::showCalendar ()
  {
    QDate newDate = QDate::currentDate ();
    DatePicker *dp = new DatePicker ( newDate );
    if ( dp->exec () == QDialog::Accepted )
      {
	year = dp->getYear();
	month = dp->getMonth();
	day = dp->getDay();
	date->setText ( preferences->getDate ( year, month, day ) );
      }
  }

void TransactionDisplay::editTransfer ()
  {
    transferid = listview->currentItem()->text ( getIDColumn() ).toInt();
    fromaccount = transfer->getFromAccountID ( transferid );
    toaccount = transfer->getToAccountID ( transferid );
    year = transfer->getYear ( transferid );
    month = transfer->getMonth ( transferid );
    day = transfer->getDay ( transferid );

    QDialog *editransfer = new QDialog ( this, "edittransfer", TRUE );
    editransfer->setCaption ( "Transfer" );

    QStringList accountnames = account->getAccountNames();
    QStringList accountids = account->getAccountIDs();

    QLabel *fromaccountlabel = new QLabel ( "From Account:", editransfer );
    QFont f = this->font();
    f.setWeight ( QFont::Bold );
    fromaccountlabel->setFont ( f );

    QComboBox *fromaccountbox = new QComboBox ( editransfer );
    fromaccountbox->insertStringList ( accountnames );
    fromaccountbox->setCurrentItem ( accountids.findIndex ( QString::number ( fromaccount ) ) );

    QLabel *toaccountlabel = new QLabel ( "To Account:", editransfer );
    toaccountlabel->setFont ( f );

    QComboBox *toaccountbox = new QComboBox ( editransfer );
    toaccountbox->insertStringList ( accountnames );
    toaccountbox->setCurrentItem ( accountids.findIndex ( QString::number ( toaccount ) ) );

    QLabel *datelabel = new QLabel ( "Date", editransfer );
    QHBox *datebox = new QHBox ( editransfer );
    datebox->setSpacing ( 2 );
    date = new QLineEdit ( datebox );
    date->setAlignment ( Qt::AlignRight );
    date->setDisabled ( TRUE );
    date->setText ( preferences->getDate ( year, month, day ) );
    QPushButton *datebutton = new QPushButton ( datebox );
    datebutton->setPixmap ( QPixmap ( "/opt/QtPalmtop/pics/date.png" ) );
    connect ( datebutton, SIGNAL ( released () ), this, SLOT ( showCalendar () ) );

    QLabel *amounttlabel = new QLabel ( "Amount", editransfer );

    QHBox *amountbox = new QHBox ( editransfer );
    amountbox->setSpacing ( 2 );
    amount = new QLineEdit ( amountbox );
    amount->setAlignment ( Qt::AlignRight );
    amount->setText ( transfer->getAmount ( transferid ) );
    QPushButton *calculatorbutton = new QPushButton( amountbox );
    calculatorbutton->setPixmap ( QPixmap ( "/opt/QtPalmtop/pics/kcalc.png" ) );
    connect ( calculatorbutton, SIGNAL ( released() ), this, SLOT ( showCalculator() ) );

    QCheckBox *clearedcheckbox = new QCheckBox ( "Cleared", editransfer );

    QBoxLayout *layout = new QVBoxLayout ( editransfer, 4, 2 );
    layout->addWidget ( fromaccountlabel, Qt::AlignLeft );
    layout->addWidget ( fromaccountbox, Qt::AlignLeft );
    layout->addWidget ( toaccountlabel, Qt::AlignLeft );
    layout->addWidget ( toaccountbox, Qt::AlignLeft );
    layout->addSpacing ( 5 );
    layout->addWidget ( datelabel, Qt::AlignLeft );
    layout->addWidget ( datebox, Qt::AlignLeft );
    layout->addWidget ( amounttlabel, Qt::AlignLeft );
    layout->addWidget ( amountbox, Qt::AlignLeft );
    layout->addWidget ( clearedcheckbox, Qt::AlignLeft );

    if ( editransfer->exec() == QDialog::Accepted )
      {
        //get fromaccount
        fromaccount = ( accountids.operator[] ( fromaccountbox->currentItem() ) ).toInt();

        //get to account
        toaccount = ( accountids.operator[] ( toaccountbox->currentItem() ) ).toInt();

        //set cleared flag
        int cleared = 0;
        if ( clearedcheckbox->isChecked() == TRUE )
          cleared = 1;

        //update transfer
        transfer->updateTransfer ( fromaccount, account->getParentAccountID ( fromaccount ), toaccount, account->getParentAccountID ( toaccount ),
          day, month, year, amount->text().toFloat(), cleared, transferid );

        account->updateAccountBalance ( fromaccount );
        if ( account->getParentAccountID ( fromaccount ) != -1 )
          account->changeParentAccountBalance ( account->getParentAccountID ( fromaccount ) );

        updateAndDisplay ( toaccount );
      }
  }

void TransactionDisplay::editTransaction ()
  {
    int cleared;

    // set the transaction id and budgetid
    int transactionid = listview->currentItem()->text ( getIDColumn() ).toInt();
    int budgetid = transaction->getBudgetID ( transactionid );
    int lineitemid = transaction->getLineItemID ( transactionid );

    // create edit transaction window
    NewTransaction *newtransaction = new NewTransaction ( this );
    int width = this->width();
    newtransaction->transactionname->setMaximumWidth ( ( int ) ( width * 0.45 ) );
    newtransaction->transactionname->setMinimumWidth ( ( int ) ( width * 0.35 ) );
    newtransaction->lineitembox->setMaximumWidth ( ( int ) ( width * 0.45 ) );
    newtransaction->transactiondatebox->setMaximumWidth ( ( int ) ( width * 0.4 ) );
    newtransaction->transactionamountbox->setMaximumWidth ( ( int ) ( width * 0.4 ) );
    newtransaction->transactionnumber->setMaximumWidth ( ( int ) ( width * 0.25 ) );

    // enter the date in the date box
    newtransaction->year = transaction->getYear ( transactionid );
    newtransaction->month = transaction->getMonth ( transactionid );
    newtransaction->day = transaction->getDay ( transactionid );
    newtransaction->transactiondate->setText ( preferences->getDate ( newtransaction->year, newtransaction->month, newtransaction->day ) );

    // set the description
    newtransaction->setDescription ( transaction->getTransactionDescription ( transactionid ) );

    // add memory items to the transactionname combobox
    memory->displayMemoryItems ( newtransaction->transactionname );

    // add correct transaction name
    newtransaction->transactionname->setEditText ( transaction->getPayee ( transactionid ) );

    // add transaction number
    newtransaction->transactionnumber->setText ( transaction->getNumber ( transactionid ) );

    // add transaction amount
    newtransaction->transactionamount->setText ( transaction->getAbsoluteAmount ( transactionid ) );

    // check for and set the correct budget
    if ( budgetid >= 1 )  // only do it if this transaction has a budget and line item
      {
        newtransaction->budgetbox->setCurrentItem ( newtransaction->getBudgetIndex ( budgetid ) + 1 );
        if ( lineitemid >= 1 )
          {
            newtransaction->setLineItems ();
            newtransaction->lineitembox->setCurrentItem ( newtransaction->getLineItemIndex ( lineitemid ) );
          }
        else
          {
            newtransaction->lineitemlabel->setEnabled ( FALSE );
            newtransaction->lineitembox->setEnabled ( FALSE );
          }
      }
    else
      {
        newtransaction->lineitemlabel->setEnabled ( FALSE );
        newtransaction->lineitembox->setEnabled ( FALSE );
      }

    // check cleared checkbox if necessary
    if ( transaction->getCleared ( transactionid ) == 1 )
      newtransaction->clearedcheckbox->setChecked ( TRUE );

    //  check deposit box if necessary
    if ( transaction->getAmount ( transactionid ).toFloat() > 0 )
      newtransaction->depositbox->setChecked ( TRUE );

    if ( newtransaction->exec () == QDialog::Accepted )
      {
        if ( newtransaction->clearedcheckbox->isChecked () == TRUE )
          cleared = 1;
        else
          cleared = 0;

        float amount = newtransaction->transactionamount->text().toFloat();
	if ( newtransaction->depositbox->isChecked() == FALSE )
	  amount = amount * -1;

       // add the transaction name to the memory items
       memory->addMemoryItem ( newtransaction->transactionname->currentText() );

        // update the transaction
        transaction->updateTransaction ( newtransaction->getDescription(), newtransaction->transactionname->currentText(), newtransaction->transactionnumber->text().toInt(),
          newtransaction->getDay(), newtransaction->getMonth(), newtransaction->getYear(),
          amount, cleared, newtransaction->getCurrentBudget(), newtransaction->getCurrentLineItem(), transactionid );

        updateAndDisplay ( transaction->getAccountID ( transactionid ) );
      }
  }

void TransactionDisplay::updateAndDisplay ( int id )
  {
    // redisplay transactions
    listview->clear();
    QString displaytext = "%";
    displaytext.prepend ( limitbox->text() );
    setTransactionDisplayDate ();
    if ( transaction->getNumberOfTransactions() > 0 )
      transaction->displayTransactions ( listview, accountid, children, displaytext, displaydate );

    // redisplay transfers
    if ( transfer->getNumberOfTransfers() > 0 )
      transfer->displayTransfers ( listview, accountid, children, displaydate );

    // add the transaction amount to the account it's associated with
    // and update its parent account balance if necessary
    account->updateAccountBalance ( id );
    if ( account->getParentAccountID ( id ) != -1 )
      account->changeParentAccountBalance ( account->getParentAccountID ( id ) );

    // format then reset the account balance
    redisplayAccountBalance ();
  }

void TransactionDisplay::checkListViewDelete ()
  {
    if ( listview->selectedItem() == 0 )
      QMessageBox::warning ( this, "QashMoney", "Please select a transaction to\ndelete.");
    else
      deleteTransaction ();
  }

void TransactionDisplay::deleteTransaction ()
  {
    int transactionid = listview->currentItem()->text ( getIDColumn() ).toInt();

    if ( transactionid > 0 ) // takes care of deleting transactions
      {
        // check if we are viewing child transactions through a parent
        // in that case we will have to update balances for the parent
        // which is represented by accountid and the child account
        // which will be represented by childaccountid
        int childaccountid = -1;
        if ( listview->columns() == 5 )
          childaccountid = transaction->getAccountID ( transactionid );

        transaction->deleteTransaction ( transactionid );

        listview->clear();
        QString displaytext = "%";
        displaytext.prepend ( limitbox->text() );
        setTransactionDisplayDate ();
        if ( transaction->getNumberOfTransactions() > 0 )
          transaction->displayTransactions ( listview, accountid, children, displaytext, displaydate );

        if ( transfer->getNumberOfTransfers() > 0 )
          transfer->displayTransfers ( listview, accountid, children, displaydate );

        // if we are viewing different child accounts through the parent account
        // ie if there are five columns and the parentid is -1
        // update the accountid ( which is the parent ) and update the child account
        // balance.  Get its accountid from the transactionid
        account->updateAccountBalance ( accountid ); // will update either a parent or child
	if ( account->getParentAccountID ( accountid ) != -1 ) // update its parent if there is one
          account->changeParentAccountBalance ( account->getParentAccountID ( accountid ) );
        if ( childaccountid != -1 ) // we've set childaccountid
          account->updateAccountBalance ( childaccountid );

        // format then reset the account balance
        redisplayAccountBalance ();
      }
    else // takes care of deleting transfers
      {
        // get the accountids before we delete the transfer
        int fromaccountid = transfer->getFromAccountID ( transactionid );
        int toaccountid = transfer->getToAccountID ( transactionid );

        // delete the transfer and redisplay transactions
        transfer->deleteTransfer ( transactionid );

        listview->clear();
        QString displaytext = "%";
        displaytext.prepend ( limitbox->text() );
        setTransactionDisplayDate ();
        if ( transaction->getNumberOfTransactions() > 0 )
          transaction->displayTransactions ( listview, accountid, children, displaytext, displaydate );

        if ( transfer->getNumberOfTransfers() > 0 )
          transfer->displayTransfers ( listview, accountid, children, displaydate );

        // for the from account
        account->updateAccountBalance ( fromaccountid );
        if ( account->getParentAccountID ( fromaccountid ) != -1 )
          account->changeParentAccountBalance ( account->getParentAccountID ( fromaccountid ) );

        // for the to account
        account->updateAccountBalance ( toaccountid );
        if ( account->getParentAccountID ( toaccountid ) != -1 )
          account->changeParentAccountBalance ( account->getParentAccountID ( toaccountid ) );

        // format then reset the account balance
        redisplayAccountBalance ();
      }
  }

void TransactionDisplay::checkListViewToggle ()
  {
    if ( listview->selectedItem() == 0 )
      QMessageBox::warning ( this, "QashMoney", "Please select a transaction to\nclear or reset.");
    else
      toggleTransaction ();
  }

void TransactionDisplay::toggleTransaction ()
  {
    //get the transaction of the selected transaction to determine if its a transaction or transfer
    int transactionid = listview->currentItem()->text ( getIDColumn() ).toInt();

    if ( transactionid > 0 ) // if this is a transaction
      {
        if ( transaction->getCleared ( transactionid ) == 0 )
          transaction->setCleared ( transactionid, 1 );
        else
          transaction->setCleared ( transactionid, 0 );
      }
    else
      {
        if ( transfer->getCleared ( transactionid ) == 0 )
          transfer->setCleared ( transactionid, 1 );
        else
          transfer->setCleared ( transactionid, 0 );
      }

    listview->clear();
    QString displaytext = "%";
    displaytext.prepend ( limitbox->text() );
    setTransactionDisplayDate ();
    if ( transaction->getNumberOfTransactions() > 0 )
      transaction->displayTransactions ( listview, accountid, children, displaytext, displaydate );

    if ( transfer->getNumberOfTransfers() != 0 )
      transfer->displayTransfers ( listview, accountid, children, displaydate );
  }

void TransactionDisplay::redisplayAccountBalance ()
  {
    QString accountbalance = account->getAccountBalance ( accountid );
    balance->setText ( accountbalance );
  }

void TransactionDisplay::setChildren ( bool c )
  {
    children = c;
  }

void TransactionDisplay::setAccountID ( int id )
  {
    accountid = id;
  }

ColorListItem::ColorListItem ( QListView *parent ) : QListViewItem ( parent )
  {
  }

ColorListItem::ColorListItem ( QListView *parent, QString label1, QString label2, QString label3, QString label4 )
  :  QListViewItem ( parent, label1, label2, label3, label4 )
  {
  }

ColorListItem::ColorListItem ( QListView *parent, QString label1, QString label2, QString label3, QString label4, QString label5 )
  :  QListViewItem ( parent, label1, label2, label3, label4, label5 )
  {
  }

void ColorListItem::paintCell ( QPainter *p, const QColorGroup &cg, int column, int width, int alignment )
  {
    QColorGroup _cg ( cg );
    _cg.setColor ( QColorGroup::Text, Qt::red );
    QListViewItem::paintCell ( p, _cg, column, width, alignment );
  }

void TransactionDisplay::saveColumnSize ( int column, int oldsize, int newsize )
  {
    if ( listview->columns() == 4 )
      preferences->changeColumnPreference ( column + 3, newsize );
    else if ( listview->columns() == 5 && column != 4 )
      preferences->changeColumnPreference ( column + 6, newsize );
    else
      preferences->changeColumnPreference ( 9, newsize );
  }

void TransactionDisplay::saveSortingPreference ( int column )
  {
    preferences->changeSortingPreference ( 2, column );
  }

void TransactionDisplay::limitDisplay ( const QString &text )
  {
    listview->clear ();
    QString displaytext = "%";
    displaytext.prepend ( text );
    setTransactionDisplayDate ();
    if ( transaction->getNumberOfTransactions() > 0 )
      transaction->displayTransactions ( listview, accountid, children, displaytext, displaydate );

    if ( displaytext.length() == 1 || preferences->getPreference ( 6 ) == 1 )
      transfer->displayTransfers ( listview, accountid, children, displaydate );
  }

int TransactionDisplay::getIDColumn ()
  {
    int counter;
    int columns = listview->columns();
    for ( counter = 0; counter <= columns; counter++ )
      if ( listview->header()->label ( counter ).length() == 0 )
        return counter;
  }

void TransactionDisplay::showTransactionNotes ()
  {
    if ( listview->selectedItem() == 0 || listview->currentItem()->text ( getIDColumn() ).toInt() < 0  )
      QMessageBox::warning ( this, "QashMoney", "Please select a valid\ntransaction to view notes.");
    else
      {
        int transactionid = listview->selectedItem()->text ( getIDColumn() ).toInt ();
        QDialog *description = new QDialog ( this, "description", TRUE );
        description->setCaption ( "Notes" );
        QMultiLineEdit *notes = new QMultiLineEdit ( description );
        notes->setFixedSize ( ( int ) (this->width() * 0.75 ), ( int ) ( this->height() * 0.5 ) );
        notes->setWrapColumnOrWidth ( ( int ) (this->width() * 0.75 ) );
        notes->setWordWrap ( QMultiLineEdit::WidgetWidth );
        notes->setEnabled ( FALSE );
        notes->setText ( transaction->getTransactionDescription ( transactionid ) );
        description->show();
      }
  }

void TransactionDisplay::setTransactionDisplayDate ()
  {
    // determine how many days of transactions to show
    int limittype = preferences->getPreference ( 7 );
    if ( limittype != 5 ) // set today's date if we are not showing all transactions
      {
        QDate today = QDate::currentDate ();
        switch ( limittype ) // if we are not showing all transactions
          {
            case 0:  // viewing two weeks
              displaydate = today.addDays ( -14 );
              break;
            case 1:  // viewing one month
              displaydate = today.addDays ( -30 );
              break;
            case 2: // three months
              displaydate = today.addDays ( -90 );
              break;
            case 3: // six months
              displaydate = today.addDays ( -180 );
              break;
            case 4: // one year
              displaydate = today.addDays ( -365 );
              break;
          }
      }
    else
      displaydate = QDate ( 1900, 1, 1 );
  }
