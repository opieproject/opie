#ifndef __SYSTEMFILE__H
#define __SYSTEMFILE__H

#include <qstring.h>
#include <qtextstream.h>

class QFile;
class ANetNodeInstance;

class SystemFile : public QTextStream {

public :

    SystemFile( const QString & Name, const QString & Path );
    ~SystemFile( void );

    const QString & name( void )
      { return Name; }

    bool open( void );
    bool close( void );

    bool preSection( void );
    bool postSection( void );
    bool preNodeSection( ANetNodeInstance * NNI, long DevNr );
    bool postNodeSection( ANetNodeInstance * NNI, long DevNr );

private :

    QString Name;
    QString Path;
    QFile * F;
    bool hasPreSection;
    bool hasPreNodeSection;
    bool hasPostSection;
    bool hasPostNodeSection;

};
#endif
