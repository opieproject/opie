#include "onotifydemo.h"

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
#include <qfileinfo.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qhbuttongroup.h>
#include <qvbuttongroup.h>
#include <qmessagebox.h>
#include <qpushbutton.h>

DemoApp::DemoApp( int argc, char** argv ) : OApplication( argc, argv, "libopie2 notify demo" )
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
    QCheckBox* c1 = new QCheckBox( "Access", g2 );
    QCheckBox* c2 = new QCheckBox( "Modify", g2 );
    QCheckBox* c3 = new QCheckBox( "Attrib", g2 );
    QCheckBox* c4 = new QCheckBox( "CloseWrite", g2 );
    QCheckBox* c5 = new QCheckBox( "CloseNoWrite", g2 );
    QCheckBox* c6 = new QCheckBox( "MovedFrom", g2 );
    QCheckBox* c7 = new QCheckBox( "MovedTo", g2 );
    QCheckBox* c8 = new QCheckBox( "DeleteSubdir", g2 );
    QCheckBox* c9 = new QCheckBox( "DeleteFile", g2 );
    QCheckBox* c10 = new QCheckBox( "CreateSubdir", g2 );
    QCheckBox* c11 = new QCheckBox( "CreateFile", g2 );
    QCheckBox* c12 = new QCheckBox( "Unmount", g2 );
    g2->insert( c1, Access );
    g2->insert( c2, Modify );
    g2->insert( c3, Attrib );
    g2->insert( c4, CloseWrite );
    g2->insert( c5, CloseNoWrite );
    g2->insert( c6, MovedFrom );
    g2->insert( c7, MovedTo );
    g2->insert( c8, DeleteSubdir );
    g2->insert( c9, DeleteFile );
    g2->insert( c10, CreateSubdir );
    g2->insert( c11, CreateFile );
    g2->insert( c12, Unmount );
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

    void DemoApp::addTrigger( bool multi )
    {
        if ( !m )
        {
            QMessageBox::warning( 0, "Add Trigger", "<p>Can't add trigger without at least one selected trigger type</p>", "&Sorry", 0 );
            return;
        }

        QString filename = OFileDialog::getOpenFileName( OFileSelector::ExtendedAll );
        if ( !filename.isEmpty() )
        {
            bool success = true;
            odebug << "Filename = " << filename << oendl;

            int fntype = m;
            QString modifier = QString().sprintf( " = 0x%08x", fntype );

            if ( QFileInfo( filename ).isFile() )
            {
                if ( !multi )
                {
                    success = OFileNotification::singleShot( filename, this, SLOT( unnamedTrigger() ), (OFileNotificationType) fntype );
                }
                else
                {
                    OFileNotification* fn = new OFileNotification();
                    success = fn->watch( filename, false, (OFileNotificationType) fntype );
                    connect( fn, SIGNAL( triggered( const QString& ) ), this, SLOT( namedTrigger( const QString& ) ) );
                }
            }
            else if ( QFileInfo( filename ).isDir() )
            {
                ODirNotification* dn = new ODirNotification();
                success = dn->watch( filename, !multi, (OFileNotificationType) fntype );
                connect( dn, SIGNAL( triggered( const QString& ) ), this, SLOT( namedTrigger( const QString& ) ) );
            }
            else
            {
                odebug << "Huh!? Neither file nor directory..." << oendl;
                return;
            }

/*            if ( !success )
            {
                QMessageBox::warning( 0, "Add Trigger", "<p>Couldn't add trigger :(</p>", "&Sorry", 0 );
                return;
            }
            else
*/            {
                new OListViewItem( l, filename, multi ? "MULTI" : "SINGLE", modifier );
            }
            return;
        }
        else
        {
            odebug << "cancelled." << oendl;
        }
    }

    void DemoApp::modifierClicked( int modifier ) { m = static_cast<OFileNotificationType>( (int)m ^ int(modifier) ); };
    void DemoApp::addSingle() { addTrigger(); };
    void DemoApp::addMulti() { addTrigger( true ); };

    void DemoApp::delTrigger()
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

    void DemoApp::unnamedTrigger()
    {
        owarn << "DemoApp::singleShotStrigger() : F I R E !!!!!" << oendl;
    }

    void DemoApp::namedTrigger( const QString& path )
    {
        owarn << "DemoApp::named trigger = " << path << " : F I R E !!!!!" << oendl;
    }

int main( int argc, char** argv )
{
    DemoApp* app = new DemoApp( argc, argv );
    app->exec();

    return 0;

}

