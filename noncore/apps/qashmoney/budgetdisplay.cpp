#include <qmessagebox.h>
#include <qheader.h>
#include <sqlite.h>

#include "budgetdisplay.h"
#include "budget.h"
#include "datepicker.h"
#include "transaction.h"

extern Preferences *preferences;
extern Budget *budget;
extern Transaction *transaction;

BudgetDisplay::BudgetDisplay ( QWidget *parent ) : QWidget ( parent )
  {
    QFont font = this->font();
    font.setWeight ( QFont::Bold );

    //set the default date to today
    newDate = QDate::currentDate ();
    year = newDate.year();
    month = newDate.month();
    day = newDate.day();
    datelabel = preferences->getDate ( year, month );

    setCaption ( "Budget" );

    firstline = new QHBox ( this );
    firstline->setSpacing ( 2 );
    secondline = new QHBox ( this );
    secondline->setSpacing ( 10 );

    menu = new QMenuBar ( this );
    menu->setFrameStyle ( QFrame::Box | QFrame::Sunken );
    budgetmenu = new QPopupMenu ( this );
    lineitemsmenu = new QPopupMenu ( this );
    datemenu = new QPopupMenu ( this );
    menu->insertItem ( "Budget", budgetmenu );
    menu->insertItem ( "Line Item", lineitemsmenu );
    menu->insertItem ( "Date", datemenu );
    budgetmenu->insertItem ( "New", this, SLOT ( newBudget () ), 0, 1 );
    budgetmenu->insertItem ( "Edit", this, SLOT ( editBudget () ), 0, 2 );
    budgetmenu->insertItem ( "Delete", this, SLOT ( deleteBudget () ), 0, 3 );
    lineitemsmenu->insertItem ( "New", this, SLOT ( newLineItem () ), 0, 1 );
    lineitemsmenu->insertItem ( "Edit", this, SLOT ( editLineItem () ), 0, 2 );
    lineitemsmenu->insertItem ( "Delete", this, SLOT ( deleteLineItem () ), 0, 3 );
    datemenu->insertItem ( "Change", this, SLOT ( showCalendar() ) );

    budgetbox = new QComboBox ( firstline );
    connect ( budgetbox, SIGNAL ( activated ( int ) ), this, SLOT ( setCurrentBudget ( int ) ) );

    budgetview = new QComboBox ( firstline );
    budgetview->insertItem ( "Month" );
    budgetview->insertItem ( "Year" );
    connect ( budgetview, SIGNAL ( activated ( int ) ), this, SLOT ( setCurrentView ( int ) ) );

    budgeted = new QLabel ( secondline );
    budgeted->setFont ( font );
    actual = new QLabel ( secondline );
    actual->setFont ( font );
    date = new QLabel ( secondline );
    date->setFont ( font );

    listview = new QListView ( this );
    listview->setAllColumnsShowFocus ( TRUE );
    listview->setShowSortIndicator ( TRUE );
    listview->setRootIsDecorated ( TRUE );
    listview->setMultiSelection ( FALSE );
    listview->addColumn ( "Line Item", preferences->getColumnPreference ( 13 ) ); // column id 13
    listview->addColumn ( "Budget", preferences->getColumnPreference ( 14 ) ); // column id 14
    listview->addColumn ( "Actual", preferences->getColumnPreference ( 15 ) ); // column id 15
    listview->addColumn ( "", 0 ); // line item ids
    listview->setColumnWidthMode ( 0, QListView::Manual );
    listview->setColumnWidthMode ( 1, QListView::Manual );
    listview->setColumnWidthMode ( 2, QListView::Manual );
    listview->setColumnAlignment ( 1, Qt::AlignRight );
    listview->setColumnAlignment ( 2, Qt::AlignRight );
    listview->setColumnWidthMode ( 3, QListView::Manual );

    listview->header()->setTracking ( FALSE );
    connect ( listview->header(), SIGNAL ( sizeChange ( int, int, int ) ), this, SLOT ( saveColumnSize ( int, int, int ) ) );
    connect ( listview->header(), SIGNAL ( clicked ( int ) ), this, SLOT ( saveSortingPreference ( int ) ) );

    // pull the column sorting preference from the preferences table, and configure the listview accordingly
    int column = 0;
    int direction = 0;
    preferences->getSortingPreference ( 3, &column, &direction );
    listview->setSorting ( column, direction );

    displayBudgetNames();

    layout = new QVBoxLayout ( this, 2, 2 );
    layout->setMenuBar ( menu );
    layout->addWidget ( firstline );
    layout->addWidget ( secondline );
    layout->addWidget ( listview );
  }

