#ifndef __RESOURCES__H
#define __RESOURCES__H

#include <qstring.h>
#include <qdict.h>
#include <qmap.h>
#include <qlist.h>
#include "netnode.h"
#include "systemfile.h"
#include "system.h"

class QLibrary;
class QPixmap;
class ANetNode;
class ANetNodeInstance;

namespace Opie {
    namespace Core {
        class OPluginLoader;
        class OPluginManager;
    }
}

typedef void (*GetNetNodeListFt_t)(QList<ANetNode>& PNN );

class CurrentQPEUser {

public :
        CurrentQPEUser() : UserName(), HomeDir(), EnvList() {}

        inline bool known( void )
          { return ! HomeDir.isEmpty() && ! UserName.isEmpty(); }

        QString        UserName;
        QString        HomeDir;
        int            Uid;
        int            Gid;
        QArray<char *> EnvList;
};

typedef QDict<ANetNode>  Name2NetNode_t;
typedef QDict<ANetNodeInstance > Name2Instance_t;
typedef QDict<NodeCollection> Name2Connection_t;
typedef QDict<SystemFile> Name2SystemFile_t;

class TheNSResources {

public :

    TheNSResources( void );
    ~TheNSResources( );

    // give busy feedback
    void busy( bool B );

    System & system()
      { return *TheSystem; }

    int assignConnectionNumber(void);
    QPixmap getPixmap( const QString & Name );

    Name2NetNode_t & netNodes( void ) 
      { return AllNodeTypes; }
    bool netNodeExists( const QString & X )
      { return AllNodeTypes.find(X)!=0; }
    ANetNode * findNetNode( const QString & N )
      { return AllNodeTypes.find(N);
      }
    // define new plugin (=node)
    void addNodeType( const QString & ID,
                      const QString & LongName,
                      const QString & Description );

    Name2SystemFile_t & systemFiles( void )
      { return SystemFiles; }
    void addSystemFile( const QString & ID, 
                        const QString & P,
                        bool KDI );

    ANetNodeInstance * createNodeInstance( const QString & S )
      { ANetNode * NN = findNetNode( S );

        Log(( "Find node type %s : %p\n", S.latin1(), NN ));

        if( NN == 0 ) 
          // type of this instance not found
          return 0;

        ANetNodeInstance * NNI = NN->createInstance();
        NNI->initialize();
        return NNI;
      }

    Name2Instance_t & netNodeInstances( void ) 
      { return AllNodes; }
    void addNodeInstance( ANetNodeInstance * I )
      { AllNodes.insert( I->name(), I ); }
    void removeNodeInstance( const QString & N )
      { AllNodes.remove( N );}
    ANetNodeInstance * findNodeInstance( const QString & S )
      { return AllNodes[S]; }

    const QString & netNode2Name( const char * Type );
    const QString & netNode2Description( const char * Type );

    void addConnection( NodeCollection * NC, bool Dangling );
    void removeConnection( const QString & N );
    NodeCollection * findConnection( const QString & N );
    NodeCollection * getConnection( int nr );
    Name2Connection_t & connections( void )
      { return ConnectionsMap; }
    Name2Connection_t & danglingConnections( void )
      { return ConnectionsMap; }

    inline bool userKnown( void )
      { return CurrentUser.known(); }
    CurrentQPEUser & currentUser( void )
      { return CurrentUser; }

private :

    void detectCurrentUser( void );
    QString tr( const char * path );

    void findAvailableNetNodes( void );

    QMap< QString, QString>   NodeTypeNameMap;
    QMap< QString, QString>   NodeTypeDescriptionMap;
    // list of connections that are valid
    Name2Connection_t         ConnectionsMap;
    // list of connection configurations that are not valid
    // e.g. because plugins are missing
    Name2Connection_t         DanglingConnectionsMap;
    System *                  TheSystem;
    Name2SystemFile_t         SystemFiles;

    // all node type classes 
    Name2NetNode_t            AllNodeTypes;

    // all nodes
    Name2Instance_t           AllNodes;

    CurrentQPEUser            CurrentUser;

    Opie::Core::OPluginLoader *  Plugins;
    Opie::Core::OPluginManager * PluginManager;

};

extern TheNSResources * _NSResources;
#define NSResources _NSResources

#endif
