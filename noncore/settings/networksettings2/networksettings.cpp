#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <opie2/odebug.h>
#include <opie2/oledbox.h>

#include <qpe/qpeapplication.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qtimer.h>
#include <qlistbox.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qiconview.h>
#include <qtimer.h>
#include <qpe/qpeapplication.h>
#include <qtoolbutton.h>
#include <qevent.h>

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
    GenConfig_TB->setPixmap( NSResources->getPixmap( "configure" ) );

    Disable_TB->setPixmap( NSResources->getPixmap( "disabled" ) );
    Up_TB->setPixmap( NSResources->getPixmap( "more" ) );
    Down_TB->setPixmap( NSResources->getPixmap( "less" ) );

    QVBoxLayout* V = new QVBoxLayout( LED_Frm );
    QHBoxLayout * H = new QHBoxLayout( 0 );
    V->addStretch(1);
    V->addLayout( H );
    Leds[0] = new Opie::Ui::OLedBox( red, LED_Frm );
    H->addWidget( Leds[0], 0, Qt::AlignVCenter );
    Leds[1] = new Opie::Ui::OLedBox( red, LED_Frm );
    H->addWidget( Leds[1], 0, Qt::AlignVCenter );
    Leds[2] = new Opie::Ui::OLedBox( red, LED_Frm );
    H->addWidget( Leds[2], 0, Qt::AlignVCenter );
    V->addStretch(1);


    SLOT_ToProfile();

    // populate main Listbox
    Profiles_LB->clear();
    QPEApplication::setStylusOperation( 
        Profiles_LB->viewport(), QPEApplication::RightOnHold );

    connect( Profiles_LB, 
             SIGNAL(rightButtonPressed(QListBoxItem*,const QPoint&)),
             this, SLOT(SLOT_EditNode(QListBoxItem*)) );

    { Name2NetworkSetup_t & M = NSResources->networkSetups();
      NetworkSetup * NC;
      // for all NetworkSetups
      for( QDictIterator<NetworkSetup> it(M);
           it.current();
           ++it ) {
        NC = it.current();
        Profiles_LB->insertItem( NC->devicePixmap(),
                                 NC->name() );
      }
    }

    if( Profiles_LB->count() ) {
      Profiles_LB->setSelected( 0, TRUE );
    }

    // if no profiles -> auto popup editing
    if( NSResources->networkSetups().count() == 0 ) {
      QTimer::singleShot( 100, this, SLOT(SLOT_AddNode() ) );
    }

    connect( &(NSResources->system()), 
             SIGNAL( stdoutLine(const QString &) ),
             this, SLOT( SLOT_CmdMessage(const QString &) ) );

    connect( &(NSResources->system()), 
             SIGNAL( stderrLine(const QString &) ),
             this, SLOT( SLOT_CmdMessage(const QString &) ) );

    connect( &(NSResources->system()), 
             SIGNAL( processEvent(const QString &) ),
             this, SLOT( SLOT_CmdMessage(const QString &) ) );

    UpdateTimer->start( 5000 );
    connect( UpdateTimer, SIGNAL( timeout() ),
             this, SLOT( SLOT_RefreshStates() ) );

    /* Add QCopChannel */
    connect( qApp, SIGNAL(appMessage(const QCString&,const QByteArray&)),
             this, SLOT(SLOT_QCopMessage(const QCString&,const QByteArray&)) );
}

NetworkSettings::~NetworkSettings() {
    QString S;

    if( NSD.isModified() ) {
      S = NSD.saveSettings();
      if( ! S.isEmpty() ) {
        S.insert( 0, "<p>" );
        S.append( "</p>" );
        // problem saving
        QMessageBox::warning(
              0, 
              tr( "Saving setup" ), S );
      }

      SLOT_GenerateConfig();
      NSD.setModified( 0 );
    }

}

void NetworkSettings::SLOT_CmdMessage( const QString & S ) {
    Messages_LB->insertItem( S );
    Messages_LB->setCurrentItem( Messages_LB->count()-1 );
    Messages_LB->ensureCurrentVisible();
}