void BudgetDisplay::deleteBudget ()
  {
    listview->clear();
    transaction->clearBudgetIDs ( currentbudget );
    budget->deleteBudget ( currentbudget );
    if ( budgetbox->count() != 0 )
      displayBudgetNames();
    checkBudgets();
  }

void BudgetDisplay::saveColumnSize ( int column, int oldsize, int newsize )
  {
    switch ( column )
      {
        case 0:
          preferences->changeColumnPreference ( 13, newsize );
          break;
        case 1:
          preferences->changeColumnPreference ( 14, newsize );
          break;
        case 2:
          preferences->changeColumnPreference ( 15, newsize );
          break;
      }
  }

void BudgetDisplay::saveSortingPreference ( int column )
  {
    preferences->changeSortingPreference ( 3, column );
  }

int BudgetDisplay::getIDColumn ()
  {
    int counter;
    int columns = listview->columns();
    for ( counter = 0; counter <= columns; counter++ )
      if ( listview->header()->label ( counter ).length() == 0 )
        return counter;
  }

void BudgetDisplay::newBudget ()
  {
    constructBudgetWindow();
    int response = nb->exec();
    if ( response == 1 )
     {
       // open a new budget object
       int addedbudget = budget->addBudget ( budgetname->text(), 0, description->text(), currencybox->currencybox->currentText(), day, month, year, day, month, year, 0 );
       transaction->clearBudgetIDs ( addedbudget );
       displayBudgetNames();
     }
    checkBudgets();
  }

void BudgetDisplay::constructBudgetWindow ()
  {
    //construct and format the new budget window
    nb = new QDialog ( this, 0, TRUE );
    nb->setCaption ( "Budget" );
    QLabel *namelabel = new QLabel ( "Budget Name", nb );
    budgetname = new QLineEdit ( nb );
    QLabel *descriptionlabel = new QLabel ( "Description", nb );
    description = new QLineEdit ( nb );
    currencybox = new Currency ( nb );
    QBoxLayout *layout = new QVBoxLayout ( nb, 2, 2 );
    layout->addWidget ( namelabel );
    layout->addWidget ( budgetname );
    layout->addWidget ( descriptionlabel );
    layout->addWidget ( description );
    layout->addWidget ( currencybox );
  }

void BudgetDisplay::displayBudgetNames ()
  {
    budgetbox->clear();
    if ( budget->getNumberOfBudgets() != 0 )
      {
        ids = budget->getBudgetIDs();
        for ( QStringList::Iterator it = ids->begin(); it != ids->end(); ++it )
          {
            QString flag = "/opt/QtPalmtop/pics/flags/";
            flag.append ( budget->getCurrency ( (*it).toInt() ) );
            flag.append ( ".png" );
            budgetbox->insertItem ( QPixmap ( flag ), budget->getBudgetName ( (*it).toInt() ) );
          }
        setCurrentBudget ( 0 );
      }
    else
      checkBudgets();
  }

void BudgetDisplay::setCurrentBudget ( int index )
  {
    currentbudget = ( ids->operator[] ( index ).toInt() );
    displayLineItems();
  }

void BudgetDisplay::setCurrentView ( int index )
  {
    displayLineItems();
  }

