#include <qpe/qpeapplication.h>
#include <time.h>
#include <qtextstream.h>
#include <qpixmap.h>

#include "resources.h"
#include "netnode.h"

#include "asdevice.h"
#include "asline.h"
#include "asconnection.h"
#include "asfullsetup.h"

QString & deQuote( QString & X ) {
    if( X[0] == '"' ) {
      // remove end and trailing "" and \x -> x
      QChar R;
      long idx;
      idx = X.length()-1;
      X = X.mid( 1, idx );

      idx = 0;
      while( ( idx = X.find( '\\', idx ) ) >= 0 ) {
        R = X[idx+1];
        X.replace( idx, 2, &R, 1 );
      }
      X = X.left( X.length()-1 );
    }
    return X;
}

QString quote( QString X ) {
    if( X.find( QRegExp( "[ \n\"\\\t]" ) ) >= 0 ) {
      // need to quote this
      QString OutString = "\"";

      X.replace( QRegExp("\""), "\\\"" );
      X.replace( QRegExp("\\"), "\\\\" );
      X.replace( QRegExp(" "), "\\ " );

      OutString += X;
      OutString += "\"";
      X = OutString;
    }
    return X;
}


//
//
// ANETNODE
//
//

void ANetNode::saveAttributes( QTextStream & TS ) {
    saveSpecificAttribute( TS );
}

void ANetNode::setAttribute( QString & Attr, QString & Value ){
    setSpecificAttribute( Attr, Value );
}

//
//
// ANETNODEINSTANCE
//
//

long ANetNodeInstance::InstanceCounter = -1;

void ANetNodeInstance::initialize( void ) {
    if( InstanceCounter == -1 )
      InstanceCounter = time(0);
    // set name
    QString N;
    N.sprintf( "-%ld", InstanceCounter++ );
    N.prepend( NodeType->nodeName() );
    setNodeName( N );
}

void ANetNodeInstance::setAttribute( QString & Attr, QString & Value ){
    if( Attr == "name" ) {
      NodeName = Value;
    } else {
      setSpecificAttribute( Attr, Value );
    }
}

void ANetNodeInstance::saveAttributes( QTextStream & TS ) {
    TS << "name=" << quote( NodeName ) << endl;
    saveSpecificAttribute( TS );
}

ANetNodeInstance * ANetNodeInstance::nextNode( void ) {
    return connection()->findNext( this );
}

//
//
// NODECOLLECTION
//
//

long NodeCollection::MaxNr = -1;

NodeCollection::NodeCollection( void ) : QList<ANetNodeInstance>() {
    IsModified = 0;
    Index = -1;
    Name="";
    IsNew = 1;
    CurrentState = Unchecked;
}

NodeCollection::NodeCollection( QTextStream & TS ) :
      QList<ANetNodeInstance>() {
    long idx;
    bool InError = 0;
    QString S, A, N;
    IsModified = 0;
    Index = -1;
    Name="";
    IsNew = 0;
    CurrentState = Unchecked;

    do {
      S = TS.readLine();
      if( S.isEmpty() ) {
        if( InError ) {
          // remove all nodes
          clear();
        }
        // empty line
        break;
      }

      idx = S.find('=');
      S.stripWhiteSpace();
      A = S.left( idx );
      A.lower();
      N = S.mid( idx+1, S.length() );
      N.stripWhiteSpace();
      N = deQuote( N );

      if( A == "name" ) {
        Name = N;
      } else if( A == "number" ) {
        setNumber( N.toLong() );
      } else if( A == "node" ) {
        ANetNodeInstance * NNI = NSResources->findNodeInstance( N );
        if( NNI && ! InError ) {
          append( NSResources->findNodeInstance( N ) );
        } else {
          // could not find a node type -> collection invalid
          InError = 1;
        }
      }
    } while( 1 );
}


NodeCollection::~NodeCollection( void ) {
}

