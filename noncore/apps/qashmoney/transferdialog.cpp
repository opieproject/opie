#include "transferdialog.h"
#include "datepicker.h"
#include "calculator.h"

#include <qdatetime.h>
#include <qfont.h>

extern Preferences *preferences;
extern Account *account;

TransferDialog::TransferDialog ( QWidget *parent, int fromaccountid, int toaccountid ) : QDialog ( parent, 0, TRUE )
  {
    dateedited = FALSE;
    setCaption ( "Transfer" );

    fromaccountlabel = new QLabel ( "From account:", this );
    QFont f = this->font();
    f.setWeight ( QFont::Bold );
    fromaccountlabel->setFont ( f );
    fromaccount = new QLabel ( account->getAccountName ( fromaccountid ), this );

    toaccountlabel = new QLabel ( "To Account:", this );
    toaccountlabel->setFont ( f );
    toaccount = new QLabel ( account->getAccountName ( toaccountid ), this );

    datelabel = new QLabel ( "Date", this );

    datebox = new QHBox ( this );
    datebox->setSpacing ( 2 );
    date = new QLineEdit ( datebox );
    date->setAlignment ( Qt::AlignRight );
    date->setDisabled ( TRUE );
    datebutton = new QPushButton ( datebox );
    datebutton->setPixmap ( QPixmap ( "/opt/QtPalmtop/pics/date.png" ) );
    connect ( datebutton, SIGNAL ( released () ), this, SLOT ( showCalendar () ) );

    amounttlabel = new QLabel ( "Amount", this );

    amountbox = new QHBox ( this );
    amountbox->setSpacing ( 2 );
    amount = new QLineEdit ( amountbox );
    amount->setAlignment ( Qt::AlignRight );
    calculatorbutton = new QPushButton( amountbox );
    calculatorbutton->setPixmap ( QPixmap ( "/opt/QtPalmtop/pics/kcalc.png" ) );
    connect ( calculatorbutton, SIGNAL ( released() ), this, SLOT ( showCalculator() ) );

    clearedcheckbox = new QCheckBox ( "Cleared", this );

    layout = new QVBoxLayout ( this, 4, 2 );
    layout->addWidget ( fromaccountlabel, Qt::AlignLeft );
    layout->addWidget ( fromaccount, Qt::AlignLeft );
    layout->addWidget ( toaccountlabel, Qt::AlignLeft );
    layout->addWidget ( toaccount, Qt::AlignLeft );
    layout->addSpacing ( 5 );
    layout->addWidget ( datelabel, Qt::AlignLeft );
    layout->addWidget ( datebox, Qt::AlignLeft );
    layout->addWidget ( amounttlabel, Qt::AlignLeft );
    layout->addWidget ( amountbox, Qt::AlignLeft );
    layout->addWidget ( clearedcheckbox, Qt::AlignLeft );
  }

bool TransferDialog::getDateEdited ()
  {
    return dateedited;
  }

void TransferDialog::showCalendar ()
  {
    QDate newDate = QDate::currentDate ();
    DatePicker *dp = new DatePicker ( newDate );
    if ( dp->exec () == QDialog::Accepted )
      {
	// Set date integers
	year = dp->getYear();
	month = dp->getMonth();
	day = dp->getDay();

	// Set dateedited to TRUE
	// This tells the accountdisplay object that the user edited an account
	// and did change the date
	dateedited = TRUE;

	// Display date with our selected format
	date->setText ( preferences->getDate ( year, month, day ) );
      }
  }

int TransferDialog::getDay ()
  {
    return day;
  }

int TransferDialog::getMonth ()
  {
    return month;
  }

int TransferDialog::getYear ()
  {
    return day;
  }

void TransferDialog::showCalculator ()
  {
    Calculator *calculator = new Calculator ( this );
    if ( calculator->exec () == QDialog::Accepted )
      amount->setText ( calculator->display->text() );
  }






