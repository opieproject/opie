#include "converter.h"

/* OPIE */
#include <qpe/qpeapplication.h>

#include <opie2/odebug.h>
#include <opie2/opimglobal.h>
// Include SQL related header files
#define __USE_SQL
#include <opie2/opimaccessfactory.h>

/* QT */
#include <qdatetime.h>
#include <qprogressbar.h>
#include <qcombobox.h>
#include <qcheckbox.h>


using namespace Opie;
using namespace Pim;

Converter::Converter():
    m_selectedDatabase( ADDRESSBOOK ),
    m_selectedSourceFormat( XML ),
    m_selectedDestFormat( SQL )
{
    m_dataBaseSelector -> setCurrentItem( m_selectedDatabase );
    m_sourceFormatSelector -> setCurrentItem( m_selectedSourceFormat );
    m_destFormatSelector -> setCurrentItem( m_selectedDestFormat );
    m_eraseDB -> setChecked( true );  // Default erase on copy
}

void Converter::selectedDatabase( int num )
{
    m_selectedDatabase = num;
}

void Converter::selectedDestFormat( int num )
{
    m_selectedDestFormat = num;
}

void Converter::selectedSourceFormat( int num )
{
    m_selectedSourceFormat = num;
}

void Converter::start_conversion(){

    // Creating backends to the requested databases..
    OPimBase* sourceDB;
    OPimBase* destDB;

    switch( m_selectedSourceFormat ){
    case XML:
        odebug << "XMLSourceDB = " << m_selectedDatabase << "" << oendl;
        switch( m_selectedDatabase ){
        case ADDRESSBOOK:{
            sourceDB = OPimAccessFactory<OPimContactAccess>::create( OPimGlobal::CONTACTLIST, OPimGlobal::XML, "converter" );
                }
            break;
        case TODOLIST:{
            sourceDB = OPimAccessFactory<OPimTodoAccess>::create( OPimGlobal::TODOLIST, OPimGlobal::XML, "converter" );
            }break;
        case DATEBOOK:{
            sourceDB = OPimAccessFactory<ODateBookAccess>::create( OPimGlobal::DATEBOOK, OPimGlobal::XML, "converter" );
                }
            break;
        default:
            owarn << "Unknown database selected (" << m_selectedDatabase << ")" << oendl;
            return;
        }
        break;
    case SQL:
        odebug << "SQLSourceDB = " << m_selectedDatabase << "" << oendl;
        switch( m_selectedDatabase ){
        case ADDRESSBOOK:{
            sourceDB = OPimAccessFactory<OPimContactAccess>::create( OPimGlobal::CONTACTLIST, OPimGlobal::SQL, "converter" );
                }
            break;
        case TODOLIST:{
            sourceDB = OPimAccessFactory<OPimTodoAccess>::create( OPimGlobal::TODOLIST, OPimGlobal::SQL, "converter" );
            }break;
        case DATEBOOK:{
            sourceDB = OPimAccessFactory<ODateBookAccess>::create( OPimGlobal::DATEBOOK, OPimGlobal::SQL, "converter" );
                }
            break;
        default:
            owarn << "Unknown database selected (" << m_selectedDatabase << ")" << oendl;
            return;
        }
        break;
    default:
        owarn << "Unknown source format selected (" << m_selectedSourceFormat << ") !!" << oendl;
        return;
    }

    switch ( m_selectedDestFormat ){
    case XML:
        odebug << "XMLDestDB = " << m_selectedDatabase << "" << oendl;
        switch( m_selectedDatabase ){
        case ADDRESSBOOK:{
            destDB = OPimAccessFactory<OPimContactAccess>::create( OPimGlobal::CONTACTLIST, OPimGlobal::XML, "converter" );
                }
            break;
        case TODOLIST:{
            destDB = OPimAccessFactory<OPimTodoAccess>::create( OPimGlobal::TODOLIST, OPimGlobal::XML, "converter" );
            }break;
        case DATEBOOK:{
            destDB = OPimAccessFactory<ODateBookAccess>::create( OPimGlobal::DATEBOOK, OPimGlobal::XML, "converter" );
                }
            break;
        default:
            owarn << "Unknown database selected (" << m_selectedDatabase << ")" << oendl;
            return;
        }
        break;
    case SQL:
        odebug << "SQLDestDB = " << m_selectedDatabase << "" << oendl;
        switch( m_selectedDatabase ){
        case ADDRESSBOOK:{
            destDB = OPimAccessFactory<OPimContactAccess>::create( OPimGlobal::CONTACTLIST, OPimGlobal::SQL, "converter" );
                }
            break;
        case TODOLIST:{
            destDB = OPimAccessFactory<OPimTodoAccess>::create( OPimGlobal::TODOLIST, OPimGlobal::SQL, "converter" );
            }break;
        case DATEBOOK:{
            destDB = OPimAccessFactory<ODateBookAccess>::create( OPimGlobal::DATEBOOK, OPimGlobal::SQL, "converter" );
                }
            break;
        default:
            owarn << "Unknown database selected (" << m_selectedDatabase << ")" << oendl;
            return;
        }
        break;
    default:
        owarn << "Unknown destination format selected (" << m_selectedDestFormat << ")!!" << oendl;
        return;
    }

    if ( !sourceDB || !destDB )
        return;

    sourceDB -> load();
    destDB -> load();

    QTime t;
    t.start();

    // Clean the dest-database if requested (isChecked)
    if ( m_eraseDB -> isChecked() ){
        odebug << "Clearing destination database!" << oendl;
        destDB -> clear();
    }

    // Now transmit every pim-item from the source database to the destination -database
    QArray<int> uidList = sourceDB->records();
    odebug << "Try to move data for addressbook.. (" << uidList.count() << " items) " << oendl;
    m_progressBar->setTotalSteps( uidList.count() );
    int count = 0;
    for ( uint i = 0; i < uidList.count(); ++i ){
        odebug << "Adding uid: " << uidList[i] << "" << oendl;
        OPimRecord* rec = sourceDB -> record( uidList[i] );
        destDB -> add( rec );
        m_progressBar->setProgress( ++count );
    }

    // Now commit data..
    destDB -> save();

    // Delete the frontends. Backends will be deleted automatically, too !
    // We have to cast them back to delete them properly !
    switch( m_selectedDatabase ){
    case ADDRESSBOOK:
        delete static_cast<OPimContactAccess*> (sourceDB);
        delete static_cast<OPimContactAccess*> (destDB);
        break;
    case TODOLIST:
        delete static_cast<OPimTodoAccess*> (sourceDB);
        delete static_cast<OPimTodoAccess*> (destDB);
        break;
    case DATEBOOK:
        delete static_cast<ODateBookAccess*> (sourceDB);
        delete static_cast<ODateBookAccess*> (destDB);
        break;
    default:
        owarn << "Unknown database selected (" << m_selectedDatabase << ")" << oendl;
        return;
    }


    owarn << "Conversion is finished and needed " << t.elapsed() << " ms !" << oendl;
}

int main( int argc, char** argv ) {

    QPEApplication a( argc, argv );

    Converter dlg;

    a.showMainWidget( &dlg );
    // dlg. showMaximized ( );

    return a.exec();

}
