#include "calculator.h"

#include <qpushbutton.h>

Calculator::Calculator ( QWidget* parent ) : QDialog ( parent, 0, TRUE )
  {

    display = new QLineEdit ( this, "display" );
    display->setFrame ( FALSE );
    display->setAlignment ( Qt::AlignRight );

    QPushButton *one = new QPushButton ( "1", this, "one" ); // make buttons for first row
    one->setFlat ( TRUE );
    connect ( one, SIGNAL ( released() ), this, SLOT ( displayOne() ) );

    QPushButton *two = new QPushButton ( "2", this, "two" );
    two->setFlat ( TRUE );
    connect ( two, SIGNAL ( released() ), this, SLOT ( displayTwo() ) );

    QPushButton *three = new QPushButton ( "3", this, "three" );
    three->setFlat ( TRUE );
    connect ( three, SIGNAL ( released() ), this, SLOT ( displayThree() ) );

    QPushButton *four = new QPushButton ( "4", this, "four" ); // make buttons for second row
    four->setFlat ( TRUE );
    connect ( four, SIGNAL ( released() ), this, SLOT ( displayFour() ) );

    QPushButton *five = new QPushButton ( "5", this, "five" );
    five->setFlat ( TRUE );
    connect ( five, SIGNAL ( released() ), this, SLOT ( displayFive() ) );

    QPushButton *six = new QPushButton ( "6", this, "six" );
    six->setFlat ( TRUE );
    connect ( six, SIGNAL ( released() ), this, SLOT ( displaySix() ) );

    QPushButton *seven = new QPushButton ( "7", this, "seven" ); // make buttons for third row
    seven->setFlat ( TRUE );
    connect ( seven, SIGNAL ( released() ), this, SLOT ( displaySeven() ) );

    QPushButton *eight = new QPushButton ( "8", this, "eight" );
    eight->setFlat ( TRUE );
    connect ( eight, SIGNAL ( released() ), this, SLOT ( displayEight() ) );

    QPushButton *nine = new QPushButton ( "9", this, "nine" );
    nine->setFlat ( TRUE );
    connect ( nine, SIGNAL ( released() ), this, SLOT ( displayNine() ) );

    QPushButton *zero = new QPushButton ( "0", this, "zero" );
    zero->setFlat ( TRUE );
    connect ( zero, SIGNAL ( released() ), this, SLOT ( displayZero() ) );

    QPushButton *dp = new QPushButton ( ".", this, "dp" );
    dp->setFlat ( TRUE );
    connect ( dp, SIGNAL ( released() ), this, SLOT ( displayPoint() ) );

    QPushButton *back = new QPushButton ( "<-", this, "back" );
    back->setFlat ( TRUE );
    connect ( back, SIGNAL ( released() ), this, SLOT ( back() ) );

    layout = new QGridLayout ( this, 5, 3, 5, 1, "calculatorlayout" );
    layout->addMultiCellWidget ( display, 0, 0, 0, 2 );
    layout->addWidget ( one, 1, 0 );
    layout->addWidget ( two, 1, 1 );
    layout->addWidget ( three, 1, 2 );
    layout->addWidget ( four, 2, 0 );
    layout->addWidget ( five, 2, 1 );
    layout->addWidget ( six, 2, 2 );
    layout->addWidget ( seven, 3, 0 );
    layout->addWidget ( eight, 3, 1 );
    layout->addWidget ( nine, 3, 2 );
    layout->addWidget ( zero, 4, 0 );
    layout->addWidget ( dp, 4, 1 );
    layout->addWidget ( back, 4, 2 );

  }

void Calculator::displayOne ()
  { display->insert ( "1" ); }

void Calculator::displayTwo ()
  { display->insert ( "2" ); }

void Calculator::displayThree ()
  { display->insert ( "3" ); }

void Calculator::displayFour ()
  { display->insert ( "4" ); }

void Calculator::displayFive ()
  { display->insert ( "5" ); }

void Calculator::displaySix ()
  { display->insert ( "6" ); }

void Calculator::displaySeven ()
  { display->insert ( "7" ); }

void Calculator::displayEight ()
  { display->insert ( "8" ); }

void Calculator::displayNine ()
  { display->insert ( "9" ); }

void Calculator::displayZero ()
  { display->insert ( "0" ); }

void Calculator::displayPoint ()
  { display->insert ( "." ); }

void Calculator::back ()
  { display->backspace(); }


