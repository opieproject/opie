#ifndef NETNODE_H
#define NETNODE_H

#include <qtextstream.h>
#include <qlist.h>
#include <qdict.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qobject.h>
#include <time.h>

#include <Utils.h>

// difference feature interfaces
class AsDevice;
class AsLine;
class AsConnection;
class AsFullSetup;

// needed for plugin creation function
#include <qlist.h>

class ANetNode;
class ANetNodeInstance;
class NodeCollection;
class QTextStream;
class RuntimeInfo;
class InterfaceInfo;

extern QString & deQuote( QString & X );
extern QString quote( QString X );

#include "systemfile.h"

typedef enum State {
    // if we have not yet detected the state of the device
    Unchecked = 0,
    // if we cannot determine the state
    Unknown = 1,
    // if connection cannot be established e.g. because
    // the hardware is not available
    Unavailable = 2,
    // if the connection cannot be establishec but NOT
    // because it is physically impossible but because
    // it has been disabled for FUNCTIONAL reasons
    Disabled = 3,
    // if connection is available to is currently down
    // i.e. the corresponding hardware is not activated
    Off = 4,
    // if connection is available to be used (i.e. the
    // devices if fully ready to be used
    Available = 5,
    // if connection is being used
    IsUp = 6
} State_t;

typedef enum Action {
    // to make the device unavailable functionally
    Disable = 0,
    // to make the device available functionally
    Enable = 1,
    // bring the hardware up
    Activate = 2,
    // bring the hardware down
    Deactivate = 3,
    // bring the connection up
    Up = 4,
    // bring the connection down
    Down = 5
} Action_t;

class ANetNode : public QObject{

public:

    typedef QArray<ANetNode *> NetNodeList;

    ANetNode( const char * Name ) : QObject( 0, Name ) {}
    virtual ~ANetNode(){};

    //
    //
    // standard methods with sensible default
    //
    //

    inline int done( void ) 
      { return Done; }
    inline void setDone( int D ) 
      { Done = D; }

    // does this Node provide a Connection
    inline bool isToplevel( void )
      { return strcmp( provides(), "fullsetup") == 0 ; }

    // set the value of an attribute
    void setAttribute( QString & Attr, QString & Value ) ;
    void saveAttributes( QTextStream & TS ) ;

    // compiled references to 'needed' NetNodes -> needs list
    inline void setAlternatives( NetNodeList * Alt )
      { Alternatives = Alt; }
    inline NetNodeList & alternatives( void ) 
      { return *Alternatives; }

    //
    //
    // Virtual methods with sensible default
    //
    //

    // do instances of this noce class have data for this file
    virtual bool hasDataForFile( const QString & ) 
      { return 0; }

    // generate instance independent stuff
    // 0 : data output, 1 no data, 2 error
    virtual short generateFile( const QString & ,
                               const QString & ,
                               QTextStream & ,
                               long ) 
      { return 1; }

    // generate instance dependent but profile common stuff
    // 0 : data output, 1 no data, 2 error
    virtual short generateFile( const QString & ,
                               const QString & ,
                               QTextStream & ,
                               ANetNodeInstance * ,
                               long ) 
      { return 1; }


    // generate NIC name based on instance nr
    // only relevant if node instances are devices
    virtual QString genNic( long ) 
      { return QString(""); }

    // max number of instances for this node type
    // only relevant if node instances are devices
    virtual long instanceCount( void ) 
      { return 1; }

    // return list of files that are specific for this node class
    virtual QStringList * properFiles( void )
      { return 0; }

    //
    //
    // pure virtual methods with sensible default
    //
    //

    // pixmap needed for this NetNode
    virtual const QString pixmapName() = 0;

    // description for  this NetNode
    virtual const QString nodeDescription() = 0;

    // create a blank instance of a net node
    virtual ANetNodeInstance * createInstance( void ) = 0;

    // return feature this NetNode provides
    virtual const char * provides( void ) = 0;
    virtual const char ** needs( void ) = 0;

protected :

    NetNodeList * Alternatives;

private :

    virtual void setSpecificAttribute( QString & , QString & ) = 0;
    virtual void saveSpecificAttribute( QTextStream & ) = 0;
    int Done;

};

class ANetNodeInstance : public QObject {

public:

    ANetNodeInstance( ANetNode * NN ) : QObject()
        { IsModified=0; NodeType = NN; IsNew = TRUE; }
    virtual ~ANetNodeInstance( void ) { }

    inline int done( void ) 
      { return Done; }
    inline void setDone( int D ) 
      { Done = D; }

    // return data was modified
    inline void setModified( bool M ) 
      {  IsModified = M; }
    inline bool isModified( void ) 
      {  return IsModified; }

    // get next node
    ANetNodeInstance * nextNode();
    // return NetNode this is an instance of
    ANetNode * nodeClass( void ) 
        { return NodeType; }

    // intialize am instance of a net node
    void initialize( void );

    // set the value of an attribute
    void setAttribute( QString & Attr, QString & Value ) ;
    void saveAttributes( QTextStream & TS ) ;

    // return true if node isntance is NEW and not loaded
    inline void setNew( bool IsN ) 
      { IsNew = IsN; }
    inline bool isNew( void ) 
      { return IsNew; }

    // return description for this instance
    inline QString & description( void ) 
      { return Description; }
    inline void setDescription( const QString & S ) 
      { Description = S; }

    // pixmap for this instance -> from NetNode
    inline const QString pixmapName( void )
      { return NodeType->pixmapName(); }

    inline const char * provides( void )
      { return NodeType->provides(); }

    inline const char ** needs( void )
      { return NodeType->needs(); }

    inline void setConnection( NodeCollection * NC ) 
      { Connection = NC; }
    inline NodeCollection * connection( void ) 
      { return Connection; }

    //
    //
    // Virtual methods with sensible defaults
    //
    //



    // open proper file identified by S
    virtual QFile * openFile( const QString & ) 
      { return 0; }

    // check if this node (or sub nodes) have data for this file
    virtual bool hasDataForFile( const QString & S ) 
      { return nodeClass()->hasDataForFile( S ); }

    // generate code specific for this node but embedded
    // in the section of the parent
    // this is called within the code of the parent
    virtual short generateFileEmbedded( const QString & ID,
                                       const QString & Path,
                                       QTextStream & TS,
                                       long DevNr ) 
      { ANetNodeInstance * NNI = nextNode();
        return (NNI) ? NNI->generateFileEmbedded( ID, Path, TS, DevNr ) : 1;
      }

    // generate code specific for this node 
    // (or find the first node that does)
    virtual short generateFile( const QString & ID,
                               const QString & Path,
                               QTextStream & TS,
                               long  DevNr ) 
      { ANetNodeInstance * NNI = nextNode();
        return (NNI) ? NNI->generateFile( ID, Path, TS, DevNr ) : 1;
      }

    // return true if this node instance is triggered by this trigger
    // could be delegated to deeper instances
    virtual bool triggeredBy( const QString & )
      { return 0; }

    //
    //
    // Pure virtual functions
    //
    //

    // return runtime information for this node
    virtual RuntimeInfo * runtime( void ) = 0;

    // create edit widget under parent
    virtual QWidget * edit( QWidget * parent ) = 0;

    // is given data acceptable
    virtual QString acceptable( void ) = 0;

    // get data from GUI and store in node
    virtual void commit( void ) = 0;

    // returns node specific data -> only useful for 'buddy'
    virtual void * data( void ) = 0;

protected :

    virtual void setSpecificAttribute( QString & , QString & ) = 0;
    virtual void saveSpecificAttribute( QTextStream & ) = 0;

    ANetNode * NodeType;
    // connection to which this node belongs to
    NodeCollection * Connection;
    QString   Description;
    bool      IsModified;
    bool      IsNew;
    int       Done;

    static long InstanceCounter;
};

class RuntimeInfo : public QObject {

      Q_OBJECT

public :

      RuntimeInfo( ANetNodeInstance * TheNNI )
        { NNI = TheNNI; }

      // downcast implemented by specify runtime classes
      virtual AsDevice * asDevice( void )
        { return 0; }
      virtual AsConnection * asConnection( void )
        { return 0; }
      virtual AsLine * asLine( void )
        { return 0; }
      virtual AsFullSetup * asFullSetup( void )
        { return 0; }

      // does this node handles this interface e.g.eth0
      // recurse deeper if this node cannot answer that question
      virtual bool handlesInterface( const QString & )
        { return 0; }
      virtual bool handlesInterface( const InterfaceInfo & )
        { return 0; }
      virtual InterfaceInfo * assignedInterface( void );
      virtual AsDevice * device( void );

