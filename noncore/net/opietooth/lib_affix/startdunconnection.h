#ifndef startdunconnection_h
#define startdunconnection_h

#include <qobject.h>
#include "../include/connection.h"
#include <opie/oprocess.h>

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
	OProcess* m_dunConnect;
	
    private slots:
	void slotExited( OProcess* proc );
                void slotStdOut( OProcess* proc, char* chars, int len );
    };

 
}

#endif
