#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <qpixmap.h>
#include <qdir.h>
#include <qpe/qlibrary.h>
#include <qpe/qpeapplication.h>
#include <opie2/odebug.h>
#include <qtopia/resource.h>

#include "netnode.h"
#include "resources.h"

#define PLUGINDIR "plugins/networksettings2"
#define ICONDIR "/pics/networksettings2/"

// single resources instance
TheNSResources * _NSResources = 0;

TheNSResources::TheNSResources( void ) : NodeTypeNameMap(),
        ConnectionsMap() {

    _NSResources = this;

    // load available netnodes
    findAvailableNetNodes(QPEApplication::qpeDir() + PLUGINDIR );

    // compile provides and needs lists
    { const char ** NeedsRun;
      QDictIterator<NetNode_t> OuterIt( AllNodeTypes );
      bool Done;

      for ( ; OuterIt.current(); ++OuterIt ) {
        // find needs list
        ANetNode::NetNodeList * NNLP = new ANetNode::NetNodeList;
        ANetNode::NetNodeList & NNL = *(NNLP);

        // must iterate this way to avoid duplication pointers
        for ( QDictIterator<NetNode_t> InnerIt( AllNodeTypes );
              InnerIt.current(); ++InnerIt ) {
          if( InnerIt.current() == OuterIt.current() )
            // avoid recursive 
            continue;

          const char * Provides = InnerIt.current()->NetNode->provides();
          NeedsRun = OuterIt.current()->NetNode->needs();
          for( ; *NeedsRun; NeedsRun ++ ) {
            if( strcmp( Provides, *NeedsRun ) == 0 ) {
              // inner provides what outer needs
              NNL.resize( NNL.size() + 1 );
              NNL[NNL.size()-1] = InnerIt.current()->NetNode;
              Done = 1; // break from 2 loops
              break;
            }
          }
        }
        OuterIt.current()->NetNode->setAlternatives( NNLP );
      }
    }

    // define Node types to Description map
    NodeTypeNameMap.insert( "device", tr( "Network Device" ) );
    NodeTypeNameMap.insert( "line", tr( "Character device" ) );
    NodeTypeNameMap.insert( "connection", tr( "IP Connection" ) );
    NodeTypeNameMap.insert( "fullsetup", tr( "Connection Profile" ) );

    NodeTypeDescriptionMap.insert( "device", 
        tr( "<p>Devices that can handle IP packets</p>" ) );
    NodeTypeDescriptionMap.insert( "line", 
        tr( "<p>Devices that can handle single bytes</p>" ) );
    NodeTypeDescriptionMap.insert( "connection", 
        tr( "<p>Nodes that provide working IP connections</p>" ) );
    NodeTypeDescriptionMap.insert( "fullsetup", 
        tr( "<p>Fully configured connection profile</p>" ) );

    // define system files
    addSystemFile( new SystemFile( "interfaces", "./interfaces" ) );

    // get access to the system
    TheSystem = new System();

    detectCurrentUser();
}

TheNSResources::~TheNSResources( void ) {
    delete TheSystem;
}

/**
 * Load all modules that are found in the path
 * @param path a directory that is scaned for any plugins that can be loaded
 *  and attempts to load them
 */
void TheNSResources::findAvailableNetNodes(const QString &path){

    QDir d(path);
    if(!d.exists())
      return;

    QString lang = ::getenv("LANG");
    
    // Don't want sym links
    d.setFilter( QDir::Files | QDir::NoSymLinks );
    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator it( *list );
    QFileInfo *fi;

    while ( (fi=it.current()) ) {

      if( fi->fileName().contains(".so")){
        /* if loaded install translation */
        if( loadNetNode(path + "/" + fi->fileName()) ) {
          QTranslator *trans = new QTranslator(qApp);
          QString fn = QPEApplication::qpeDir()+
                    "/i18n/"+lang+"/"+ 
                    fi->fileName().left( fi->fileName().find(".") )+
                    ".qm";

          if( trans->load( fn ) )
              qApp->installTranslator( trans );
          else
              delete trans;
        }
      }
      ++it;
    }
}

