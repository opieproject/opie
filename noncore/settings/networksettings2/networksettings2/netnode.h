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
#include <system.h>

// difference feature interfaces
class AsDevice;
class AsLine;
class AsNetworkSetup;
class AsFullSetup;

// needed for plugin creation function
#include <qlist.h>

class ANetNode;
class ANetNodeInstance;
class NetworkSetup;
class QTextStream;
class RuntimeInfo;
class InterfaceInfo;
class NSResources;

extern QString & deQuote( QString & X );
extern QString quote( QString X );

#include "systemfile.h"

typedef enum State {
    // if we have not yet detected the state of the device
    Unchecked = 0,
    // if we cannot determine the state
    Unknown = 1,
    // if networkSetup cannot be established e.g. because
    // the hardware is not available
    Unavailable = 2,
    // if the networkSetup cannot be establishec but NOT
    // because it is physically impossible but because
    // it has been disabled for FUNCTIONAL reasons
    Disabled = 3,
    // if networkSetup is available to is currently down
    // i.e. the corresponding hardware is not activated
    Off = 4,
    // if networkSetup is available to be used (i.e. the
    // devices if fully ready to be used
    Available = 5,
    // if networkSetup is being used
    IsUp = 6
} State_t;

typedef enum Action {
    // to make the device unavailable functionally -> to disabled
    Disable = 0,
    // to make the device available functionally -> to off
    Enable = 1,
    // bring the hardware up -> to Available
    Activate = 2,
    // bring the hardware down -> to off
    Deactivate = 3,
    // bring the networkSetup up -> to IsUp
    Up = 4,
    // bring the networkSetup down -> to Available
    Down = 5
} Action_t;

class ANetNode : public QObject {

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

    // does this Node provide a NetworkSetup
    bool isToplevel( void );

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
    virtual bool hasDataForFile( SystemFile & ) 
      { return 0; }

    // open proper file SF identified by S
    // this method is called by NS2.  
    //
    // overrule this ONLY if this proper file is a common file
    // for all NNI of this node class and the data generated
    // by each of the NNI needs to be put in one file
    //
    // if this is the case the file should be (re)opened in append
    // return 0 if file cannot be opened
    virtual bool openFile( SystemFile &SF,
                             ANetNodeInstance * NNI );

    // generate instance independent stuff
    // 0 : data output, 1 no data, 2 error
    virtual short generatePreamble( SystemFile & )
      { return 1; }

    // generate instance independent stuff
    // 0 : data output, 1 no data, 2 error
    virtual short generatePostamble( SystemFile & )
      { return 1; }

