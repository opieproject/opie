#include <GUIUtils.h>
#include <resources.h>
#include <qarray.h>
#include <qlistview.h>
#include <qcheckbox.h>
#include <qheader.h>
#include <qregexp.h>

#include "lancardedit.h"
#include "lancard_NN.h"
#include "lancard_NNI.h"

LanCardEdit::LanCardEdit( QWidget * Parent ) : LanCardGUI( Parent ){
    LanCards_LV->header()->hide();

}

QString LanCardEdit::acceptable( void ) {
    return QString();
}

bool LanCardEdit::commit( LanCardData & Data ) {
    bool SM = 0;
    CBM( Data.AnyLanCard, AnyCard_CB, SM );

    if( ! Data.AnyLanCard ) {
      // take copy for orig list
      QStringList NewList( Data.HWAddresses );

      // update HWAddresses to new state
      // remove item also from NewList
      int idx;
      QCheckListItem * CLI = (QCheckListItem *)LanCards_LV->firstChild();
      while( CLI ) {
        idx = Data.HWAddresses.findIndex(CLI->text(0));
        if( CLI->isOn() ) {
          if( idx < 0 ) {
            // should be in list
            Data.HWAddresses.append( CLI->text(0) );
            SM = 1;
          }
        } else {
          // should not be in list
          if( idx >= 0 ) {
            NewList.remove( CLI->text(0) );
            Data.HWAddresses.remove( CLI->text(0) );
            SM = 1;
          }
        }
        CLI = (QCheckListItem *)CLI->nextSibling();
      }

      // if newlist still contains items. it were items
      // that were checked but no longer are present in the system
      SM |= ( NewList.count() > 0 ) ;
    }
    return SM;
}

void LanCardEdit::showData( ALanCard * LC ) {
    NNI = LC;
    LanCardData & Data = *((LanCardData *)LC->data());

    AnyCard_CB->setChecked( Data.AnyLanCard );

    // load all cards
    populateList();

    // set checks
    QCheckListItem * CLI = (QCheckListItem *)LanCards_LV->firstChild();
    while( CLI ) {
      CLI->setOn( Data.HWAddresses.findIndex(CLI->text(0)) >= 0 );
      CLI = (QCheckListItem *)CLI->nextSibling();
    }
}

// load all known cards in list
void LanCardEdit::populateList( void ) {
    LanCardNetNode *NN = (LanCardNetNode *)NNI->nodeClass();
    QCheckListItem * CLI;
    bool Found;

    LanCards_LV->clear();

    for( QStringList::Iterator it = NN->addressesOfNIC().begin();
         it != NN->addressesOfNIC().end(); 
         ++it ) {
      CLI = new QCheckListItem( LanCards_LV, (*it), QCheckListItem::CheckBox );

      // check interfaces and see if this card is present
      Found = 0;
      for( QDictIterator<InterfaceInfo> NIt(NSResources->system().interfaces());
           NIt.current();
           ++NIt ) {
        if( NIt.current()->MACAddress == (*it) ) {
          Found = 1;
          break;
        }
      }

      CLI->setPixmap( 0, NSResources->getPixmap( 
        (Found) ? "add" : "remove" ) );
    }
}

// rescan system for new cards
void LanCardEdit::SLOT_ScanCards( void ) {
    LanCardNetNode *NN = (LanCardNetNode *)NNI->nodeClass();

    // add any NIC that is new and matches our interfacename
    System & S = NSResources->system();
    QRegExp R( "eth[0-9]" );
    // populate with all lancards in system
    for( QDictIterator<InterfaceInfo> It(S.interfaces());
         It.current();
         ++It ) {
      fprintf( stderr, "TEST %s %s\n", 
          It.current()->Name.latin1(),
          It.current()->MACAddress.latin1() );
      if( R.match( It.current()->Name ) >= 0 &&
          ( It.current()->CardType == ARPHRD_ETHER
#ifdef ARPHRD_IEEE1394
            || It.current()->CardType == ARPHRD_IEEE1394
#endif
          )
        ) {
        // old item ?
        QCheckListItem * CLI = 
                          (QCheckListItem *)LanCards_LV->firstChild();
        while( CLI ) {
          if( CLI->text(0) == It.current()->MACAddress ) {
            break;
          }
          CLI = (QCheckListItem *)CLI->nextSibling();
        }

        if( ! CLI ) {
          // new item
          CLI = new QCheckListItem( LanCards_LV, 
                It.current()->MACAddress,
                QCheckListItem::CheckBox );
        }

        // mark present
        CLI->setPixmap( 0, NSResources->getPixmap( 
                    "add" ) );

        if( NN->addressesOfNIC().findIndex( It.current()->MACAddress) < 0 ) {
          // new
          NN->addressesOfNIC().append( It.current()->MACAddress );
        }
      }
    }

}

// remove all cards that are not present -> flagged with 'remove'
// and unchecked
void LanCardEdit::SLOT_RemoveUnknown( void ) {
    QArray<QCheckListItem *> AllItems;

    LanCardNetNode *NN = (LanCardNetNode *)NNI->nodeClass();

    QCheckListItem * CLI = (QCheckListItem *)LanCards_LV->firstChild();
    while( CLI ) {
      AllItems.resize( AllItems.size()+1 );
      AllItems[ AllItems.size()-1 ] = CLI;
      CLI = (QCheckListItem *)CLI->nextSibling();
    }

    // force update of system
    System & S = NSResources->system();
    S.probeInterfaces();

    // add any NIC that is new and matches our interfacename
    QRegExp R( "eth[0-9]" );


    for( QDictIterator<InterfaceInfo> It(S.interfaces());
         It.current();
         ++It ) {
      if( R.match( It.current()->Name ) >= 0 &&
          ( It.current()->CardType == ARPHRD_ETHER
#ifdef ARPHRD_IEEE1394
            || It.current()->CardType == ARPHRD_IEEE1394
#endif
          )
        ) {

        for ( unsigned i = 0; i< AllItems.size(); i++ ) {
          if( AllItems[i] &&
              AllItems[i]->text(0) == It.current()->MACAddress ) {
            AllItems[i] = 0;
            break;
          }
        }
      }
    }

    // AllItems now contains all cards NOT present
    // remove all items non null and not ON
    for ( unsigned i = 0; i< AllItems.size(); i++ ) {
      if( AllItems[i] && ! AllItems[i]->isOn() ) {
        NN->addressesOfNIC().remove( AllItems[i]->text(0) );
        delete AllItems[i];
      }
    }
}
