
#include "rfcommassigndialogimpl.h"
#include "rfcommassigndialogitem.h"
#include "rfcommconfhandler.h"

#include <qpe/config.h>

#include <qlayout.h>

using namespace OpieTooth;

// TODO: write only the file in bluebase?
// muss rfcommd dann neu gestartet werden
// sollte rfcomm bind all nicht eh default config sein ( polled das? - d.h. sobald nen gerät in der nähe ist bindet es?


RfcommAssignDialog::RfcommAssignDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : RfcommAssignDialogBase( parent, name, modal, fl ) {

    m_range = 5;

    m_scrollView = new QScrollView( this );
    m_scrollView->setResizePolicy( QScrollView::AutoOneFit );
    m_scrollView->setHScrollBarMode( QScrollView::AlwaysOff );

    RfcommAssignDialogBaseLayout->addWidget( m_scrollView );

    m_box = new QVBox( m_scrollView->viewport() );
    m_scrollView->addChild( m_box );

    confHandler = new RfCommConfHandler( "/etc/bluetooth/rfcomm.conf" );

    loadConfig();
}

RfcommAssignDialog::~RfcommAssignDialog()  {
    if ( confHandler )  {
        delete confHandler;
    }
}


void RfcommAssignDialog::newDevice( const QString & mac )  {

    for ( int i = 0 ; i < m_range; i++ )  {

        QMap<QString, RfCommConfObject*>::Iterator it;
        it = confHandler->foundEntries().find( QString("%1").arg( i ) );
        // make sure that rfcommX is not assigned yet
        if (  it == confHandler->foundEntries().end() )  {
            QDialog dialog( this,  "newdevice", true, WStyle_ContextHelp );
            dialog.showMaximized();
            RfcommDialogItem *newDev = new RfcommDialogItem( &dialog );
            newDev->setIdent( i );
            newDev->setMac( mac );

            if ( dialog.exec() == QDialog::Accepted )  {
                RfcommDialogItem *rfcomm = new RfcommDialogItem( m_box );
                m_itemList.insert( i , rfcomm );
                rfcomm->setIdent( i );
                rfcomm->setMac( mac );
                rfcomm->setChannel( newDev->channel() );
                rfcomm->setComment( newDev->comment() );
                qDebug( "New device set up" );
            }
        }
    }
}

void RfcommAssignDialog::loadConfig()  {

    //Config cfg( "bluetoothmanager-rfcommbind" );

    for ( int i = 0 ; i < m_range; i++ )  {
        // cfg.setGroup( QString("%1").arg( i ) );
        RfcommDialogItem *rfcomm = new RfcommDialogItem( m_box );
        m_itemList.insert( i , rfcomm );
        rfcomm->setIdent( i );
        QMap<QString, RfCommConfObject*>::Iterator it;
        it = confHandler->foundEntries().find( QString("%1").arg( i ) );
        if ( it != confHandler->foundEntries().end() )  {
            qDebug( "Found key in foundEntries() " );
            rfcomm->setMac( it.data()->mac() );
            rfcomm->setChannel( it.data()->channel() );
            rfcomm->setComment( it.data()->comment() );
        }
          /* Use rfcomm.conf directly for now
           * rfcomm->setMac( cfg.readEntry( "mac",  "" ) );
           * rfcomm->setChannel( cfg.readNumEntry( "channel", 1 ) );
           * rfcomm->setComment( cfg.readEntry( "comment", "" ) );
           */
    }
}


void RfcommAssignDialog::saveConfig()  {

    //Config cfg( "bluetoothmanager-rfcommbind" );

    QMap< int, RfcommDialogItem*>::Iterator it;

    QMap< QString, RfCommConfObject*> outMap;

    for( it = m_itemList.begin(); it != m_itemList.end(); ++it )  {

        //cfg.setGroup( QString("%1").arg( it.key() ) );
        RfcommDialogItem *rfcomm = it.data();


        outMap.insert( QString( "%1").arg( it.key() ), new RfCommConfObject( it.key(), rfcomm->mac(), rfcomm->channel(),  rfcomm->comment() ) );


        //cfg.writeEntry( "mac",  rfcomm->mac() );
        //cfg.writeEntry( "channel", rfcomm->channel() );
        //cfg.writeEntry( "comment", rfcomm->comment() );
    }

    confHandler->save( outMap );

}
