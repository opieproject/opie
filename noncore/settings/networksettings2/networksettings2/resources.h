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

#ifdef MYPLUGIN

typedef struct NetNode_S {
        ANetNode * NetNode;
        QLibrary * TheLibrary;
        long NodeCountInLib;
} NetNode_t;
typedef QDict<NetNode_t> Name2NetNode_t;

#else

typedef QDict<ANetNode>  Name2NetNode_t;

#endif

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
#ifdef MYPLUGIN
    ANetNode * findNetNode( const QString & N )
      { NetNode_t * NNT = AllNodeTypes.find(N);
        return (NNT) ? NNT->NetNode : 0;
      }
#else
    ANetNode * findNetNode( const QString & N )
      { return AllNodeTypes.find(N);
      }
#endif
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
      { ANetNodeInstance * NNI = 0;
        printf( "Find node type %s\n", S.latin1() );
#ifdef MYPLUGIN
        NetNode_t * NNT = AllNodeTypes[S];
        if( ! NNT ) {
          return 0;
        }
        NNI = NNT->NetNode->createInstance();
#else
        ANetNode * NNT = AllNodeTypes[S];
        if( ! NNT ) {
          return 0;
        }
        NNI = NNT->createInstance();
#endif
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

    void addConnection( NodeCollection * NC );
    void removeConnection( const QString & N );
    NodeCollection * findConnection( const QString & N );
    NodeCollection * getConnection( int nr );
    Name2Connection_t & connections( void )
      { return ConnectionsMap; }

    inline bool userKnown( void )
      { return CurrentUser.known(); }
    CurrentQPEUser & currentUser( void )
      { return CurrentUser; }

private :

    void detectCurrentUser( void );
    QString tr( const char * path );

#ifdef MYPLUGIN
    void findAvailableNetNodes( const QString &path );
    bool loadNetNode(
         const QString &pluginFileName,
         const QString &resolveString = "create_plugin");
#else
    void findAvailableNetNodes( void );
#endif

    QMap< QString, QString>   NodeTypeNameMap;
    QMap< QString, QString>   NodeTypeDescriptionMap;
    Name2Connection_t         ConnectionsMap;
    System *                  TheSystem;
    Name2SystemFile_t         SystemFiles;

    // all node type classes 
    Name2NetNode_t            AllNodeTypes;

    // all nodes
    Name2Instance_t           AllNodes;

    CurrentQPEUser            CurrentUser;

#ifndef MYPLUGIN
    Opie::Core::OPluginLoader *  Plugins;
    Opie::Core::OPluginManager * PluginManager;
#endif

};

extern TheNSResources * _NSResources;
#define NSResources _NSResources

#endif
