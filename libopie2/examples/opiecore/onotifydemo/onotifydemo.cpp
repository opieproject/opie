/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oapplication.h>
#include <opie2/ofiledialog.h>
#include <opie2/olistview.h>
#include <opie2/ofilenotify.h>
using namespace Opie::Core;
using namespace Opie::Ui;

/* QT */
#include <qcheckbox.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qhbuttongroup.h>
#include <qvbuttongroup.h>
#include <qmessagebox.h>
#include <qpushbutton.h>

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
    l->addColumn( "Trigger Type" );
    l->addColumn( "Trigger Mask" );
    l->setColumnAlignment( 1, AlignCenter );
    l->setColumnAlignment( 2, AlignCenter );

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
    connect( g2, SIGNAL( pressed(int) ), this, SLOT( modifierClicked(int) ) );

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
        if ( !m )
        {
            QMessageBox::warning( 0, "Add Trigger", "<p>Can't add trigger without at least one selected trigger type</p>", "&Sorry", 0 );
            return;
        }

        QString filename = OFileDialog::getOpenFileName( OFileSelector::ExtendedAll );
        if ( !filename.isEmpty() )
        {
            odebug << "Filename = " << filename << oendl;

            int fntype = m;
            if ( multi ) fntype |=(int) Multi;

            QString modifier = QString().sprintf( " = 0x%08x", fntype );
            new OListViewItem( l, filename, multi ? "MULTI" : "SINGLE", modifier );
            if ( !multi )
                OFileNotification::singleShot( filename, this, SLOT( trigger() ), (OFileNotificationType) fntype );
            else
                OFileNotification::singleShot( filename, this, SLOT( trigger() ), (OFileNotificationType) fntype );
        }
        else
        {
            odebug << "cancelled." << oendl;
        }
    }

public slots:
    void modifierClicked( int modifier ) { (int)m ^= modifier; };
    void addSingle() { addTrigger(); };
    void addMulti() { addTrigger( true ); };

    void delTrigger()
    {
        QListViewItem* item = l->selectedItem();
        if ( !item )
        {
            QMessageBox::warning( 0, "Del Trigger", "<p>No trigger selected!</p>", "&Sorry", 0 );
            return;
        }
        else
        {
            QString filename( item->text( 0 ) );
            odebug << "Filename = " << filename << oendl;
        }
    }

    void trigger()
    {
        owarn << "FIRE!" << oendl;
    }

private:
  OListView* l;
  QButtonGroup* g1;
  QButtonGroup* g2;
  OFileNotificationType m;
};

int main( int argc, char** argv )
{
    DemoApp* app = new DemoApp( argc, argv );
    app->exec();

    return 0;

}

#include "moc/onotifydemo.moc"
