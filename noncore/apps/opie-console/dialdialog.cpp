

#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qscrollview.h>
#include <qpushbutton.h>
#include <qfont.h>
#include <qbuttongroup.h>

#include "dialdialog.h"



DialDialog::DialDialog(  QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ) {

    setCaption( tr( "Enter number" ) );

    QVBoxLayout *mainLayout = new QVBoxLayout( this );

    QLabel *textLabel = new QLabel( this );
    textLabel->setText( tr("Enter the number you want to dial. When finished, press ok") );

    LCD1 = new QLCDNumber( this, "LCD" );
    QFont LCD_font1(  LCD1->font() );
    LCD_font1.setPointSize( 7 );
    LCD1->setFont( LCD_font1 );
    LCD1->setNumDigits( 8 );
    LCD1->setSegmentStyle( QLCDNumber::Flat );
    LCD1->setMaximumHeight( 30 );

    LCD2 = new QLCDNumber( this, "LCD" );
    QFont LCD_font2(  LCD2->font() );
    LCD_font2.setPointSize( 7 );
    LCD2->setFont( LCD_font2 );
    LCD2->setNumDigits( 8 );
    LCD2->setSegmentStyle( QLCDNumber::Flat );
    LCD2->setMaximumHeight( 30 );

    QGridLayout *layout = new QGridLayout( this , 4, 3 );

    QButtonGroup *dialButtons = new QButtonGroup( );

    QPushButton *number0 = new QPushButton( this );
    number0->setText( QString( "0" ) );
    QFont number0_font( number0->font() );
    number0_font.setBold( TRUE );
    number0->setFont( number0_font );
    layout->addWidget( number0, 4, 1 );
    dialButtons->insert( number0 );

    int x = 0, y = 0;
    for ( int i = 0 ; i < 9;  i++ ) {
        QPushButton *number = new QPushButton( this );
        number->setText( QString( "%1" ).arg( i + 1 ) );
        QFont number_font( number->font() );
        number_font.setBold( TRUE );
        number->setFont( number_font );

        dialButtons->insert( number );

        layout->addWidget( number, x, y );

        if ( y < 2 ) {
            y++;
        } else {
            x++;
            y = 0;
        }
    }

    connect( dialButtons, SIGNAL( clicked( int ) ), this, SLOT( slotEnterNumber( int ) ) );

    mainLayout->addStretch( 0 );
    mainLayout->addWidget( textLabel );
	QHBoxLayout *lcdLayout = new QHBoxLayout(mainLayout);
    lcdLayout->addWidget( LCD1 );
    lcdLayout->addWidget( LCD2 );
    mainLayout->addStretch( 0 );
    mainLayout->addLayout( layout );
    mainLayout->addStretch( 0 );
}


void DialDialog::slotEnterNumber( int number ) {

    // pretty stupid, just for testing .-)

	m_number.append(QString("%1").arg(number));

	setNumber(m_number);
}

DialDialog::~DialDialog() {
}

QString DialDialog::number() {
    return m_number;

}

void DialDialog::setNumber( QString number )
{
	QString n1;
	if(number.length() > 8) n1 = number.left(number.length() - 8);
	QString n2 = number.right(8);
    LCD1->display( n1.toInt() );
	LCD2->display( n2.toInt() );
}

