#ifndef __SYSTEMFILE__H
#define __SYSTEMFILE__H

#include <qstring.h>
#include <qtextstream.h>

class QFile;
class ANetNodeInstance;

class SystemFile : public QTextStream {

public :

    SystemFile( const QString & Name, 
                const QString & Path,
                bool KnowsDevicesInstances );
    ~SystemFile( void );

    const QString & name( void ) const
      { return Name; }
    const QString & path( void ) const
      { return Path; }
    bool  knowsDeviceInstances( void ) const
      { return KnowsDeviceInstances; }

    bool open( void );
    bool close( void );

    bool preSection( void );
    bool postSection( void );
    bool preNodeSection( ANetNodeInstance * NNI, long DevNr );
    bool postNodeSection( ANetNodeInstance * NNI, long DevNr );
    bool preDeviceSection( ANetNode * NN );
    bool postDeviceSection( ANetNode * NN );

private :

    QString Name;
    QString Path;
    QFile * F;
    bool hasPreSection;
    bool hasPostSection;
    bool hasPreNodeSection;
    bool hasPostNodeSection;
    bool hasPreDeviceSection;
    bool hasPostDeviceSection;
    bool KnowsDeviceInstances;

};
#endif
