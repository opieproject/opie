#include <stdio.h>

#include <qpe/qpeapplication.h>
#include <qiconset.h>
#include <qgroupbox.h>
#include <qtimer.h>
#include <qlistbox.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qiconview.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtimer.h>
#include <qpe/qpeapplication.h>
#include <qtoolbutton.h>

#include <asdevice.h>
#include "networksettings.h"
#include "netnode.h"
#include "editconnection.h"

static QString CfgFile;

NetworkSettingsData::NetworkSettingsData( void ) {
    // init global resources structure
    new TheNSResources();

    CfgFile.sprintf( "%s/NETCONFIG", getenv("HOME") );

    // load settings
    Force = 0;
    loadSettings();
}

// saving is done by caller
NetworkSettingsData::~NetworkSettingsData( void ) {
    delete NSResources;
}

void NetworkSettingsData::loadSettings( void ) {
    QString S;
    ANetNodeInstance* NNI;
    QString Attr, Value;
    long idx;

    QFile F( CfgFile );
    QTextStream TS( &F );

    do {

      if( ! F.open(IO_ReadOnly) )
        break;

      /* load the file ->

         FORMAT :

           [NETNODETYPE] 
           Entries ...
           <EMPTYLINE>
           [connection] 
           Name=Name
           Node=Name
           <EMPTYLINE>
      */
      while( ! TS.atEnd() ) {
        S = TS.readLine();

        if ( S.isEmpty() || S[0] != '[' )
          continue;

        S = S.mid( 1, S.length()-2 );

        if( ! NSResources ) {
          continue;
        }

        if( S == "connection" ) {
          // load connections -> collections of nodes
          NodeCollection * NC = new NodeCollection( TS );
          if ( NC->count() == 0 ) {
            if( QMessageBox::warning(
                  0, 
                  qApp->translate( "NetworkSettings2", "Invalid connection" ),
                  qApp->translate( "NetworkSettings2", 
                    "<p>Connection %1 contains unrecognized nodes and cannot be loaded</p>" ).arg(NC->name()), 
                  qApp->translate( "NetworkSettings2", 
                    "Remove node"),
                  qApp->translate( "NetworkSettings2", 
                    "Exit program") ) == 1 ) {
              exit( 0 );
            }
            delete NC;
          } else 
            NSResources->addConnection( NC );
        } else {
          // load nodes
          NNI = NSResources->createNodeInstance( S );
          if( ! NNI ) {
            printf( "SKIPPING %s\n", S.latin1() );
          }

          do {
            S = TS.readLine();
            if( S.isEmpty() ) {
              // empty line
              break;
            }
            // node found ?
            if( NNI ) {
              idx = S.find( '=' );
              if( idx > 0 ) {
                Attr = S.left( idx );
                Value = S.mid( idx+1, S.length() );
              } else {
                Value="";
                Attr = S;
              }

              Value.stripWhiteSpace();
              Attr.stripWhiteSpace();
              Attr.lower();
              // dequote Attr
              Value = deQuote(Value);

              // set the attribute
              NNI->setAttribute( Attr, Value );
            }

          } while( 1 );
          if( NNI ) {
            // loading from file -> exists
            NNI->setNew( FALSE );
            NSResources->addNodeInstance( NNI );
          }
        }
      }

    } while( 0 );

}

QString NetworkSettingsData::saveSettings( void ) {
    QString ErrS = "";

    if( ! isModified() )
      return ErrS;

    QString S;
    QFile F( CfgFile + ".bup" );

    printf( "Saving settings to %s\n", CfgFile.latin1() );
    if( ! F.open( IO_WriteOnly | IO_Truncate ) ) {
      ErrS = qApp->translate( "NetworkSettings", 
              "<p>Could not save setup to %1 !</p>" ).
        arg(CfgFile);
      // problem
      return ErrS;
    }

    QTextStream TS( &F );
    { Name2Connection_t & M = NSResources->connections();
      ANetNodeInstance * NNI;

      // for all connections
      for( QDictIterator<NodeCollection> it(M);
           it.current();
           ++it ) {
        // all nodes in those connections
        for( QListIterator<ANetNodeInstance> nit(*(it.current())); 
             nit.current();
             ++nit ) {
          // header
          NNI = nit.current();
          TS << '[' <<NNI->netNode()->nodeName() << ']' << endl;
          NNI->saveAttributes( TS );
          TS << endl;
        }

        TS << "[connection]" << endl;
        it.current()->save(TS);
      }
    }

    QDir D(".");
    D.rename( CfgFile + ".bup", CfgFile );

    //
    // proper files AND system files regenerated
    //

    setModified( 0 );
    return ErrS;
}

