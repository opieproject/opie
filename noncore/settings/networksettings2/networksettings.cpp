#include <stdio.h>

#include <qpe/qpeapplication.h>
#include <qiconset.h>
#include <qgroupbox.h>
#include <qtimer.h>
#include <qlistbox.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qiconview.h>
#include <qtimer.h>
#include <qpe/qpeapplication.h>
#include <qtoolbutton.h>

#include <asdevice.h>
#include "networksettings.h"
#include "netnode.h"
#include "editconnection.h"

NetworkSettings::NetworkSettings( QWidget *parent, 
               const char *name, 
               WFlags fl ) : NetworkSettingsGUI(parent,name,fl), 
                             NSD() {

    UpdateTimer = new QTimer( this );
    // set pixmaps 
    Add_TB->setPixmap( NSResources->getPixmap( "add" ) );
    Delete_TB->setPixmap( NSResources->getPixmap( "remove" ) );
    CheckState_TB->setPixmap( NSResources->getPixmap( "check" ) );
    Enable_TB->setPixmap( NSResources->getPixmap( "disabled" ) );
    GenConfig_TB->setPixmap( NSResources->getPixmap( "configure" ) );

    Connect_TB->setPixmap( NSResources->getPixmap( "connected" ) );

    On_TB->setPixmap( NSResources->getPixmap( "off" ) );

    // populate main Listbox
    Profiles_IV->clear();
    { Name2Connection_t & M = NSResources->connections();
      NodeCollection * NC;
      QIconViewItem * IVI;

      // for all connections
      for( QDictIterator<NodeCollection> it(M);
           it.current();
           ++it ) {
        NC = it.current();
        IVI = new QIconViewItem( Profiles_IV,
                                 NC->name(),
                                 NC->devicePixmap() );
      }
    }

    if( Profiles_IV->count() ) {
      Profiles_IV->setSelected( Profiles_IV->firstItem(), TRUE );
    }

    // if no profiles -> auto popup editing
    if( NSResources->connections().count() == 0 ) {
      QTimer::singleShot( 100, this, SLOT(SLOT_AddNode() ) );
    }

    UpdateTimer->start( 5000 );
    connect( UpdateTimer, SIGNAL( timeout() ),
             this, SLOT( SLOT_RefreshStates() ) );

    /* Add QCopChannel */
    connect( qApp, SIGNAL(appMessage(const QCString&,const QByteArray&)),
             this, SLOT(SLOT_QCopMessage(const QCString&,const QByteArray&)) );
}

NetworkSettings::~NetworkSettings() {
    QString S;

    S = NSD.generateSettings();
    if( ! S.isEmpty() ) {
      QMessageBox::warning(
        0, 
        tr( "Generating system configuration" ),
        S 
      );
    }

    S = NSD.saveSettings();
    if( ! S.isEmpty() ) {
      // problem saving
      QMessageBox::warning(
            0, 
            tr( "Saving setup" ), S );
    }
}

void NetworkSettings::SLOT_RefreshStates( void ) {
    QIconViewItem * IVI = Profiles_IV->currentItem(); // remeber

    if( IVI ) {
      NodeCollection * NC;
      NSResources->system().probeInterfaces();
      // update current selection only
      NC = NSResources->findConnection( IVI->text() );
      if( NC ) {
        State_t OldS = NC->state();
        State_t NewS = NC->state(1);
        if( OldS != NewS ) {
          updateProfileState( IVI );
        }
      }
    }


    /* -> LATER !!
    bool is;
    NodeCollection * NC;

    for( unsigned int i = 0; i < Profiles_LB->count() ; i ++ ) {
      NC = NSResources->findConnection( Profiles_LB->text(i) );
      if( NC ) {
        State_t OldS = NC->state();
        State_t NewS = NC->state(1);
        if( OldS != NewS ) {
          is = Profiles_LB->isSelected(i);
          Profiles_LB->changeItem( NC->statePixmap(NewS), 
                                   NC->name(), 
                                   i );
          if( is ) {
            Profiles_LB->setSelected( i, TRUE );
          }
        }
      }
    }
    if( ci >= 0 )
      Profiles_LB->setCurrentItem( ci );
    */
}

void NetworkSettings::SLOT_AddNode( void ) {
    SLOT_EditNode( 0 );
}

void NetworkSettings::SLOT_DeleteNode( void ) {
    QIconViewItem * IVI = Profiles_IV->currentItem();

    if ( ! IVI ) 
      return;

    if( QMessageBox::warning(
          0, 
          tr( "Removing profile" ),
          tr( "Remove selected profile ?" ), 
          1, 0 ) == 1 ) {
      NSResources->removeConnection( IVI->text() );
      delete IVI;
      setModified( 1 );
      NSD.forceGeneration(1);
    }
}

void NetworkSettings::SLOT_EditNode( QIconViewItem * IVI ) {
    QString OldName = "";
    EditConnection EC( this );

    if( IVI ) {
      NodeCollection * NC = NSResources->findConnection( IVI->text() );
      if( ! NC ) {
        return;
      }
      OldName = NC->name();
      EC.setConnection( NC );
    }

    EC.showMaximized();
    // disable refresh timer 
    UpdateTimer->stop();
    if( EC.exec() == QDialog::Accepted ) {
      // toplevel item -> store
      NodeCollection * NC = EC.connection();
      if( NC->isModified() ) {
        setModified( 1 );
        if( IVI ) {
          // new name -> remove item
          NSResources->removeConnection( OldName );
          // must add it here since change will trigger event
          NSResources->addConnection( NC );
          IVI->setText( NC->name() );
          IVI->setPixmap( NC->devicePixmap() );
        } else {
          // new item
          NSResources->addConnection( NC );
          NC->setNumber( NC->maxConnectionNumber()+1 );
          IVI = new QIconViewItem( Profiles_IV,
                                   NC->name(),
                                   NC->devicePixmap()
                                 );
          Profiles_IV->setSelected( IVI, TRUE );
        }
        updateProfileState( IVI );
      }
    } else {
      // cancelled : reset connection 
      if( IVI ) {
        NodeCollection * NC = NSResources->findConnection( IVI->text() );
        NC->reassign();
      }
    }
    // reenable 
    UpdateTimer->start( 5000 );
}

