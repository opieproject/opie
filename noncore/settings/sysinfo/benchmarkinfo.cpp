/**********************************************************************
** BenchmarkInfo
**
** A benchmark for Qt/Embedded
**
** Copyright (C) 2004 Michael Lauer <mickey@vanille.de>
** Inspired by ZBench (C) 2002 Satoshi <af230533@im07.alpha-net.ne.jp>
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "benchmarkinfo.h"

/* OPIE */
#include <opie2/ostorageinfo.h>
#include <opie2/olistview.h>
#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpedecoration_qws.h>
#include <qpe/resource.h>
#include <qpe/config.h>

/* QT */
#include <qclipboard.h>
#include <qcolor.h>
#include <qcombobox.h>
#include <qdirectpainter_qws.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qfiledialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qwhatsthis.h>

/* STD */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#if defined (__GNUC__) && (__GNUC__ < 3)
extern double round(double);
#endif

using namespace Opie::Ui;
using namespace Opie::Core;
extern "C"

{
    void BenchFFT( void );
    double dhry_main( int );
}

#define DHRYSTONE_RUNS   20000000
#define TEST_DURATION    3

#define BUFF_SIZE        8192
#define FILE_SIZE        1024 * 1024  // 1Mb

//===========================================================================

class BenchmarkPaintWidget : public QWidget
{
  public:
    BenchmarkPaintWidget() : QWidget( 0, "Benchmark Paint Widget", WStyle_Customize|WStyle_StaysOnTop|WPaintUnclipped|WPaintClever )
    {
        resize( QApplication::desktop()->size() );
        show();
        p.begin( this );
    };

    ~BenchmarkPaintWidget()
    {
        p.end();
        hide();
    };

    QPainter p;
};

//===========================================================================

BenchmarkInfo::BenchmarkInfo( QWidget *parent, const char *name, int wFlags )
        : QWidget( parent, name, wFlags )
{

    setMinimumSize( 200, 150 );

    QVBoxLayout* vb = new QVBoxLayout( this );
    vb->setSpacing( 4 );
    vb->setMargin( 4 );

    tests = new OListView( this );
    QWhatsThis::add( tests->viewport(), tr( "This area shows the available tests, the results for which the tests "
                                "have been performed, and comparison values for one selected device. "
                                "Use the checkboxes to define which tests you want to perform." ) );
    tests->setMargin( 0 );
    tests->addColumn( tr( "Tests" ) );
    tests->addColumn( tr( "Results" ) );
    tests->addColumn( tr( "Comparison" ) );
    tests->setShowSortIndicator( true );

    test_alu = new OCheckListItem( tests, tr( "1. Integer Arithmetic  " ), OCheckListItem::CheckBox );
    test_fpu = new OCheckListItem( tests, tr( "2. Floating Point Unit  " ), OCheckListItem::CheckBox );
    test_txt = new OCheckListItem( tests, tr( "3. Text Rendering  " ), OCheckListItem::CheckBox );
    test_gfx = new OCheckListItem( tests, tr( "4. Gfx Rendering  " ), OCheckListItem::CheckBox );
    test_ram = new OCheckListItem( tests, tr( "5. RAM Performance  " ), OCheckListItem::CheckBox );
    test_sd = new OCheckListItem( tests, tr( "6. SD Card Performance  " ), OCheckListItem::CheckBox );
    test_cf = new OCheckListItem( tests, tr( "7. CF Card Performance  " ), OCheckListItem::CheckBox );

    test_alu->setText( 1, "n/a" );
    test_fpu->setText( 1, "n/a" );
    test_txt->setText( 1, "n/a" );
    test_gfx->setText( 1, "n/a" );
    test_ram->setText( 1, "n/a" );
    test_sd->setText( 1, "n/a" );
    test_cf->setText( 1, "n/a" );

    test_alu->setText( 2, "n/a" );
    test_fpu->setText( 2, "n/a" );
    test_txt->setText( 2, "n/a" );
    test_gfx->setText( 2, "n/a" );
    test_ram->setText( 2, "n/a" );
    test_sd->setText( 2, "n/a" );
    test_cf->setText( 2, "n/a" );

    startButton = new QPushButton( tr( "&Start Tests!" ), this );
    QWhatsThis::add( startButton, tr( "Click here to perform the selected tests." ) );
    connect( startButton, SIGNAL( clicked() ), this, SLOT( run() ) );
    vb->addWidget( tests, 2 );

    QFile f( QPEApplication::qpeDir() + "/share/sysinfo/results" );
    if ( f.open( IO_ReadOnly ) )
    {
        machineCombo = new QComboBox( this );
        QWhatsThis::add( machineCombo, tr( "Choose a model to compare your results with." ) );

        QTextStream ts( &f );
        while( !ts.eof() )
        {
            QString machline = ts.readLine();
            qDebug( "sysinfo: parsing benchmark results for '%s'", (const char*) machline );
            QString resline = ts.readLine();
            machines.insert( machline, new QStringList( QStringList::split( ",", resline ) ) );
            machineCombo->insertItem( machline );
        }

        QHBoxLayout* hb = new QHBoxLayout( vb );
        hb->addWidget( new QLabel( tr( "Compare To:" ), this ) );
        hb->addWidget( machineCombo, 2 );
        connect( machineCombo, SIGNAL( activated(int) ), this, SLOT( machineActivated(int) ) );
    }

    vb->addWidget( startButton, 2 );
}


