

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
    textLabel->setTextFormat( QLabel::RichText );
    textLabel->setText( tr("Enter the number you want to dial. When finished, press ok") );

    m_dialLine = new QLineEdit( this );
    m_dialLine->setReadOnly( true );
    m_dialLine->setFrame( false );
    m_dialLine->setAlignment( Qt::AlignLeft );
    QFont dialLine_font( m_dialLine->font() );
    dialLine_font.setBold( TRUE );
    dialLine_font.setPointSize( 18 );
    m_dialLine->setFont( dialLine_font );

    QWidget* dialWidget = new QWidget( this );
    QGridLayout *layout = new QGridLayout( dialWidget , 4, 3 );

    QButtonGroup *dialButtons = new QButtonGroup( );

    QPushButton *number0 = new QPushButton( dialWidget );
    number0->setText( QString( "0" ) );
    QFont number0_font( number0->font() );
    number0_font.setBold( TRUE );
    number0->setFont( number0_font );
    layout->addWidget( number0, 4, 1 );
    dialButtons->insert( number0 );

    int x = 0, y = 0;
    for ( int i = 0 ; i < 9;  i++ ) {
        QPushButton *number = new QPushButton( dialWidget );
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

    mainLayout->addStretch( 2 );
    mainLayout->addWidget( textLabel );
    mainLayout->addStretch( 1 );
    mainLayout->addWidget( m_dialLine );
    mainLayout->addStretch( 2 );
    mainLayout->addWidget( dialWidget );
    mainLayout->addStretch( 4 );
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
    m_dialLine->setText( QString("%1").arg( number ) );
}