void BudgetDisplay::showCalendar ()
  {
    // create new calendar object and show it
    DatePicker *dp = new DatePicker ( QDate ( year, month, day ) );
    dp->daylabel->hide();
    dp->daybox->hide();
    if ( budgetview->currentItem() == 1 )
      {
        dp->monthlabel->hide();
        dp->monthbox->hide();
      }
    dp->setMaximumWidth ( ( int ) ( this->size().width() * 0.9 ) );

    int response = dp->exec();
    if ( response == 1 )
      {
	// Set date integers
	year = dp->getYear();
        if ( budgetview->currentItem() == 0 )
          month = dp->getMonth();
        else
          month = newDate.month();
        datelabel = preferences->getDate ( year, month );
        displayLineItems();
      }
  }

void BudgetDisplay::newLineItem ()
  {
    //construct and format the new line item window
    constructLineItemWindow ();

    int response = newlineitem->exec();
    if ( response == 1 )
     {
       float amount;
       if ( lineitemtime->currentItem() == 0 )
         amount = lineitemamount->text().toFloat();
       else if ( lineitemtime->currentItem() == 1 )
         amount = lineitemamount->text().toFloat() * 12;
       else
         amount = lineitemamount->text().toFloat() * 52;
       int lineitemadded = budget->addLineItem ( currentbudget, lineitemname->text(), amount, lineitemtime->currentItem() );
       transaction->clearBudgetIDs ( currentbudget, lineitemadded );
       displayLineItems();
     }
    checkBudgets();
  }

void BudgetDisplay::constructLineItemWindow ()
  {
    //construct and format the new budget window
    newlineitem = new QDialog ( this, 0, TRUE );
    newlineitem->setCaption ( "Line Item" );
    QLabel *namelabel = new QLabel ( "Line Item Name", newlineitem );
    lineitemname = new QLineEdit ( newlineitem );
    QLabel *budgetamountlabel = new QLabel ( "Budget Amount", newlineitem );
    lineitemamount = new QLineEdit ( newlineitem );
    QLabel *lineitemtimelabel = new QLabel ( "Per:", newlineitem );
    lineitemtime = new QComboBox ( newlineitem );
    lineitemtime->insertItem ( "Year" ); // type 0
    lineitemtime->insertItem ( "Month" ); // type 1
    lineitemtime->insertItem ( "Week" ); // type 2
    QBoxLayout *layout = new QVBoxLayout ( newlineitem, 2, 2 );
    layout->addWidget ( namelabel );
    layout->addWidget ( lineitemname );
    layout->addWidget ( budgetamountlabel );
    layout->addWidget ( lineitemamount );
    layout->addWidget ( lineitemtimelabel );
    layout->addWidget ( lineitemtime );
  }

void BudgetDisplay::deleteLineItem ()
  {
    if ( listview->selectedItem() != 0 )
      {
        int lineitemid = listview->selectedItem()->text ( getIDColumn() ).toInt();
        transaction->clearBudgetIDs ( currentbudget, lineitemid );
        budget->deleteLineItem ( currentbudget, lineitemid );
        displayBudgetNames();
      }
    else
      QMessageBox::warning ( this, "QashMoney", "Please select a line item to delete." );
    checkBudgets();
  }

void BudgetDisplay::displayLineItems ()
  {
    listview->clear();
    if ( budget->getNumberOfBudgets() != 0 )
      {
        QString budgettable = budgetbox->currentText();
        budgettable.append ( QString::number ( currentbudget ) );
        budget->displayLineItems ( currentbudget, listview, month, year, budgetview->currentItem() );
        totalactual = transaction->getActualTotal ( currentbudget, year, month, budgetview->currentItem() );
        totalbudget = budget->getBudgetTotal ( currentbudget, budgetview->currentItem() );
        updateBudgetInformation();
      }
  }

