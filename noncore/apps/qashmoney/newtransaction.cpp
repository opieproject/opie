#include "newtransaction.h"
#include "calculator.h"
#include "datepicker.h"
#include "budget.h"

#include <qmultilineedit.h>

extern Budget *budget;
extern Preferences *preferences;

NewTransaction::NewTransaction ( QWidget* parent ) : QDialog ( parent, 0, TRUE )
  {
    transactiondescription = "";
    currentlineitem = -1;
    currentbudget = -1;
    dateedited = FALSE;
    setCaption( tr( "Transaction" ) );

    // START FIRST COLUMN

    namelabel = new QLabel ( "Transaction", this );

    transactionnamebox = new QHBox ( this );
    transactionname = new QComboBox ( transactionnamebox );
    transactionname->setEditable ( TRUE );
    descriptionbutton = new QPushButton ( transactionnamebox );
    descriptionbutton->setPixmap ( QPixmap ( "/opt/QtPalmtop/pics/info.png" ) );
    connect ( descriptionbutton, SIGNAL ( released() ), this, SLOT ( addTransactionDescription() ) );

    amountlabel = new QLabel ( "Amount", this );

    transactionamountbox = new QHBox ( this );
    transactionamount = new QLineEdit ( transactionamountbox );
    transactionamount->setAlignment ( Qt::AlignRight );
    transactionamount->setText ( "0.00" );
    calculatorbutton = new QPushButton( transactionamountbox );
    calculatorbutton->setPixmap ( QPixmap ( "/opt/QtPalmtop/pics/kcalc.png" ) );
    connect ( calculatorbutton, SIGNAL ( released() ), this, SLOT ( showCalculator() ) );

    datelabel = new QLabel ( "Date", this );

    transactiondatebox = new QHBox ( this );
    transactiondate = new QLineEdit ( transactiondatebox );
    transactiondate->setAlignment ( Qt::AlignRight );
    transactiondate->setDisabled ( TRUE );
    datebutton = new QPushButton( transactiondatebox );
    datebutton->setPixmap ( QPixmap ( "/opt/QtPalmtop/pics/date.png" ) );
    connect ( datebutton, SIGNAL ( released() ), this, SLOT ( showCalendar() ) );

    clearedcheckbox = new QCheckBox ( "Cleared", this );
    depositbox = new QCheckBox ( "Credit", this );

    // START SECOND COLUMN

    numberlabel = new QLabel ( "Number", this );
    transactionnumber = new QLineEdit ( this );

    budgetlabel = new QLabel ( "Budget", this );
    budgetbox = new QComboBox ( FALSE, this );

    lineitemlabel = new QLabel ( "LineItem", this );
    lineitembox = new QComboBox ( FALSE, this );

    layout = new QGridLayout ( this, 7, 2, 2, 2 );
    layout->addWidget ( namelabel, 0, 0, Qt::AlignLeft );
    layout->addWidget ( transactionnamebox, 1, 0, Qt::AlignLeft );
    layout->addWidget ( amountlabel, 2, 0, Qt::AlignLeft );
    layout->addWidget ( transactionamountbox, 3, 0, Qt::AlignLeft );
    layout->addWidget ( datelabel, 4, 0, Qt::AlignLeft );
    layout->addWidget ( transactiondatebox, 5, 0, Qt::AlignLeft );
    layout->addWidget ( clearedcheckbox, 6, 0, Qt::AlignLeft );
    layout->addWidget ( numberlabel, 0, 1, Qt::AlignLeft );
    layout->addWidget ( transactionnumber, 1, 1, Qt::AlignLeft );
    layout->addWidget ( budgetlabel, 2, 1, Qt::AlignLeft );
    layout->addWidget ( budgetbox, 3, 1, Qt::AlignLeft );
    layout->addWidget ( lineitemlabel, 4, 1, Qt::AlignLeft );
    layout->addWidget ( lineitembox, 5, 1, Qt::AlignLeft );
    layout->addWidget ( depositbox, 6, 1, Qt::AlignLeft );

    if ( budget->getNumberOfBudgets() != 0 )
      {
        budgetnameslist = budget->getBudgetNames();
        budgetidslist = budget->getBudgetIDs();
        budgetbox->insertStringList ( *budgetnameslist );
        lineitemlabel->setEnabled ( FALSE );
        lineitembox->setEnabled ( FALSE );
        connect ( budgetbox, SIGNAL ( activated(int) ), this, SLOT ( setCurrentBudget(int) ) );
        connect ( lineitembox, SIGNAL ( activated(int) ), this, SLOT ( setCurrentLineItem(int) ) );
      }
    else
      {
        budgetlabel->setEnabled ( FALSE );
        budgetbox->setEnabled ( FALSE );
        lineitemlabel->setEnabled ( FALSE );
        lineitembox->setEnabled ( FALSE );
      }

}

NewTransaction::~NewTransaction ()
  {
  }

void NewTransaction::showCalculator ()
{
    Calculator *calculator = new Calculator ( this );
    calculator->setMaximumWidth ( ( int ) ( this->size().width() * 0.9 ) );
    if ( calculator->exec () == QDialog::Accepted )
      transactionamount->setText ( calculator->display->text() );
}

