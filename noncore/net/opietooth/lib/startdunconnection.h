#ifndef startdunconnection_h
#define startdunconnection_h

#include <qobject.h>
#include "connection.h"
#include <opie2/oprocess.h>

namespace OpieTooth {

    class StartDunConnection : StartConnection {

	Q_OBJECT

    public:
	StartDunConnection();
	StartDunConnection( QString mac );
	~StartDunConnection();

	QString name();
	void setName( QString name );
	StartConnection::ConnectionType type();
	void setConnectionType( );
	void start();
	void stop();

    private:
	QString m_name;
	QString m_mac;
	ConnectionType m_connectionType;
	Opie::Core::OProcess* m_dunConnect;
	
    private slots:
	void slotExited( Opie::Core::OProcess* proc );
                void slotStdOut( Opie::Core::OProcess* proc, char* chars, int len );
    };

 
}

#endif
