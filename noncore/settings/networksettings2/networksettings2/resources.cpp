#include <qpixmap.h>
#include <qpe/qlibrary.h>
#include <qpe/qpeapplication.h>
#include <qdir.h>
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
      AllNodeTypes.insert( NN->NetNode->nodeName(), NN );
    }

    return 1;
}

QPixmap TheNSResources::getPixmap( const QString & QS ) {
    return Resource::loadPixmap( QString("networksettings2")+QS );
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
        if( findNodeInstance( NNI->nodeName() ) == 0 ) {
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
        removeNodeInstance( NNI->nodeName() );
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
