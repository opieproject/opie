#ifndef NETNODE_H
#define NETNODE_H

#include <qtextstream.h>
#include <qlist.h>
#include <qdict.h>
#include <qpixmap.h>
#include <qobject.h>
#include <time.h>

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

  // pixmap needed for this NetNode
  virtual const QString pixmapName() = 0;

  // description for  this NetNode
  virtual const QString nodeDescription() = 0;

  // create a blank instance of a net node
  virtual ANetNodeInstance * createInstance( void ) = 0;

  // return feature this NetNode provides
  virtual const char * provides( void ) = 0;
  virtual const char ** needs( void ) = 0;

  // generate files specific for this node (if any)
  virtual bool generateProperFilesFor( ANetNodeInstance * NNI ) = 0;
  // return TRUE if this node has data to be inserted in systemfile
  // with name S
  virtual bool hasDataFor( const QString & S ) = 0;
  // generate data specific for the device for the system file S
  // called only IF data was needed
  virtual bool generateDeviceDataForCommonFile( 
      SystemFile & SF, long DevNr ) = 0;

  // does this Node provide a Connection
  bool isToplevel( void )
    { return strcmp( provides(), "fullsetup") == 0 ; }

  // generate NIC name based on instance nr
  // only relevant if node instances are devices
  virtual QString genNic( long ) 
    { return QString(""); }

  // max number of instances for this node type
  // only relevant if node instances are devices
  virtual long instanceCount( void ) 
    { return 1; }

  // set the value of an attribute
  void setAttribute( QString & Attr, QString & Value ) ;
  void saveAttributes( QTextStream & TS ) ;

  // compiled references to 'needed' NetNodes -> needs list
  void setAlternatives( NetNodeList * Alt )
    { Alternatives = Alt; }
  NetNodeList & alternatives( void ) 
    { return *Alternatives; }

protected :

  NetNodeList * Alternatives;

private :

  virtual void setSpecificAttribute( QString & , QString & ) = 0;
  virtual void saveSpecificAttribute( QTextStream & ) = 0;

};

class ANetNodeInstance : public QObject {

public:

  ANetNodeInstance( ANetNode * NN ) : QObject()
      { IsModified=0; NodeType = NN; IsNew = TRUE; }
  virtual ~ANetNodeInstance( void ) { }

  virtual RuntimeInfo * runtime( void ) = 0;

  void setConnection( NodeCollection * NC ) 
    { Connection = NC; }
  NodeCollection * connection( void ) 
    { return Connection; }

  // create edit widget under parent
  virtual QWidget * edit( QWidget * parent ) = 0;
  // is given data acceptable
  virtual QString acceptable( void ) = 0;

  // return data was modified
  void setModified( bool M ) 
    {  IsModified = M; }
  bool isModified( void ) 
    {  return IsModified; }

  // get data from GUI and store in node
  virtual void commit( void ) = 0;

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
  void setNew( bool IsN ) 
    { IsNew = IsN; }
  bool isNew( void ) 
    { return IsNew; }

  // return description for this instance
  QString & description( void ) 
    { return Description; }
  void setDescription( const QString & S ) 
    { Description = S; }

  // pixmap for this instance -> from NetNode
  const QString pixmapName( void )
    { return NodeType->pixmapName(); }

  const char * provides( void )
    { return NodeType->provides(); }

  const char ** needs( void )
    { return NodeType->needs(); }

  // returns node specific data -> only useful for 'buddy'
  virtual void * data( void ) = 0;

  // return TRUE if this node has data to be inserted in systemfile
  // with name S
  virtual bool hasDataFor( const QString & S ) = 0;

  // generate data specific for a profile and for the system file S
  // called only IF data was needed
  virtual bool generateDataForCommonFile( 
      SystemFile & SF, long DevNr ) = 0;

protected :

  virtual void setSpecificAttribute( QString & , QString & ) = 0;
  virtual void saveSpecificAttribute( QTextStream & ) = 0;

  ANetNode * NodeType;
  // connection to which this node belongs to
  NodeCollection * Connection;
  QString   Description;
  bool      IsModified;
  bool      IsNew;

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
      virtual bool setState( NodeCollection * NC, Action_t A ) = 0;
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

      int number( void ) 
        { return Number; }
      void setNumber( int i ) 
        { Number = i; if( MaxNr < i ) MaxNr = i; }
      bool isNew( void ) 
        { return IsNew; }
      void setNew( bool N ) 
        { IsNew = N ; }
      bool isModified( void ) 
        { return IsModified; }
      void setModified( bool N ) 
        { IsModified = N ; }

      bool handlesInterface( const QString & S ) {
        return getToplevel()->runtime()->handlesInterface( S );
      }

      InterfaceInfo * assignedInterface( void ) {
        return getToplevel()->runtime()->assignedInterface();
      }

      AsDevice * device() {
        return getToplevel()->runtime()->device();
      }

      State_t state( bool Update = 0 )
        { if( CurrentState == Unchecked || Update ) {
            // need to get current state
            getToplevel()->runtime()->detectState( this );
          } 
          return CurrentState;
        }

      // get the ixmap for this device
      QPixmap devicePixmap( void );
      QPixmap statePixmap( State_t S );
      QPixmap statePixmap( bool Update = 0 ) 
        { return statePixmap( state(Update) ); }
      QString stateName( State_t );
      QString stateName( bool Update = 0 ) 
        { return stateName( state(Update) ); }

      bool setState( Action_t A )
        { return getToplevel()->runtime()->setState( this, A ); }
      bool canSetState( Action_t A )
        { return getToplevel()->runtime()->canSetState( CurrentState, A ); }

      void save( QTextStream & TS );

      void append( ANetNodeInstance * NNI );

      // makes sure that all items in the connection point to 
      // that connectoin
      void reassign( void );

      ANetNodeInstance * getToplevel( void );
      ANetNodeInstance * findNext( ANetNodeInstance * NNI );
      ANetNodeInstance * findByName( const QString & S );

      const QString & name()
        { return Name; }

      const QString & description( void );

      void setName( const QString & N)
        { Name = N; }

      State_t currentState( void )
        { return CurrentState; }
      void setCurrentState( State_t S )
        { CurrentState = S; }

      long maxConnectionNumber( void )
        { return MaxNr; }

      static void resetMaxNr( void )
        { MaxNr = -1; }

private :

      int compareItems ( QCollection::Item item1, 
                         QCollection::Item item2 );

      static long MaxNr;
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

};

#endif
