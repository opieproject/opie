#ifndef IRSERVER_H
#define IRSERVER_H

#include <qobject.h>

class QCopChannel;
class QLibrary;
struct ObexInterface;

class IrServer : public QObject
{
    Q_OBJECT
public:
    IrServer( QObject *parent = 0, const char *name = 0 );
    ~IrServer();

private:
    QLibrary *lib;
    ObexInterface *iface;
};

#endif
