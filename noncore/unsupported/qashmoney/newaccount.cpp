#include "newaccount.h"
#include "calculator.h"
#include "datepicker.h"
#include <qmultilineedit.h>

extern Preferences *preferences;

NewAccount::NewAccount ( QWidget *parent, const char *name, bool modal ) : QDialog ( parent, name, modal )
  {
    accountdescription = "";
    dateedited = FALSE;
    setCaption( tr( "Account" ) );

    namelabel = new QLabel ( "Account Name", this );

    accountbox = new QHBox ( this );
    accountname = new QLineEdit ( accountbox );
    descriptionbutton = new QPushButton ( accountbox );
    descriptionbutton->setPixmap ( QPixmap ( "/opt/QtPalmtop/pics/info.png" ) );

    datelabel = new QLabel ( "Date", this );

    datebox = new QHBox ( this );
    startdate = new QLineEdit ( datebox );
    startdate->setDisabled ( TRUE );
    datebutton = new QPushButton ( datebox );
    datebutton->setPixmap ( QPixmap ( "/opt/QtPalmtop/pics/date.png" ) );

    childcheckbox = new QCheckBox ( this );
    childcheckbox->setText( tr ( "Child Account" ) );

    childlabel = new QLabel ( "Child of", this );
    childbox = new QComboBox ( FALSE, this );
    hideChildPulldownMenu ();

    balancelabel = new QLabel ( "Balance", this );

    balancebox = new QHBox ( this );
    accountbalance = new QLineEdit ( balancebox );
    accountbalance->setText ( "0.00" );
    balancecalculator = new QPushButton( balancebox );
    balancecalculator->setPixmap ( QPixmap ( "/opt/QtPalmtop/pics/kcalc.png" ) );

    creditlimitlabel = new QLabel ( "Credit Limit", this );

    creditlimitbox = new QHBox ( this );
    creditlimit = new QLineEdit ( creditlimitbox );
    creditlimitbox->setEnabled ( FALSE );
    creditlimitcalculator = new QPushButton( creditlimitbox );
    creditlimitcalculator->setPixmap ( QPixmap ( "/opt/QtPalmtop/pics/kcalc.png" ) );

    currencybox = new Currency ( this );

    typelabel = new QLabel ( "Type", this );
    accounttype = new QComboBox ( FALSE, this );
    accounttype->insertItem( tr( "Bank" ) );
    accounttype->insertItem( tr( "Cash" ) );
    accounttype->insertItem( tr( "Credit Card" ) );
    accounttype->insertItem( tr( "Equity" ) );
    accounttype->insertItem( tr( "Asset" ) );
    accounttype->insertItem( tr( "Liability" ) );

    layout = new QGridLayout ( this, 7, 2, 4, 2 );
    layout->addWidget ( namelabel , 0, 0, Qt::AlignLeft );
    layout->addWidget ( accountbox, 1, 0, Qt::AlignLeft );
    layout->addWidget ( datelabel, 2, 0, Qt::AlignLeft );
    layout->addWidget ( datebox, 3, 0, Qt::AlignLeft );
    layout->addWidget ( childcheckbox, 4, 0, Qt::AlignLeft );
    layout->addWidget ( childlabel, 5, 0, Qt::AlignLeft );
    layout->addWidget ( childbox, 6, 0, Qt::AlignLeft );
    layout->addWidget ( balancelabel, 0, 1, Qt::AlignLeft );
    layout->addWidget ( balancebox, 1, 1, Qt::AlignLeft );
    layout->addWidget ( creditlimitlabel, 2, 1, Qt::AlignLeft );
    layout->addWidget ( creditlimitbox, 3, 1, Qt::AlignLeft );
    layout->addWidget ( currencybox, 4, 1, Qt::AlignLeft );
    layout->addWidget ( typelabel, 5, 1, Qt::AlignLeft );
    layout->addWidget ( accounttype, 6, 1, Qt::AlignLeft );

    connect ( childcheckbox, SIGNAL ( clicked() ), this, SLOT ( showChildPulldownMenu() ) );
    connect ( balancecalculator, SIGNAL ( released() ), this, SLOT ( showCalculator() ) );
    connect ( creditlimitcalculator, SIGNAL ( released() ), this, SLOT ( showCreditLimitCalculator() ) );
    connect ( accounttype, SIGNAL ( activated(int) ), this, SLOT ( activateCreditLimit(int) ) );
    connect ( datebutton, SIGNAL ( released() ), this, SLOT ( showCalendar() ) );
    connect ( descriptionbutton, SIGNAL ( released() ), this, SLOT ( addAccountDescription() ) );
}

NewAccount::~NewAccount ()
  {
  }

void NewAccount::showChildPulldownMenu ()
  {
    if ( childcheckbox->isChecked() == TRUE )
      {
        childlabel->setEnabled ( TRUE );
        childbox->setEnabled ( TRUE );
      }
    else
      hideChildPulldownMenu();
  }

void NewAccount::hideChildPulldownMenu ()
  {
    childlabel->setEnabled ( FALSE );
    childbox->setEnabled ( FALSE );
  }

void NewAccount::showCalculator ()
  {
    Calculator *calculator = new Calculator ( this );
    calculator->setMaximumWidth ( ( int ) ( this->size().width() * 0.9 ) );
    if ( calculator->exec () == QDialog::Accepted )
      accountbalance->setText ( calculator->display->text() );
  }

void NewAccount::showCreditLimitCalculator ()
  {
    Calculator *calculator = new Calculator ( this );
    calculator->setMaximumWidth ( ( int ) ( this->size().width() * 0.9 ) );
    if ( calculator->exec () == QDialog::Accepted )
      creditlimit->setText ( calculator->display->text() );
  }

void NewAccount::activateCreditLimit ( int index )
  {
    if ( index == 2 || index == 5 )
      creditlimitbox->setEnabled ( TRUE );
    else
     {
       creditlimit->clear ();
       creditlimitbox->setEnabled ( FALSE );
     }
  }

void NewAccount::showCalendar ()
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
	// This tells the accountdisplay object that the user edited an account
	// and did change the date
	dateedited = TRUE;

	// Display date with our selected format
	startdate->setText ( preferences->getDate ( year, month, day ) );
      }
  }

bool NewAccount::getDateEdited ()
  {
    return dateedited;
  }

int NewAccount::getDay ()
  {
    return day;
  }

int NewAccount::getMonth ()
  {
    return month;
  }

int NewAccount::getYear ()
  {
    return year;
  }

QString NewAccount::getDescription ()
  {
    return accountdescription;
  }

void NewAccount::setDescription ( QString description )
  {
    accountdescription = description;
  }

void NewAccount::addAccountDescription ()
  {
    // Function for adding or editing an account description.
    QDialog *description = new QDialog ( this, "description", TRUE );
    description->setCaption ( "Notes" );
    QMultiLineEdit *enter = new QMultiLineEdit ( description );
    enter->setFixedSize ( ( int ) (this->width() * 0.75 ), ( int ) ( this->height() * 0.5 ) );
    enter->setWrapColumnOrWidth ( ( int ) (this->width() * 0.75 ) );
    enter->setWordWrap ( QMultiLineEdit::WidgetWidth );
    if ( accountdescription != "(NULL)" )
      enter->setText ( accountdescription );
    if ( description->exec () == QDialog::Accepted )
      accountdescription = enter->text ();
  }