QString NetworkSettingsData::generateSettings( bool ForceReq ) {
    bool ForceIt;
    QString S = "";

    // include own force flag
    ForceIt = (Force) ? 1 : ForceReq;

    if( ! ForceIt && ! isModified() )
      return S;

    // regenerate system files
    printf( "Generating settings from %s\n", CfgFile.latin1() );

    { Name2SystemFile_t & SFM = NSResources->systemFiles();
      Name2Connection_t & M = NSResources->connections();
      NodeCollection * NC;
      ANetNodeInstance * NNI;
      SystemFile * SF;
      bool needToRegenerate = ForceIt;

      //
      // check if we need to generate at least one of the system files
      //
      if( ! ForceIt ) {
        for( QDictIterator<SystemFile> sfit(SFM);
             sfit.current();
             ++sfit ) {
          SF = sfit.current();

          // check if there are nodes that are modified and require
          // data for this system file

          // for all connections
          for( QDictIterator<NodeCollection> ncit(M);
               ncit.current();
               ++ncit ) {
            NC = ncit.current();

            if( NC->isModified() ) {
              // does this connection 'touch' this system file ?
              for( QListIterator<ANetNodeInstance> cncit(*NC); 
                   cncit.current();
                   ++cncit ) {
                NNI = cncit.current();
                if( NNI->netNode()->hasDataFor( SF->name() ) &&
                    NNI->isModified() ) {
                  needToRegenerate = 1;
                  break;
                }
              }
            }
            if( needToRegenerate )
              break;
          }
          if( needToRegenerate )
            break;
        }
      }

      // we cannot renumber with a FORCE request since
      // we probably are NOT going to save the config
      // e.g. when using --regen option
      if( ! ForceReq && needToRegenerate ) {
        NSResources->renumberConnections();
        setModified(1);
      }

      //
      // generate files proper to each netnodeinstance
      //
      { Name2Instance_t & NNIs = NSResources->netNodeInstances();
        ANetNodeInstance * NNI;

        for( QDictIterator<ANetNodeInstance> NNIIt(NNIs);
             NNIIt.current();
             ++NNIIt
           ){
          // for all nodes find those that are modified
          NNI = NNIIt.current();

          if( ForceIt || NNI->isModified() ) {
            if( ! NNI->netNode()->generateProperFilesFor( NNI ) ) {
              // problem generating
              S = qApp->translate( "NetworkSettings", 
                                   "<p>Cannot generate files proper to %1</p>" ).
                      arg(NNI->netNode()->nodeName()) ;
              return S;
            }
          }
        }
      }

      //
      // generate system files
      //
      for( QDictIterator<SystemFile> sfit(SFM);
           sfit.current();
           ++sfit ) {
        SF = sfit.current();

        // 
        // regenerate current file
        // 
        printf( "Generating %s\n", SF->name().latin1() );
        SF->open();

        do { // so we can break;

          if( SF->preSection() ) {
            S = qApp->translate( "NetworkSettings", 
                                 "<p>Error in preSection for file %1</p>" ).
                arg( SF->name() );
            return S;
          }

          for( QDictIterator<NodeCollection> ncit(M);
               ncit.current();
               ++ncit ) {
            NC = ncit.current();

            // get the netnode that serves as the device for this
            // connection
            AsDevice * Dev = NC->device();

            // generate 'entry' for every possible device this profile handles

            for( QListIterator<ANetNodeInstance> cncit(*NC); 
                 cncit.current();
                 ++cncit ) {
              NNI = cncit.current();
              for( int i = 0; i < Dev->count(); i ++ ) {
                if( NNI->netNode()->hasDataFor( SF->name() ) ) {
                  if( SF->preNodeSection( NNI, i ) ) {
                    S = qApp->translate( "NetworkSettings", 
                        "<p>Error in preNodeSection for file %1 and node %2</p>" ).
                        arg( SF->name() ).
                        arg( NNI->netNode()->nodeName() );
                    return S;
                  }

                  if( NNI->netNode()->generateDataForCommonFile(*SF,i,NNI) ) {
                    S = qApp->translate( "NetworkSettings", 
                      "<p>Error in node part for file %1 and node %2</p>" ).
                        arg( SF->name() ).
                        arg( NNI->netNode()->nodeName() );
                    return S;
                  }

                  if( SF->postNodeSection( NNI, i ) ) {
                    S = qApp->translate( "NetworkSettings", 
                        "<p>Error in postNodeSection for file %1 and node %2</p>" ).
                            arg( SF->name() ).
                            arg( NNI->netNode()->nodeName() );
                    return S;
                  }
                }
              }
            }
            *SF << endl;
          }

          if( SF->postSection() ) {
            S = qApp->translate( "NetworkSettings", 
                    "<p>Error in postSection for file %1</p>" ).
                      arg( SF->name() );
            return S;
          }
        } while( 0 );
        SF->close();
      }
    }
    Force = 0;
    return S;
}

//
// GUI part
//

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

/*
    Called by the system to see if interface can be brought UP

    if allowed, echo Interface-allowed else Interface-disallowed
*/

void NetworkSettings::canStart( const char * Interface ) {
    // load situation
    NetworkSettingsData NSD;

    { Name2Connection_t & M = NSResources->connections();
      NodeCollection * NC;

      // for all connections
      for( QDictIterator<NodeCollection> it(M);
           it.current();
           ++it ) {
        NC = it.current();
        // check if this profile handles the requested interface
        if( NC->handlesInterface( Interface ) ) {
          switch( NC->state() ) {
            case Unchecked :
            case Unknown :
            case Unavailable :
            case Disabled :
              // this profile does not allow interface to be UP
              // -> try others
              break;
            case Off :
              // try to UP the device
              if( ! NC->setState( Activate ) ) {
                // cannot bring device Online -> try other alters
                break;
              }
              // FT
            case Available :
            case IsUp : // also called for 'down'
              // device is ready -> done
              printf( "%s-c%d-allowed\n", 
                  Interface, NC->number() );
              return;
          }
        }
      }
    }
    // if we come here no alternatives are possible
    printf( "%s-cnn-disallowed\n", Interface );
}

/*
    Called by the system to regenerate config files
*/

bool NetworkSettings::regenerate( void ) {
    QString S;
    // load situation
    NetworkSettingsData NSD;

    S = NSD.generateSettings( TRUE );
    if( ! S.isEmpty() ) {
      fprintf( stdout, "%s\n", S.latin1() );
      return 1;
    }
    return 0;
}