BenchmarkInfo::~BenchmarkInfo()
{}


void BenchmarkInfo::machineActivated( int index )
{
    QStringList* results = machines[ machineCombo->text( index ) ];
    if ( !results )
    {
        qDebug( "sysinfo: no results available." );
        return;
    }
    QStringList::Iterator it = results->begin();
    test_alu->setText( 2, *(it++) );
    test_fpu->setText( 2, *(it++) );
    test_txt->setText( 2, *(it++) );
    test_gfx->setText( 2, *(it++) );
    test_ram->setText( 2, *(it++) );
    test_sd->setText( 2, *(it++) );
    test_cf->setText( 2, *(it++) );
}


void BenchmarkInfo::run()
{
    startButton->setText( "> Don't touch! Running Tests! Don't touch! <" );
    qApp->processEvents();
    QTime t;

    if ( test_alu->isOn() )
    {
        int d = round( dhry_main( DHRYSTONE_RUNS ) );
        test_alu->setText( 1, QString( "%1 dhrys" ).arg( QString::number( d ) ) );
        test_alu->setOn( false );
    }

    if ( test_fpu->isOn() )
    {
        t.start();
        BenchFFT();
        test_fpu->setText( 1, QString( "%1 secs" ).arg( QString::number( t.elapsed() / 1000.0 ) ) );
        test_fpu->setOn( false );
    }

    if ( test_txt->isOn() )
    {
        int value = textRendering( TEST_DURATION );
        test_txt->setText( 1, QString( "%1 chars/sec" ).arg( QString::number( value / TEST_DURATION ) ) );
        test_txt->setOn( false );
    }

    if ( test_gfx->isOn() )
    {
        int value = gfxRendering( TEST_DURATION );
        test_gfx->setText( 1, QString( "%1 gops/sec" ).arg( QString::number( value / 4 / TEST_DURATION ) ) ); // 4 tests
        test_gfx->setOn( false );
    }

    if ( test_ram->isOn() )    // /tmp is supposed to be in RAM on a PDA
    {
        performFileTest( "/tmp/benchmarkFile.dat", test_ram );
    }

    if ( test_cf->isOn() )
    {
        OStorageInfo storage;
        performFileTest( storage.cfPath() + "/benchmarkFile.dat", test_cf );
    }

    if ( test_sd->isOn() )
    {
        OStorageInfo storage;
        performFileTest( storage.sdPath() + "/benchmarkFile.dat", test_sd );
    }

    startButton->setText( tr( "&Start Tests!" ) );
}


int BenchmarkInfo::textRendering( int seconds )
{
    QTime t;
    t.start();
    int stop = t.elapsed() + seconds * 1000;

    int rr[] = { 255, 255, 255, 0, 0, 0, 0, 128, 128 };
    int gg[] = { 0, 255, 0, 0, 255, 255, 0, 128, 128 };
    int bb[] = { 0, 0, 255, 0, 0, 255, 255, 128, 0 };
    const QString text( "Opie Benchmark Test" );

    int w = QApplication::desktop()->width();
    int h = QApplication::desktop()->height();

    srand( time( NULL ) );

    BenchmarkPaintWidget bpw;

    int loops = 0;

    while ( t.elapsed() < stop )
    {
        int k = rand() % 9;
        int s = rand() % 100;
        bpw.p.setPen( QColor( rr[ k ], gg[ k ], bb[ k ] ) );
        bpw.p.setFont( QFont( "Vera", s ) );
        bpw.p.drawText( rand() % w, rand() % h, text, text.length() );
        ++loops;
    }

    return loops * text.length();
}

