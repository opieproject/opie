/*
                             This file is part of the Opie Project
                             Copyright (C) 2010 Paul Eggleton <bluelightning@bluelightning.org>
              =.             Copyright (C) 2010 The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "opimautoconvert.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/opimaccessfactory.h>
#include <opie2/obackendfactory.h>

#include <qpe/config.h>
#include <qpe/qpeapplication.h>

/* QT */
#include <qmessagebox.h>
#include <qwidget.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qlayout.h>

using namespace Opie;
using namespace Opie::Pim;

OPimBase *OPimAutoConverter::createAccess( OPimGlobal::PimType type, OPimGlobal::DatabaseStyle dbtype )
{
    switch( type ) {
        case OPimGlobal::CONTACTLIST:
            {
                return OPimAccessFactory<OPimContactAccess>::create( type, dbtype, "converter" );
            }
            break;
        case OPimGlobal::TODOLIST:
            {
                return OPimAccessFactory<OPimTodoAccess>::create( type, dbtype, "converter" );
            }
            break;
        case OPimGlobal::DATEBOOK:
            {
                return OPimAccessFactory<ODateBookAccess>::create( type, dbtype, "converter" );
            }
            break;
        case OPimGlobal::NOTES:
            {
                return OPimAccessFactory<OPimMemoAccess>::create( type, dbtype, "converter" );
            }
            break;
        default:
            owarn << "Unknown database selected (" << type << ")" << oendl;
            return NULL;
    }

}


void OPimAutoConverter::promptConvertData( OPimGlobal::PimType type, QWidget *parent, const QString &appTitle )
{
    Config config("pimaccess");
    QString group_name;
    switch ( type ) {
        case OPimGlobal::TODOLIST:
            group_name = "todo";
            break;
        case OPimGlobal::CONTACTLIST:
            group_name = "contact";
            break;
        case OPimGlobal::DATEBOOK:
            group_name = "datebook";
            break;
        case OPimGlobal::NOTES:
            group_name = "notes";
            break;
        default:
            group_name = "unknown";
    }
    config.setGroup ( group_name );
    bool prompted = config.readBoolEntry( "convertprompted" );
    if( !prompted ) {
        OPimBase *sourceDB = createAccess( type, OPimGlobal::XML );
        OPimBase *destDB = createAccess( type, OPimGlobal::SQL );

        if( sourceDB->dataSourceExists() && !destDB->dataSourceExists() ) {
            int result = QMessageBox::information( parent, appTitle,
                                        "<p>" + QObject::tr( "Opie is now able to store your PIM data in a new database format, offering improved performance and functionality. Would you like to convert over to the new format now?"), QObject::tr("Yes"), QObject::tr("No") );
            if(result == 0) {
                // FIXME check results of these functions
                if( sourceDB->load() && destDB->load()) {
                    // Now transmit every pim-item from the source database to the destination -database
                    QArray<int> uidList = sourceDB->records();
                    odebug << "Try to move data for " << group_name << ".. (" << uidList.count() << " items) " << oendl;

                    // Set up progress dialog
                    QWidget *dlg = new QWidget( parent, 0, Qt::WType_TopLevel | Qt::WStyle_Dialog );
                    QVBoxLayout *vb = new QVBoxLayout( dlg );
                    QLabel *label = new QLabel(dlg);
                    label->setIndent(4);
                    label->setText( QObject::tr( "Converting data..." ) );
                    vb->addWidget(label);
                    QProgressBar *progress = new QProgressBar(dlg);
                    progress->setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred ) );
                    vb->addWidget(progress);

                    dlg->setCaption( appTitle );
                    progress->setTotalSteps( uidList.count() );
                    dlg->setGeometry( qApp->desktop()->width() / 8, (qApp->desktop()->height() / 2) - 25,
                                    3 * (qApp->desktop()->width() / 4), 50 );
                    dlg->show();

                    for ( uint i = 0; i < uidList.count(); ++i ) {
                        odebug << "Adding uid: " << uidList[i] << "" << oendl;
                        OPimRecord* rec = sourceDB->record( uidList[i] );
                        destDB->add( rec );

                        progress->setProgress( i+1 );
                        qApp->processEvents();
                    }

                    dlg->hide();
                    delete dlg;

                    // Now commit data..
                    if( destDB->save() ) {
                        QMessageBox::information(parent, appTitle,
                                            "<p>" + QObject::tr("Data converted successfully. If you are happy with the results you can delete the old data files (please see the documentation for details on how to do this)."));
                        config.writeEntry( "usebackend", "sql" );
                        config.writeEntry( "convertprompted", true );
                        // FIXME Send out message to all apps that db has changed?
                    }
                    else // FIXME make application quit here
                        QMessageBox::critical(parent, appTitle,
                                            "<p>" + QObject::tr("Data conversion failed. Please ensure you have enough disk space and try again."));
                }
                else // FIXME make application quit here
                    QMessageBox::critical(parent, appTitle,
                                        "<p>" + QObject::tr("Data conversion failed - unable to load source or destination data format."));
            }
            else {
                // User has said no, let them access their data in XML format still
                config.writeEntry( "convertprompted", true );
                config.writeEntry( "usebackend", "xml" );
            }
        }
        else
            config.writeEntry( "convertprompted", true );

        // Delete the frontends. Backends will be deleted automatically, too !
        // We have to cast them back to delete them properly !
        switch ( type ) {
            case OPimGlobal::CONTACTLIST:
                delete static_cast<OPimContactAccess*> (sourceDB);
                delete static_cast<OPimContactAccess*> (destDB);
                break;
            case OPimGlobal::TODOLIST:
                delete static_cast<OPimTodoAccess*> (sourceDB);
                delete static_cast<OPimTodoAccess*> (destDB);
                break;
            case OPimGlobal::DATEBOOK:
                delete static_cast<ODateBookAccess*> (sourceDB);
                delete static_cast<ODateBookAccess*> (destDB);
                break;
            case OPimGlobal::NOTES:
                delete static_cast<OPimMemoAccess*> (sourceDB);
                delete static_cast<OPimMemoAccess*> (destDB);
                break;
            default:
                owarn << "Unknown database selected (" << type << ")" << oendl;
        }
    }
}
