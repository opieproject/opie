/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oapplication.h>
#include <opie2/ofiledialog.h>
#include <opie2/olistview.h>
#include <opie2/ofilenotify.h>
using namespace Opie::Core;
using namespace Opie::Ui;

/* QT */
#include <qvbox.h>
#include <qhbox.h>
#include <qhbuttongroup.h>
#include <qvbuttongroup.h>
#include <qpushbutton.h>
#include <qcheckbox.h>

class DemoApp : public OApplication
{
  Q_OBJECT
public:
  DemoApp( int argc, char** argv ) : OApplication( argc, argv, "libopie2 notify demo" )
  {

    QVBox* vbox = new QVBox();
    setMainWidget( vbox );

    l = new OListView( vbox );
    l->addColumn( "Notification Path" );
    l->addColumn( "Trigger" );

    QHBox* hbox = new QHBox( vbox );

    g2 = new QVButtonGroup( "Specify Trigger Type", hbox );
    //QCheckBox* c1 = new QCheckBox( "Multi", g2 );
    QCheckBox* c2 = new QCheckBox( "Access", g2 );
    QCheckBox* c3 = new QCheckBox( "Modify", g2 );
    QCheckBox* c4 = new QCheckBox( "Create", g2 );
    QCheckBox* c5 = new QCheckBox( "Delete", g2 );
    QCheckBox* c6 = new QCheckBox( "Rename", g2 );
    QCheckBox* c7 = new QCheckBox( "Attrib", g2 );
    g2->insert( c2, Access );
    g2->insert( c3, Modify );
    g2->insert( c4, Create );
    g2->insert( c5, Delete );
    g2->insert( c6, Rename );
    g2->insert( c7, Attrib );

    g1 = new QVButtonGroup( "Add/Remove", hbox );
    QPushButton* plus1 = new QPushButton( "Add\n&Single", g1 );
    QPushButton* plus2 = new QPushButton( "Add\n&Multi", g1 );
    QPushButton* minus = new QPushButton( "&Remove\nIt!", g1 );
    g1->insert( plus1, 0 );
    g1->insert( plus2, 1 );
    g1->insert( minus, 2 );
    connect( plus1, SIGNAL( clicked() ), this, SLOT( addSingle() ) );
    connect( plus2, SIGNAL( clicked() ), this, SLOT( addMulti() ) );
    connect( minus, SIGNAL( clicked() ), this, SLOT( delTrigger() ) );

    g1->show();
    g2->show();
    l->show();
    hbox->show();
    vbox->show();
    showMainWidget( vbox );
  }

public:
    void addTrigger( bool multi = false )
    {
        QString filename = OFileDialog::getOpenFileName( OFileSelector::ExtendedAll );
        if ( !filename.isEmpty() )
        {
            odebug << "Filename = " << filename << oendl;
            new OListViewItem( l, filename, "Modify" );



            OFileNotifier::singleShot( filename, this, SLOT( trigger() ) );
        }
        else
        {
            odebug << "cancelled." << oendl;
        }
    }

public slots:

    void addSingle() { addTrigger(); };
    void addMulti() { addTrigger( true ); };

    void delTrigger()
    {
        QString filename( "bla" );
        odebug << "Filename = " << filename << oendl;
    }

    void trigger()
    {
        owarn << "FIRE!" << oendl;
    }

private:
  OListView* l;
  QButtonGroup* g1;
  QButtonGroup* g2;
  int m;
};

int main( int argc, char** argv )
{
    DemoApp* app = new DemoApp( argc, argv );
    app->exec();

    return 0;

}

#include "moc/onotifydemo.moc"