int BenchmarkInfo::gfxRendering( int seconds )
{
    int rr[] = { 255, 255, 255, 0, 0, 0, 0, 128, 128 };
    int gg[] = { 0, 255, 0, 0, 255, 255, 0, 128, 128 };
    int bb[] = { 0, 0, 255, 0, 0, 255, 255, 128, 0 };

    int w = QApplication::desktop()->width();
    int h = QApplication::desktop()->height();

    srand( time( NULL ) );

    BenchmarkPaintWidget bpw;

    QTime t;
    t.start();
    int stop = t.elapsed() + seconds*1000;
    int loops = 0;

    while ( t.elapsed() < stop )
    {
        int k = rand() % 9;
        bpw.p.setPen( QColor( rr[ k ], gg[ k ], bb[ k ] ) );
        bpw.p.drawLine( rand()%w, rand()%h, rand()%w, rand()%h );
        ++loops;
    }

    t.restart();
    stop = t.elapsed() + seconds*1000;

    while ( t.elapsed() < stop )
    {
        int k = rand() % 9;
        bpw.p.setPen( QColor( rr[ k ], gg[ k ], bb[ k ] ) );
        bpw.p.drawArc( rand()%w, rand()%h, rand()%w, rand()%h, 360 * 16, 360 * 16 );
        ++loops;
    }

    QBrush br1;
    br1.setStyle( SolidPattern );
    t.restart();
    stop = t.elapsed() + seconds*1000;

    while ( t.elapsed() < stop )
    {
        int k = rand() % 9;
        br1.setColor( QColor( rr[ k ], gg[ k ], bb[ k ] ) );
        bpw.p.fillRect( rand()%w, rand()%h, rand()%w, rand()%h, br1 );
        ++loops;
    }

    QPixmap p = Resource::loadPixmap( "sysinfo/pattern" );
    t.restart();
    stop = t.elapsed() + seconds*1000;

    while ( t.elapsed() < stop )
    {
        bpw.p.drawPixmap( rand()%w, rand()%h, p );
        ++loops;
    }

    return loops;

}

void BenchmarkInfo::performFileTest( const QString& fname, OCheckListItem* item )
{
    QTime time;
    time.start();
    if ( writeFile( fname ) &&
            readFile( fname ) )
    {
        QFile::remove( fname );
        item->setText( 1, QString( "%1 kb/sec" ).arg( QString::number( 1024.0 / ( time.elapsed() / 1000.0 ) ) ) );
        item->setOn( false );
    }
    else
    {
        item->setText( 1, tr( "error" ) );
    }
}

char FileBuf[ BUFF_SIZE + 1 ];

bool BenchmarkInfo::writeFile( const QString& w_path )
{
    int i;
    int k;
    int n;
    int pos;
    int len;
    char *data = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"; // 62


    // /*------------------------------------
    int w_len;

    QFile writeFile( w_path );
    srand( time( NULL ) );

    for ( n = 0 ; n < 20 ; n++ )
    {
        if ( ! writeFile.open( IO_WriteOnly ) )
        {
            writeFile.close();
            writeFile.remove();
            return ( false );
        }
        // ------------------------------------------ sequential write
        for ( k = 0 ; k < 256 ; k++ )
        {
            n = rand() % 30;
            memcpy( &FileBuf[ k * 32 ], &data[ n ], 32 );
        }

        for ( i = 0 ; i < FILE_SIZE / BUFF_SIZE ; i++ )
        {
            w_len = writeFile.writeBlock( FileBuf, BUFF_SIZE );
            if ( w_len != BUFF_SIZE )
            {
                writeFile.close();
                writeFile.remove();
                return ( false );
            }
            writeFile.flush();
        }
        // ------------------------------------------ random write
        for ( i = 0 ; i < 400 ; i++ )
        {
            len = rand() % 90 + 4000;
            for ( k = 0 ; k < 128 ; k++ )
            {
                n = rand() % 30;
                memcpy( &FileBuf[ k * 8 ], &data[ n ], 32 );
            }
            pos = rand() % ( FILE_SIZE - BUFF_SIZE );

            writeFile.at( pos );
            w_len = writeFile.writeBlock( FileBuf, len );
            if ( w_len != len )
            {
                writeFile.close();
                writeFile.remove();
                return ( false );
            }
            writeFile.flush();
        }
        writeFile.close();
    }
    return ( true );

}


bool BenchmarkInfo::readFile( const QString& r_path )
{
    int i;
    int k;
    int len;
    int pos;
    int r_len;

    QFile readFile( r_path );
    srand( time( NULL ) );

    for ( k = 0 ; k < 200 ; k++ )
    {
        if ( ! readFile.open( IO_ReadOnly ) )
        {
            readFile.remove();
            return ( false );
        }
        // ------------------------------------------ sequential read
        readFile.at( 0 );
        for ( i = 0 ; i < FILE_SIZE / BUFF_SIZE ; i++ )
        {
            readFile.at( i * BUFF_SIZE );
            r_len = readFile.readBlock( FileBuf, BUFF_SIZE );
            if ( r_len != BUFF_SIZE )
            {
                readFile.close();
                readFile.remove();
                return ( false );
            }
        }
        // ------------------------------------------ random read
        for ( i = 0 ; i < 1000 ; i++ )
        {
            len = rand() % 120 + 8;
            pos = rand() % ( FILE_SIZE / BUFF_SIZE - BUFF_SIZE );
            readFile.at( pos );
            r_len = readFile.readBlock( FileBuf, len );
            if ( r_len != len )
            {
                readFile.close();
                readFile.remove();
                return ( false );
            }
        }
        readFile.close();
    }
    return ( true );
}
