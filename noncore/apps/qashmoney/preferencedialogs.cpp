#include "preferencedialogs.h"
#include "preferences.h"

extern Preferences *preferences;

DatePreferences::DatePreferences ( QWidget* parent ) : QDialog ( parent, 0, TRUE )
  {
    setCaption( tr( "Date" ) );

    QLabel *datelabel = new QLabel ( "Format", this );
    dateformat = new QComboBox ( this );
    dateformat->setEditable ( FALSE );
    dateformat->insertItem ( "yyyymmdd" );
    dateformat->insertItem ( "yymmdd" );
    dateformat->insertItem ( "mmddyyyy" );
    dateformat->insertItem ( "mmddyy" );
    dateformat->insertItem ( "yyyyddmm" );
    dateformat->insertItem ( "yyddmm" );
    dateformat->insertItem ( "ddmmyyyy" );
    dateformat->insertItem ( "ddmmyy" );
    connect ( dateformat, SIGNAL ( activated ( int ) ), this, SLOT ( changeDateFormat ( int ) ) );

    QLabel *dateseparatorlabel = new QLabel ( "Separator", this );
    dateseparator = new QComboBox ( this );
    dateseparator->insertItem ( "/" );
    dateseparator->insertItem ( "-" );
    dateseparator->insertItem ( "." );
    connect ( dateseparator, SIGNAL ( activated ( int ) ), this, SLOT ( changeDateSeparator ( int ) ) );

    defaults = new QPushButton ( QPixmap ( "/opt/QtPalmtop/pics/defaults.png" ), "Defaults", this );
    connect ( defaults, SIGNAL ( released () ), this, SLOT ( setDefaultDatePreferences () ) );

    dateformat->setCurrentItem ( ( preferences->getPreference ( 1 ) ) - 1 );
    dateseparator->setCurrentItem ( ( preferences->getPreference ( 2 ) ) - 1 );

    layout = new QVBoxLayout ( this, 2, 2 );
    layout->addWidget ( datelabel );
    layout->addWidget ( dateformat );
    layout->addWidget ( dateseparatorlabel );
    layout->addWidget ( dateseparator );
    layout->insertSpacing ( 4, 5 );
    layout->addWidget ( defaults );

  }

DatePreferences::~DatePreferences ()
  {
  }

void DatePreferences::changeDateFormat ( int index )
  {
    index ++;
    preferences->changePreference ( 1, index );
  }

void DatePreferences::changeDateSeparator ( int index )
  {
    index ++;
    preferences->changePreference ( 2, index );
  }

void DatePreferences::setDefaultDatePreferences ()
  {
    preferences->setDefaultDatePreferences ();
    dateformat->setCurrentItem ( ( preferences->getPreference ( 1 ) ) - 1 );
    dateseparator->setCurrentItem ( ( preferences->getPreference ( 2 ) ) - 1 );
  }

// START TRANSACTION PREFERENCES

TransactionPreferences::TransactionPreferences ( QWidget* parent ) : QDialog ( parent, 0, TRUE )
{
    setCaption( tr ( "Transaction" ) );

    showclearedtransactions = new QCheckBox ( this );
    showclearedtransactions->setText ( "Show Cleared Transactions" );

    limittransactionsbox = new QHBox ( this );
    limittransactionsbox->setSpacing ( 2 );
    limittransactionslabel = new QLabel ( "Show ", limittransactionsbox );
    limittransactions = new QComboBox ( limittransactionsbox );
    QLabel *limittransactionslabel2 = new QLabel ( "of cleared transactions. ", limittransactionsbox );
    limittransactions->insertItem ( "14 days" );
    limittransactions->insertItem ( "30 days" );
    limittransactions->insertItem ( "90 days" );
    limittransactions->insertItem ( "180 days" );
    limittransactions->insertItem ( "365 days" );
    limittransactions->insertItem ( "All" );
    limittransactions->setCurrentItem ( preferences->getPreference ( 7 ) );

    excludetransfers = new QCheckBox ( this );
    excludetransfers->setText ( "Include Transfers In Limit View" );

    if ( preferences->getPreference ( 3 ) == 1 )
      showclearedtransactions->setChecked ( TRUE );
    else
      showclearedtransactions->setChecked ( FALSE );

    if ( preferences->getPreference ( 6 ) == 1 )
      excludetransfers->setChecked ( TRUE );
    else
      excludetransfers->setChecked ( FALSE );

    defaults = new QPushButton ( QPixmap ( "/opt/QtPalmtop/pics/defaults.png" ), "Defaults", this );
    connect ( defaults, SIGNAL ( released () ), this, SLOT ( setDefaultTransactionPreferences () ) );

    layout = new QVBoxLayout ( this, 2, 2 );
    layout->addWidget ( showclearedtransactions );
    layout->addWidget ( limittransactionsbox );
    layout->addWidget ( excludetransfers );
    layout->insertSpacing ( 3, 5 );
    layout->addWidget ( defaults );

    connect ( showclearedtransactions, SIGNAL ( toggled ( bool ) ), this, SLOT ( changeShowClearedPreference ( bool ) ) );
    connect ( excludetransfers, SIGNAL ( toggled ( bool ) ), this, SLOT ( changeExcludeTranfersPreference ( bool ) ) );
    connect ( limittransactions, SIGNAL ( activated ( int ) ), this, SLOT ( changeLimitTransactionsPreference ( int ) ) );
}

