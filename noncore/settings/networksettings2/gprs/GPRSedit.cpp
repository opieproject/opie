#include <qtoolbutton.h>
#include <qlistview.h>
#include <qheader.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qtabwidget.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <GUIUtils.h>
#include <resources.h>
#include "GPRSedit.h"

GPRSEdit::GPRSEdit( QWidget * Parent ) : GPRSGUI( Parent ){
    Routing_LV->header()->hide();
    Add_TB->setPixmap( NSResources->getPixmap( "add" ) );
    Delete_TB->setPixmap( NSResources->getPixmap( "delete" ) );
    Routing_LV->setColumnAlignment( 1, Qt::AlignRight );
}

QString GPRSEdit::acceptable( void ) {
    if( APN_LE->text().isEmpty() ) {
      return tr("APN is required");
    }

    return QString();
}

bool GPRSEdit::commit( GPRSData & Data ) {
    bool SM = 0;
    bool RM;

    TXTM( Data.APN, APN_LE, SM );
    TXTM( Data.User, User_LE, SM );
    TXTM( Data.Password, Password_LE, SM );

    if( AssignedByServer_CB->isChecked() ) {
      if( ! Data.DNS1.isEmpty() ) {
        SM = 1;
        Data.DNS1 = "";
      }
      if( ! Data.DNS2.isEmpty() ) {
        SM = 1;
        Data.DNS2 = "";
      }
    } else {
      TXTM( Data.DNS1, DNS1_LE, SM );
      TXTM( Data.DNS2, DNS2_LE, SM );
    }

    CBM( Data.DefaultGateway, DefaultGateway_RB, SM );
    CBM( Data.SetIfSet, SetIfSet_CB, SM );

    // find new routes
    unsigned int i;

    RM = 0; // routing modified
    QListViewItem * it = Routing_LV->firstChild();
    while( ! RM && it ) {
      for( i = 0; i < Data.Routing.count(); i ++ ) {
        if( it->text(0) == Data.Routing[i]->Address ) {
          // still exists
          break;
        }
      }
      if( i == Data.Routing.count() ) {
        // modified
        RM = 1;
      }
      it = it->nextSibling();
    }

    // find obsoleted
    for( i = 0; ! RM && i < Data.Routing.count(); i ++ ) {
      it = Routing_LV->firstChild();
      while( it ) {
        if( it->text(0) == Data.Routing[i]->Address ) {
          // still exists
          break;
        }
        it = it->nextSibling();
      }

      if( it == 0 ) {
        RM = 1;
      }
    }

    if( RM ) {
      unsigned int i = 0;
      GPRSRoutingEntry * E;
      // update routing table
      Data.Routing.resize(0);
      Data.Routing.resize( Routing_LV->childCount() );

      it = Routing_LV->firstChild();
      while( it ) {
        E = new GPRSRoutingEntry;
        E->Address = it->text(0);
        E->Mask = it->text(1).toShort();
        Data.Routing.insert( i, E );
        i ++;
        it = it->nextSibling();
      }
    }

    SBM( Data.Debug, Debug_SB, SM );
    return SM;
}

void GPRSEdit::showData( GPRSData & Data ) {
    STXT( Data.APN, APN_LE );
    STXT( Data.User, User_LE );
    STXT( Data.Password, Password_LE );

    SCB( ( Data.DNS1.isEmpty() && Data.DNS2.isEmpty() ),
         AssignedByServer_CB );
    STXT( Data.DNS1, DNS1_LE );
    STXT( Data.DNS2, DNS2_LE );

    SCB( Data.DefaultGateway, DefaultGateway_RB );
    SCB( (! Data.DefaultGateway), FixedGateway_RB );
    SCB( Data.SetIfSet, SetIfSet_CB );

    for( unsigned int i = 0; i < Data.Routing.count(); i ++ ) {
      QListViewItem * it;
      it = new QListViewItem( Routing_LV );

      it->setText( 0, Data.Routing[i]->Address );
      it->setText( 1, QString().setNum( Data.Routing[i]->Mask ) );
    }

    SSB( Data.Debug, Debug_SB );
}

void GPRSEdit::SLOT_AddRoute( void ) {
    QListViewItem * it, *last;
    QListViewItem * after = Routing_LV->firstChild();
    last = 0;
    while( after ) {
      if( after->isSelected() ) {
        break;
      }
      last = after;
      after = after->nextSibling();
    }

    it = new QListViewItem( Routing_LV, (after) ? after : last );

    it->setText( 0, Net_LE->text() );
    it->setText( 1, QString().setNum( Mask_SB->value() ) );
}

void GPRSEdit::SLOT_DeleteRoute( void ) {
    QListViewItem * nit;
    QListViewItem * it = Routing_LV->firstChild();
    while( it ) {
      nit = it->nextSibling();
      if( it->isSelected() ) {
        delete it;
      }
      it = nit;
    }
}
