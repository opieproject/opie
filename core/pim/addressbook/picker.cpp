#include "picker.h"

#include <qfont.h>
#include <qstring.h>
#include <qtimer.h>
#include <qlayout.h>

char PickerLabel::lastLetter = '\0';

PickerLabel::PickerLabel( QWidget *parent, const char *name ) 
	: QLabel ( parent, name )
{
	currentLetter = 0;
	//lastLetter = 0;

	letter1 = '\0';
	letter2 = '\0';
	letter3 = '\0';

	setFont( QFont( "smallsmooth", 9 ) );
	setTextFormat( Qt::RichText );

}

PickerLabel::~PickerLabel()
{

}

void PickerLabel::setLetters( char ch1, char ch2, char ch3 )
{
	QString tmpStr;

	if (ch1 != '\0') 
		letter1 = ch1;
	else
		letter1 = ' ';

	if (ch2 != '\0') 
		letter2 = ch2;
	else
		letter2 = ' ';

	if (ch3 != '\0') 
		letter3 = ch3;
	else
		letter3 = ' ';

	tmpStr = "<qt>";
	tmpStr += letter1;
	tmpStr += letter2;
	tmpStr += letter3;
	tmpStr += "</qt>";

	setText(tmpStr);

	currentLetter = 0;

}

void PickerLabel::clearLetter()
{

	QString tmpStr;
	
	tmpStr = "<qt>";
	tmpStr += letter1;
	tmpStr += letter2;
	tmpStr += letter3;
	tmpStr += "</qt>";

	setText(tmpStr);

	currentLetter = 0;

}

void PickerLabel::mousePressEvent( QMouseEvent* e )
{
	// If one pickerlabel is was, and an other is now selected, we 
	// have to simulate the releaseevent.. Otherwise the new label
	// will not get a highlighted  letter.. 
	// Maybe there is a more intelligent solution, but this works and I am tired.. (se)
	if ( ( currentLetter == 0 ) && ( lastLetter != '\0' ) ) mouseReleaseEvent( e );
} 

void PickerLabel::mouseReleaseEvent( QMouseEvent* /* e */ )
{
	QString tmpStr;

	if (lastLetter != letter1 && lastLetter != letter2 && lastLetter != letter3 && lastLetter != '\0')
		QTimer::singleShot( 0, this, SLOT(emitClearSignal()) );
	
	switch (currentLetter) {
		case 0:
			tmpStr = "<qt><u><font color=\"#7F0000\">";
			tmpStr += letter1;
			tmpStr += "</font></u>";
			tmpStr += letter2;
			tmpStr += letter3;
			tmpStr += "</qt>";

			setText(tmpStr);

			currentLetter++;
			lastLetter = letter1;
			emit selectedLetter( letter1 );
			break;

		case 1:
			tmpStr = "<qt>";
			tmpStr += letter1;
			tmpStr += "<u><font color=\"#7F0000\">";
			tmpStr += letter2;
			tmpStr += "</font></u>";
			tmpStr += letter3;
			tmpStr += "</qt>";

			setText(tmpStr);

			currentLetter++;
			lastLetter = letter2;
			emit selectedLetter( letter2 );
			break;
			
		case 2:
			tmpStr = "<qt>";
			tmpStr += letter1;
			tmpStr += letter2;
			tmpStr += "<u><font color=\"#7F0000\">";
			tmpStr += letter3;
			tmpStr += "</font></u></qt>";

			setText(tmpStr);

			currentLetter++;
			lastLetter = letter3;
			emit selectedLetter( letter3 );
			break;

		default:
			clearLetter();
			lastLetter = '\0';
			emit selectedLetter( '\0' );


	}
}

void PickerLabel::emitClearSignal() {
	emit clearAll();
}
	
LetterPicker::LetterPicker( QWidget *parent, const char *name )
	: QFrame( parent, name )
{
	QHBoxLayout *l = new QHBoxLayout(this);

	lblABC = new PickerLabel( this );
	l->addWidget( lblABC );
	
	lblDEF = new PickerLabel( this );
	l->addWidget( lblDEF );

	lblGHI = new PickerLabel( this );
	l->addWidget( lblGHI );

	lblJKL = new PickerLabel( this );
	l->addWidget( lblJKL );

	lblMNO = new PickerLabel( this );
	l->addWidget( lblMNO );

	lblPQR = new PickerLabel( this );
	l->addWidget( lblPQR );

	lblSTU = new PickerLabel( this );
	l->addWidget( lblSTU );

	lblVWX = new PickerLabel( this );
	l->addWidget( lblVWX );

	lblYZ = new PickerLabel( this );
	l->addWidget( lblYZ );

	lblABC->setLetters( 'A', 'B', 'C' );
	lblDEF->setLetters( 'D', 'E', 'F' );
	lblGHI->setLetters( 'G', 'H', 'I' );
	lblJKL->setLetters( 'J', 'K', 'L' );
	lblMNO->setLetters( 'M', 'N', 'O' );
	lblPQR->setLetters( 'P', 'Q', 'R' );
	lblSTU->setLetters( 'S', 'T', 'U' );
	lblVWX->setLetters( 'V', 'W', 'X' );
	lblYZ->setLetters( 'Y', 'Z', '#' );

	connect(lblABC, SIGNAL(selectedLetter(char)), this, SLOT(newLetter(char)));
	connect(lblDEF, SIGNAL(selectedLetter(char)), this, SLOT(newLetter(char)));
	connect(lblGHI, SIGNAL(selectedLetter(char)), this, SLOT(newLetter(char)));
	connect(lblJKL, SIGNAL(selectedLetter(char)), this, SLOT(newLetter(char)));
	connect(lblMNO, SIGNAL(selectedLetter(char)), this, SLOT(newLetter(char)));
	connect(lblPQR, SIGNAL(selectedLetter(char)), this, SLOT(newLetter(char)));
	connect(lblSTU, SIGNAL(selectedLetter(char)), this, SLOT(newLetter(char)));
	connect(lblVWX, SIGNAL(selectedLetter(char)), this, SLOT(newLetter(char)));
	connect(lblYZ, SIGNAL(selectedLetter(char)), this, SLOT(newLetter(char)));
	connect(lblABC, SIGNAL(clearAll()), this, SLOT(clear()));
	connect(lblDEF, SIGNAL(clearAll()), this, SLOT(clear()));
	connect(lblGHI, SIGNAL(clearAll()), this, SLOT(clear()));
	connect(lblJKL, SIGNAL(clearAll()), this, SLOT(clear()));
	connect(lblMNO, SIGNAL(clearAll()), this, SLOT(clear()));
	connect(lblPQR, SIGNAL(clearAll()), this, SLOT(clear()));
	connect(lblSTU, SIGNAL(clearAll()), this, SLOT(clear()));
	connect(lblVWX, SIGNAL(clearAll()), this, SLOT(clear()));
	connect(lblYZ, SIGNAL(clearAll()), this, SLOT(clear()));

}

LetterPicker::~LetterPicker()
{
}

void LetterPicker::clear()
{
	lblABC->clearLetter();
	lblDEF->clearLetter();
	lblGHI->clearLetter();
	lblJKL->clearLetter();
	lblMNO->clearLetter();
	lblPQR->clearLetter();
	lblSTU->clearLetter();
	lblVWX->clearLetter();
	lblYZ->clearLetter();
}

void LetterPicker::newLetter( char letter )
{
	qWarning("LetterClicked");
	emit letterClicked( letter );

}
