// (C) Michael 'Mickey' Lauer <mickey@Vanille.de>
// LICENSE = "GPLv2"

#include "main.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oapplication.h>
#include <opie2/ofilenotify.h>
using namespace Opie::Core;

/* QT */
#include <qdir.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include <qmessagebox.h>

App::App( int argc, char** argv ) : QApplication( argc, argv )
{
    odebug << "App()" << oendl;
#if 0
    tmpfoo = new OFile( "/tmp/foo" );
    if ( tmpfoo->open( IO_ReadWrite ) )
    {
        QTextStream stream( tmpfoo );
        stream << "This is my content";
    }

    QObject::connect( tmpfoo, SIGNAL(accessed(const QString&)), this, SLOT(quit()) );
    QObject::connect( tmpfoo, SIGNAL(closed(const QString&,bool)), this, SLOT(quit()) );
#endif

    tmpfoo = new ODirNotification( 0, 0 );

    int result = tmpfoo->watch( "/tmp/foo", false, CreateFile, 2 );

    if ( result != -1 )
    {
        QObject::connect( tmpfoo, SIGNAL(triggered(const QString&,unsigned int,const QString&)),
                          this,   SLOT(triggered(const QString&,unsigned int,const QString&)) );
    }
    else
    {
        QMessageBox::warning( qApp->desktop(), "info", "Couldn't watch /tmp/foo\nDoes it exist?" );
    }
}

App::~App()
{
    odebug << "~App()" << oendl;
    delete tmpfoo;
}

void App::triggered( const QString& str1, unsigned int id, const QString& str2 )
{
    QMessageBox::information( qApp->desktop(), "info", QString().sprintf( "%s\n0x%08x\n%s",
    (const char*) str1, id, (const char*) str2 ) );
}

int main( int argc, char** argv )
{
    App* app = new App( argc, argv );
    QPushButton* b = new QPushButton( "Click me to close", 0 );
    QObject::connect( b, SIGNAL(clicked()), qApp, SLOT(quit()) );
    b->resize( 200, 200 );
    b->move( 150, 150 );
    b->show();
    app->setMainWidget( b );
    app->exec();
    delete app;

    return 0;

}

