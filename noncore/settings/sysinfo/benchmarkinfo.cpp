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

/* OPIE */
#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpedecoration_qws.h>
#include <qpe/resource.h>
#include <qpe/config.h>

/* QT */
#include <qlayout.h>
#include <qfiledialog.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qdirectpainter_qws.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qclipboard.h>
#include <qtimer.h>
#include <qcolor.h>
#include <qpushbutton.h>

/* STD */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "benchmarkinfo.h"

extern "C"
{
    void BenchFFT( void );
    double dhry_main( int );
}

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

    tests = new QListView( this );
    tests->setMargin( 1 );
    tests->addColumn( "Tests" );
    tests->addColumn( "Results" );
    tests->setShowSortIndicator( true );

    test_alu = new QCheckListItem( tests, "1: Integer Arithmetic  ", QCheckListItem::CheckBox );
    test_alu->setText( 1, "n/a" );
    test_fpu = new QCheckListItem( tests, "2: Floating Point Unit  ", QCheckListItem::CheckBox );
    test_fpu->setText( 1, "n/a" );
    test_txt = new QCheckListItem( tests, "3: Text Rendering  ", QCheckListItem::CheckBox );
    test_txt->setText( 1, "n/a" );
    test_gfx = new QCheckListItem( tests, "4: Gfx Rendering  ", QCheckListItem::CheckBox );
    test_gfx->setText( 1, "n/a" );
    test_ram = new QCheckListItem( tests, "5: RAM Performance  ", QCheckListItem::CheckBox );
    test_ram->setText( 1, "n/a" );
    test_sd = new QCheckListItem( tests, "6: SD Card Performance  ", QCheckListItem::CheckBox );
    test_sd->setText( 1, "n/a" );
    test_cf = new QCheckListItem( tests, "7: CF Card Performance  ", QCheckListItem::CheckBox );
    test_cf->setText( 1, "n/a" );

    startButton = new QPushButton( tr( "&Start Tests!" ), this );
    connect( startButton, SIGNAL( clicked() ), this, SLOT( run() ) );

    vb->addWidget( tests, 2 );
    vb->addWidget( startButton );
}


BenchmarkInfo::~BenchmarkInfo()
{}


void BenchmarkInfo::run()
{
    startButton->setText( "> Don't touch! Running Tests! Don't touch! <" );
    qApp->processEvents();
    QTime t;

    if ( test_alu->isOn() )
    {
        int d = round( dhry_main( DHRYSTONE_RUNS ) );
        test_alu->setText( 1, QString( "%1 DHRYS" ).arg( QString::number( d ) ) );
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
        t.start();
        paintChar();
        test_txt->setText( 1, QString( "%1 secs" ).arg( QString::number( t.elapsed() / 1000.0 ) ) );
        test_txt->setOn( false );
    }

    if ( test_gfx->isOn() )
    {
        t.start();
        paintLineRect();
        test_gfx->setText( 1, QString( "%1 secs" ).arg( QString::number( t.elapsed() / 1000.0 ) ) );
        test_gfx->setOn( false );
    }

    if ( test_ram->isOn() )
    {
        t.start();
        writeFile( "/tmp/benchmarkFile.dat" );    // /tmp is supposed to be in RAM on a PDA
        readFile( "/tmp/benchmarkFile.dat" );
        QFile::remove( "/tmp/benchmarkFile.dat" );
        test_ram->setText( 1, QString( "%1 secs" ).arg( QString::number( t.elapsed() / 1000.0 ) ) );
        test_ram->setOn( false );
    }
/*
    if ( test_cf->isOn() )
    {
        t.start();
        benchInteger();
        test_alu->setText( 1, QString( "%1 secs" ).arg( QString::number( t.elapsed() / 1000.0 ) ) );
        test_alu->setOn( false );
    }

    if ( test_sd->isOn() )
    {
        t.start();
        benchInteger();
        test_alu->setText( 1, QString( "%1 secs" ).arg( QString::number( t.elapsed() / 1000.0 ) ) );
        test_alu->setOn( false );
    }

    if ( ( which_clipb ) && ( buf.length() > 0 ) )
    {
        clb = QApplication::clipboard();
        clb->setText( dt_buf + buf );
    }
    */

    startButton->setText( tr( "&Start Tests!" ) );
}


