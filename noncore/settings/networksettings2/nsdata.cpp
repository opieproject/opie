#include <stdlib.h>
#include <opie2/odebug.h>
#include <qpe/qpeapplication.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>

#include "nsdata.h"
#include <netnode.h>
#include <resources.h>

static QString CfgFile;

NetworkSettingsData::NetworkSettingsData( void ) {
    // init global resources structure
    new TheNSResources();

    if( ! NSResources->userKnown() ) {
      Log(( "Cannot detect qpe user HOME=\"%s\" USER=\"%s\"\n",
          NSResources->currentUser().HomeDir.latin1(),
          NSResources->currentUser().UserName.latin1() ));
      return;
    }

    CfgFile.sprintf( "%s/Settings/NS2.conf", 
          NSResources->currentUser().HomeDir.latin1() );
    Log(( "Cfg from %s\n", CfgFile.latin1() ));

    // load settings
    loadSettings();

    // assign interfaces by scanning /tmp/profile-%s.Up files
    { QDir D( "/tmp" );
      QFile * F = new QFile;
      int profilenr;
      QString interfacename;
      QTextStream TS ( F );

      QStringList SL = D.entryList( "profile-*.up");

      Log(( "System reports %d interfaces. Found %d up\n", 
          NSResources->system().interfaces().count(),
          SL.count() ));

      for ( QStringList::Iterator it = SL.begin(); 
            it != SL.end(); 
            ++it ) {
        profilenr = atol( (*it).mid( 8 ).latin1() );
        // read the interface store int 'up'
        F->setName( D.path() + "/" + (*it) );
        if( F->open( IO_ReadOnly ) ) {
          NodeCollection * NC;
          interfacename = TS.readLine();
          F->close();

          Log(( "Assign interface %s to Profile nr %d\n", 
                interfacename.latin1(), profilenr ));

          NC = NSResources->getConnection( profilenr );
          if( NC ) {
            NC->assignInterface( 
                NSResources->system().findInterface( interfacename ) );
          } else {
            Log(( "Profile nr %d no longer defined\n", 
                profilenr ));
          }
        }
      }
    }
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

    ForceModified = 0;

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
            S = S.mid( 9, S.length()-9 );
            S = deQuote(S);
            // try to find netnode
            NN = NSResources->findNetNode( S );
          } else {
            // try to find instance
            NNI = NSResources->createNodeInstance( S );
          }

          if( NN == 0 && NNI == 0 ) {
            LeftOvers.append( Line );
          }

          do {
            S = Line = TS.readLine();

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
                NN->setAttribute( Attr, Value );
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
            Log( ( "NodeInstance %s : %p\n", NNI->name(), NNI  ));
            NNI->setNew( FALSE );
            NSResources->addNodeInstance( NNI );
          }
          if( NN ) {
            Log( ( "Node %s : %p\n", NN->name(), NN ) );
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

    Log( ( "Saving settings to %s\n", CfgFile.latin1()  ));
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
           << quote( QString( it.current()->NetNode->name() ) )
           << "]" 
           << endl;

        it.current()->NetNode->saveAttributes( TS );
        TS << endl;
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
          TS << '[' 
             << QString(NNI->nodeClass()->name()) 
             << ']' 
             << endl;
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


    for( QDictIterator<NodeCollection> it(NSResources->connections());
         it.current();
         ++it ) {
      it.current()->setModified( 0 );
    }

    return ErrS;
}