void NewTransaction::showCalendar ()
  {
    QDate newDate = QDate::currentDate ();
    DatePicker *dp = new DatePicker ( newDate );
    dp->setMaximumWidth ( ( int ) ( this->size().width() * 0.9 ) );

    int response = dp->exec();
    if ( response == QDialog::Accepted )
      {
	// Set date integers
	year = dp->getYear();
	month = dp->getMonth();
	day = dp->getDay();

	// Set dateedited to TRUE
	// This tells the transactiondisplay object that the user edited an transaction
	// and did change the date3
	dateedited = TRUE;

	// Display date with our selected format
	transactiondate->setText ( preferences->getDate ( year, month, day ) );
      }
  }

bool NewTransaction::getDateEdited ()
  {
    return dateedited;
  }

int NewTransaction::getDay ()
  {
    return day;
  }

int NewTransaction::getMonth ()
  {
    return month;
  }

int NewTransaction::getYear ()
  {
    return year;
  }

QString NewTransaction::getDescription ()
  {
    return transactiondescription;
  }

void NewTransaction::setDescription ( QString description )
  {
    transactiondescription = description;
  }

void NewTransaction::addTransactionDescription ()
  {
    // Function for adding or editing an transaction description.
    QDialog *description = new QDialog ( this, "description", TRUE );
    description->setCaption ( "Notes" );
    QMultiLineEdit *enter = new QMultiLineEdit ( description );
    enter->setFixedSize ( ( int ) (this->width() * 0.75 ), ( int ) ( this->height() * 0.5 ) );
    enter->setWrapColumnOrWidth ( ( int ) (this->width() * 0.75 ) );
    enter->setWordWrap ( QMultiLineEdit::WidgetWidth );
    if ( transactiondescription != "(NULL)" )
      enter->setText ( transactiondescription );
    if ( description->exec () == QDialog::Accepted )
      transactiondescription = enter->text ();
  }

int NewTransaction::getNameIndex ( QString name )
  {
    int counter;
    int items = transactionname->count();
    for ( counter = 0; ( items - 1 ); counter++ )
      {
        if ( name == transactionname->text ( counter ) )
          {
            return counter;
            break;
          }
      }
    return 0;
  }

void NewTransaction::setCurrentBudget ( int index )
  {
    if ( index != 0 )
      {
        currentbudget = budgetidslist->operator[] ( index - 1 ).toInt();
        lineitemslist = budget->getLineItems ( currentbudget );
        lineitemidslist = budget->getLineItemIDs ( currentbudget );
        lineitemlabel->setEnabled ( TRUE );
        lineitembox->setEnabled ( TRUE );
        lineitembox->clear();
        lineitembox->insertStringList ( lineitemslist );
        setCurrentLineItem ( 0 );
      }
    else
      {
        lineitembox->clear();
        lineitemlabel->setEnabled ( FALSE );
        lineitembox->setEnabled ( FALSE );
        currentlineitem = -1;
        currentbudget = -1;
      }
  }

void NewTransaction::setCurrentLineItem ( int index )
  {
    currentlineitem = ( lineitemidslist.operator[] ( index ).toInt() );
  }

int NewTransaction::getCurrentBudget ()
  {
    return currentbudget;
  }

int NewTransaction::getBudgetIndex ( int budgetid )
  {
    currentbudget = budgetid;
    const QString budget = QString::number ( budgetid );
    return budgetidslist->findIndex ( budget );
  }

int NewTransaction::getLineItemIndex ( int lineitemid )
  {
    currentlineitem = lineitemid;
    const QString lineitem = QString::number ( lineitemid );
    return lineitemidslist.findIndex ( lineitem );
  }

void NewTransaction::setLineItems ()
  {
    lineitemslist = budget->getLineItems ( currentbudget );
    lineitemidslist = budget->getLineItemIDs ( currentbudget );
    lineitemlabel->setEnabled ( TRUE );
    lineitembox->setEnabled ( TRUE );
    lineitembox->clear();
    lineitembox->insertStringList ( lineitemslist );
  }

int NewTransaction::getCurrentLineItem ()
  {
    return currentlineitem;
  }

void NewTransaction::setComboBoxes ( int budgetid, int lineitemid )
  {
    const QString budgetname = QString::number ( budgetid );
    budgetbox->setCurrentItem ( ( budgetidslist->findIndex ( budgetname ) ) );
    currentbudget = budgetidslist->operator[] ( budgetbox->currentItem() - 1 ).toInt();

    lineitemslist = budget->getLineItems ( currentbudget );
    lineitemidslist = budget->getLineItemIDs ( currentbudget );
    lineitemlabel->setEnabled ( TRUE );
    lineitembox->setEnabled ( TRUE );
    lineitembox->clear();
    lineitembox->insertStringList ( lineitemslist );

    const QString lineitem = QString::number ( lineitemid );
    lineitembox->setCurrentItem ( lineitemidslist.findIndex ( lineitem ) );
    currentlineitem = ( lineitemidslist.operator[] ( lineitembox->currentItem() ).toInt() );
  }
