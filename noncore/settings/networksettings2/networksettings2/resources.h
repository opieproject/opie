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

typedef void (*GetNetNodeListFt_t)(QList<ANetNode>& PNN );

typedef struct NetNode_S {
        ANetNode * NetNode;
        QLibrary * TheLibrary;
        long NodeCountInLib;
} NetNode_t;

class CurrentQPEUser {

public :
        CurrentQPEUser() : UserName(), HomeDir(), EnvList() {}

        QString        UserName;
        QString        HomeDir;
        int            Uid;
        int            Gid;
        QArray<char *> EnvList;
};

typedef QDict<NetNode_t> Name2NetNode_t;
typedef QDict<ANetNodeInstance > Name2Instance_t;
typedef QDict<NodeCollection> Name2Connection_t;
typedef QDict<SystemFile> Name2SystemFile_t;

class TheNSResources {

public :

    TheNSResources( void );
    ~TheNSResources( );

    System & system()
      { return *TheSystem; }

    QPixmap getPixmap( const QString & Name );

    Name2NetNode_t & netNodes( void ) 
      { return AllNodeTypes; }
    bool netNodeExists( const QString & X )
      { return AllNodeTypes.find(X)!=0; }

    Name2SystemFile_t & systemFiles( void )
      { return SystemFiles; }
    void addSystemFile( SystemFile * SF )
      { SystemFiles.insert( SF->name(), SF ); }

    ANetNodeInstance * createNodeInstance( const QString & S )
      { ANetNodeInstance * NNI = 0;
        NetNode_t * NNT = AllNodeTypes[S];
        ANetNode * NN;
        if( ! NNT ) {
          NN = FakeNode = 
            ( FakeNode ) ?  FakeNode : new FakeNetNode();
        } else {
          NN = NNT->NetNode;
        }
        NNI = NN->createInstance();
        NNI->initialize();
        return NNI;
      }

    Name2Instance_t & netNodeInstances( void ) 
      { return AllNodes; }
    void addNodeInstance( ANetNodeInstance * I )
      { AllNodes.insert( I->nodeName(), I ); }
    void removeNodeInstance( const QString & N )
      { AllNodes.remove( N );}
    ANetNodeInstance * findNodeInstance( const QString & S )
      { return (AllNodes.find(S)!=0) ? AllNodes[S] : 0; }

    const QString & netNode2Name( const char * Type );
    const QString & netNode2Description( const char * Type );

    void renumberConnections( void );
    void addConnection( NodeCollection * NC );
    void removeConnection( const QString & N );
    NodeCollection * findConnection( const QString & N );
    Name2Connection_t & connections( void )
      { return ConnectionsMap; }

    CurrentQPEUser & currentUser( void )
      { return CurrentUser; }

private :

    void detectCurrentUser( void );
    QString tr( const char * path );
    void findAvailableNetNodes( const QString &path );
    bool loadNetNode(
         const QString &pluginFileName,
         const QString &resolveString = "create_plugin");

    QMap< QString, QString>   NodeTypeNameMap;
    QMap< QString, QString>   NodeTypeDescriptionMap;
    Name2Connection_t ConnectionsMap;
    System * TheSystem;
    Name2SystemFile_t SystemFiles;

    // all node type classes 
    Name2NetNode_t      AllNodeTypes;

    // all nodes
    Name2Instance_t     AllNodes;

    CurrentQPEUser    CurrentUser;
};

extern TheNSResources * _NSResources;
#define NSResources _NSResources

#endif