void BudgetDisplay::checkBudgets ()
  {
    if ( budget->getNumberOfBudgets() == 0 )
      {
        budgetview->setEnabled ( FALSE );
        budgetmenu->setItemEnabled ( 2, FALSE );
        budgetmenu->setItemEnabled ( 3, FALSE );
        lineitemsmenu->setItemEnabled ( 1, FALSE );
        lineitemsmenu->setItemEnabled ( 2, FALSE );
        lineitemsmenu->setItemEnabled ( 3, FALSE );
      }
    else
      {
        budgetview->setEnabled ( TRUE );
        budgetmenu->setItemEnabled ( 2, TRUE );
        budgetmenu->setItemEnabled ( 3, TRUE );
        lineitemsmenu->setItemEnabled ( 1, TRUE );
        lineitemsmenu->setItemEnabled ( 2, FALSE );
        lineitemsmenu->setItemEnabled ( 3, FALSE );

        if ( budget->getNumberOfLineItems ( currentbudget ) != 0 )
          {
            lineitemsmenu->setItemEnabled ( 2, TRUE );
            lineitemsmenu->setItemEnabled ( 3, TRUE );
          }
      }
  }

void BudgetDisplay::updateBudgetInformation ()
  {
    if ( budgetview->currentItem() == 0 )
      {
        datelabel = preferences->getDate ( year, month );
        datelabel.prepend ( "Date: " );
        date->setText ( datelabel );
      }
    else
      date->setText ( QString::number ( year ) );

    QString budget = "Budget: ";
    budget.append ( totalbudget );
    budgeted->setText ( budget );

    QString actualamount = "Actual: ";
    actualamount.append ( totalactual );
    actual->setText ( actualamount );
  }

void BudgetDisplay::editBudget ()
  {
    constructBudgetWindow();

    //set the title
    budgetname->setText ( budget->getBudgetName ( currentbudget ) );
    //set the description
    description->setText ( budget->getBudgetDescription ( currentbudget ) );
    // retrieve the two character currency code then
    // go through the currencty box and find the code
    //set the currency box to that index number
    int count = currencybox->currencybox->count();
    QString code = budget->getCurrency ( currentbudget );
    for ( int counter = 0; count - 1; counter++ )
      {
        if ( QString::compare (currencybox->currencybox->text ( counter ), code ) == 0 )
          {
            currencybox->currencybox->setCurrentItem ( counter );
            break;
          }
      }
    int response = nb->exec();
    if ( response == 1 )
     {
       budget->updateBudget ( budgetname->text(), description->text(), currencybox->currencybox->currentText(), currentbudget );
       displayBudgetNames();
     }
  }

void BudgetDisplay::editLineItem ()
  {
    if ( listview->selectedItem() != 0 )
      {
        constructLineItemWindow();

        // set the line item name
        lineitemname->setText ( listview->selectedItem()->text( 0 ) );

        // set the line item time combobox
        int lineitemtype = budget->getLineItemTime ( currentbudget, listview->selectedItem()->text ( 3 ).toInt() );
        lineitemtime->setCurrentItem ( lineitemtype );

        // set the line item amount
        float amount = budget->getLineItemAmount ( currentbudget, listview->selectedItem()->text ( 3 ).toInt() );
        if ( lineitemtype == 1 )
          amount = amount / 12;
        else if ( lineitemtype == 2 )
          amount = amount / 52;
        lineitemamount->setText ( QString::number ( amount ) );

        int response = newlineitem->exec();
        if ( response == 1 )
         {
           float amount;
           if ( lineitemtime->currentItem() == 0 )
             amount = lineitemamount->text().toFloat();
           else if ( lineitemtime->currentItem() == 1 )
             amount = lineitemamount->text().toFloat() * 12;
           else
             amount = lineitemamount->text().toFloat() * 52;
           budget->updateLineItem ( lineitemname->text(), amount, lineitemtime->currentItem(), currentbudget, listview->selectedItem()->text ( 3 ).toInt() );
           displayLineItems();
         }
      }
    else
      QMessageBox::warning ( this, "QashMoney", "Please select a line item to edit." );
  }