    // generate instance dependent but instance common stuff
    // 0 : data output, 1 no data, 2 error
    virtual short generateFile( SystemFile &,
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

    // return ID list for each file generated specially for
    // this node type
    virtual QStringList properFiles( void )
      { return QStringList(); }

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

    // return features this NetNode provides
    virtual const char ** provides( void ) = 0;

    // return features this NetNode needs
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

    inline const char ** provides( void )
      { return NodeType->provides(); }

    inline const char ** needs( void )
      { return NodeType->needs(); }

    inline void setNetworkSetup( NetworkSetup * NC ) 
      { TheNetworkSetup = NC; }
    inline NetworkSetup * networkSetup( void ) 
      { return TheNetworkSetup; }

    //
    //
    // Virtual methods with sensible defaults
    //
    //



    // open proper file identified by S
    virtual bool openFile( SystemFile & ) 
      { return 0; }

    // check if this node (or sub nodes) have data for this file
    virtual bool hasDataForFile( SystemFile & S ) 
      { return nodeClass()->hasDataForFile( S ); }

    // generate code specific for this node but embedded
    // in the section of the parent
    // this is called within the code of the parent
    virtual short generateFileEmbedded( SystemFile & SF,
                                       long DevNr ) 
      { ANetNodeInstance * NNI = nextNode();
        return (NNI) ? NNI->generateFileEmbedded( SF, DevNr ) : 1;
      }

    // generate code specific for this node 
    // (or find the first node that does)
    virtual short generateFile( SystemFile & SF,
                               long  DevNr ) 
      { ANetNodeInstance * NNI = nextNode();
        return (NNI) ? NNI->generateFile( SF, DevNr ) : 1;
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
    // networkSetup to which this node belongs to
    NetworkSetup * TheNetworkSetup;
    QString   Description;
    bool      IsModified;
    // true if this nodeinstance was just created (and not
    // loaded from file
    bool      IsNew;
    int       Done;

    static long InstanceCounter;
};

class ErrorNNI: public ANetNodeInstance {

public:

    ErrorNNI( const QString & _Name ) : ANetNodeInstance( 0 ) {
      setName( _Name.latin1() );
    }

    RuntimeInfo * runtime( void ) {
      return 0;
    }

    // create edit widget under parent
    QWidget * edit( QWidget * parent ) {
      return 0;
    }

    // is given data acceptable
    QString acceptable( void ) {
      return QString();
    }

    // get data from GUI and store in node
    void commit( void ) {
    }

    // returns node specific data -> only useful for 'buddy'
    void * data( void ) {
      return 0;
    }

protected :

    void setSpecificAttribute( QString & , QString & ) {
    }

    void saveSpecificAttribute( QTextStream & ) {
    }
};

class RuntimeInfo : public QObject {

      Q_OBJECT

public :

      RuntimeInfo( ANetNodeInstance * TheNNI )
        { NNI = TheNNI; }

      //
      //
      // methods to be overloaded by networkSetup capable
      // runtimes
      //
      //


      //
      //
      // methods to be overloaded by device capable
      // runtimes
      //
      //

      // does this node handles this interface e.g.eth0
      // recurse deeper if this node cannot answer that question
      virtual bool handlesInterface( const QString & S ) {
        RuntimeInfo * RI = device();
        if( RI ) {
          return RI->handlesInterface( S );
        }
        return 0;
      }
      bool handlesInterface( const InterfaceInfo & I ) {
        RuntimeInfo * RI = device();
        if( RI ) {
          return RI->handlesInterface( I );
        }
        return 0;
      }

      //
      //
      // methods to be overloaded by full setup capable
      // runtimes
      //
      //

      // return description for this full setup
      virtual const QString & description( void ) {
        return fullSetup()->description( );
      }
      // return triggers that should fire when this 
      // setup is brought up
      virtual const QStringList & triggers( void ) {
        return fullSetup()->triggers( );
      }

      //
      //
      // methods to be overloaded by line capable
      // runtimes
      //
      //

      // return the device file ('/dev/xxx') created
      // by this line capable runtime
      virtual QString deviceFile( void ) {
        RuntimeInfo * RI = line();
        if( RI ) {
          return RI->deviceFile();
        }
        return QString();
      }

      //
      //
      // runtime interface
      //
      //

      // return the node that offers device capability
      virtual RuntimeInfo * device( void )
        { RuntimeInfo * RI = nextNode();
          return (RI) ? RI->device() : 0;
        }

      // return the node that offers connection capability
      virtual RuntimeInfo * connection( void )
        { RuntimeInfo * RI = nextNode();
          return (RI) ? RI->connection() : 0;
        }

      // return the node that offers line capability
      virtual RuntimeInfo * line( void )
        { RuntimeInfo * RI = nextNode();
          return (RI) ? RI->line() : 0;
        }

      // return the node that offers full setup capability
      virtual RuntimeInfo * fullSetup( void )
        { RuntimeInfo * RI = nextNode();
          return (RI) ? RI->fullSetup() : 0;
        }

      inline ANetNodeInstance * netNode() 
        { return NNI; }

      inline NetworkSetup * networkSetup() 
        { return NNI->networkSetup(); }

      virtual State_t detectState( void ) = 0;
      // public API to set the state
      virtual QString setState( NetworkSetup * NC, 
                                Action_t A, 
                                bool Force = 0 );

      inline RuntimeInfo * nextNode( void ) {
         ANetNodeInstance * NNI = netNode()->nextNode();
         return (NNI) ? NNI->runtime() : 0;
      }

signals :

      // sent by device if state changes
      void stateChanged( State_t S, ANetNodeInstance * NNI );

protected :

      // set state of this node (private API)
      virtual QString setMyState( NetworkSetup * NC, 
                                Action_t A, 
                                bool Force = 0 ) = 0;

      // networkSetup this runtime info belongs to
      ANetNodeInstance * NNI;
};

class NetworkSetup : public QList<ANetNodeInstance> {

public :

      NetworkSetup( void );
      NetworkSetup( QTextStream & TS, bool & Dangling );
      ~NetworkSetup( void );

      // copy settings from NC to this 
      void copyFrom( const NetworkSetup & NC );

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

      // return the interface in the OS that is assigned to
      // this device
      inline InterfaceInfo * assignedInterface( void ) {
        return AssignedInterface;
      }

      // assign the interface to this device
      inline void assignInterface( InterfaceInfo * NI ) {
        // cleanup previous
        if( AssignedInterface ) {
          AssignedInterface->assignToNetworkSetup( 0 );
        }
        if( NI ) {
          // assign new
          NI->assignToNetworkSetup( this );
        }
        AssignedInterface = NI;
      }

      inline RuntimeInfo * device() {
        return getToplevel()->runtime()->device();
      }

      const QStringList & triggers();

      State_t state( bool Update = 0 );

      // get the ixmap for this device
      QPixmap devicePixmap( void );
      QPixmap statePixmap( State_t S );
      inline QPixmap statePixmap( bool Update = 0 ) 
        { return statePixmap( state(Update) ); }
      QString stateName( State_t );
      inline QString stateName( bool Update = 0 ) 
        { return stateName( state(Update) ); }

      QString setState( Action_t A, bool Force = 0 );

      void save( QTextStream & TS );

      void append( ANetNodeInstance * NNI );

      // makes sure that all items in the networkSetup point to 
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
      bool hasDataForFile( SystemFile & S );
      ANetNodeInstance * firstWithDataForFile( SystemFile & );

      // generate items for this file -> toplevel call
      short generateFile( SystemFile & SF,
                         long DN // device number
                       )
        { return getToplevel()->generateFile( SF, DN ); }

      bool triggeredBy( const QString & Trigger )
        { return getToplevel()->triggeredBy( Trigger ); }

private :

      int compareItems ( QCollection::Item item1, 
                         QCollection::Item item2 );

      long Number;

      // state of this networkSetup
      State_t CurrentState;

      QString Name;
      // true if this collection was just created (and not
      // loaded from file
      bool    IsNew;
      // index in listbox
      int     Index;
      bool    IsModified;
      int     Done;

      InterfaceInfo * AssignedInterface;

};

#endif
