// (C) Michael 'Mickey' Lauer <mickey@Vanille.de>
// LICENSE = "GPLv2"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oapplication.h>
#include <opie2/ofilenotify.h>
using namespace Opie::Core;

/* QT */
#include <qpushbutton.h>
#include <qtextstream.h>

class App : public OApplication
{

public:
OFile* tmpfoo;

App( int argc, char** argv ) : OApplication( argc, argv, "libopie2 notify test" )
{
    odebug << "App()" << oendl;

    tmpfoo = new OFile( "/tmp/foo" );
    if ( tmpfoo->open( IO_ReadWrite ) )
    {
        QTextStream stream( tmpfoo );
        stream << "This is my content";
    }

    QObject::connect( tmpfoo, SIGNAL(accessed(const QString&)), this, SLOT(quit()) );
    QObject::connect( tmpfoo, SIGNAL(closed(const QString&,bool)), this, SLOT(quit()) );
}

~App()
{
    odebug << "~App()" << oendl;

    delete tmpfoo;
}

};

int main( int argc, char** argv )
{
    App* app = new App( argc, argv );
    QPushButton* b = new QPushButton( "Click me to close", 0 );
    QObject::connect( b, SIGNAL(clicked()), app, SLOT(quit()) );
    b->resize( 200, 200 );
    b->move( 150, 150 );
    b->show();
    app->setMainWidget( b );
    app->exec();
    delete app;

    return 0;

}

