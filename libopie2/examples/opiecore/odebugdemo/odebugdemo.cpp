/* QT */

#include <qvbox.h>
#include <qhbox.h>
#include <qvbuttongroup.h>
#include <qhbuttongroup.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qpushbutton.h>

/* OPIE */

#include <qpe/config.h>

#include <opie2/odebug.h>
#include <opie2/oapplication.h>
#include <opie2/oglobal.h>
#include <opie2/oglobalsettings.h>

class DemoApp : public OApplication
{
Q_OBJECT
public:
  DemoApp( int argc, char** argv ) : OApplication( argc, argv, "libopie2 debug demo" )
  {
    // you have access to your OApplication object via oApp
    qDebug( "Process-wide OApplication object @ %0x", oApp );

    // you have access to global settings via OGlobalSettings
    int mode = OGlobalSettings::debugMode();

    QVBox* vbox = new QVBox();
    setMainWidget( vbox );

    g = new QVButtonGroup( "Output Strategy", vbox );
    QRadioButton* r0 = new QRadioButton( "file", g );
    QRadioButton* r1 = new QRadioButton( "messagebox", g );
    QRadioButton* r2 = new QRadioButton( "stderr", g );
    QRadioButton* r3 = new QRadioButton( "syslog", g );
    QRadioButton* r4 = new QRadioButton( "socket", g );
    g->insert( r0, 0 );
    g->insert( r1, 1 );
    g->insert( r2, 2 );
    g->insert( r3, 3 );
    g->insert( r4, 4 );
    g->setRadioButtonExclusive( true );
    connect( g, SIGNAL( clicked(int) ), this, SLOT( chooseMethod(int) ) );

    if ( mode != -1 ) g->setButton( mode );

    QHButtonGroup* hbox = new QHButtonGroup( "Extra Output Information", vbox );
    e = new QLineEdit( hbox );
    QPushButton* pb = new QPushButton( hbox );

    connect( e, SIGNAL( returnPressed() ), this, SLOT( updateDebugOutput() ) );
    connect( pb, SIGNAL( clicked() ), this, SLOT( updateDebugOutput() ) );

    // show the additional debug mode dependent output information
    e->setText( OGlobalSettings::debugOutput() );

    // buttos
    QPushButton* info = new QPushButton( "Emit Debug(Info) Output!", vbox );
    connect( info, SIGNAL( clicked() ), this, SLOT( emitInfoOutput() ) );
    QPushButton* warn = new QPushButton( "Emit a Warning Output!", vbox );
    connect( warn, SIGNAL( clicked() ), this, SLOT( emitWarningOutput() ) );
    QPushButton* error = new QPushButton( "Emit an Error Output!", vbox );
    connect( error, SIGNAL( clicked() ), this, SLOT( emitErrorOutput() ) );
    QPushButton* fatal = new QPushButton( "Emit a Fatal Output!", vbox );
    connect( fatal, SIGNAL( clicked() ), this, SLOT( emitFatalOutput() ) );

    QPushButton* tb = new QPushButton( "Emit a Fatal Backtrace!", vbox );
    connect( tb, SIGNAL( clicked() ), this, SLOT( emitTBOutput() ) );

    info->show();
    warn->show();
    error->show();
    fatal->show();
    tb->show();
    g->show();
    hbox->show();
    e->show();
    vbox->show();
    showMainWidget( vbox );
  }

public slots:
  void chooseMethod(int method)
  {
    m = method;
    qDebug( "choosing method: %d", method );
    OConfig* g = OGlobal::config();
    g->setGroup( "General" );
    g->writeEntry( "debugMode", m );
    e->setText( OGlobalSettings::debugOutput() );
  }
  void updateDebugOutput()
  {
    OConfig* g = OGlobal::config();
    g->setGroup( "General" );
    g->writeEntry( "debugOutput"+QString::number(OGlobalSettings::debugMode()), e->text() );
  }
  void emitInfoOutput()
  {
    odebug << "This is a debug message" << oendl;
  }
  void emitWarningOutput()
  {
    owarn << "This is a warning message" << oendl;
  }
  void emitErrorOutput()
  {
    oerr << "This is an errror message" << oendl;
  }
  void emitFatalOutput()
  {
    ofatal << "This is a fatal message" << oendl;
  }
  void emitTBOutput()
  {
    ofatal << "This is a fatal message + backtrace\n" + odBacktrace(); // odBacktrace includes \n
  }

private:
  QButtonGroup* g;
  int m;
  QLineEdit* e;
};

int main( int argc, char** argv )
{
    DemoApp* app = new DemoApp( argc, argv );
    app->exec();

    return 0;

}

#include "moc/odebugdemo.moc"
