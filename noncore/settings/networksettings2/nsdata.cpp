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

    CfgFile.sprintf( "%s/NETCONFIG", getenv("HOME") );

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
      AsDevice * CurDev;
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
                if( ( NNI->netNode()->hasDataFor( SF->name(), 1 ) ||
                      NNI->netNode()->hasDataFor( SF->name(), 0 ) 
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
      // generate all system files
      //
      for( QDictIterator<SystemFile> sfit(SFM);
           sfit.current();
           ++sfit ) {
        SF = sfit.current();

        printf( "Generating %s\n", SF->name().latin1() );
        SF->open();

        do { // so we can break;

          // global presection for this system file
          if( SF->preSection() ) {
            S = qApp->translate( "NetworkSettings", 
                                 "<p>Error in preSection for file %1</p>" ).
                arg( SF->name() );
            return S;
          }

          // find all netnodes and figure out if
          // for that node there are instances 
          for( QDictIterator<NetNode_t> nnit(
                      NSResources->netNodes() );
               nnit.current();
               ++nnit ) {
            ANetNode * NN;

            NN = nnit.current()->NetNode;

              // are there instances ? 
              NNI = 0;
              for( QDictIterator<ANetNodeInstance> nniit(
                          NSResources->netNodeInstances() );
                   nniit.current();
                   ++nniit ) {
                if( nniit.current()->netNode() == NN ) { 
                  NNI = nniit.current();
                  break;
                }
              }

              if( ! NNI )
                // no instances
                continue;

            // has this node data for this system file ?
            if( (CurDev = NNI->runtime()->asDevice() ) ) {
              // generate start for this nodetype for all possible devices of this type
              for( int i = 0; i < CurDev->count(); i ++ ) {
                if( SF->preDeviceSection( NNI, i ) ) {
                  S = qApp->translate( "NetworkSettings", 
                      "<p>Error in preDeviceSection for file %1 and node %2</p>" ).
                      arg( SF->name() ).
                      arg( NN->nodeName() );
                  return S;
                }

                if( ! NN->hasDataFor( SF->name(), 1 ) ) {
                  if( NN->generateDeviceDataForCommonFile( *SF, i, NNI ) ) {
                    S = qApp->translate( "NetworkSettings", 
                      "<p>Error in node part for file %1 and node %2</p>" ).
                        arg( SF->name() ).
                        arg( NN->nodeName() );
                    return S;
                  }
                }
              }
            } else {
              // just request this once
              if( SF->preDeviceSection( NNI, -1 ) ) {
                S = qApp->translate( "NetworkSettings", 
                    "<p>Error in preDeviceSection for file %1 and node %2</p>" ).
                    arg( SF->name() ).
                    arg( NN->nodeName() );
                return S;
              }

              if( ! NN->hasDataFor( SF->name(), 1 ) ) {
                if( NN->generateDeviceDataForCommonFile( *SF, -1, NNI ) ) {
                  S = qApp->translate( "NetworkSettings", 
                    "<p>Error in node part for file %1 and node %2</p>" ).
                      arg( SF->name() ).
                      arg( NN->nodeName() );
                  return S;
                }
              }
            }

            // now generate profile specific data for all 
            // connections working on a device of the current
            // netnode type
            for( QDictIterator<NodeCollection> ncit(M);
                 ncit.current();
                 ++ncit ) {
              NC = ncit.current();

              NNI = NC->getToplevel();

              // no output needed
              if( ! NNI->netNode()->hasDataFor( SF->name(), 0 ) )
                continue;

              // get the netnode that serves as the device for this
              // connection
              AsDevice * Dev = NC->device();

              if( CurDev ) {
                if( CurDev != Dev ) {
                  // other device -> later
                  continue;
                }

                // generate 'entry' for every combination of device and profile
                // each node delegates to deeper level
                for( int i = 0; i < CurDev->count(); i ++ ) {
                  if( SF->preNodeSection( NNI, i ) ) {
                    S = qApp->translate( "NetworkSettings", 
                        "<p>Error in preNodeSection for file %1 and node %2</p>" ).
                        arg( SF->name() ).
                        arg( NNI->netNode()->nodeName() );
                    return S;
                  }

                  // ask all nodes in connection 

                  for( QListIterator<ANetNodeInstance> cncit(*NC); 
                       cncit.current();
                       ++cncit ) {
                    NNI = cncit.current();

                    if( NNI->netNode()->hasDataFor( SF->name(), 0 ) ) {
                      if( NNI->netNode()->generateDataForCommonFile(*SF,i,NNI) ) {
                        S = qApp->translate( "NetworkSettings", 
                          "<p>Error in node part for file %1 and node %2</p>" ).
                            arg( SF->name() ).
                            arg( NNI->netNode()->nodeName() );
                        return S;
                      }
                    }
                  }

                  if( SF->postNodeSection( NNI, i ) ) {
                    S = qApp->translate( "NetworkSettings", 
                        "<p>Error in postNodeSection for file %1 and node %2</p>" ).
                            arg( SF->name() ).
                            arg( NNI->netNode()->nodeName() );
                    return S;
                  }
                }

              } else {
                if( Dev ) {
                  // other 
                  continue;
                }

                // one entry to generate
                if( SF->preNodeSection( NNI, -1 ) ) {
                  S = qApp->translate( "NetworkSettings", 
                      "<p>Error in preNodeSection for file %1 and node %2</p>" ).
                      arg( SF->name() ).
                      arg( NNI->netNode()->nodeName() );
                  return S;
                }

                if( NNI->netNode()->generateDataForCommonFile(*SF,-1,NNI) ) {
                  S = qApp->translate( "NetworkSettings", 
                    "<p>Error in node part for file %1 and node %2</p>" ).
                      arg( SF->name() ).
                      arg( NNI->netNode()->nodeName() );
                  return S;
                }

                if( SF->postNodeSection( NNI, -1 ) ) {
                  S = qApp->translate( "NetworkSettings", 
                      "<p>Error in postNodeSection for file %1 and node %2</p>" ).
                          arg( SF->name() ).
                          arg( NNI->netNode()->nodeName() );
                  return S;
                }
              }

              // generated some data
              if( SF->postNodeSection( NNI, -1 ) ) {
                S = qApp->translate( "NetworkSettings", 
                    "<p>Error in postNodeSection for file %1 and node %2</p>" ).
                        arg( SF->name() ).
                        arg( NNI->netNode()->nodeName() );
                return S;
              }
              *SF << endl;
            }

            if( CurDev ) {
              // generate 'entry' for every combination of device and profile
              // each node delegates to deeper level
              for( int i = 0; i < CurDev->count(); i ++ ) {
                if( SF->postDeviceSection( NNI, i ) ) {
                  S = qApp->translate( "NetworkSettings", 
                      "<p>Error in postDeviceSection for file %1 and node %2</p>" ).
                          arg( SF->name() ).
                          arg( NNI->netNode()->nodeName() );
                  return S;
                }
              }
            } else {
              if( SF->postDeviceSection( NNI, -1 ) ) {
                S = qApp->translate( "NetworkSettings", 
                    "<p>Error in postDeviceSection for file %1 and node %2</p>" ).
                        arg( SF->name() ).
                        arg( NNI->netNode()->nodeName() );
                return S;
              }
            }
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
      if( NC->handlesInterface( Interface ) && // if different Intf.
          NC->state() != Disabled && // if not enabled
          NC->state() != IsUp  // if already used
        ) {
        PossibleConnections.append( NC );
      }
    }
    return PossibleConnections;
}


/*
    Called by the system to see if interface can be brought UP

    if allowed, echo Interface-allowed else Interface-disallowed
*/

void NetworkSettingsData::canStart( const char * Interface ) {
    // load situation
    NodeCollection * NC = 0;
    QList<NodeCollection> PossibleConnections;

    PossibleConnections = collectPossible( Interface );

    switch( PossibleConnections.count() ) {
      case 0 : // no connections
        break;
      case 1 : // one connection
        NC = PossibleConnections.first();
        break;
      default : // need to ask user ?
        // are we connected to a server
        // system( "su %d networksettings2 --prompt %s\n", 
        //    "", Interface );
        break;
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
          return;
      }
    } else {
      // if we come here no alternatives are possible
      printf( "%s-cnn-disallowed\n", Interface );
    }
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