void NetworkSettings::SLOT_RefreshStates( void ) {
    QListBoxItem * LBI = Profiles_LB->item( Profiles_LB->currentItem() ); // remember

    if( LBI ) {
      NetworkSetup * NC;
      NSResources->system().probeInterfaces();
      // update current selection only
      NC = NSResources->findNetworkSetup( LBI->text() );
      if( NC ) {
        State_t OldS = NC->state();
        State_t NewS = NC->state(1);
        if( OldS != NewS ) {
          updateProfileState( LBI );
        }
      }
    }


    /* -> LATER !!
    bool is;
    NetworkSetup * NC;

    for( unsigned int i = 0; i < Profiles_LB->count() ; i ++ ) {
      NC = NSResources->findNetworkSetup( Profiles_LB->text(i) );
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
    QListBoxItem * LBI = Profiles_LB->item( Profiles_LB->currentItem() ); 

    if ( ! LBI ) 
      return;

    if( QMessageBox::warning(
          0, 
          tr( "Removing profile" ),
          tr( "Remove selected profile ?" ), 
          1, 0 ) == 1 ) {
      NSResources->removeNetworkSetup( LBI->text() );
      delete LBI;
      NSD.setModified( 1 );
    }
}

void NetworkSettings::SLOT_EditNode( QListBoxItem * LBI ) {
    QString OldName = "";

    EditNetworkSetup EC( this );

    if( LBI ) {
      NetworkSetup * NC = NSResources->findNetworkSetup( LBI->text() );
      if( ! NC ) {
        return;
      }
      OldName = NC->name();
      EC.setNetworkSetup( NC );
    }

    EC.showMaximized();
    // disable refresh timer 
    UpdateTimer->stop();

    // we need to retry
    while( 1 ) {
      if( EC.exec() == QDialog::Accepted ) {
        // toplevel item -> store
        NetworkSetup * NC = EC.networkSetup();
        if( NC->isModified() ) {
          if( LBI ) {
            if( NC->name() != OldName ) {
              // find if new name is free
              NetworkSetup * LCN = NSResources->findNetworkSetup(
                    NC->name() );
              if( LCN ) {
                QMessageBox::warning(
                  0, 
                  tr( "In System Config" ),
                  tr( "Name %1 already exists" ).arg(NC->name())
                );
                continue; // restart exec
              } // else new name
              // new name -> remove item
              NSResources->removeNetworkSetup( OldName );
              NSResources->addNetworkSetup( NC, 0 );
            } // else not changed

            // must add it here since change will trigger event
            Profiles_LB->changeItem( NC->devicePixmap(),
                                     NC->name(),
                                     Profiles_LB->index( LBI )
                                   );
          } else {
            // new item
            int ci = Profiles_LB->count();
            NSResources->addNetworkSetup( NC, 0 );
            NC->setNumber( NSResources->assignNetworkSetupNumber() );
            Profiles_LB->insertItem( NC->devicePixmap(), NC->name() );
            Profiles_LB->setSelected( ci, TRUE );
          }
          updateProfileState( LBI );
        }
      } else {
        // cancelled : reset NetworkSetup 
        if( LBI ) {
          NetworkSetup * NC = NSResources->findNetworkSetup( LBI->text() );
          NC->reassign();
        }
      }
      break;
    }
    // reenable 
    UpdateTimer->start( 5000 );
}

void NetworkSettings::SLOT_ShowNode( QListBoxItem * LBI ) {
    if( LBI == 0 ) 
      return;

    NetworkSetup * NC = NSResources->findNetworkSetup( LBI->text() );

    if( NC->description().isEmpty() ) {
      Description_LBL->setText( tr( "<<No description>>" ) );
    } else {
      Description_LBL->setText( NC->description() );
    }

    Profile_GB->setTitle( LBI->text() + " : " + NC->stateName() );

    bool FrmActive = 1;
    bool IsEnabled = 1;
    int leds = 0;

    switch( NC->state() ) {
      case Disabled : // no further work
        IsEnabled = 0;
        FrmActive = 0;
        break;
      case Unknown :
      case Unchecked :
      case Unavailable :
        FrmActive = 0;
        break;
      case Off :
        leds = 1;
        break;
      case Available :
        leds = 2;
        break;
      case IsUp :
        leds = 3;
        break;
    }

    Disable_TB->setOn( ! IsEnabled );
    LED_Frm->setEnabled( FrmActive );

    for( int i = 0 ; i < leds; i ++ ) {
      Leds[i]->setColor( red );
      Leds[i]->setOn( true );
    }
    for( int i = leds ; i < 3; i ++ ) {
      Leds[i]->setColor( red );
      Leds[i]->setOn( false );
    }

    Up_TB->setEnabled( leds < 3 && leds != 0 );
    Down_TB->setEnabled( leds > 0 );
}

void NetworkSettings::SLOT_CheckState( void ) {
    QListBoxItem * LBI = Profiles_LB->item( Profiles_LB->currentItem() ); 
    if ( ! LBI ) 
      return;
    updateProfileState( LBI );
}

void NetworkSettings::updateProfileState( QListBoxItem * LBI ) {
    if( LBI == Profiles_LB->item( Profiles_LB->currentItem() ) ) {
      SLOT_ShowNode( LBI );
    }
}

void NetworkSettings::SLOT_GenerateConfig( void ) {
    QString S = NSD.generateSettings();
    if( ! S.isEmpty() ) {
      S.insert( 0, "<p>" );
      S.append( "</p>" );
      QMessageBox::warning(
          0, 
          tr( "Generate config" ),
          S);
    }
}

void NetworkSettings::SLOT_Disable( bool T ) {
    QListBoxItem * LBI = Profiles_LB->item( Profiles_LB->currentItem() );
    QString Msg;

    if ( ! LBI ) 
      return;

    NetworkSetup * NC = NSResources->findNetworkSetup( LBI->text() );

    Log(( "Prepare to %sable\n", (T) ? "en" : "dis" ));
    Msg = NC->setState( (T) ? Disable : Enable );
    if( ! Msg.isEmpty() ) {
      Msg.insert( 0, "<p>" );
      Msg.append( "</p>" );
      QMessageBox::warning(
          0, 
          tr( "Activating profile" ),
          Msg );
      return;
    }

    // reload new state
    NC->state( true );
    updateProfileState( LBI );
}

void NetworkSettings::SLOT_Up( void ) {
    // bring more up

    QListBoxItem * LBI = Profiles_LB->item( Profiles_LB->currentItem() );
    QString Msg;
    int led = -1;

    if ( ! LBI ) 
      return;

    NetworkSetup * NC = 
        NSResources->findNetworkSetup( LBI->text() );

    switch( NC->state() ) {
      case Disabled : // cannot modify this state
      case Unknown : // cannot modify this state
      case Unchecked : // cannot modify this state
      case Unavailable : // cannot modify this state
      case IsUp : // highest UP state
        return;
      case Off : // -> activate
        led = 1;
        Down_TB->setEnabled( true );
        Log(( "Activate interface %s\n", NC->name().latin1() ));
        Msg = NC->setState( Activate );
        break;
      case Available : // -> up
        led = 2;
        Log(( "Bring up interface %s\n", NC->name().latin1() ));
        Msg = NC->setState( Up );
        if( Msg.isEmpty() ) {
          Up_TB->setEnabled( false );
        }
        break;
    }

    if( ! Msg.isEmpty() ) {
      Msg.insert( 0, "<p>" );
      Msg.append( "</p>" );
      QMessageBox::warning(
          0, 
          tr( "Increase availability" ),
          Msg );
      return;
    }

    updateProfileState( LBI );

    // set color of led we should change 
    if( led > 0 ) {
      Leds[led]->setColor( blue );
      Leds[led]->setOn( true );
    }

}

void NetworkSettings::SLOT_Down( void ) {
    // bring more down

    QListBoxItem * LBI = Profiles_LB->item( Profiles_LB->currentItem() );
    int led = -1;
    QString Msg;

    if ( ! LBI ) 
      return;

    NetworkSetup * NC = 
        NSResources->findNetworkSetup( LBI->text() );

    switch( NC->state() ) {
      case Disabled : // cannot modify this state
      case Unknown : // cannot modify this state
      case Unchecked : // cannot modify this state
      case Unavailable : // cannot modify this state
      case Off : // highest DOWN state
        break;
      case Available : // -> down
        led = 0;
        Log(( "Deactivate interface %s\n", NC->name().latin1() ));
        Msg = NC->setState( Deactivate );
        Down_TB->setEnabled( false );
        break;
      case IsUp : // highest UP state
        led = 1;
        Up_TB->setEnabled( true );
        Log(( "Bring down interface %s\n", NC->name().latin1() ));
        Msg = NC->setState( Down, 1 );
        if( Msg.isEmpty() ) {
          // remove 'up' file to make sure
          unlink ( QString().sprintf( "/tmp/Profile-%d.up", NC->number() ).latin1() );;
        }
        break;
    }

    if( ! Msg.isEmpty() ) {
      Msg.insert( 0, "<p>" );
      Msg.append( "</p>" );
      QMessageBox::warning(
          0, 
          tr( "Decrease availability" ),
          Msg );
      return;
    }

    updateProfileState( LBI );

    // set color of led we should change 
    if( led >= 0 ) {
      Leds[led]->setColor( blue );
    }
}

void NetworkSettings::SLOT_ToMessages( void ) {
    Profiles_LB->hide();
    Profile_GB->hide();
    Messages_GB->show();
}

void NetworkSettings::SLOT_ToProfile( void ) {
    Profiles_LB->show();
    Profile_GB->show();
    Messages_GB->hide();
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
