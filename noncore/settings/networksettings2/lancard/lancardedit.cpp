#include <GUIUtils.h>
#include <resources.h>
#include <qlistview.h>
#include <qcheckbox.h>
#include <qheader.h>
#include <qregexp.h>
#include "lancardedit.h"

LanCardEdit::LanCardEdit( QWidget * Parent ) : LanCardGUI( Parent ){
    System & S = NSResources->system();
    QRegExp R( "eth[0-9]" );
    QCheckListItem * CLI;

    LanCards_LV->header()->hide();

    // populate with all lancards in system
    for( QDictIterator<InterfaceInfo> It(S.interfaces());
         It.current();
         ++It ) {
      if( R.match( It.current()->Name ) >= 0 &&
          It.current()->CardType == ARPHRD_ETHER
        ) {
        CLI = new QCheckListItem( LanCards_LV, It.current()->Name,
              QCheckListItem::CheckBox );
        CLI->setText( 1, It.current()->MACAddress );
      }
    }
}

QString LanCardEdit::acceptable( void ) {
    return QString();
}


bool LanCardEdit::commit( LanCardData & Data ) {
    bool SM = 0;
    CBM( Data.AnyLanCard, AnyCard_CB, SM );

    if( ! Data.AnyLanCard ) {
      // collect set of lancards that match
      int idx;
      QCheckListItem * CLI = (QCheckListItem *)LanCards_LV->firstChild();
      while( CLI ) {
        idx = Data.HWAddresses.findIndex(CLI->text(1));
        if( CLI->isOn() ) {
          if( idx < 0 ) {
            // should be in list
            Data.HWAddresses.append( CLI->text(1) );
            SM = 1;
          }
        } else {
          // should not be in list
          if( idx >= 0 ) {
            Data.HWAddresses.remove( Data.HWAddresses.at(idx) );
            SM = 1;
          }
        }
        CLI = (QCheckListItem *)CLI->nextSibling();
      }
    }
    return SM || ContainedObsoleteMAC ;
}

void LanCardEdit::showData( LanCardData & Data ) {
    AnyCard_CB->setChecked( Data.AnyLanCard );

    QCheckListItem * CLI = (QCheckListItem *)LanCards_LV->firstChild();

    ContainedObsoleteMAC = 0;
    // remove obsolete address
    for( QStringList::Iterator it=Data.HWAddresses.begin();
         it != Data.HWAddresses.end(); 
         ) {
      CLI = (QCheckListItem *)LanCards_LV->firstChild();
      while( CLI ) {
        if( CLI->text(1) == (*it) ) 
          // still valid
          break;
        CLI = (QCheckListItem *)CLI->nextSibling();
      }
      if( CLI == 0 ) {
        // address not found -> remove
        ContainedObsoleteMAC = 1;
        Data.HWAddresses.remove( it );
      } else {
        ++ it;
      }
    }

    // set checks
    while( CLI ) {
      CLI->setOn( Data.HWAddresses.findIndex(CLI->text(1)) >= 0 );
      CLI = (QCheckListItem *)CLI->nextSibling();
    }
}
