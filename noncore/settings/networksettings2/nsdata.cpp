#include <stdlib.h>
#include <qpe/qpeapplication.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>

#include "nsdata.h"
#include <asdevice.h>
#include <resources.h>

static QString CfgFile;

NetworkSettingsData::NetworkSettingsData( void ) {
    // init global resources structure
    new TheNSResources();

    CfgFile.sprintf( "%s/NETCONFIG", 
          NSResources->currentUser().HomeDir.latin1() );
    fprintf( stderr, "Cfg from %s\n", CfgFile.latin1() );

    // load settings
    Force = 0;
    IsModified = 0;
    loadSettings();
}

// saving is done by caller
NetworkSettingsData::~NetworkSettingsData( void ) {
    delete NSResources;
}

void NetworkSettingsData::loadSettings( void ) {
    QString Line, S;
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
        S = Line = TS.readLine();

        if ( S.isEmpty() || S[0] != '[' )
          continue;

        S = S.mid( 1, S.length()-2 );

        if( ! NSResources ) {
          continue;
        }

        if( S == "connection" ) {
          // load connections -> collections of nodes
          NodeCollection * NC = new NodeCollection( TS );
          NSResources->addConnection( NC );
        } else {
          ANetNode * NN = 0;
          ANetNodeInstance* NNI = 0;
          if( S.startsWith( "nodetype " ) ) {
            S = S.mid( 9, S.length()-9-1 );
            fprintf( stderr, "Node %s\n", S.latin1() );
            // try to find netnode
            NN = NSResources->findNetNode( S );
          } else {
            // try to find instance
            NNI = NSResources->createNodeInstance( S );
          }

          do {
            S = TS.readLine();

            if( NN || NNI ) {
              if( S.isEmpty() ) {
                // empty line
                break;
              }
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

              if( NN ) {
                // set the attribute
                NNI->setAttribute( Attr, Value );
              } else {
                // set the attribute
                NNI->setAttribute( Attr, Value );
              }
            } else {
              LeftOvers.append( Line );
              // add empty line too as delimiter
              if( S.isEmpty() ) {
                // empty line
                break;
              }
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
              "<p>Could not save setup to \"%1\" !</p>" ).
        arg(CfgFile);
      // problem
      return ErrS;
    }

    QTextStream TS( &F );

    // save leftovers
    for ( QStringList::Iterator it = LeftOvers.begin(); 
          it != LeftOvers.end(); ++it ) {
      TS << (*it) << endl;
    }

    // save global configs
    for( QDictIterator<NetNode_t> it( NSResources->netNodes() );
        it.current();
        ++it ) {
        TS << "[nodetype "
           << it.current()->NetNode->name()
           << "]" 
           << endl;

        it.current()->NetNode->saveAttributes( TS );
    }

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
          TS << '[' <<NNI->nodeClass()->nodeName() << ']' << endl;
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
    fprintf( stderr, "Generating settings from %s\n", CfgFile.latin1() );

    { Name2SystemFile_t & SFM = NSResources->systemFiles();
      Name2Connection_t & M = NSResources->connections();
      NodeCollection * NC;
      ANetNodeInstance * NNI;
      SystemFile * SF;
      AsDevice * CurDev;
      ANetNode * CurDevNN;
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
                if( ( NNI->nodeClass()->hasDataFor( SF->name() ) ||
                      NNI->hasDataFor( SF->name() ) 
                    ) &&
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

        for( QDictIterator<ANetNodeInstance> NNIIt(NNIs);
             NNIIt.current();
             ++NNIIt
           ){
          // for all nodes find those that are modified
          NNI = NNIIt.current();

          if( ForceIt || NNI->isModified() ) {
            if( ! NNI->nodeClass()->generateProperFilesFor( NNI ) ) {
              // problem generating
              S = qApp->translate( "NetworkSettings", 
                                   "<p>Cannot generate files proper to \"%1\"</p>" ).
                      arg(NNI->nodeClass()->nodeName()) ;
              return S;
            }
          }
        }
      }

      //
      // generate all system files
      //
      for( QDictIterator<SystemFile> sfit(SFM);
           sfit.current();
           ++sfit ) {
        SF = sfit.current();

        fprintf( stderr, "Generating %s\n", SF->name().latin1() );
        SF->open();

        do { // so we can break;

          // global presection for this system file
          if( SF->preSection() ) {
            S = qApp->translate( "NetworkSettings", 
                                 "<p>Error in preSection for file \"%1\"</p>" ).
                arg( SF->name() );
            return S;
          }

          // find all netnodes and figure out if
          // for that node there are instances 
          for( QDictIterator<NetNode_t> nnit(
                      NSResources->netNodes() );
               nnit.current();
               ++nnit ) {

            CurDevNN = nnit.current()->NetNode;

            // are there instances for this netnode ? 
            NNI = 0;
            for( QDictIterator<ANetNodeInstance> nniit(
                        NSResources->netNodeInstances() );
                 nniit.current();
                 ++nniit ) {
              if( nniit.current()->nodeClass() == CurDevNN ) { 
                NNI = nniit.current();
                break;
              }
            }

            if( ! NNI )
              // no instances -> next netnode type
              continue;

            // has this node data for this system file ?
            if( (CurDev = NNI->runtime()->asDevice() ) ) {
              // generate start for this nodetype for all possible devices of this type
              for( int i = 0; i < CurDevNN->instanceCount(); i ++ ) {
                S = generateSystemFileNode( *SF, CurDev, NNI, i );
                if( ! S.isEmpty() )
                  return S;
              }
            } else {
              S = generateSystemFileNode( *SF, 0, NNI, -1 );
              if( ! S.isEmpty() )
                return S;
            }
          }

          if( SF->postSection() ) {
            S = qApp->translate( "NetworkSettings", 
                    "<p>Error in postSection for file \"%1\"</p>" ).
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

QList<NodeCollection> NetworkSettingsData::collectPossible( const char * Interface ) {
    // collect connections that can work on top of this interface
    NodeCollection * NC;
    QList<NodeCollection> PossibleConnections;
    Name2Connection_t & M = NSResources->connections();

    // for all connections
    for( QDictIterator<NodeCollection> it(M);
         it.current();
         ++it ) {
      NC = it.current();
      // check if this profile handles the requested interface
      fprintf( stderr, "check %s\n", NC->name().latin1() );
      if( NC->handlesInterface( Interface ) && // if different Intf.
          NC->state() != Disabled && // if not enabled
          NC->state() != IsUp  // if already used
        ) {
        fprintf( stderr, "Append %s\n", NC->name().latin1() );
        PossibleConnections.append( NC );
      }
    }
    return PossibleConnections;
}


/*
    Called by the system to see if interface can be brought UP

    if allowed, echo Interface-allowed else Interface-disallowed
*/

bool NetworkSettingsData::canStart( const char * Interface ) {
    // load situation
    NodeCollection * NC = 0;
    QList<NodeCollection> PossibleConnections;

    PossibleConnections = collectPossible( Interface );

    fprintf( stderr, "Possiblilies %d\n", 
      PossibleConnections.count() );
    switch( PossibleConnections.count() ) {
      case 0 : // no connections
        break;
      case 1 : // one connection
        NC = PossibleConnections.first();
        break;
      default : // need to ask user ?
        return 1;
    }

    if( NC ) {
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
        case IsUp : // also called for 'ifdown'
          // device is ready -> done
          printf( "%s-c%d-allowed\n", Interface, NC->number() );
          return 0;
      }
    } 

    // if we come here no alternatives are possible
    printf( "%s-cnn-disallowed\n", Interface );
    return 0;
}

/*
    Called by the system to regenerate config files
*/

bool NetworkSettingsData::regenerate( void ) {
    QString S;
    // load situation
    S = generateSettings( TRUE );
    if( ! S.isEmpty() ) {
      fprintf( stdout, "%s\n", S.latin1() );
      return 1;
    }
    return 0;
}

QString NetworkSettingsData::generateSystemFileNode(
        SystemFile &SF,
        AsDevice * CurDev,
        ANetNodeInstance * DevNNI,
        long DevInstNr ) {

      QString S="";
      ANetNode * CurDevNN = DevNNI->nodeClass();
      Name2Connection_t & M = NSResources->connections();

      if( SF.preDeviceSection( CurDevNN ) ) {
        S = qApp->translate( "NetworkSettings", 
            "<p>Error in preDeviceSection for file \"%1\" and nodetype \"%2\"</p>" ).
            arg( SF.name() ).
            arg( CurDevNN->nodeName() );
        return S;
      }

      if( CurDevNN->hasDataFor( SF.name() ) ) {
        if( CurDevNN->generateDeviceDataForCommonFile( SF, DevInstNr ) ) {
          S = qApp->translate( "NetworkSettings", 
            "<p>Error in node Device part for file \"%1\" and node \"%2\"</p>" ).
              arg( SF.name() ).
              arg( CurDevNN->nodeName() );
          return S;
        }
      }

      if( CurDev ) 
        fprintf( stderr, "Cur %s\n", CurDevNN->nodeName().latin1() );
      else 
        fprintf( stderr, "Cur NO\n" );

      // now generate profile specific data for all 
      // connections working on a device of the current
      // netnode type
      for( QDictIterator<NodeCollection> ncit(M);
           ncit.current();
           ++ncit ) {
        NodeCollection * NC = ncit.current();

        // currenly only those connections that work on 
        // the current device (or on no device if no current)
        AsDevice * Dev = NC->device();

        fprintf( stderr, "%s\n", Dev->netNode()->nodeName().latin1() );
        if( CurDev ) {
          if( CurDevNN != Dev->netNode()->nodeClass() ) {
            // other device type -> later
            fprintf( stderr, "Other Dev type\n" );
            continue;
          }
        } else {
          if( Dev ) {
            // other 
            continue;
          }
        }

        // generate 'entry' 
        if( SF.preNodeSection( DevNNI, DevInstNr ) ) {
          S = qApp->translate( "NetworkSettings", 
              "<p>Error in preNodeSection for file \"%1\" and node \"%2\"</p>" ).
              arg( SF.name() ).
              arg( CurDevNN->nodeName() );
          return S;
        }

        // ask all nodes in connection 
        for( QListIterator<ANetNodeInstance> cncit(*NC); 
             cncit.current();
             ++cncit ) {
          ANetNodeInstance * NNI = cncit.current();

          if( NNI->hasDataFor( SF.name() ) ) {
            if( NNI->generateDataForCommonFile(SF,DevInstNr) ) {
              S = qApp->translate( "NetworkSettings", 
                "<p>Error in node part for file \"%1\" and node \"%2\"</p>" ).
                  arg( SF.name() ).
                  arg( NNI->nodeClass()->nodeName() );
              return S;
            }
          }
        }

        if( SF.postNodeSection( DevNNI, DevInstNr ) ) {
          S = qApp->translate( "NetworkSettings", 
              "<p>Error in postNodeSection for file \"%1\" and node \"%2\"</p>" ).
                  arg( SF.name() ).
                  arg( CurDevNN->nodeName() );
          return S;
        }
        SF << endl;
      }

      if( SF.postDeviceSection( CurDevNN ) ) {
        S = qApp->translate( "NetworkSettings", 
            "<p>Error in postDeviceSection for file \"%1\" and node \"%2\"</p>" ).
                arg( SF.name() ).
                arg( CurDevNN->nodeName() );
        return S;
      }

      return S;
}