void BenchmarkInfo::benchInteger() const
{
    long dummy = 1;

    for ( long i= 0 ; i < INT_TEST_ITERATIONS ; i++ )
    {
        for ( long j= 0 ; j < INT_TEST_ITERATIONS ; j++ )
        {
            for( long k= 0 ; k < INT_TEST_ITERATIONS ; k++ )
            {
                long xx = ( rand() % 1000 + 111 ) * 7 / 3 + 31;
                long yy = ( rand() % 100 + 23 ) * 11 / 7 + 17;
                long zz = ( rand() % 100 + 47 ) * 13 / 11 - 211;
                dummy = xx * yy / zz;
                dummy *= 23;
                dummy += ( xx - yy + zz );
                dummy -= ( xx + yy - zz );
                dummy *= ( xx * zz * yy );
                dummy /= 1 + ( xx * yy * zz );
            }
        }
    }
}


void BenchmarkInfo::paintChar()
{
    int rr[] = { 255, 255, 255, 0, 0, 0, 0, 128, 128 };
    int gg[] = { 0, 255, 0, 0, 255, 255, 0, 128, 128 };
    int bb[] = { 0, 0, 255, 0, 0, 255, 255, 128, 0 };
    const QString text( "Opie Benchmark Test" );

    int w = QApplication::desktop()->width();
    int h = QApplication::desktop()->height();

    srand( time( NULL ) );

    BenchmarkPaintWidget bpw;

    for ( int i = 0; i < CHAR_TEST_ITERATIONS; ++i )
    {
        int k = rand() % 9;
        bpw.p.setPen( QColor( rr[ k ], gg[ k ], bb[ k ] ) );
        bpw.p.setFont( QFont( "Vera", k*10 ) );
        bpw.p.drawText( rand() % w, rand() % h, text, text.length() );
    }
}

void BenchmarkInfo::paintLineRect()
{
    int rr[] = { 255, 255, 255, 0, 0, 0, 0, 128, 128 };
    int gg[] = { 0, 255, 0, 0, 255, 255, 0, 128, 128 };
    int bb[] = { 0, 0, 255, 0, 0, 255, 255, 128, 0 };

    int w = QApplication::desktop()->width();
    int h = QApplication::desktop()->height();

    srand( time( NULL ) );

    BenchmarkPaintWidget bpw;

    for ( int i = 0; i < DRAW_TEST_ITERATIONS*3; ++i )
    {
        int k = rand() % 9;
        bpw.p.setPen( QColor( rr[ k ], gg[ k ], bb[ k ] ) );
        bpw.p.drawLine( rand()%w, rand()%h, rand()%w, rand()%h );
    }

    for ( int i = 0; i < DRAW_TEST_ITERATIONS; ++i )
    {
        int k = rand() % 9;
        bpw.p.setPen( QColor( rr[ k ], gg[ k ], bb[ k ] ) );
        bpw.p.drawArc( rand()%w, rand()%h, rand()%w, rand()%h, 360 * 16, 360 * 16 );
    }

    QBrush br1;
    br1.setStyle( SolidPattern );

    for ( int i = 0; i < DRAW_TEST_ITERATIONS*2; ++i )
    {
        int k = rand() % 9;
        br1.setColor( QColor( rr[ k ], gg[ k ], bb[ k ] ) );
        bpw.p.fillRect( rand()%w, rand()%h, rand()%w, rand()%h, br1 );
    }

    QPixmap p = Resource::loadPixmap( "pattern" );
    for ( int i = 0; i < DRAW_TEST_ITERATIONS; ++i )
    {
        bpw.p.drawPixmap( rand()%w, rand()%h, p );
    }


}

// **********************************************************************
// Read & Write
// v2.0.0
// **********************************************************************
#define   BUFF_SIZE        8192
#define   FILE_SIZE        1024 * 1024  // 1Mb

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
    // ------------------------------------*/

    /* ----------------------------------
     srand( time( NULL ) );

     FILE *fp;

     for( n= 0 ; n < 40 ; n++ )
     {
      if (( fp = fopen( w_path, "wt" )) == NULL )
       return( false );
      memset( FileBuf, '\0', BUFF_SIZE+1 );
      // ------------------------------------------ sequential write
      for( i= 0 ; i < FILE_SIZE / BUFF_SIZE ; i++ )
            {
             for( k= 0 ; k < 128 ; k++ )
                {
        n = rand() % 30;
        memcpy( &FileBuf[k*8], &data[n], 32 );
                }
       fputs( FileBuf, fp );
            }
      // ------------------------------------------ random write
      for( i= 0 ; i < 300 ; i++ )
            {
       memset( FileBuf, '\0', 130 );
             len = rand() % 120 + 8;
             for( k= 0 ; k < 16 ; k++ )
                {
        n = rand() % 54;
        memcpy( &FileBuf[k*8], &data[n], 8 );
                }
       pos = rand() % ( FILE_SIZE / BUFF_SIZE - BUFF_SIZE );

       fseek( fp, pos, SEEK_SET );
       fputs( FileBuf, fp );
            }
            fclose( fp );
     }
     return( true );
    -------------------------------------*/
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