      ANetNodeInstance * netNode() 
        { return NNI; }
      NodeCollection * connection() 
        { return NNI->connection(); }

      virtual void detectState( NodeCollection * NC ) = 0;
      virtual bool setState( NodeCollection * NC, Action_t A, bool Force = 0 ) = 0;
      virtual bool canSetState( State_t Curr, Action_t A ) = 0;

signals :

      // sent by device if state changes
      void stateChanged( State_t S, ANetNodeInstance * NNI );

protected :

      // connection this runtime info belongs to
      ANetNodeInstance * NNI;
};

class NodeCollection : public QList<ANetNodeInstance> {

public :

      NodeCollection( void );
      NodeCollection( QTextStream & TS );
      ~NodeCollection( void );

      inline int done( void ) 
        { return Done; }
      inline void setDone( int D ) 
        { Done = D; }

      inline int number( void ) 
        { return Number; }
      inline void setNumber( int i ) 
        { Number = i; }
      inline bool isNew( void ) 
        { return IsNew; }
      inline void setNew( bool N ) 
        { IsNew = N ; }
      inline bool isModified( void ) 
        { return IsModified; }
      inline void setModified( bool N ) 
        { IsModified = N ; }

      inline bool handlesInterface( const QString & S ) {
        return getToplevel()->runtime()->handlesInterface( S );
      }

      inline InterfaceInfo * assignedInterface( void ) {
        return getToplevel()->runtime()->assignedInterface();
      }

      inline AsDevice * device() {
        return getToplevel()->runtime()->device();
      }

      bool triggersVPN();

      inline State_t state( bool Update = 0 )
        { Log(( "%s state %d(=%d?)\n", Name.latin1(), CurrentState,
            Unchecked ));
          if( CurrentState == Unchecked || Update ) {
            Log(( "TL %p TLR %p\n", 
                    getToplevel(),
                    getToplevel()->runtime() ));
            // need to get current state
            getToplevel()->runtime()->detectState( this );
          } 
          return CurrentState;
        }

      // get the ixmap for this device
      QPixmap devicePixmap( void );
      QPixmap statePixmap( State_t S );
      inline QPixmap statePixmap( bool Update = 0 ) 
        { return statePixmap( state(Update) ); }
      QString stateName( State_t );
      inline QString stateName( bool Update = 0 ) 
        { return stateName( state(Update) ); }

      inline bool setState( Action_t A, bool Force =0 )
        { return getToplevel()->runtime()->setState( this, A, Force ); }
      inline bool canSetState( Action_t A )
        { return getToplevel()->runtime()->canSetState( CurrentState, A ); }

      void save( QTextStream & TS );

      void append( ANetNodeInstance * NNI );

      // makes sure that all items in the connection point to 
      // that connectoin
      void reassign( void );

      ANetNodeInstance * getToplevel( void );
      ANetNodeInstance * findNext( ANetNodeInstance * NNI );
      ANetNodeInstance * findByName( const QString & S );

      inline const QString & name()
        { return Name; }

      const QString & description( void );

      inline void setName( const QString & N)
        { Name = N; }

      inline State_t currentState( void )
        { return CurrentState; }
      inline void setCurrentState( State_t S )
        { CurrentState = S; }

      // return TRUE if this node can have data to be inserted in 
      // file identified by S
      bool hasDataForFile( const QString & S );
      ANetNodeInstance * firstWithDataForFile( const QString & S );

      // generate items for this file -> toplevel call
      short generateFile( const QString & FID,  // identification of file
                         const QString & FName, // effective filename of file
                         QTextStream & TS,  // stream to file
                         long DN // device number
                       )
        { return getToplevel()->generateFile( FID, FName, TS, DN ); }

      bool triggeredBy( const QString & Trigger )
        { return getToplevel()->triggeredBy( Trigger ); }

private :

      int compareItems ( QCollection::Item item1, 
                         QCollection::Item item2 );

      long Number;

      // state of this connection
      State_t CurrentState;

      QString Name;
      // true if this collection was just created (and not
      // loaded from file
      bool    IsNew;
      // index in listbox
      int Index;
      bool    IsModified;
      int Done;

};

#endif
