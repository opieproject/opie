#ifndef IRSERVER_H
#define IRSERVER_H

#include <qobject.h>

class QCopChannel;
class OpieLibrary;

class IrServer : public QObject
{
    Q_OBJECT
public:
    IrServer( QObject *parent = 0, const char *name = 0 );
    ~IrServer();

private:
    OpieLibrary *lib;
};

#endif