const QString & NodeCollection::description( void ) {
    ANetNodeInstance * NNI = getToplevel();
    return (NNI) ? NNI->runtime()->asFullSetup()->description() : Name;
}

void NodeCollection::append( ANetNodeInstance * NNI ) {
    NNI->setConnection( this );
    QList<ANetNodeInstance>::append( NNI );
}

void NodeCollection::save( QTextStream & TS ) {

    TS << "name=" << quote( Name ) << endl;
    TS << "number=" << number() << endl;
    ANetNodeInstance * NNI;
    for( QListIterator<ANetNodeInstance> it(*this); 
         it.current();
         ++it ) {
      NNI = it.current();
      TS << "node=" << quote( NNI->nodeName() ) << endl;
    }
    TS << endl;
    IsNew = 0;
}

ANetNodeInstance * NodeCollection::getToplevel( void ) {
    ANetNodeInstance * NNI = 0;
    for( QListIterator<ANetNodeInstance> it(*this); 
         it.current();
         ++it ) {
      NNI = it.current();
      if( NNI->nodeClass()->isToplevel() )
        break;
    }
    return NNI;
}

ANetNodeInstance * NodeCollection::findByName( const QString & S ) {
    ANetNodeInstance * NNI = 0;
    for( QListIterator<ANetNodeInstance> it(*this); 
         it.current();
         ++it ) {
      NNI = it.current();
      if( NNI->name() == S )
        break;
    }
    return NNI;
}

ANetNodeInstance * NodeCollection::findNext( ANetNodeInstance * NNI ) {
    ANetNodeInstance * NNNI;

    if( ! NNI )
      getToplevel();

    for( QListIterator<ANetNodeInstance> it(*this); 
         it.current();
         ++it ) {
      NNNI = it.current();
      if( NNNI == NNI ) {
        ++it;
        return it.current();
      }
    }
    return 0; // no more next
}

int NodeCollection::compareItems( QCollection::Item I1, 
                                  QCollection::Item I2 ) {
    ANetNodeInstance * NNI1, * NNI2;
    NNI1 = (ANetNodeInstance *)I1;
    NNI2 = (ANetNodeInstance *)I2;
    return NNI1->nodeName().compare( NNI2->nodeName() );
}

static char * State2PixmapTbl[] = {
    "NULL", // Unchecked : no pixmap
    "check", // Unknown 
    "delete", // unavailable 
    "disabled", // disabled 
    "off", // off 
    "disconnected", // available 
    "connected" // up 
};

QPixmap NodeCollection::devicePixmap( void ) {
    return NSResources->getPixmap(
          device()->netNode()->pixmapName()+"-large" );
}

QPixmap NodeCollection::statePixmap( State_t S) {
    return NSResources->getPixmap( State2PixmapTbl[S] );
}

QString NodeCollection::stateName( State_t S) {
    switch( S ) {
      case Unknown :
        return qApp->translate( "networksettings2", "Unknown");
      case Unavailable :
        return qApp->translate( "networksettings2", "Unavailable");
      case Disabled :
        return qApp->translate( "networksettings2", "Disabled");
      case Off :
        return qApp->translate( "networksettings2", "Off");
      case Available :
        return qApp->translate( "networksettings2", "Available");
      case IsUp :
        return qApp->translate( "networksettings2", "IsUp");
      case Unchecked : /* FT */
      default :
        break;
    }
    return QString("");
}

void NodeCollection::reassign( void ) {
    for( QListIterator<ANetNodeInstance> it(*this); 
         it.current();
         ++it ) {
      it.current()->setConnection( this );
    }
}

//
//
// RUNTIMEINFO
//
//

InterfaceInfo * RuntimeInfo::assignedInterface( void ) { 
    return netNode()->nextNode()->runtime()->assignedInterface(); 
}

AsDevice * RuntimeInfo::device( void ) { 
    return netNode()->nextNode()->runtime()->device(); 
}
