#include <qtoolbutton.h>
#include <qmessagebox.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <resources.h>
#include <GUIUtils.h>
#include "PPPDNSedit.h"

PPPDNSEdit::PPPDNSEdit( QWidget * Parent ) : PPPDNSGUI( Parent ){

    Add_TB->setPixmap( NSResources->getPixmap( "add" ) );
    Remove_TB->setPixmap( NSResources->getPixmap( "remove" ) );
}

QString PPPDNSEdit::acceptable( void ) {
    if( DNSFixed_RB->isChecked() ) {
      if( DomainName_LE->text().isEmpty() )
        tr("Domainname needed");
      if( Servers_LB->count() == 0 )
        tr("DNS Servers needed");
    }
    return QString();
}

bool PPPDNSEdit::commit( PPPData_t & D ) {
    bool SM = 0;

    CBM( D.DNS.ServerAssigned, DNSServerAssigned_RB, SM );
    TXTM( D.DNS.DomainName, DomainName_LE, SM );

    // new items in old ?
    for ( unsigned i = 0; i < Servers_LB->count(); i++ ) {
      QString S;
      bool Found;
      Found = 0;
      S = Servers_LB->text(i);
      for ( unsigned j = 0; j < D.DNS.Servers.size(); j++ ) {
        if( *(D.DNS.Servers[i]) == S ) {
          // still in list
          Found = 1;
          break;
        }
      }
      if( ! Found ) {
        SM = 1;
        D.DNS.Servers.resize( D.DNS.Servers.size()+1 );
        D.DNS.Servers[D.DNS.Servers.size()] = new QString( S );
      }
    }

    // old in new ?
    for ( unsigned j = 0; j < D.DNS.Servers.size(); j++ ) {
      bool Found;
      Found = 0;
      for ( unsigned i = 0; i < Servers_LB->count(); i++ ) {
        if( *(D.DNS.Servers[j]) == Servers_LB->text(i) ) {
          // still in list
          Found = 1;
          break;
        }
      }
      if( ! Found ) {
        // old obsolete
        SM = 1;
        delete D.DNS.Servers[j];
        for( unsigned i = j+1; i < D.DNS.Servers.size(); i ++ ) {
          // compress array
          D.DNS.Servers[i-1] = D.DNS.Servers[i];
        }
        // shrink
        D.DNS.Servers.resize( D.DNS.Servers.size()-1 );
      }
    }
    return SM;
}

void PPPDNSEdit::showData( PPPData_t & D) {

    DNSServerAssigned_RB->setChecked( D.DNS.ServerAssigned );

    DomainName_LE->setText( D.DNS.DomainName );
    for( unsigned i = 0; i < D.DNS.Servers.size(); i ++ ) {
      Servers_LB->insertItem( *(D.DNS.Servers[i]) );
    }
}

void PPPDNSEdit::SLOT_AddServer( void ) {
    if( ServerAddress_LE->text().isEmpty() )
      return;

    Servers_LB->insertItem( ServerAddress_LE->text() );
    ServerAddress_LE->setText( "" );
}

void PPPDNSEdit::SLOT_RemoveServer( void ) {
    long i = Servers_LB->currentItem();
    if( i < 0 )
      return;
    Servers_LB->removeItem( i );
    if( (unsigned) i >= Servers_LB->count() ) {
      i --;
    }
    if( i >= 0 ) {
      Servers_LB->setCurrentItem( i );
    }

}