TransactionPreferences::~TransactionPreferences ()
  {
  }

void TransactionPreferences::changeLimitTransactionsPreference ( int pref )
  {
    preferences->changePreference ( 7, pref );
  }

void TransactionPreferences::changeShowClearedPreference ( bool state )
  {
    if ( state == TRUE )
      preferences->changePreference ( 3, 1 );
    else
      preferences->changePreference ( 3, 0 );
  }

void TransactionPreferences::changeExcludeTranfersPreference ( bool state )
  {
    if ( state == TRUE )
      preferences->changePreference ( 6, 1 );
    else
      preferences->changePreference ( 6, 0 );
  }

void TransactionPreferences::setDefaultTransactionPreferences ()
  {
    preferences->changePreference ( 3, 0 );
    preferences->changePreference ( 6, 0 );
    preferences->changePreference ( 7, 0 );
    showclearedtransactions->setChecked ( FALSE );
    limittransactions->setCurrentItem ( 0 );
  }

// START ACCOUNT PREFERNCES

AccountPreferences::AccountPreferences ( QWidget* parent ) : QDialog ( parent, 0, TRUE )
{

    setCaption( tr ( "Account" ) );

    currencysupport = new QCheckBox ( this );
    currencysupport->setText ( "Enable Currency Support" );

    onetouch = new QCheckBox ( this );
    onetouch->setText ( "One Touch Account Viewing" );

    if ( preferences->getPreference ( 4 ) == 1 )
      currencysupport->setChecked ( TRUE );
    else
      currencysupport->setChecked ( FALSE );

    if ( preferences->getPreference ( 5 ) == 1 )
      onetouch->setChecked ( TRUE );
    else
      onetouch->setChecked ( FALSE );

    defaults = new QPushButton ( QPixmap ( "/opt/QtPalmtop/pics/defaults.png" ), "Defaults", this );
    connect ( defaults, SIGNAL ( released () ), this, SLOT ( setDefaultAccountPreferences () ) );

    layout = new QVBoxLayout ( this, 2, 2 );
    layout->addWidget ( currencysupport );
    layout->addWidget ( onetouch );
    layout->insertSpacing ( 2, 5 );
    layout->addWidget ( defaults );

    connect ( currencysupport, SIGNAL ( toggled ( bool ) ), this, SLOT ( changeCurrencySupport ( bool ) ) );
    connect ( onetouch, SIGNAL ( toggled ( bool ) ), this, SLOT ( changeOneTouchViewing ( bool ) ) );
}

AccountPreferences::~AccountPreferences ()
  {
  }

void AccountPreferences::changeCurrencySupport ( bool state )
  {
    if ( state == TRUE )
      preferences->changePreference ( 4, 1 );
    else
      preferences->changePreference ( 4, 0 );
  }

void AccountPreferences::changeOneTouchViewing ( bool state )
  {
    if ( state == TRUE )
      preferences->changePreference ( 5, 1 );
    else
      preferences->changePreference ( 5, 0 );
  }

void AccountPreferences::setDefaultAccountPreferences ()
  {
    preferences->changePreference ( 4, 0 );
    preferences->changePreference ( 5, 0 );
    currencysupport->setChecked ( FALSE );
    onetouch->setChecked ( FALSE );
  }

