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

SystemFile::SystemFile( const QString & N, 
                        const QString & P,
                        bool KDI ){
      Name = N;
      Path = P;
      InAppend = 0;
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

      KnowsDeviceInstances = KDI;
}

SystemFile::SystemFile( const QString & N, bool KDI ){
      Name = N;
      Path = "";
      InAppend = 0;
      F =0;
      KnowsDeviceInstances = KDI;
      hasPreSection = 
        hasPostSection = 
        hasPreNodeSection = 
        hasPostNodeSection = 
        hasPreDeviceSection = 
        hasPostDeviceSection = 0;
}

SystemFile::~SystemFile( void ) {
      close();
}

bool SystemFile::open( void ) {
      QString Prefix = getenv( "NS2OUTPUTTO" );

      if( Prefix != "stderr" /* && Name != "interfaces" */ ) {
        // generate files where the need to be
        if( F ) {
          F->close();
          delete F;
        }

        F = new QFile( Prefix + Path + ((InAppend)?"":"bup") );
        Log(( "Open systemfile %s\n", F->name().latin1() ));
        if( ! F->open( ((InAppend)?IO_Append : 0 ) | IO_WriteOnly ) ) {
          return 0;
        }
      } else {
        if( ! F ) {
          owarn << "!!!!!!!!!!!!!!!!!! " << oendl;
          owarn << "!!!! TESTMODE !!!!" << oendl;
          owarn << "!!!!!!!!!!!!!!!!!! " << oendl;
          owarn << "!!!!" << oendl;
          owarn << "!!!! GENERATE " << Path << oendl;
          if( InAppend ) {
            owarn << "!!!! In APPEND mode" << oendl;
          }
          owarn << "!!!!" << oendl;
          owarn << "!!!!!!!!!!!!!!!!!!" << oendl;

          F = new QFile();
          F->open( IO_WriteOnly, stderr );
        }
      }
      setDevice( F );
      return 1;
}

bool SystemFile::close( void ) {
      if( ! F || ! F->isOpen() ) {
        return 1 ;
      }

      QString Prefix = getenv( "NS2OUTPUTTO" );

      if( Prefix == "stderr" ) {
        return 1;
      }

      QString OldP = Prefix + Path + "bup";

      F->close();
      delete F;
      F = 0;

      if( ! InAppend ) {
        owarn << "Rename " << OldP << " to " << Path << oendl;
        return ( rename( OldP.latin1(), Path.latin1() ) >= 0 );
      } 
      return 1;
}

bool SystemFile::preSection( void ) {
      if( hasPreSection ) {
        QFile Fl( TemplDir + Name + "/presection" );
        if( ! Fl.open( IO_ReadOnly ) )
          return 0; // error
        // copy file to this file
        F->writeBlock( Fl.readAll() );
      }
      return 1;
}

bool SystemFile::postSection( void ) {
      if( hasPostSection ) {
        QFile Fl( TemplDir + Name + "/postsection" );
        if( ! Fl.open( IO_ReadOnly ) )
          return 0; // error
        // copy file to this file
        F->writeBlock( Fl.readAll() );
      }
      return 1;
}

bool SystemFile::preNodeSection( ANetNodeInstance * NNI, long ) {
      if( hasPreNodeSection ) {
        QFile Fl( TemplDir + Name + "/prenodesection" );
        if( ! Fl.open( IO_ReadOnly ) )
          return 0; // error
        QTextStream TX( &Fl );
        QString Out;
        QString S = TX.readLine();
        while( ! TX.eof() ) {
          Out = S.
              arg(NNI->nodeClass()->name());
          (*this) << Out << endl;
          S = TX.readLine();
        }
      }
      return 1;
}

bool SystemFile::postNodeSection( ANetNodeInstance * NNI, long ) {
      if( hasPostNodeSection ) {
        QFile Fl( TemplDir + Name + "/postnodesection" );
        if( ! Fl.open( IO_ReadOnly ) )
          return 0; // error
        QTextStream TX( &Fl );
        QString Out;
        QString S = TX.readLine();
        while( ! TX.eof() ) {
          Out = S.
              arg(NNI->name());
          (*this) << Out << endl;
          S = TX.readLine();
        }
      }
      return 1;
}

bool SystemFile::preDeviceSection( ANetNode * NN ) {
      if( hasPreDeviceSection ) {
        QFile Fl( TemplDir + Name + "/predevicesection" );
        if( ! Fl.open( IO_ReadOnly ) )
          return 0; // error
        QTextStream TX( &Fl );
        QString Out;
        QString S = TX.readLine();
        while( ! TX.eof() ) {
          Out = S.arg(NN->name());
          (*this) << Out << endl;
          S = TX.readLine();
        }
      }
      return 1;
}

bool SystemFile::postDeviceSection( ANetNode * NN ) {
      if( hasPostDeviceSection ) {
        QFile Fl( TemplDir + Name + "/postdevicesection" );
        if( ! Fl.open( IO_ReadOnly ) )
          return 0; // error
        QTextStream TX( &Fl );
        QString Out;
        QString S = TX.readLine();
        while( ! TX.eof() ) {
          Out = S.arg(NN->name());
          (*this) << Out << endl;
          S = TX.readLine();
        }
      }
      return 1;
}
