
#include "performancetest.h"
#include <qpe/qpeapplication.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qdatetime.h>
#include <qmultilineedit.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <stdlib.h>

class TestItem
{
public:
    TestItem( const QString &d, int it ) : msecs(0), desc(d), iter(it)
    {
    }

    void begin()
    {
	timeKeeper.start();
    }

    void end()
    {
	msecs = timeKeeper.elapsed();
    }

    QString report() const
    {
	QString rpt( "%1 (%2): %3msecs" );
	return rpt.arg(desc).arg(iter).arg(msecs);
    }

    int iterations() const { return iter; }
    int elapsed() const { return msecs; }

private:
    int msecs;
    QString desc;
    int iter;
    QTime timeKeeper;
};

void fillRect( QWidget *w, TestItem *ti )
{
    QPainter p( w );
    ti->begin();
    for ( int i = 0; i < ti->iterations(); i++ ) {
	p.fillRect( rand() % 40, rand() % 40, 200, 200, Qt::red );
    }
    ti->end();
}

void bitBlt( QWidget *w, TestItem *ti )
{
    QPixmap pm( 200, 200 );
    QPainter pmp( &pm );
    pmp.setBrush( Qt::white );
    pmp.drawRect( 0, 0, 200, 200 );
    pmp.drawLine( 0, 0, 200, 200 );
    pmp.drawLine( 0, 200, 200, 0 );
    pmp.end();

    QPainter p( w );
    ti->begin();
    for ( int i = 0; i < ti->iterations(); i++ ) {
	p.drawPixmap( rand() % 100, rand() % 100, pm );
    }
    ti->end();
}

void alphaBlt( QWidget *w, TestItem *ti )
{
    QImage img( 200, 200, 32 );
    img.setAlphaBuffer( TRUE );
    for ( int y = 0; y < img.height(); y++ ) {
	for ( int x = 0; x < img.width(); x++ ) {
	    QColor col;
	    col.setHsv( 360*x/img.width(), 255, 255 );
	    QRgb rgb = col.rgb();
	    rgb &= 0x00ffffff;
	    rgb |= (255*y/img.height()) << 24;
	    img.setPixel( x, y, rgb );
	}
    }

    QPixmap pm;
    pm.convertFromImage( img );
    QPainter p( w );
    ti->begin();
    for ( int i = 0; i < ti->iterations(); i++ ) {
	p.drawPixmap( rand() % 20, rand() % 20, pm );
    }
    ti->end();
}

void drawText( QWidget *w, TestItem *ti )
{
    QPainter p( w );
    p.setPen( Qt::white );
    QString text( "The quick red fox jumps over the lazy brown dog." ); // No tr
    ti->begin();
    for ( int i = 0; i < ti->iterations(); i++ ) {
	p.drawText( rand() % 100, rand() % 300, text );
    }
    ti->end();
}


struct {
    int id;
    const char *name;
    int iterations;
    void (*testFunc)(QWidget *, TestItem *);
}
perfTests[] =
{
    { 0, "Fill Rect", 1000, &fillRect }, // No tr
    { 1, "Bit Blt", 1000, &bitBlt }, // No tr
    { 2, "Alpha Blt", 100, &alphaBlt }, // No tr
    { 3, "Draw Text", 5000, &drawText }, // No tr
    {-1, "", 0, 0 }
};

PerformanceTest::PerformanceTest( QWidget *parent, const char *name, WFlags f )
    : PerformanceTestBase( parent, name, f )
{
    connect( testButton, SIGNAL(clicked()), this, SLOT(doTest()) );
    connect( optionGroup, SIGNAL(clicked(int)), this, SLOT(testClicked(int)) );
    QVBoxLayout *vb = new QVBoxLayout( testFrame );
    testWidget = new QWidget( testFrame );
    testWidget->setBackgroundColor( black );
    vb->addWidget( testWidget );

    int count = 0;
    while ( perfTests[count].id >= 0 ) {
	QCheckBox *cb = new QCheckBox( perfTests[count].name, optionGroup );
	cb->setChecked( TRUE );
	optionGroupLayout->addWidget( cb );
	count++;
    }
    enabledTests.resize( count );
    enabledTests.fill( TRUE );
}

void PerformanceTest::testClicked( int btn )
{
    enabledTests[btn] = optionGroup->find( btn )->isOn();
}

void PerformanceTest::doTest()
{
    testButton->setEnabled( FALSE );
    qApp->processEvents();
    int totalTime = 0;
    int i = 0;
    while ( perfTests[i].id >= 0 ) {
	if ( enabledTests[i] ) {
	    srand( 1 );
	    testButton->setText( perfTests[i].name );
	    qApp->processEvents();
	    TestItem ti( perfTests[i].name, perfTests[i].iterations );
	    (perfTests[i].testFunc)(testWidget, &ti);
	    resultsEdit->append( ti.report() );
	    totalTime += ti.elapsed();
	    testWidget->erase();
	}
	i++;
    }
    resultsEdit->append( QString("-> Total time: %1ms\n").arg(totalTime) ); // No tr
    testButton->setText( "Test" ); // No tr
    testButton->setEnabled( TRUE );
}