/**
 * Attempt to load a function and resolve a function.
 * @param pluginFileName - the name of the file in which to attempt to load
 * @param resolveString - function pointer to resolve
 * @return true of loading is successful
 */
bool TheNSResources::loadNetNode(
      const QString &pluginFileName, const QString &resolveString){

    QLibrary *lib = new QLibrary(pluginFileName);
    void * res = lib->resolve(resolveString);
    if( ! res ){
      delete lib;
      return 0;
    }

    GetNetNodeListFt_t getNetNodeList = (GetNetNodeListFt_t)res;
    
    // Try to get an object.
    QList<ANetNode> PNN;

    getNetNodeList( PNN );
    if( PNN.isEmpty() ) {
      delete lib;
      return 0;
    }

    ANetNode * NNP;
    for( QListIterator<ANetNode> it(PNN);
         it.current();
         ++it ) {
      NetNode_t * NN;

      NNP = it.current();
      NN = new NetNode_t;
      NN->NetNode = NNP;
      NN->TheLibrary = lib;
      NN->NodeCountInLib = PNN.count();

      // store mapping
      printf( "Store %s\n", NN->NetNode->name() );
      AllNodeTypes.insert( NN->NetNode->name(), NN );
    }

    return 1;
}

QPixmap TheNSResources::getPixmap( const QString & QS ) {
    QString S("networksettings2/");
    S += QS;
    fprintf( stderr, "%s\n", S.latin1() );
    return Resource::loadPixmap( S );
}

QString TheNSResources::tr( const char * s ) {
    return qApp->translate( "resource", s );
}

const QString & TheNSResources::netNode2Name( const char * s ) {
    return NodeTypeNameMap[s];
}

const QString & TheNSResources::netNode2Description( const char * s ) {
    return NodeTypeDescriptionMap[s];
}

void TheNSResources::addConnection( NodeCollection * NC ) {
      ANetNodeInstance * NNI;
      ConnectionsMap.insert( NC->name(), NC );
      // add (new) nodes to NodeList
      for( QListIterator<ANetNodeInstance> it(*NC);
           it.current();
           ++it ) {
        NNI = it.current();
        if( findNodeInstance( NNI->name() ) == 0 ) {
          // new item
          addNodeInstance( NNI );
        }
      }
}

void TheNSResources::removeConnection( const QString & N ) {
      NodeCollection * NC = findConnection( N );
      if( ! NC )
        return;

      // delete netnodes in this connection
      ANetNodeInstance * NNI;
      for( NNI = NC->first(); NNI != 0; NNI = NC->next() ) {
        removeNodeInstance( NNI->name() );
      }
      ConnectionsMap.remove( N ); 
}

NodeCollection * TheNSResources::findConnection( const QString & S ) {
      return ConnectionsMap[ S ];
}

void TheNSResources::renumberConnections( void ) {
      Name2Connection_t & M = NSResources->connections();
      NodeCollection * NC;

      // for all connections
      NodeCollection::resetMaxNr();
      for( QDictIterator<NodeCollection> it(M);
           it.current();
           ++it ) {
        NC = it.current();
        NC->setNumber( NC->maxConnectionNumber()+1 );
        NC->setModified( 1 );
      }
}

typedef struct EnvVars {
      char * Name;
      int Len;
} EnvVar_t;

#define AnEV(x) x, sizeof(x)-1

static EnvVar_t EV[] = {
    // AnEV( "HOME=" ), -> SPECIAL
    // AnEV( "LOGNAME=" ), -> SPECIAL
    AnEV( "USER=" ),
    AnEV( "LD_LIBRARY_PATH=" ),
    AnEV( "PATH=" ),
    AnEV( "QTDIR=" ),
    AnEV( "OPIEDIR=" ),
    AnEV( "SHELL=" ),
    { NULL, 0 }
};

