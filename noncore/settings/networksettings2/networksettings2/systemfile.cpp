#include <stdio.h>
#include <qpe/qpeapplication.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qtextstream.h>

#include "resources.h"
#include "systemfile.h"

#define TEMPLATEDIR "networktemplates/"
QString TemplDir;

SystemFile::SystemFile( const QString & N, const QString & P ){
      Name = N;
      Path = P;
      F = 0;
      // get template info
      { QString S;
        QFileInfo FI;

        // find location of templates
        TemplDir = QPEApplication::qpeDir() + "etc/" + TEMPLATEDIR;
        FI.setFile( TemplDir );
        if( ! FI.isDir() ) {
          // try current dir
          TemplDir = "./" TEMPLATEDIR;
          FI.setFile( TemplDir );
          if( ! FI.isDir() ) {
            hasPreSection = 
              hasPostSection = 
              hasPreNodeSection = 
              hasPostNodeSection = 
              hasPreDeviceSection = 
              hasPostDeviceSection = 0;
            return;
          }
        }

        // have found location
        S = TemplDir + Name + "/presection";
        FI.setFile( S );
        hasPreSection = ( FI.exists() && FI.isReadable() );
        S = TemplDir + Name + "/postsection";
        FI.setFile( S );
        hasPostSection = ( FI.exists() && FI.isReadable() );
        S = TemplDir + Name + "/prenodesection";
        FI.setFile( S );
        hasPreNodeSection = ( FI.exists() && FI.isReadable() );
        S = TemplDir + Name + "/postnodesection";
        FI.setFile( S );
        hasPostNodeSection = ( FI.exists() && FI.isReadable() );
        S = TemplDir + Name + "/predevicesection";
        FI.setFile( S );
        hasPreDeviceSection = ( FI.exists() && FI.isReadable() );
        S = TemplDir + Name + "/postdevicesection";
        FI.setFile( S );
        hasPostDeviceSection = ( FI.exists() && FI.isReadable() );
      }
}

SystemFile::~SystemFile( void ) {
      if( F )
        delete F;
}

bool SystemFile::open( void ) {
      if( F ) {
        F->close();
        delete F;
      }

      F = new QFile( Path + "bup" );
      if( ! F->open( IO_WriteOnly ) ) {
        return 0;
      }
      setDevice( F );
      return 1;
}

bool SystemFile::close( void ) {
      if( ! F ) {
        return 1 ;
      }

      QString OldP = Path + "bup";

      F->close();
      delete F;
      F = 0;

      return ( rename( OldP.latin1(), Path.latin1() ) >= 0 );
}

bool SystemFile::preSection( void ) {
      if( hasPreSection ) {
        QFile Fl( TemplDir + Name + "/presection" );
        if( ! Fl.open( IO_ReadOnly ) )
          return 1; // error
        // copy file to this file
        F->writeBlock( Fl.readAll() );
      }
      return 0;
}

bool SystemFile::postSection( void ) {
      if( hasPostSection ) {
        QFile Fl( TemplDir + Name + "/postsection" );
        if( ! Fl.open( IO_ReadOnly ) )
          return 1; // error
        // copy file to this file
        F->writeBlock( Fl.readAll() );
      }
      return 0;
}

bool SystemFile::preNodeSection( ANetNodeInstance * NNI, long ) {
      if( hasPreNodeSection ) {
        QFile Fl( TemplDir + Name + "/prenodesection" );
        if( ! Fl.open( IO_ReadOnly ) )
          return 1; // error
        QTextStream TX( &Fl );
        QString Out;
        QString S = TX.readLine();
        while( ! TX.eof() ) {
          Out = S.
              arg(NNI->nodeClass()->nodeName());
          (*this) << Out << endl;
          S = TX.readLine();
        }
      }
      return 0;
}

bool SystemFile::postNodeSection( ANetNodeInstance * NNI, long ) {
      if( hasPostNodeSection ) {
        QFile Fl( TemplDir + Name + "/postnodesection" );
        if( ! Fl.open( IO_ReadOnly ) )
          return 1; // error
        QTextStream TX( &Fl );
        QString Out;
        QString S = TX.readLine();
        while( ! TX.eof() ) {
          Out = S.
              arg(NNI->nodeName());
          (*this) << Out << endl;
          S = TX.readLine();
        }
      }
      return 0;
}

bool SystemFile::preDeviceSection( ANetNode * NN ) {
      if( hasPreDeviceSection ) {
        QFile Fl( TemplDir + Name + "/predevicesection" );
        if( ! Fl.open( IO_ReadOnly ) )
          return 1; // error
        QTextStream TX( &Fl );
        QString Out;
        QString S = TX.readLine();
        while( ! TX.eof() ) {
          Out = S.arg(NN->nodeName());
          (*this) << Out << endl;
          S = TX.readLine();
        }
      }
      return 0;
}

bool SystemFile::postDeviceSection( ANetNode * NN ) {
      if( hasPostDeviceSection ) {
        QFile Fl( TemplDir + Name + "/postdevicesection" );
        if( ! Fl.open( IO_ReadOnly ) )
          return 1; // error
        QTextStream TX( &Fl );
        QString Out;
        QString S = TX.readLine();
        while( ! TX.eof() ) {
          Out = S.arg(NN->nodeName());
          (*this) << Out << endl;
          S = TX.readLine();
        }
      }
      return 0;
}