void NetworkSettings::SLOT_ShowNode( QIconViewItem * IVI ) {
    if( IVI == 0 ) 
      return;

    NodeCollection * NC = NSResources->findConnection( IVI->text() );

    // is button possible
    bool EnabledPossible, OnPossible, ConnectPossible;
    // is button On or Off
    bool DisabledOn, OnOn, ConnectOn;

    EnabledPossible = OnPossible = ConnectPossible = 1;
    DisabledOn = 1;
    OnOn = ConnectOn = 0;

    switch( NC->state() ) {
      case Unknown :
        // cannot occur here
        break;
      case Unchecked :
      case Unavailable :
        // cannot do anything but recheck
        EnabledPossible = OnPossible = ConnectPossible = 0;
        break;
      case Disabled :
        OnPossible = ConnectPossible = 0;
        break;
      case Off :
        DisabledOn = 0;
        break;
      case Available :
        OnOn = 1;
        DisabledOn = 0;
        break;
      case IsUp :
        OnOn = ConnectOn = 1;
        DisabledOn = 0;
        break;
    }

    // set button state
    Enable_TB->setEnabled( EnabledPossible );
    On_TB->setEnabled( OnPossible );
    Connect_TB->setEnabled( ConnectPossible );

    Enable_TB->setOn( DisabledOn );
    On_TB->setOn( OnOn );
    Connect_TB->setOn( ConnectOn );

    if( NC->description().isEmpty() ) {
      Description_LBL->setText( tr( "No description" ) );
    } else {
      Description_LBL->setText( NC->description() );
    }

    CurProfile_GB->setTitle( IVI->text() );
    State_LBL->setText( NC->stateName() );
}

void NetworkSettings::SLOT_CheckState( void ) {
    QIconViewItem * IVI  = Profiles_IV->currentItem();
    if ( ! IVI ) 
      return;
    updateProfileState( IVI );
}

void NetworkSettings::updateProfileState( QIconViewItem * IVI ) {
    if( IVI == Profiles_IV->currentItem() ) {
      SLOT_ShowNode( IVI );
    }
}

void NetworkSettings::SLOT_GenerateConfig( void ) {
    QString S = NSD.generateSettings( TRUE );

    if( ! S.isEmpty() ) {
      QMessageBox::warning(
        0, 
        tr( "Generating system configuration" ),
        S 
      );
    }
}

void NetworkSettings::SLOT_Enable( void ) {
    QIconViewItem * IVI = Profiles_IV->currentItem();
    QString Msg;
    if ( ! IVI ) 
      return;

    NodeCollection * NC = 
        NSResources->findConnection( IVI->text() );

    bool rv;
    switch( NC->state() ) {
      case Disabled :
        Msg = tr( "Cannot enable profile" );
        rv = NC->setState( Enable );
        break;
      default :
        Msg = tr( "Cannot disable profile" );
        rv = NC->setState( Disable );
        break;
    }

    if( ! rv ) {
      QMessageBox::warning(
          0, 
          tr( "Activating profile" ),
          Msg );
      return;
    }
    updateProfileState( IVI );
}

void NetworkSettings::SLOT_On( void ) {
    QIconViewItem * IVI = Profiles_IV->currentItem();

    if ( ! IVI ) 
      return;

    NodeCollection * NC = 
        NSResources->findConnection( IVI->text() );

    bool rv;
    switch( NC->state() ) {
      case Off :
        // activate interface
        rv = NC->setState( Activate );
        break;
      case Available :
        // deactivate
        rv = NC->setState( Deactivate );
        break;
      case IsUp :
        // bring down and deactivate
        rv = ( NC->setState( Down ) &&
               NC->setState( Deactivate ) );
        break;
      default :
        // others no change
        return;
    } 

    if( ! rv ) {
      QMessageBox::warning(
          0, 
          tr( "Activating profile" ),
          tr( "Cannot enable profile" ) );
      return;
    }
    updateProfileState( IVI );
}

void NetworkSettings::SLOT_Connect( void ) {
    QIconViewItem * IVI = Profiles_IV->currentItem();

    if ( ! IVI ) 
      return;

    NodeCollection * NC = 
        NSResources->findConnection( IVI->text() );

    bool rv;
    switch( NC->state() ) {
      case IsUp :
        // down interface
        rv = NC->setState( Down );
        break;
      case Available :
        // up interface
        rv = NC->setState( Up );
        break;
      case Off :
        // activate and bring up
        rv = ( NC->setState( Activate ) &&
               NC->setState( Up ) );
        break;
      default :
        // others no change
        return;
    } 

    if( ! rv ) {
      QMessageBox::warning(
          0, 
          tr( "Activating profile" ),
          tr( "Cannot enable profile" ) );
      return;
    }

    // we do not update the GUI but wait for the REAL upping of the device
}

void NetworkSettings::SLOT_QCopMessage(const QCString &msg, const QByteArray &data) {
  QDataStream stream( data, IO_ReadOnly );

  if( msg == "raise" ) {
      raise();
      return;
  } /* if ( msg == "someMessage(int,int,int)" ) {
    int a,b,c;
    stream >> a >> b >> c;
    ...
  } */
}