QString NetworkSettingsData::generateSettings( void ) {
    QString S = "";
    Name2SystemFile_t & SFM = NSResources->systemFiles();
    Name2Connection_t & M = NSResources->connections();
    NodeCollection * NC;
    ANetNodeInstance * NNI;
    ANetNodeInstance * FirstWithData;
    RuntimeInfo * CurDev;
    ANetNode * NN, * CurDevNN = 0;
    long NoOfDevs;
    long DevCtStart;
    bool needToGenerate;

    // regenerate system files
    Log( ( "Generating settings from %s\n", CfgFile.latin1()  ));

    for( QDictIterator<NetNode_t> nnit( NSResources->netNodes() );
         nnit.current();
         ++nnit ) {
      { QStringList SL;
        bool FirstItem = 1;
        bool Generated = 0;

        CurDevNN = nnit.current()->NetNode;
        SL = CurDevNN->properFiles();

        for ( QStringList::Iterator it = SL.begin(); 
              it != SL.end(); 
              ++it ) {

          Generated = 0;
          FirstItem = 1;
          // iterate over NNI's of this class
          for( QDictIterator<ANetNodeInstance> nniit(
                                 NSResources->netNodeInstances() );
               nniit.current();
               ++nniit ) {
            if( nniit.current()->nodeClass() != CurDevNN )
              // different class
              continue;

            // open proper file
            { SystemFile SF( (*it) );

              if( ! CurDevNN->openFile( SF, nniit.current()) ) {
                // cannot open
                S = qApp->translate( "NetworkSettings", 
                  "<p>Cannot open proper file \"%1\" for node \"%2\"</p>" ).
                    arg( (*it) ).arg( CurDevNN->name() );
                return S;
              }

              if( ! SF.open() ) {
                S = qApp->translate( "NetworkSettings", 
                  "<p>Cannot open proper file \"%1\" for node \"%2\"</p>" ).
                    arg( (*it) ).arg( CurDevNN->name() );
                return S;
              }

              // preamble on first
              if( FirstItem ) {
                if( CurDevNN->generatePreamble( SF ) == 2 ) {
                  S = qApp->translate( "NetworkSettings", 
                    "<p>Error in section \"preamble\" for proper file \"%1\" and node \"%2\"</p>" ).
                      arg( (*it) ).
                      arg( CurDevNN->name() );
                  return S;
                }
              }
              FirstItem = 0;
              Generated = 1;

              // item specific
              if( nniit.current()->generateFile( SF, -1 ) == 2 ) {
                S = qApp->translate( "NetworkSettings", 
                  "<p>Error in section for node \"%1\" for proper file \"%2\" and node class \"%3\"</p>" ).
                    arg( nniit.current()->name() ).
                    arg( (*it) ).
                    arg( CurDevNN->name() );
                return S;
              }
            }
          }

          if( Generated ) {
            SystemFile SF( (*it) );

            if( CurDevNN->openFile( SF, 0 ) &&
                ! SF.path().isEmpty()
              ) {

              if( ! SF.open() ) {
                S = qApp->translate( "NetworkSettings", 
                  "<p>Cannot open proper file \"%1\" for node \"%2\"</p>" ).
                    arg( (*it) ).arg( CurDevNN->name() );
                return S;
              }

              if( CurDevNN->generatePostamble( SF ) == 2 ) {
                S = qApp->translate( "NetworkSettings", 
                  "<p>Error in section \"postamble\" for proper file \"%1\" and node \"%2\"</p>" ).
                    arg( (*it) ).
                    arg( CurDevNN->name() );
                return S;
              }
            } // no postamble
          }
        }
      }
    }

    //
    // generate all registered files
    //
    for( QDictIterator<SystemFile> sfit(SFM);
         sfit.current();
         ++sfit ) {
      SystemFile * SF;

      SF = sfit.current();

      // reset all 
      for( QDictIterator<NetNode_t> nnit( NSResources->netNodes() );
           nnit.current();
           ++nnit ) {
        nnit.current()->NetNode->setDone(0);
      }

      for( QDictIterator<ANetNodeInstance> nniit(
                  NSResources->netNodeInstances() );
           nniit.current();
           ++nniit ) {
        nniit.current()->setDone(0);
      }

      for( QDictIterator<NodeCollection> ncit(M);
           ncit.current();
           ++ncit ) {
        ncit.current()->setDone(0);
      }

      Log( ( "Generating system file %s\n", SF->name().latin1() ));

      needToGenerate = 0;

      // are there netnodes that have instances and need 
      // to write data in this system file ?
      for( QDictIterator<NetNode_t> nnit( NSResources->netNodes() );
           ! needToGenerate && nnit.current();
           ++nnit ) {

        NN = nnit.current()->NetNode;

        if( NN->hasDataForFile( *SF ) ) {
          // netnode can have data

          // are there instances of this node ?
          for( QDictIterator<ANetNodeInstance> nniit(
                      NSResources->netNodeInstances() );
               ! needToGenerate && nniit.current();
               ++nniit ) {
            if( nniit.current()->nodeClass() == NN ) {
              // yes
              Log(("Node %s has data\n",
                  nniit.current()->name() ));
              needToGenerate = 1;
              break;
            }
          }
        }
      }

      if( ! needToGenerate ) {
        // no instances found that might need to write data
        // in this systemfile
        Log(("No nodes for systemfile %s\n", SF->name().latin1() ));
        continue;
      }

      // ok generate this system file
      if( ! SF->open() ) {
        S = qApp->translate( "NetworkSettings", 
                             "<p>Cannot open system file \"%1\"</p>" ).
            arg( SF->name() );
        return S;
      }

      // global presection for this system file
      if( ! SF->preSection() ) {
        S = qApp->translate( "NetworkSettings", 
                             "<p>Error in section \"Preamble\" for file \"%1\"</p>" ).
            arg( SF->name() );
        return S;
      }

      // find connections that want to write to this file
      for( QDictIterator<NodeCollection> ncit(M);
           ncit.current();
           ++ncit ) {

        NC = ncit.current();

        if( NC->done() ) {
          // already done
          continue;
        }

        if( ! NC->hasDataForFile( *SF ) ) {
          // no data
          continue;
        }

        Log(("Generating %s for connection %s\n",
              SF->name().latin1(), NC->name().latin1() ));
        // find highest item that wants to write data to this file
        FirstWithData = NC->firstWithDataForFile( *SF );

        // find device on which this connection works
        CurDev = NC->device();
        // class of that node
        CurDevNN = CurDev->netNode()->nodeClass();

        if( ! FirstWithData->nodeClass()->done() ) {
          // generate fixed part 
          if( ! SF->preDeviceSection( CurDevNN ) ) {
            S = qApp->translate( "NetworkSettings", 
                                 "<p>Error in section \"Pre-Device\" for file \"%1\"</p>" ).
                arg( SF->name() );
            return S;
          }

          if( FirstWithData->nodeClass()->generateFile( 
                                            *SF, 
                                            FirstWithData,
                                            -2 ) == 2 ) {
            S = qApp->translate( "NetworkSettings", 
              "<p>Error in section \"Common\" for file \"%1\" and node \"%2\"</p>" ).
                arg( SF->name() ).
                arg( CurDevNN->name() );
            return S;
          }
          FirstWithData->nodeClass()->setDone( 1 );
          Log(( "Systemfile %s for node instance %s is done\n",
            SF->name().latin1(),
            FirstWithData->name() ));
        }

        NoOfDevs = 0;
        DevCtStart = -1;

        if( SF->knowsDeviceInstances() ) {
          DevCtStart = 0;
          NoOfDevs = CurDevNN->instanceCount();
        }

        if( ! CurDev->netNode()->nodeClass()->done() ) {
          // first time this device is handled
          // generate common device specific part
          for( int i = DevCtStart; i < NoOfDevs ; i ++ ) {

            if( FirstWithData->nodeClass()->generateFile( 
                *SF, CurDev->netNode(), i ) == 2 ) {
              S = qApp->translate( "NetworkSettings", 
                "<p>Error in section \"Device\" for file \"%1\" and node \"%2\"</p>" ).
                  arg( SF->name() ).
                  arg( CurDevNN->name() );
              return S;
            }
          }
          CurDev->netNode()->nodeClass()->setDone( 1 );

          Log(( "Systemfile %s for Nodeclass %s is done\n",
            SF->name().latin1(),
            CurDev->netNode()->nodeClass()->name()
            ));
        }

        // generate profile specific info
        // for all nodeconnections that work on the same device
        for( QDictIterator<NodeCollection> ncit2(M);
             ncit2.current();
             ++ncit2 ) {

          if( ncit2.current()->device() != CurDev ) {
            // different device
            continue;
          }

          Log(("Connection %s of family %s\n", 
                ncit2.current()->name().latin1(), 
                CurDev->name() ));
          // generate 
          NNI = ncit2.current()->firstWithDataForFile( *SF );
          for( int i = DevCtStart; i < NoOfDevs ; i ++ ) {
            if( ! SF->preNodeSection( NNI, i ) ) {
              S = qApp->translate( "NetworkSettings", 
                  "<p>Error in \"Pre-Node Part\" for file \"%1\" and node \"%2\"</p>" ).
                  arg( SF->name() ).
                  arg( CurDevNN->name() );
              return S;
            }

            switch( NNI->generateFile( *SF, i ) ) {
              case 0 :
                (*SF) << endl;
                break;
              case 1 :
                break;
              case 2 :
                S = qApp->translate( "NetworkSettings", 
                  "<p>Error in section \"Node\" for file \"%1\" and node \"%2\"</p>" ).
                    arg( SF->name() ).
                    arg( CurDevNN->name() );
                return S;
            }

            if( ! SF->postNodeSection( NNI, i ) ) {
              S = qApp->translate( "NetworkSettings", 
                  "<p>Error in \"Post-Node Part\" for file \"%1\" and node \"%2\"</p>" ).
                  arg( SF->name() ).
                  arg( CurDevNN->name() );
              return S;
            }
          }

          ncit2.current()->setDone( 1 );

        }
      }

      if( ! SF->postDeviceSection( CurDevNN ) ) {
        S = qApp->translate( "NetworkSettings", 
            "<p>Error in section \"Post-Device\" for file \"%1\" and node \"%2\"</p>" ).
                arg( SF->name() ).
                arg( CurDevNN->name() );
        return S;
      }


      if( ! SF->postSection() ) {
        S = qApp->translate( "NetworkSettings", 
                "<p>Error in section \"Closure\" for file \"%1\"</p>" ).
                  arg( SF->name() );
        return S;
      }

      // end of file
      SF->close();
    }
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
        Log( ( "Append %s for %s\n", NC->name().latin1(), Interface));
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

    Log( ( "for %s : Possiblilies %d\n", 
        Interface, PossibleConnections.count() ));
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
          { QString S= NC->setState( Activate );
            if( ! S.isEmpty() ) {
              // could not bring device Online -> try other alters
              Log(( "%s-c%d-disallowed : %s\n", 
                  Interface, NC->number(), S.latin1() ));
              break;
            }
            // interface assigned 
          }
          // FT
        case Available :
        case IsUp : // also called for 'ifdown'
          // device is ready -> done
          Log(( "%s-c%d-allowed\n", Interface, NC->number() ));
          printf( "%s-c%d-allowed\n", Interface, NC->number() );
          return 0;
      }
    } 

    // if we come here no alternatives are possible
    Log(( "%s-cnn-disallowed\n", Interface ));
    printf( "%s-cnn-disallowed\n", Interface );
    return 0;
}

bool NetworkSettingsData::isModified( void ) {
    if( ForceModified )
      return 1;

    for( QDictIterator<NodeCollection> it(NSResources->connections());
         it.current();
         ++it ) {
      if( it.current()->isModified() ) {
        return 1;
      }
    }
    return 0;
}