void TheNSResources::detectCurrentUser( void ) {
    // find current running qpe
    QString QPEEnvFile = "";

    // open proc dir and find all dirs in it
    { QRegExp R("[0-9]+");
      QDir ProcDir( "/proc" );
      QString QPELoc = QPEApplication::qpeDir() + "bin/qpe";
      QFileInfo FI;
      QStringList EL = ProcDir.entryList( QDir::Dirs );

      // print it out
      for ( QStringList::Iterator it = EL.begin(); 
            it != EL.end(); 
            ++it ) {
        if( R.match( (*it) ) >= 0 ) {
          QString S = ProcDir.path()+"/"+ (*it);
          S.append( "/exe" );
          FI.setFile( S );
          // get the linke
          S = FI.readLink();
          if( S == QPELoc ) {
            // found running qpe
            QPEEnvFile.sprintf( ProcDir.path()+ "/" + (*it) + "/environ" );
            break;
          }
        }
      }
    }

    if( QPEEnvFile.isEmpty() ) {
      // could not find qpe
      fprintf( stderr, "Could not find qpe\n" );
      return;
    }

    // FI now contains path ProcDir to the cmd dir
    { char * Buf = 0;
      char TB[1024];
      long BufSize = 0;
      int fd;
      int rd;

      fd = open( QPEEnvFile.latin1(), O_RDONLY );
      if( fd < 0 ) {
        fprintf( stderr, "Could not open %s : %d\n", 
            QPEEnvFile.latin1(), errno );
        return;
      }

      while( (rd = read( fd, TB, sizeof(TB) ) ) > 0 ) {
        Buf = (char *)realloc( Buf, BufSize+rd );
        memcpy( Buf+BufSize, TB, rd );
        BufSize += rd;
      }

      char * Data = Buf;
      char * DataEnd = Data+BufSize-1;

      // get env items out of list
      while( Data < DataEnd ) {
        if( strncmp( Data, "LOGNAME=", 8 ) == 0 ) {
          CurrentUser.UserName = Data+8;
          CurrentUser.EnvList.resize( CurrentUser.EnvList.size()+1 );
          CurrentUser.EnvList[CurrentUser.EnvList.size()-1] = 
              strdup( Data );
        } else if( strncmp( Data, "HOME=", 5 ) == 0 ) {
          CurrentUser.HomeDir = Data+5;
          CurrentUser.EnvList.resize( CurrentUser.EnvList.size()+1 );
          CurrentUser.EnvList[CurrentUser.EnvList.size()-1] = 
              strdup( Data );
        } else {
          EnvVar_t * Run = EV;
          while( Run->Name ) {
            if( strncmp( Data, Run->Name, Run->Len ) == 0 ) {
              CurrentUser.EnvList.resize( CurrentUser.EnvList.size()+1 );
              CurrentUser.EnvList[CurrentUser.EnvList.size()-1] = 
                  strdup( Data );
              break;
            }
            Run ++;
          }
        }

        Data += strlen( Data )+1;
      }

      free( Buf );

      if( ! CurrentUser.UserName.isEmpty() ) {
        // find user info
        struct passwd pwd;
        struct passwd * pwdres;

        if( getpwnam_r( CurrentUser.UserName.latin1(), 
                        &pwd, TB, sizeof(TB), &pwdres ) ||
            pwdres == 0 ) {
          fprintf( stderr, "Could not determine user %s : %d\n", 
              CurrentUser.UserName.latin1(), errno );
          return;
        }
        CurrentUser.Uid = pwd.pw_uid;
        CurrentUser.Gid = pwd.pw_gid;
      } else{
        CurrentUser.Uid = 
          CurrentUser.Gid = -1;
      }
    }
}
