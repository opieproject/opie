#include "datepicker.h"

DatePicker::DatePicker ( QDate entrydate ) : QDialog ( 0, 0, TRUE )
  {
    setCaption ( "Select Date" );
    date = entrydate;
    day = date.day();
    month = date.month();
    year = date.year();

    daylabel = new QLabel ( "Day", this );
    monthlabel = new QLabel ( "Month", this );
    yearlabel = new QLabel ( "Year", this );

    daybox = new QComboBox ( this, "daybox" );
    connect ( daybox, SIGNAL ( activated(int) ), this, SLOT ( setDay(int) ) );
    displayDays ( daybox );
    monthbox = new QComboBox ( this, "monthbox" );
    connect ( monthbox, SIGNAL ( activated(int) ), this, SLOT ( setMonth(int) ) );
    displayMonths ( monthbox );
    yearbox = new QComboBox ( this, "yearbox" );
    connect ( yearbox, SIGNAL ( activated(int) ), this, SLOT ( setYear(int) ) );
    displayYears ( yearbox );

    layout = new QGridLayout ( this, 2, 3, 5, 5, "datepickerlayout" );
    layout->addWidget ( daylabel, 0, 2 );
    layout->addWidget ( monthlabel, 0, 1 );
    layout->addWidget ( yearlabel, 0, 0 );
    layout->addWidget ( daybox, 1, 2 );
    layout->addWidget ( monthbox, 1, 1 );
    layout->addWidget ( yearbox, 1, 0 );
  }

void DatePicker::displayDays ( QComboBox *daybox )
  {
    int counter;
    int days = date.daysInMonth();
    for ( counter = 1; counter <= days; counter++ )
      daybox->insertItem ( QString::number ( counter ) );
    daybox->setCurrentItem ( ( date.day() ) - 1 );
  }

void DatePicker::displayMonths ( QComboBox *monthbox )
  {
    int counter;
    for ( counter = 1; counter <= 12; counter++ )
      monthbox->insertItem ( QString::number ( counter ) );
    monthbox->setCurrentItem ( ( date.month() ) - 1 );
  }

void DatePicker::displayYears ( QComboBox *yearbox )
  {
    int counter;
    int indexcounter = 0;
    int yearindex = 0;
    int year = date.year();
    for ( counter = ( year - 1 ); counter <= ( year + 1 ); counter++ )
      {
        yearbox->insertItem ( QString::number ( counter ) );
        if ( date.year() == counter )
          yearindex = indexcounter;
        indexcounter ++;
      }
    yearbox->setCurrentItem ( yearindex );
  }

void DatePicker::setDay ( int index )
  {
    day = daybox->text ( index ).toInt();
  }

void DatePicker::setMonth ( int index )
  {
    month = monthbox->text( index ).toInt();
  }

void DatePicker::setYear ( int index )
  {
    year = yearbox->text ( index ).toInt();
  }

int DatePicker::getDay ()
  { return day; }

int DatePicker::getMonth ()
  { return month; }

int DatePicker::getYear ()
  { return year; }






