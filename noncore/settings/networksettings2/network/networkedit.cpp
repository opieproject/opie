#include <qtoolbutton.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <GUIUtils.h>
#include <resources.h>
#include "networkedit.h"

NetworkEdit::NetworkEdit( QWidget * Parent ) : NetworkGUI( Parent ){

    AddPreDown_TB->setPixmap( NSResources->getPixmap( "add" ) );
    AddPreUp_TB->setPixmap( NSResources->getPixmap( "add" ) );
    AddPostDown_TB->setPixmap( NSResources->getPixmap( "add" ) );
    AddPostUp_TB->setPixmap( NSResources->getPixmap( "add" ) );
    
    DeletePreDown_TB->setPixmap( NSResources->getPixmap( "remove" ) );
    DeletePreUp_TB->setPixmap( NSResources->getPixmap( "remove" ) );
    DeletePostDown_TB->setPixmap( NSResources->getPixmap( "remove" ) );
    DeletePostUp_TB->setPixmap( NSResources->getPixmap( "remove" ) );

    UpPreDown_TB->setPixmap( NSResources->getPixmap( "up" ) );
    UpPreUp_TB->setPixmap( NSResources->getPixmap( "up" ) );
    UpPostDown_TB->setPixmap( NSResources->getPixmap( "up" ) );
    UpPostUp_TB->setPixmap( NSResources->getPixmap( "up" ) );

    DownPreDown_TB->setPixmap( NSResources->getPixmap( "down" ) );
    DownPreUp_TB->setPixmap( NSResources->getPixmap( "down" ) );
    DownPostDown_TB->setPixmap( NSResources->getPixmap( "down" ) );
    DownPostUp_TB->setPixmap( NSResources->getPixmap( "down" ) );

}

QString NetworkEdit::acceptable( void ) {
    if( DHCP_CB->isChecked() ) {
      if( SendHostname_CB->isChecked() )
        if( Hostname_LE->text().isEmpty() ) 
          return tr("Hostname needed");
      return QString();
    }

    if( IPAddress_LE->text().isEmpty() )
      return tr("IPAddress needed");
    if( Broadcast_LE->text().isEmpty() )
      return tr("Broadcast needed");
    if( SubnetMask_LE->text().isEmpty() ) 
      return tr("Subnet mask needed");

    // valid IP ?
    if( ! validIP( IPAddress_LE->text() ) )
      return tr("IPAddress not valid");
    if( ! validIP( SubnetMask_LE->text() ) )
      return tr("Subnet mask not valid");
    if( ! validIP( Broadcast_LE->text() ) )
      return tr("Broadcast address not valid");
    if( Gateway_LE->text().isEmpty() &&
        ! validIP( Gateway_LE->text() ) )
      return tr("Gateway address not valid");
    if( DNS1_LE->text().isEmpty() &&
        ! validIP( DNS1_LE->text() ) )
      return tr("DNS1 address not valid");
    if( DNS2_LE->text().isEmpty() &&
        ! validIP( DNS2_LE->text() ) )
      return tr("DNS2 address not valid");
    return QString();
}

bool NetworkEdit::commit( NetworkData_t & Data ) {
    bool SM = 0;
    CBM( Data.UseDHCP, DHCP_CB, SM );
    TXTM( Data.IPAddress, IPAddress_LE, SM );
    CBM( Data.SendHostname, SendHostname_CB, SM );
    TXTM( Data.Hostname, Hostname_LE, SM );
    TXTM( Data.Gateway, Gateway_LE, SM );
    TXTM( Data.Broadcast, Broadcast_LE, SM );
    TXTM( Data.NetMask, SubnetMask_LE, SM );
    TXTM( Data.DNS1, DNS1_LE, SM );
    TXTM( Data.DNS2, DNS2_LE, SM );

    SM |= updateList( Data.PreUp_SL, PreUp_LB );
    SM |= updateList( Data.PostUp_SL, PostUp_LB );
    SM |= updateList( Data.PreDown_SL, PreDown_LB );
    SM |= updateList( Data.PostDown_SL, PostDown_LB );

    return SM;
}

void NetworkEdit::showData( NetworkData_t & Data ) {
    DHCP_CB->setChecked( Data.UseDHCP );
    SendHostname_CB->setChecked( Data.SendHostname );
    Hostname_LE->setText( Data.Hostname );
    IPAddress_LE->setText( Data.IPAddress );
    Gateway_LE->setText( Data.Gateway );
    SubnetMask_LE->setText( Data.NetMask );
    Broadcast_LE->setText( Data.Broadcast );
    DNS1_LE->setText( Data.DNS1 );
    DNS2_LE->setText( Data.DNS2 );

    populateList( Data.PreUp_SL, PreUp_LB );
    populateList( Data.PostUp_SL, PostUp_LB );
    populateList( Data.PreDown_SL, PreDown_LB );
    populateList( Data.PostDown_SL, PostDown_LB );
}

bool NetworkEdit::updateList( QStringList & SL, QListBox * LB ) {
    bool Changed;
    QStringList NewSL;

    // collect new list
    for( unsigned int i = 0; i < LB->count() ; i ++ ) {
      NewSL.append( LB->text(i) );
    }

    // check if at least ONE item in new list is NEW
    Changed = 0;
    for ( QStringList::Iterator it = NewSL.begin(); 
          it != NewSL.end(); 
          ++it ) {
      if( SL.findIndex( (*it) ) < 0 ) {
        // new or modified item
        Changed = 1;
        SL = NewSL;
        break;
      }
    }
    return Changed;
}

void NetworkEdit::populateList( QStringList & SL, QListBox * LB ) {
    LB->clear();
    for ( QStringList::Iterator it = SL.begin(); 
          it != SL.end(); 
          ++it ) {
      LB->insertItem( (*it) );
    }
}

void NetworkEdit::SLOT_NetmaskModified( const QString & ) {
    QString IP, SN;
    IP = IPAddress_LE->text();
    SN = SubnetMask_LE->text();
    if( IP.isEmpty() || SN.isEmpty() ) 
      return;

    if( ! validIP(IP) || ! validIP( SN ) )
      return;

    // if all ones
    // broadcast = (IPAddress | ~netmask )
    { QString NW;
      QStringList ipal = QStringList::split( '.', IP );
      QStringList nmal = QStringList::split( '.', SN );

      NW = QString( "%1.%2.%3.%4" ).
          arg( ipal[0].toShort() | ( ~ nmal[0].toShort() & 0x00ff) ).
          arg( ipal[1].toShort() | ( ~ nmal[1].toShort() & 0x00ff) ).
          arg( ipal[2].toShort() | ( ~ nmal[2].toShort() & 0x00ff) ).
          arg( ipal[3].toShort() | ( ~ nmal[3].toShort() & 0x00ff) );
      Broadcast_LE->setText( NW );
    }
}
