

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

    m_number = 0;

    setCaption( tr( "Enter number" ) );

    QVBoxLayout *mainLayout = new QVBoxLayout( this );

    QLabel *textLabel = new QLabel( this );
    textLabel->setText( tr("Enter the number you want to dial. When finished, press ok") );

    LCD = new QLCDNumber( this, "LCD" );
    QFont LCD_font(  LCD->font() );
    LCD_font.setPointSize( 7 );
    LCD->setFont( LCD_font );
    LCD->setNumDigits( 25 );
    LCD->setSegmentStyle( QLCDNumber::Flat );
    LCD->setMaximumHeight( 30 );

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
    mainLayout->addWidget( LCD );
    mainLayout->addStretch( 0 );
    mainLayout->addLayout( layout );
    mainLayout->addStretch( 0 );


}


void DialDialog::slotEnterNumber( int number ) {

    // pretty stupid, just for testing .-)

    m_number = ( m_number * 10 ) + number;
    qDebug( QString("%1").arg( m_number ) );
    LCD->display( m_number );

}

DialDialog::~DialDialog() {
}

QString DialDialog::number() {
    return QString( "%1").arg( m_number );

}

void DialDialog::setNumber( int number )
{
    m_number = number;
    LCD->display( m_number );
}

